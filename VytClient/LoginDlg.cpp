// LoginDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "VytClient.h"
#include "LoginDlg.h"
#include "ClientPeer.h"
#include "afxdialogex.h"
#include "Commands.h"

using namespace vyt;

// LoginDlg 对话框

IMPLEMENT_DYNAMIC(LoginDlg, CDialogEx)

LoginDlg::LoginDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_LOGINDLG, pParent), m_regisDlg(this), m_verifyDlg(this)
	, m_username(_T(""))
	, m_password(_T(""))
{
	m_regisDlg.Create(IDD_REGISDLG, this);
	m_regisDlg.ShowWindow(SW_HIDE);
}

LoginDlg::~LoginDlg()
{
}

void LoginDlg::HandleLoginMsg(byte msg)
{
	if (msg == 0)
	{
		MessageBox(GetStringByTable(IDS_LOGINSUCCESSHINT), GetStringByTable(IDS_LOGINSUCCESSTITLE), MB_ICONINFORMATION);
		EndDialog(SUCCESS_FLAG);
	}
	else if (msg == 1)
		MessageBox(GetStringByTable(IDS_LOGINERROR1), GetStringByTable(IDS_LOGINERRORTITLE), MB_ICONERROR);
	else if (msg == 2)
		MessageBox(GetStringByTable(IDS_LOGINERROR2), GetStringByTable(IDS_LOGINERRORTITLE), MB_ICONERROR);
	else if (msg == 3)
		MessageBox(GetStringByTable(IDS_LOGINERROR3), GetStringByTable(IDS_LOGINERRORTITLE), MB_ICONERROR);
}

void LoginDlg::HandleRegisMsg(byte msg)
{
	if (msg == 0)
		m_verifyDlg.ShowWindow(m_regisDlg.m_username);
	else if (msg == 1)
		MessageBox(GetStringByTable(IDS_REGISERROR1), GetStringByTable(IDS_REGISERRORTITLE), MB_ICONERROR);
}

void LoginDlg::HandleVerifyMsg(byte msg)
{
	if (msg == 0)
	{
		MessageBox(GetStringByTable(IDS_LOGINSUCCESSHINT), GetStringByTable(IDS_REGISSUCCESSTITLE), MB_ICONINFORMATION);
		EndDialog(SUCCESS_FLAG);
	}
	else if (msg == 1)
	{
		MessageBox(_T("输入的不是有效的验证码"));
	}
}

void LoginDlg::ShowLoginDlg()
{
	m_regisDlg.ShowWindow(SW_HIDE);
	this->ShowWindow(SW_SHOW);
}


void LoginDlg::ShowRegisDlg()
{
	m_regisDlg.ShowWindow(SW_SHOW);
	this->ShowWindow(SW_HIDE);
}

void LoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_username);
	DDX_Text(pDX, IDC_EDIT2, m_password);
}


BEGIN_MESSAGE_MAP(LoginDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &LoginDlg::OnClickLogin)
	ON_BN_CLICKED(IDC_BUTTON1, &LoginDlg::ShowRegisDlg)
END_MESSAGE_MAP()


// LoginDlg 消息处理程序


BOOL LoginDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetWindowText(GetStringByTable(IDS_LOGIN));

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void LoginDlg::OnClickLogin()
{
	UpdateData(TRUE);
	vyt::ClientPeer::Get().Send(_Packet(command(OpCommand::User), command(UserCommand::Login), "ss", m_username, m_password));
}
