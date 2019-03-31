#include "MsgHandler.h"
#include "LoginDlg.h"

std::map<vyt::command, std::map<vyt::command, Handler>> handlers = {
	{ vyt::command(OpCommand::Kernel), {
		{ vyt::command(KernelCommand::Version), &KernelVersionHandler },
	} },
	{ vyt::command(OpCommand::User), {
		{ vyt::command(UserCommand::Login), &UserLoginHandler },
		{ vyt::command(UserCommand::Regis), &UserRegisHandler },
		{ vyt::command(UserCommand::Verify), &UserVerifyHandler },
	} },
	{ vyt::command(OpCommand::Lobby), {
		{ vyt::command(LobbyCommand::Join), &LobbyJoinHandler },
		{ vyt::command(LobbyCommand::Chat), &LobbyChatHandler },
	} },
	{ vyt::command(OpCommand::Friend), {
		{ vyt::command(FriendCommand::Online), &FriendOnlineHandler },
		{ vyt::command(FriendCommand::Offline), &FriendOfflineHandler },
		{ vyt::command(FriendCommand::Add), &FriendAddHandler },
		{ vyt::command(FriendCommand::Del), &FriendDelHandler },
		{ vyt::command(FriendCommand::Chat), &FriendChatHandler },
		{ vyt::command(FriendCommand::File), &FriendFileHandler },
		{ vyt::command(FriendCommand::Video), &FriendVideoHandler },
	} },
};

void KernelVersionHandler(CWnd *wnd, vyt::Packet packet)
{
}

void UserLoginHandler(CWnd *wnd, vyt::Packet packet)
{
	LoginDlg *pLoginDlg = dynamic_cast<LoginDlg*>(wnd);
	if (nullptr == pLoginDlg) return;
	if (packet->getMessageSize() != 1)
		MessageBox(wnd->GetSafeHwnd(), _T("非法的消息"), _T("错误"), MB_ICONERROR);
	else
		pLoginDlg->HandleLoginMsg(packet->getMessage()[0]);
}

void UserRegisHandler(CWnd *wnd, vyt::Packet packet)
{
	LoginDlg *pLoginDlg = dynamic_cast<LoginDlg*>(wnd);
	if (nullptr == pLoginDlg) return;
	if (packet->getMessageSize() != 1)
		MessageBox(wnd->GetSafeHwnd(), _T("非法的消息"), _T("错误"), MB_ICONERROR);
	else
		pLoginDlg->HandleRegisMsg(packet->getMessage()[0]);
}

void UserVerifyHandler(CWnd * wnd, vyt::Packet packet)
{
	LoginDlg *pLoginDlg = dynamic_cast<LoginDlg*>(wnd);
	if (nullptr == pLoginDlg) return;
	if (packet->getMessageSize() != 1)
		MessageBox(wnd->GetSafeHwnd(), _T("非法的消息"), _T("错误"), MB_ICONERROR);
	else
		pLoginDlg->HandleVerifyMsg(packet->getMessage()[0]);
}

void LobbyJoinHandler(CWnd *wnd, vyt::Packet packet)
{
}

void LobbyChatHandler(CWnd *wnd, vyt::Packet packet)
{
}

void FriendOnlineHandler(CWnd *wnd, vyt::Packet packet)
{
}

void FriendOfflineHandler(CWnd *wnd, vyt::Packet packet)
{
}

void FriendAddHandler(CWnd *wnd, vyt::Packet packet)
{
}

void FriendDelHandler(CWnd *wnd, vyt::Packet packet)
{
}

void FriendChatHandler(CWnd *wnd, vyt::Packet packet)
{
}

void FriendFileHandler(CWnd *wnd, vyt::Packet packet)
{
}

void FriendVideoHandler(CWnd *wnd, vyt::Packet packet)
{
	CString test1, test2;
	bool isOnline;
	int number;
	float fpi;
	double pi;
	packet->Decode("bifdss", &isOnline, &number, &fpi, &pi, &test1, &test2);
}
