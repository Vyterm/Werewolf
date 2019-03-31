// RegisVerifyDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "VytClient.h"
#include "RegisVerifyDlg.h"
#include "afxdialogex.h"
#include "ClientPeer.h"
#include "Commands.h"

using namespace vyt;

// RegisVerifyDlg 对话框

IMPLEMENT_DYNAMIC(RegisVerifyDlg, CDialogEx)

RegisVerifyDlg::RegisVerifyDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_R_VERIFY, pParent)
	, m_code(_T(""))
{
	Create(IDD_R_VERIFY);
}

RegisVerifyDlg::~RegisVerifyDlg()
{
}

void RegisVerifyDlg::ShowWindow(CString phone)
{
	m_phone = phone;
	CDialogEx::ShowWindow(SW_SHOW);
}

void RegisVerifyDlg::HideWindow()
{
	CDialogEx::ShowWindow(SW_HIDE);
}

void RegisVerifyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_code);
}


BEGIN_MESSAGE_MAP(RegisVerifyDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &RegisVerifyDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// RegisVerifyDlg 消息处理程序


void RegisVerifyDlg::OnBnClickedOk()
{
	UpdateData(TRUE);
	vyt::ClientPeer::Get().Send(_Packet(command(OpCommand::User), command(UserCommand::Verify), "ss", m_phone, m_code));
}
