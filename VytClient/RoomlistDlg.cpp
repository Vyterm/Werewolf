// RoomlistDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "VytClient.h"
#include "RoomlistDlg.h"
#include "afxdialogex.h"


// RoomlistDlg 对话框

IMPLEMENT_DYNAMIC(RoomlistDlg, CDialogEx)

RoomlistDlg::RoomlistDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_H_ROOMLIST, pParent)
{

}

RoomlistDlg::~RoomlistDlg()
{
}

void RoomlistDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(RoomlistDlg, CDialogEx)
END_MESSAGE_MAP()


// RoomlistDlg 消息处理程序
