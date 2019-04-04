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

void FriendChatDlg::Create(CString selfname, CString friendname, CWnd* pParent, unsigned long extraFlags)
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

void FriendChatDlg::Rename(FriendChatDlg *pDlg, CString &newname)
{
	pFriends.erase(pFriends.find(pDlg->m_friendname));
	pDlg->m_friendname = newname;
	pFriends[pDlg->m_friendname] = pDlg;
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
	UpdateTitle();
	NetHandler::Get().RegisterHandler(command(OpCommand::User), command(UserCommand::Rename), *this);
	NetHandler::Get().RegisterHandler(command(OpCommand::Friend), command(FriendCommand::Offline), *this);
	NetHandler::Get().RegisterHandler(command(OpCommand::Friend), command(FriendCommand::File), *this);
	NetHandler::Get().RegisterHandler(command(OpCommand::Friend), command(FriendCommand::Video), *this);
}

FriendChatDlg::~FriendChatDlg()
{
	NetHandler::Get().UnregisterHandler(command(OpCommand::User), command(UserCommand::Rename), *this);
	NetHandler::Get().UnregisterHandler(command(OpCommand::Friend), command(FriendCommand::Offline), *this);
	NetHandler::Get().UnregisterHandler(command(OpCommand::Friend), command(FriendCommand::File), *this);
	NetHandler::Get().UnregisterHandler(command(OpCommand::Friend), command(FriendCommand::Video), *this);
}

void FriendChatDlg::UpdateTitle()
{
	CString title;
	title.Format(_T("%s 与 %s 的对话"), m_selfname, m_friendname);
	CDialogEx::SetWindowText(title);
}

void FriendChatDlg::ShowChat(CString &sender, CString & chat)
{
	UpdateData(TRUE);
	m_chatview += sender + _T(":") + chat + _T("\r\n");
	UpdateData(FALSE);
}

void FriendChatDlg::SendFile(CString filepath)
{
	CString filename = filepath.Mid(filepath.ReverseFind('\\') + 1);
	CFile file;
	file.Open(filepath, CFile::modeRead);
	vytsize length = vytsize(file.GetLength());
	char *pBuffer = new char[length];
	file.Read(pBuffer, length);
	Buffer buffer = _Buffer("ss", m_friendname, filename);
	ClientPeer::Get().Send(_Packet(command(OpCommand::Friend), command(FriendCommand::File), {
		{ buffer->m_buffer, buffer->m_size },
		{ pBuffer, length },
		}));
	file.Close();
}

void FriendChatDlg::HandleRename(vyt::Packet & packet)
{
	CString oldname, newname;
	packet->Decode("ss", &oldname, &newname);
	if (oldname == m_selfname)
		m_selfname = newname;
	else if (oldname == m_friendname)
		Rename(this, newname);
	UpdateTitle();
}

void FriendChatDlg::HandleChat(vyt::Packet & packet)
{
	CString sender, chat;
	packet->Decode("ss", &sender, &chat);
	ShowChat(sender, chat);
}

void FriendChatDlg::HandleFile(vyt::Packet & packet)
{
	CString filename;
	vytsize size = packet->Decode("ss", nullptr, &filename);
	CString filepath;
	filepath.Format(_T("C:\\Users\\Vyterm\\Documents\\%s"), filename);
	char path[MAX_PATH] = {};
	WideCharToMultiByte(CP_ACP, 0, filepath, -1, path, MAX_PATH, nullptr, FALSE);
	FILE *file = nullptr;
	fopen_s(&file, path, "w+");
	if (nullptr != file)
	{
		fwrite(packet->getMessage() + size, packet->getMessageSize() - size, 1, file);
		fclose(file);
		system("explorer C:\\Users\\Vyterm\\Documents");
		CString message;
		message.Format(_T("收到来自 %s 的文件 %s"), m_friendname, filename);
		MessageBox(message);
	}
}

void FriendChatDlg::HandleVideo(vyt::Packet & packet)
{
}

void FriendChatDlg::HandlePacket(vyt::Packet & packet)
{
	if (packet->getOpCommand() == command(OpCommand::Friend))
	{
		CString player;
		packet->Decode("s", &player);
		if (player != m_friendname) return;
		if (packet->getSubCommand() == command(FriendCommand::Offline))
			Delete(this);
		else if (packet->getSubCommand() == command(FriendCommand::Chat))
			HandleChat(packet);
		else if (packet->getSubCommand() == command(FriendCommand::File))
			HandleFile(packet);
		else if (packet->getSubCommand() == command(FriendCommand::Video))
			HandleVideo(packet);
	}
	else if (packet->getOpCommand() == command(OpCommand::User))
	{
		if (packet->getSubCommand() == command(UserCommand::Rename))
			HandleRename(packet);
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
	ON_WM_DROPFILES()
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


void FriendChatDlg::OnDropFiles(HDROP hDropInfo)
{
	UpdateData(TRUE);
	TCHAR path[MAX_PATH];
	DragQueryFile(hDropInfo, 0, path, MAX_PATH);
	SendFile(path);
	UpdateData(FALSE);

	__super::OnDropFiles(hDropInfo);
}
