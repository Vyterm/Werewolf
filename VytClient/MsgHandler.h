#ifndef MSG_HANDLER_H_INCLUDED
#define MSG_HANDLER_H_INCLUDED

#include <afxwin.h>
#include <map>
#include <functional>
#include "Commands.h"
#include "Packet.h"

template<typename TSubCommand>
int ToMsgID(OpCommand opCommand, TSubCommand subCommand)
{
	return WM_USER + 1 + (int(opCommand) << 16) + int(subCommand);
}

using Handler = std::function<void(CWnd *wnd, vyt::Packet)>;
void KernelVersionHandler(CWnd *wnd, vyt::Packet packet);
void UserLoginHandler(CWnd *wnd, vyt::Packet packet);
void UserRegisHandler(CWnd *wnd, vyt::Packet packet);
void LobbyJoinHandler(CWnd *wnd, vyt::Packet packet);
void LobbyChatHandler(CWnd *wnd, vyt::Packet packet);
void FriendOnlineHandler(CWnd *wnd, vyt::Packet packet);
void FriendOfflineHandler(CWnd *wnd, vyt::Packet packet);
void FriendAddHandler(CWnd *wnd, vyt::Packet packet);
void FriendDelHandler(CWnd *wnd, vyt::Packet packet);
void FriendChatHandler(CWnd *wnd, vyt::Packet packet);
void FriendFileHandler(CWnd *wnd, vyt::Packet packet);
void FriendVideoHandler(CWnd *wnd, vyt::Packet packet);

extern std::map<vyt::command, std::map<vyt::command, Handler>> handlers;

inline void LogInvalidPacket(const vyt::Packet &packet)
{
	CString error;
	error.Format(_T("δ����Ĳ����룬��������Ϊ %d ����������Ϊ %d ������Ϣ�������ݽ�������\n"), packet->getOpCommand(), packet->getSubCommand());
	OutputDebugString(error);
}

inline void HandlePacket(CWnd *wnd, const vyt::Packet &packet)
{
	if (handlers.end() != handlers.find(packet->getOpCommand()))
	{
		auto subHandlers = handlers[packet->getOpCommand()];
		if (subHandlers.end() != subHandlers.find(packet->getSubCommand()))
			subHandlers[packet->getSubCommand()](wnd, packet);
		else
			LogInvalidPacket(packet);
	}
	else
		LogInvalidPacket(packet);
}

#endif