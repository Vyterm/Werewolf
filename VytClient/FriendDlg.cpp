// FriendDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "VytClient.h"
#include "FriendDlg.h"
#include "afxdialogex.h"


// FriendDlg 对话框

IMPLEMENT_DYNAMIC(FriendDlg, CDialogEx)

FriendDlg::FriendDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_H_FRIEND, pParent)
{

}

FriendDlg::~FriendDlg()
{
}

void FriendDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(FriendDlg, CDialogEx)
END_MESSAGE_MAP()


// FriendDlg 消息处理程序
