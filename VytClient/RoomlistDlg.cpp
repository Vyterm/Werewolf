﻿// RoomlistDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "VytClient.h"
#include "RoomlistDlg.h"
#include "afxdialogex.h"
#include "CreateRoomDlg.h"
#include "GameRoomDlg.h"
#include "ClientPeer.h"
#include "Commands.h"
using namespace vyt;

// RoomlistDlg 对话框

#pragma pack(push)
#pragma pack(1)
struct RoomInfo
{
	CString roomname;
	bool haspassword;
	CString creator;
	CString description;
	int currentplayer;
	int maxplayer;
	int mode;
};
#pragma pack(pop)

IMPLEMENT_DYNAMIC(RoomlistDlg, BaseDialog)

RoomlistDlg::RoomlistDlg(CWnd* pParent /*=nullptr*/)
	: BaseDialog(IDD_H_ROOMLIST, pParent)
	, IHandler(command(OpCommand::Lobby), command(LobbyCommand::Refresh))
{
	Refresh();
}

RoomlistDlg::~RoomlistDlg()
{
}

void RoomlistDlg::Refresh()
{
	ClientPeer::Get().Send(_Packet(command(OpCommand::Lobby), command(LobbyCommand::Refresh)));
}

void RoomlistDlg::HandlePacket(vyt::Packet & packet)
{
	if (packet->getOpCommand() == command(OpCommand::Lobby))
	{
		if (packet->getSubCommand() == command(LobbyCommand::Refresh))
		{
			CStringA decodeFormat = "i";
			int lobbyCount;
			packet->Decode(decodeFormat, &lobbyCount);
			RoomInfo *pRooms = new RoomInfo[lobbyCount];
			packet->DecodeStruct("i", "sbssiii", pRooms, lobbyCount);
			m_roomlist.DeleteAllItems();
			for (int i = 0; i < lobbyCount; ++i)
			{
				RoomInfo &room = pRooms[i];
				CString count;
				count.Format(_T("%d / %d"), room.currentplayer, room.maxplayer);
				m_roomlist.InsertItemWithTexts(i, room.roomname, 5,
					room.haspassword ? CString(_T("*")) : CString(_T(" ")),
					room.creator,
					CString(_T("聊天室")),
					room.description,
					count);
			}
		}
	}
}


BOOL RoomlistDlg::OnInitDialog()
{
	__super::OnInitDialog();

	m_roomlist.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_roomlist.InsertColumn(0, _T("房间名"), 0, 150);
	m_roomlist.InsertColumn(1, _T("加密"), 0, 50);
	m_roomlist.InsertColumn(2, _T("创建者"), 0, 120);
	m_roomlist.InsertColumn(3, _T("模式"), 0, 80);
	m_roomlist.InsertColumn(4, _T("描述"), 0, 250);
	m_roomlist.InsertColumn(5, _T("当前玩家数"), 0, 100);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void RoomlistDlg::DoDataExchange(CDataExchange* pDX)
{
	BaseDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_HR_ROOMLIST, m_roomlist);
}


BEGIN_MESSAGE_MAP(RoomlistDlg, BaseDialog)
	ON_BN_CLICKED(IDC_HR_REFRESH, &RoomlistDlg::OnBnClickedHrRefresh)
	ON_BN_CLICKED(IDC_HR_NEWROOM, &RoomlistDlg::OnBnClickedHrNewroom)
	ON_NOTIFY(NM_DBLCLK, IDC_HR_ROOMLIST, &RoomlistDlg::OnJoinRoom)
END_MESSAGE_MAP()


// RoomlistDlg 消息处理程序

void RoomlistDlg::OnBnClickedHrNewroom()
{
	CreateRoomDlg crd;
	ModalDialog(crd);
	Refresh();
}

void RoomlistDlg::OnBnClickedHrRefresh()
{
	Refresh();
}


void RoomlistDlg::OnJoinRoom(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	if (-1 == pNMItemActivate->iItem) return;
	GameRoomDlg::Create(m_roomlist.GetItemText(pNMItemActivate->iItem, 0));

	*pResult = 0;
}
