// JoinVerifyDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "VytClient.h"
#include "JoinVerifyDlg.h"
#include "afxdialogex.h"


// JoinVerifyDlg 对话框

IMPLEMENT_DYNAMIC(JoinVerifyDlg, CDialogEx)

JoinVerifyDlg::JoinVerifyDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_HR_JOINVERIFY, pParent)
{

}

JoinVerifyDlg::~JoinVerifyDlg()
{
}

void JoinVerifyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(JoinVerifyDlg, CDialogEx)
END_MESSAGE_MAP()


// JoinVerifyDlg 消息处理程序
