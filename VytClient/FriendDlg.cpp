// FriendDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "VytClient.h"
#include "FriendDlg.h"
#include "afxdialogex.h"
#include "ClientPeer.h"
#include "Commands.h"
#include "UserInfoDlg.h"
using namespace vyt;

// FriendDlg 对话框

IMPLEMENT_DYNAMIC(FriendDlg, BaseDialog)

FriendDlg::FriendDlg(CWnd* pParent /*=nullptr*/)
	: BaseDialog(IDD_H_FRIEND, pParent), IHandler(command(OpCommand::User), command(UserCommand::GetName))
{
	vyt::NetHandler::Get().RegisterHandler(command(OpCommand::Friend), command(FriendCommand::List), *this);
	vyt::NetHandler::Get().RegisterHandler(command(OpCommand::Friend), command(FriendCommand::Add), *this);
	vyt::ClientPeer::Get().Send(_Packet(command(OpCommand::User), command(UserCommand::GetName)));
	vyt::ClientPeer::Get().Send(_Packet(command(OpCommand::Friend), command(FriendCommand::List)));
}

FriendDlg::~FriendDlg()
{
	vyt::NetHandler::Get().RegisterHandler(command(OpCommand::Friend), command(FriendCommand::List), *this);
	vyt::NetHandler::Get().RegisterHandler(command(OpCommand::Friend), command(FriendCommand::Add), *this);
}

void FriendDlg::HandlePacket(vyt::Packet & packet)
{
	if (packet->getOpCommand() == command(OpCommand::User) && packet->getSubCommand() == command(UserCommand::GetName))
	{
		CString username;
		packet->Decode("s", &username);
		this->SetDlgItemText(IDC_HF_USERNAME, username);
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
	}
}

void FriendDlg::DoDataExchange(CDataExchange* pDX)
{
	BaseDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_HFU_FRIENDLIST, m_friends);
}


BEGIN_MESSAGE_MAP(FriendDlg, BaseDialog)
	ON_BN_CLICKED(IDC_HF_USERNAME, &FriendDlg::OnShowDetails)
END_MESSAGE_MAP()


// FriendDlg 消息处理程序


void FriendDlg::OnShowDetails()
{
	CString username;
	GetDlgItemText(IDC_HF_USERNAME, username);
	UserInfoDlg infodlg(username, this);
	ModalDialog(infodlg);
}
