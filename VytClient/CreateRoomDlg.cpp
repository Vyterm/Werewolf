﻿// CreateRoomDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "VytClient.h"
#include "CreateRoomDlg.h"
#include "afxdialogex.h"
#include "ClientPeer.h"
#include "Commands.h"
#include "GameRoomDlg.h"
using namespace vyt;

// CreateRoomDlg 对话框

IMPLEMENT_DYNAMIC(CreateRoomDlg, CDialogEx)

CreateRoomDlg::CreateRoomDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_HR_CREATEROOM, pParent)
	, IHandler(command(OpCommand::Lobby), command(LobbyCommand::Create))
	, m_roomname(_T(""))
	, m_password(_T(""))
	, m_ispassword(FALSE)
{

}

CreateRoomDlg::~CreateRoomDlg()
{
}

void CreateRoomDlg::HandlePacket(vyt::Packet & packet)
{
	byte access = packet->getMessage()[0];
	if (1 == access)
		MessageBox(_T("创建房间失败，该房间已存在！"));
	else if (0 == access)
	{
		MessageBox(_T("创建房间成功！"));
		CString roomname;
		packet->Decode("bs", nullptr, &roomname);
		GameRoomDlg::Create(roomname); 
		EndDialog(0);
	}
}

void CreateRoomDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_HFU_USERNAME, m_roomname);
	DDX_Text(pDX, IDC_EDIT1, m_password);
	DDX_Control(pDX, IDC_EDIT1, m_passwordInputfield);
	DDX_Check(pDX, IDC_CHECK1, m_ispassword);
}


BEGIN_MESSAGE_MAP(CreateRoomDlg, CDialogEx)
	ON_BN_CLICKED(IDC_CHECK1, &CreateRoomDlg::PasswordSetting)
	ON_BN_CLICKED(IDC_BUTTON2, &CreateRoomDlg::OnCreateRoom)
	ON_BN_CLICKED(IDC_BUTTON4, &CreateRoomDlg::OnClickCancel)
END_MESSAGE_MAP()


// CreateRoomDlg 消息处理程序


void CreateRoomDlg::PasswordSetting()
{
	UpdateData(TRUE);
	m_passwordInputfield.EnableWindow(m_ispassword);
}


void CreateRoomDlg::OnCreateRoom()
{
	UpdateData(TRUE);
	ClientPeer::Get().Send(_Packet(command(OpCommand::Lobby), command(LobbyCommand::Create), "s", m_roomname));
}


void CreateRoomDlg::OnClickCancel()
{
	EndDialog(0);
}
