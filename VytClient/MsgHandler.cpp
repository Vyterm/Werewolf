#include "MsgHandler.h"
#include "LoginDlg.h"

std::map<vyt::command, std::map<vyt::command, Handler>> handlers = {
	{ vyt::command(OpCommand::Kernel), {
		{ vyt::command(KernelCommand::Version), &KernelVersionHandler },
	} },
	{ vyt::command(OpCommand::User), {
		{ vyt::command(UserCommand::Login), &UserLoginHandler },
		{ vyt::command(UserCommand::Regis), &UserRegisHandler },
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

void KernelVersionHandler(CWnd *wnd, vyt::pByte msg, vyt::vytsize msgSize)
{
}

void UserLoginHandler(CWnd *wnd, vyt::pByte msg, vyt::vytsize msgSize)
{
	LoginDlg *pLoginDlg = dynamic_cast<LoginDlg*>(wnd);
	if (nullptr == pLoginDlg) return;
	if (msgSize != 1)
		MessageBox(wnd->GetSafeHwnd(), _T("非法的消息"), _T("错误"), MB_ICONERROR);
	else
		pLoginDlg->HandleLoginMsg(msg[0]);
}

void UserRegisHandler(CWnd *wnd, vyt::pByte msg, vyt::vytsize msgSize)
{
	LoginDlg *pLoginDlg = dynamic_cast<LoginDlg*>(wnd);
	if (nullptr == pLoginDlg) return;
	if (msgSize != 1)
		MessageBox(wnd->GetSafeHwnd(), _T("非法的消息"), _T("错误"), MB_ICONERROR);
	else
		pLoginDlg->HandleRegisMsg(msg[0]);
}

void LobbyJoinHandler(CWnd *wnd, vyt::pByte msg, vyt::vytsize msgSize)
{
}

void LobbyChatHandler(CWnd *wnd, vyt::pByte msg, vyt::vytsize msgSize)
{
}

void FriendOnlineHandler(CWnd *wnd, vyt::pByte msg, vyt::vytsize msgSize)
{
}

void FriendOfflineHandler(CWnd *wnd, vyt::pByte msg, vyt::vytsize msgSize)
{
}

void FriendAddHandler(CWnd *wnd, vyt::pByte msg, vyt::vytsize msgSize)
{
}

void FriendDelHandler(CWnd *wnd, vyt::pByte msg, vyt::vytsize msgSize)
{
}

void FriendChatHandler(CWnd *wnd, vyt::pByte msg, vyt::vytsize msgSize)
{
}

void FriendFileHandler(CWnd *wnd, vyt::pByte msg, vyt::vytsize msgSize)
{
}

void FriendVideoHandler(CWnd *wnd, vyt::pByte msg, vyt::vytsize msgSize)
{

}
