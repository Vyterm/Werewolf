// RegisDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "VytClient.h"
#include "RegisDlg.h"
#include "LoginDlg.h"
#include "ClientPeer.h"
#include "Commands.h"
#include "afxdialogex.h"

using namespace vyt;

// RegisDlg 对话框

IMPLEMENT_DYNAMIC(RegisDlg, CDialogEx)

RegisDlg::RegisDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_REGISDLG, pParent)
	, m_username(_T(""))
	, m_password(_T(""))
{
	m_pLoginDlg = dynamic_cast<LoginDlg*>(pParent);
	ASSERT(nullptr != m_pLoginDlg);
}

RegisDlg::~RegisDlg()
{
}

void RegisDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_username);
	DDX_Text(pDX, IDC_EDIT2, m_password);
}


BEGIN_MESSAGE_MAP(RegisDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &RegisDlg::OnClickRegis)
	ON_BN_CLICKED(IDC_BUTTON1, &RegisDlg::OnShowLogin)
END_MESSAGE_MAP()


// RegisDlg 消息处理程序


void RegisDlg::OnClickRegis()
{
	UpdateData(TRUE);
	vyt::ClientPeer::Get().Send(_Packet(command(OpCommand::User), command(UserCommand::Regis), "ss", m_username, m_password));
}


void RegisDlg::OnShowLogin()
{
	m_pLoginDlg->ShowLoginDlg();
}


BOOL RegisDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetWindowText(GetStringByTable(IDS_REGIS));

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}
