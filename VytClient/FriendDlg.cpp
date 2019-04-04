// FriendDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "VytClient.h"
#include "FriendDlg.h"
#include "afxdialogex.h"
#include "ClientPeer.h"
#include "Commands.h"
#include "UserInfoDlg.h"
#include "FriendChatDlg.h"
using namespace vyt;

// FriendDlg 对话框

IMPLEMENT_DYNAMIC(FriendDlg, BaseDialog)

FriendDlg::FriendDlg(CWnd* pParent /*=nullptr*/)
	: BaseDialog(IDD_H_FRIEND, pParent), IHandler(command(OpCommand::User), command(UserCommand::GetName))
{
	vyt::NetHandler::Get().RegisterHandler(command(OpCommand::User), command(UserCommand::Rename), *this);
	vyt::NetHandler::Get().RegisterHandler(command(OpCommand::Friend), command(FriendCommand::List), *this);
	vyt::NetHandler::Get().RegisterHandler(command(OpCommand::Friend), command(FriendCommand::Add), *this);
	vyt::NetHandler::Get().RegisterHandler(command(OpCommand::Friend), command(FriendCommand::Del), *this);
	NetHandler::Get().RegisterHandler(command(OpCommand::Friend), command(FriendCommand::Access), *this);
	vyt::ClientPeer::Get().Send(_Packet(command(OpCommand::User), command(UserCommand::GetName)));
	vyt::ClientPeer::Get().Send(_Packet(command(OpCommand::Friend), command(FriendCommand::List)));
}

FriendDlg::~FriendDlg()
{
	vyt::NetHandler::Get().UnregisterHandler(command(OpCommand::User), command(UserCommand::Rename), *this);
	vyt::NetHandler::Get().UnregisterHandler(command(OpCommand::Friend), command(FriendCommand::List), *this);
	vyt::NetHandler::Get().UnregisterHandler(command(OpCommand::Friend), command(FriendCommand::Add), *this);
	vyt::NetHandler::Get().UnregisterHandler(command(OpCommand::Friend), command(FriendCommand::Del), *this);
	NetHandler::Get().UnregisterHandler(command(OpCommand::Friend), command(FriendCommand::Access), *this);
}

void FriendDlg::ShowFriends(vyt::Packet & packet)
{
	int count;
	packet->Decode("i", &count);
	CString *buffers = new CString[count];
	packet->Decode("iS", nullptr, &buffers, count);
	for (int i = 0; i < count; ++i)
		m_friends.InsertItem(i, buffers[i]);
	delete[] buffers;
}

void FriendDlg::FriendRename(vyt::Packet & packet)
{
	CString oldname, newname;
	packet->Decode("ss", &oldname, &newname);
	for (int i = 0; i < m_friends.GetItemCount(); ++i)
		if (m_friends.GetItemText(i, 0) == oldname)
		{
			m_friends.SetItemText(i, 0, newname);
			break;
		}
}

void FriendDlg::ChatAccess(vyt::Packet & packet)
{
	byte access = packet->getMessage()[0];
	if (access == 0)
	{
		CString friendname;
		packet->Decode("bs", nullptr, &friendname);
		FriendChatDlg::Create(m_username, friendname, this, m_friendFlag);
		m_friendFlag = FriendChatDlg::CHAT_FLAG;
	}
	else if (access == 1)
		MessageBox(_T("好友名称错误!"));
	else if (access == 2)
		MessageBox(_T("与该玩家不是好友关系!"));
	else if (access == 3)
		MessageBox(_T("该玩家未在线!"));
}

void FriendDlg::AddFriend(vyt::Packet & packet)
{
	byte access = packet->getMessage()[0];
	if (1 == access)
		MessageBox(_T("添加好友失败，可能是因为用户名错误或对方不在线"));
	else if (2 == access)
		MessageBox(_T("不能添加自身为好友"));
	else if (3 == access)
		MessageBox(_T("与对方已经是好友关系，不能重复添加"));
	else if (0 == access)
	{
		CString friendName;
		packet->Decode("bs", nullptr, &friendName);
		m_friends.InsertItem(m_friends.GetItemCount(), friendName);
		friendName.Format(_T("已添加%s为好友！"), friendName);
		MessageBox(friendName);
	}
}

void FriendDlg::DelFriend(vyt::Packet & packet)
{
	CString friendname;
	packet->Decode("s", &friendname);
	for (int i = 0; i < m_friends.GetItemCount(); ++i)
		if (m_friends.GetItemText(i, 0) == friendname)
		{
			m_friends.DeleteItem(i);
			break;
		}
}

void FriendDlg::HandlePacket(vyt::Packet & packet)
{
	if (packet->getOpCommand() == command(OpCommand::User))
	{
		if (packet->getSubCommand() == command(UserCommand::GetName))
		{
			packet->Decode("s", &m_username);
			this->SetDlgItemText(IDC_HF_USERNAME, m_username);
		}
		else if (packet->getSubCommand() == command(UserCommand::Rename))
			FriendRename(packet);
	}
	if (packet->getOpCommand() == command(OpCommand::Friend))
	{
		if (packet->getSubCommand() == command(FriendCommand::List))
			ShowFriends(packet);
		else if (packet->getSubCommand() == command(FriendCommand::Add))
			AddFriend(packet);
		else if (packet->getSubCommand() == command(FriendCommand::Del))
			DelFriend(packet);
		else if (packet->getSubCommand() == command(FriendCommand::Access))
			ChatAccess(packet);
	}
}

void FriendDlg::DoDataExchange(CDataExchange* pDX)
{
	BaseDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_HFU_FRIENDLIST, m_friends);
}


BEGIN_MESSAGE_MAP(FriendDlg, BaseDialog)
	ON_BN_CLICKED(IDC_HF_USERNAME, &FriendDlg::OnShowDetails)
	ON_NOTIFY(NM_RCLICK, IDC_HFU_FRIENDLIST, &FriendDlg::InteractionWithFriend)
	ON_COMMAND(ID_HF_CHAT, &FriendDlg::OnChatToFriend)
	ON_COMMAND(ID_HF_FILE, &FriendDlg::OnFileFriend)
	ON_COMMAND(ID_HF_VIDEO, &FriendDlg::OnVideoFriend)
	ON_COMMAND(ID_HF_ADD, &FriendDlg::OnAppendFriend)
	ON_COMMAND(ID_HF_DEL, &FriendDlg::OnDeleteFriend)
END_MESSAGE_MAP()


// FriendDlg 消息处理程序


void FriendDlg::OnShowDetails()
{
	CString username;
	GetDlgItemText(IDC_HF_USERNAME, username);
	UserInfoDlg infodlg(username, this);
	ModalDialog(infodlg);
}


void FriendDlg::InteractionWithFriend(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	m_friendID = pNMItemActivate->iItem;
	if (-1 == m_friendID) return;
	CMenu menu;
	menu.LoadMenu(IDR_HOMETRACK);
	POINT pos;
	GetCursorPos(&pos);
	menu.GetSubMenu(1)->TrackPopupMenu(TPM_LEFTALIGN, pos.x, pos.y, this);

	*pResult = 0;
}


void FriendDlg::OnChatToFriend()
{
	m_friendFlag = FriendChatDlg::CHAT_FLAG;
	ClientPeer::Get().Send(_Packet(command(OpCommand::Friend), command(FriendCommand::Access), "is", 0, m_friends.GetItemText(m_friendID, 0)));
}


void FriendDlg::OnFileFriend()
{
	m_friendFlag = FriendChatDlg::FILE_FLAG;
	ClientPeer::Get().Send(_Packet(command(OpCommand::Friend), command(FriendCommand::Access), "is", 0, m_friends.GetItemText(m_friendID, 0)));
}


void FriendDlg::OnVideoFriend()
{
	m_friendFlag = FriendChatDlg::VIDEO_FLAG;
	ClientPeer::Get().Send(_Packet(command(OpCommand::Friend), command(FriendCommand::Access), "is", 0, m_friends.GetItemText(m_friendID, 0)));
}


void FriendDlg::OnAppendFriend()
{
	// TODO: 在此添加命令处理程序代码
}


void FriendDlg::OnDeleteFriend()
{
	ClientPeer::Get().Send(_Packet(command(OpCommand::Friend), command(FriendCommand::Del), "s", m_friends.GetItemText(m_friendID, 0)));
}
