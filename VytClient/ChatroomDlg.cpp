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
	, m_playerHandler(m_players)
	, m_roomID(_T(""))
	, m_message(_T(""))
	, m_chats(_T(""))
{
	vyt::ClientPeer::Get().Send(_Packet(command(OpCommand::Lobby), command(LobbyCommand::Join), "s", m_roomID));
}

ChatroomDlg::~ChatroomDlg()
{
}

void ChatroomDlg::HandlePacket(vyt::Packet & packet)
{
	CString lobbyId;
	packet->Decode("s", &lobbyId);
	if (m_roomID != lobbyId) return;
	CString senderName, chatMessage;
	packet->Decode("sss", nullptr, &senderName, &chatMessage);
	UpdateData(TRUE);
	m_chats += senderName + _T(":") + chatMessage + _T("\r\n");
	UpdateData(FALSE);
	m_chatscroll.LineScroll(m_chatscroll.GetLineCount());
	//m_chatscroll.SendMessage(WM_VSCROLL, SB_BOTTOM, 0);
}

void ChatroomDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_HCR_PLAYERLIST, m_players);
	DDX_Text(pDX, IDC_EDIT1, m_message);
	DDX_Text(pDX, IDC_HCR_CHAT, m_chats);
	DDX_Control(pDX, IDC_HCR_CHAT, m_chatscroll);
}


BEGIN_MESSAGE_MAP(ChatroomDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON2, &ChatroomDlg::DoSend)
	ON_NOTIFY(NM_RCLICK, IDC_HCR_PLAYERLIST, &ChatroomDlg::OnSelectPlayer)
	ON_COMMAND(ID_HC_FRIEND, &ChatroomDlg::OnAddFriend)
END_MESSAGE_MAP()


// ChatroomDlg 消息处理程序


void ChatroomDlg::DoSend()
{
	UpdateData(TRUE);
	if (_T("") != m_message)
	{
		vyt::ClientPeer::Get().Send(_Packet(command(OpCommand::Lobby), command(LobbyCommand::Chat), "ss", m_roomID, m_message));
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

BOOL ChatroomDlg::OnInitDialog()
{
	__super::OnInitDialog();

	m_players.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void PlayerListHandler::PlayerJoin(CString player)
{
	m_players.InsertItem(m_players.GetItemCount(), player);
}

void PlayerListHandler::PlayerLeave(CString player)
{
	for (int i = 0; i < m_players.GetItemCount(); ++i)
		if (m_players.GetItemText(i, 0) == player)
		{
			m_players.DeleteItem(i);
			break;
		}
}

void PlayerListHandler::PlayerRename(CString oldname, CString newname)
{
	for (int i = 0; i < m_players.GetItemCount(); ++i)
		if (m_players.GetItemText(i, 0) == oldname)
		{
			m_players.SetItemText(i, 0, newname);
			break;
		}
}

PlayerListHandler::PlayerListHandler(CListCtrl & players)
	: IHandler(command(OpCommand::Lobby), command(LobbyCommand::Join)), m_players(players)
{
	NetHandler::Get().RegisterHandler(command(OpCommand::Lobby), command(LobbyCommand::Leave), *this);
	NetHandler::Get().RegisterHandler(command(OpCommand::User), command(UserCommand::Rename), *this);
}

PlayerListHandler::~PlayerListHandler()
{
	NetHandler::Get().UnregisterHandler(command(OpCommand::Lobby), command(LobbyCommand::Leave), *this);
	NetHandler::Get().UnregisterHandler(command(OpCommand::User), command(UserCommand::Rename), *this);
}

void PlayerListHandler::HandlePacket(vyt::Packet & packet)
{
	if (packet->getOpCommand() == command(OpCommand::Lobby))
	{
		if (packet->getSubCommand() == command(LobbyCommand::Join))
		{
			CString player;
			packet->Decode("s", &player);
			PlayerJoin(player);
		}
		else if (packet->getSubCommand() == command(LobbyCommand::Leave))
		{
			CString player;
			packet->Decode("s", &player);
			PlayerLeave(player);
		}
	}
	else if (packet->getOpCommand() == command(OpCommand::User))
	{
		if (packet->getSubCommand() == command(UserCommand::Rename))
		{
			CString player;
			CString newname;
			packet->Decode("ss", &player, &newname);
			PlayerRename(player, newname);
		}
	}
}


void ChatroomDlg::OnSelectPlayer(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	m_friendID = pNMItemActivate->iItem;
	if (-1 == m_friendID) return;
	CMenu menu;
	menu.LoadMenu(IDR_HOMETRACK);
	POINT point;
	GetCursorPos(&point);
	menu.GetSubMenu(2)->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
	*pResult = 0;
}


void ChatroomDlg::OnAddFriend()
{
	if (-1 == m_friendID || m_friendID > m_players.GetItemCount()) return;
	ClientPeer::Get().Send(_Packet(command(OpCommand::Friend), command(FriendCommand::Add), "s", m_players.GetItemText(m_friendID, 0)));
}
