// UserInfoDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "VytClient.h"
#include "UserInfoDlg.h"
#include "afxdialogex.h"
#include "Commands.h"
#include "ClientPeer.h"

using namespace vyt;
// UserInfoDlg 对话框

IMPLEMENT_DYNAMIC(UserInfoDlg, CDialogEx)

UserInfoDlg::UserInfoDlg(CString username, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_HF_USERINFO, pParent), IHandler(command(OpCommand::User), command(UserCommand::SetName))
	, m_beforeUsername(username)
	, m_username(_T(""))
{
	NetHandler::Get().RegisterHandler(command(OpCommand::User), command(UserCommand::GetInfo), *this);
	ClientPeer::Get().Send(_Packet(command(OpCommand::User), command(UserCommand::GetInfo)));
}

UserInfoDlg::~UserInfoDlg()
{
	NetHandler::Get().UnregisterHandler(command(OpCommand::User), command(UserCommand::GetInfo), *this);
}

void UserInfoDlg::HandlePacket(vyt::Packet & packet)
{
	if (packet->getSubCommand() == command(UserCommand::SetName))
	{
		byte code = packet->getMessage()[0];
		if (code == 0)
		{
			m_beforeUsername = m_username;
			MessageBox(_T("修改用户名成功！"));
		}
		else if (code == 1)
		{
			m_username = m_beforeUsername;
			MessageBox(_T("修改用户名失败！"));
		}
		UpdateData(FALSE);
	}
	else if (packet->getSubCommand() == command(UserCommand::GetInfo))
	{
		int win, lose, run, achieve;
		packet->Decode("iiii", &win, &lose, &run, &achieve);
		CString gamecount;
		gamecount.Format(_T("胜场：%d  逃跑场：%d  总场次：%d  积分：%d"), win, run, win + lose + run, win * 5 - run * 10);
		SetDlgItemText(IDC_HFU_GAMECOUNT, gamecount);
	}
}

void UserInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_HFU_USERNAME, m_username);
}


BEGIN_MESSAGE_MAP(UserInfoDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON2, &UserInfoDlg::OnSetname)
END_MESSAGE_MAP()


// UserInfoDlg 消息处理程序


void UserInfoDlg::OnSetname()
{
	UpdateData(TRUE);
	ClientPeer::Get().Send(_Packet(command(OpCommand::User), command(UserCommand::SetName), "s", m_username));
}


BOOL UserInfoDlg::OnInitDialog()
{
	__super::OnInitDialog();

	m_username = m_beforeUsername;
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}
