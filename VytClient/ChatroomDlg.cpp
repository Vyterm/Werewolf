// ChatroomDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "VytClient.h"
#include "ChatroomDlg.h"
#include "afxdialogex.h"
#include "Commands.h"
#include "ClientPeer.h"

using namespace vyt;
// ChatroomDlg 对话框

IMPLEMENT_DYNAMIC(ChatroomDlg, CDialogEx)

ChatroomDlg::ChatroomDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_H_CHATROOM, pParent), IHandler(command(OpCommand::Lobby), command(LobbyCommand::Chat))
	, m_message(_T(""))
{
	vyt::ClientPeer::Get().Send(_Packet(command(OpCommand::Lobby), command(LobbyCommand::Join), "i", 0));
}

ChatroomDlg::~ChatroomDlg()
{
}

void ChatroomDlg::HandlePacket(vyt::Packet & packet)
{
	int lobbyId;
	packet->Decode("i", &lobbyId);
	if (0 != lobbyId) return;
	CString senderName, chatMessage;
	packet->Decode("iss", &lobbyId, &senderName, &chatMessage);
	m_chats.InsertItem(m_chats.GetItemCount(), senderName + _T(":") + chatMessage);
}

void ChatroomDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_chats);
	DDX_Text(pDX, IDC_EDIT1, m_message);
}


BEGIN_MESSAGE_MAP(ChatroomDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON2, &ChatroomDlg::DoSend)
END_MESSAGE_MAP()


// ChatroomDlg 消息处理程序


void ChatroomDlg::DoSend()
{
	UpdateData(TRUE);
	if (_T("") != m_message)
	{
		vyt::ClientPeer::Get().Send(_Packet(command(OpCommand::Lobby), command(LobbyCommand::Chat), "is", 0, m_message));
		m_message = _T("");
		UpdateData(FALSE);
	}
}


BOOL ChatroomDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
		if (VK_RETURN == pMsg->wParam)
			DoSend();

	return CDialogEx::PreTranslateMessage(pMsg);
}
