// FriendChatDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "VytClient.h"
#include "FriendChatDlg.h"
#include "afxdialogex.h"
#include "Commands.h"
#include "ClientPeer.h"
#include <map>

using namespace vyt;

// FriendChatDlg 对话框

static std::map<CString, FriendChatDlg*> pFriends;

void FriendChatDlg::Create(CString selfname, CString friendname, CWnd* pParent)
{
	FriendChatDlg *fcd = nullptr;
	if (pFriends.find(friendname) == pFriends.end())
	{
		fcd = new FriendChatDlg(selfname, friendname);
		pFriends.emplace(friendname, fcd);
	}
	else
		fcd = pFriends.find(friendname)->second;
	fcd->ShowWindow(SW_SHOW);
}

void FriendChatDlg::Delete(FriendChatDlg * pDlg)
{
	pFriends.erase(pFriends.find(pDlg->m_friendname));
	pDlg->ShowWindow(SW_HIDE);
	pDlg->SetTimer(0x1234, 1000, nullptr);
}

IMPLEMENT_DYNAMIC(FriendChatDlg, CDialogEx)

FriendChatDlg::FriendChatDlg(CString selfname, CString friendname, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_HF_FRIENDCHAT, pParent)
	, IHandler(command(OpCommand::Friend), command(FriendCommand::Chat))
	, m_selfname(selfname)
	, m_friendname(friendname)
	, m_chatview(_T(""))
	, m_inputchat(_T(""))
{
	CDialogEx::Create(IDD_HF_FRIENDCHAT, pParent);
	CString title;
	title.Format(_T("%s 与 %s 的对话"), m_selfname, m_friendname);
	CDialogEx::SetWindowText(title);
	NetHandler::Get().RegisterHandler(command(OpCommand::Friend), command(FriendCommand::Offline), *this);
}

FriendChatDlg::~FriendChatDlg()
{
	NetHandler::Get().UnregisterHandler(command(OpCommand::Friend), command(FriendCommand::Offline), *this);
}

void FriendChatDlg::ShowChat(CString & sender, CString & chat)
{
	UpdateData(TRUE);
	m_chatview += sender + _T(":") + chat + _T("\r\n");
	UpdateData(FALSE);
}

void FriendChatDlg::HandlePacket(vyt::Packet & packet)
{
	if (packet->getSubCommand() == command(FriendCommand::Chat))
	{
		CString sender, chat;
		packet->Decode("s", &sender);
		if (sender == m_friendname)
		{
			packet->Decode("ss", nullptr, &chat);
			ShowChat(sender, chat);
		}
	}
	else if (packet->getSubCommand() == command(FriendCommand::Offline))
	{
		CString player;
		packet->Decode("s", &player);
		if (player == m_friendname)
			Delete(this);
	}
}

void FriendChatDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_HFF_CHATVIEW, m_chatview);
	DDX_Text(pDX, IDC_HFF_CHATINPUT, m_inputchat);
}


BEGIN_MESSAGE_MAP(FriendChatDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &FriendChatDlg::SendChat)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// FriendChatDlg 消息处理程序


void FriendChatDlg::SendChat()
{
	UpdateData(TRUE);
	if (m_inputchat.IsEmpty()) return;
	ShowChat(m_selfname, m_inputchat);
	ClientPeer::Get().Send(_Packet(command(OpCommand::Friend), command(FriendCommand::Chat), "ss", m_friendname, m_inputchat));
	m_inputchat = _T("");
	UpdateData(FALSE);
}


BOOL FriendChatDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
		SendChat();

	return __super::PreTranslateMessage(pMsg);
}


void FriendChatDlg::OnTimer(UINT_PTR nIDEvent)
{
	__super::OnTimer(nIDEvent);

	delete this;
}
