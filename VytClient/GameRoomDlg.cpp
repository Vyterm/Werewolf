// GameRoomDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "VytClient.h"
#include "GameRoomDlg.h"
#include "afxdialogex.h"
#include "Commands.h"
#include "ClientPeer.h"
#include <map>
using namespace vyt;

static std::map<CString, GameRoomDlg*> pGamerooms;

IMPLEMENT_DYNAMIC(GameRoomDlg, ChatroomDlg)

void GameRoomDlg::Create(CString lobbyID)
{
	if (pGamerooms.find(lobbyID) != pGamerooms.end())
		pGamerooms[lobbyID]->ShowWindow(SW_SHOW);
	else
	{
		auto pRoom = new GameRoomDlg(lobbyID);
		pRoom->ShowWindow(SW_SHOW);
		pGamerooms[lobbyID] = pRoom;
	}
}

void GameRoomDlg::Delete(GameRoomDlg * dlg)
{
	pGamerooms.erase(pGamerooms.find(dlg->getLobbyID()));
	dlg->ShowWindow(SW_HIDE);
	dlg->SetTimer(0x1234, 1000, nullptr);
}

GameRoomDlg::GameRoomDlg(CString lobbyID, CWnd* pParent /*=nullptr*/)
	: ChatroomDlg(lobbyID, pParent, IDD_HR_GAMEROOM)
{
	ChatroomDlg::Create(IDD_HR_GAMEROOM, pParent);
}

GameRoomDlg::~GameRoomDlg()
{
}

void GameRoomDlg::DoDataExchange(CDataExchange* pDX)
{
	ChatroomDlg::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(GameRoomDlg, ChatroomDlg)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// GameRoomDlg 消息处理程序


void GameRoomDlg::OnTimer(UINT_PTR nIDEvent)
{
	ChatroomDlg::OnTimer(nIDEvent);
	delete this;
}


void GameRoomDlg::OnCancel()
{
	__super::OnCancel();
	Delete(this);
	ClientPeer::Get().Send(_Packet(command(OpCommand::Lobby), command(LobbyCommand::Leave), "s", m_lobbyID));
}
