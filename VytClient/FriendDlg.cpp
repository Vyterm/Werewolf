﻿// FriendDlg.cpp: 实现文件
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
	vyt::NetHandler::Get().RegisterHandler(command(OpCommand::Friend), command(FriendCommand::List), *this);
	vyt::NetHandler::Get().RegisterHandler(command(OpCommand::Friend), command(FriendCommand::Add), *this);
	NetHandler::Get().RegisterHandler(command(OpCommand::Friend), command(FriendCommand::Access), *this);
	vyt::ClientPeer::Get().Send(_Packet(command(OpCommand::User), command(UserCommand::GetName)));
	vyt::ClientPeer::Get().Send(_Packet(command(OpCommand::Friend), command(FriendCommand::List)));
}

FriendDlg::~FriendDlg()
{
	vyt::NetHandler::Get().RegisterHandler(command(OpCommand::Friend), command(FriendCommand::List), *this);
	vyt::NetHandler::Get().RegisterHandler(command(OpCommand::Friend), command(FriendCommand::Add), *this);
	NetHandler::Get().UnregisterHandler(command(OpCommand::Friend), command(FriendCommand::Access), *this);
}

void FriendDlg::HandleAccess(vyt::Packet & packet)
{
	byte access = packet->getMessage()[0];
	if (access == 0)
	{
		CString friendname;
		packet->Decode("bs", nullptr, &friendname);
		FriendChatDlg::Create(m_username, friendname, this);
	}
	else if (access == 1)
		MessageBox(_T("好友名称错误!"));
	else if (access == 2)
		MessageBox(_T("与该玩家不是好友关系!"));
	else if (access == 3)
		MessageBox(_T("该玩家未在线!"));
}

void FriendDlg::HandlePacket(vyt::Packet & packet)
{
	if (packet->getOpCommand() == command(OpCommand::User) && packet->getSubCommand() == command(UserCommand::GetName))
	{
		packet->Decode("s", &m_username);
		this->SetDlgItemText(IDC_HF_USERNAME, m_username);
	}
	if (packet->getOpCommand() == command(OpCommand::Friend))
	{
		if (packet->getSubCommand() == command(FriendCommand::List))
		{
			CStringA packetFormat = "i";
			int count;
			packet->Decode(packetFormat, &count);
			CString *buffers = new CString[count];
			for (int i = 0; i < count; ++i)
				packetFormat += "s";
			packet->Decode(packetFormat, nullptr, buffers);
			for (int i = 0; i < count; ++i)
				m_friends.InsertItem(i, buffers[i]);
			delete[] buffers;
		}
		else if (packet->getSubCommand() == command(FriendCommand::Add))
		{
			if (1 == packet->getMessage()[0])
				MessageBox(_T("添加好友失败，可能是因为用户名错误或对方不在线"));
			else if (0 == packet->getMessage()[0])
			{
				CString friendName;
				packet->Decode("bs", nullptr, &friendName);
				m_friends.InsertItem(m_friends.GetItemCount(), friendName);
				friendName.Format(_T("已添加%s为好友！"), friendName);
				MessageBox(friendName);
			}
		}
		else if (packet->getSubCommand() == command(FriendCommand::Access))
		{
			HandleAccess(packet);
		}
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
	ClientPeer::Get().Send(_Packet(command(OpCommand::Friend), command(FriendCommand::Access), "is", 0, m_friends.GetItemText(m_friendID, 0)));
}


void FriendDlg::OnDeleteFriend()
{
	// TODO: 在此添加命令处理程序代码
}
