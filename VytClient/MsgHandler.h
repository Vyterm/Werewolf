#ifndef MSG_HANDLER_H_INCLUDED
#define MSG_HANDLER_H_INCLUDED

#include <afxwin.h>
#include <map>
#include <functional>
#include "Packet.h"

enum class OpCommand
{
	Kernel = 0, 
	User = 1, 
	Lobby = 2, 
	Friend = 3, 
};


enum class KernelCommand
{
	Version = 0, 
};


enum class UserCommand
{
	Login = 0, 
	Regis = 1, 
};


enum class LobbyCommand
{
	Join = 0, 
	Chat = 1, 
};


enum class FriendCommand
{
	Online = 0, 
	Offline = 1, 
	Add = 2, 
	Del = 3, 
	Chat = 4, 
	File = 5, 
	Video = 6, 
};

template<typename TSubCommand>
int ToMsgID(OpCommand opCommand, TSubCommand subCommand)
{
	return WM_USER + 1 + (int(opCommand) << 16) + int(subCommand);
}

using Handler = std::function<void(CWnd *wnd, vyt::pByte, vyt::vytsize)>;
void KernelVersionHandler(CWnd *wnd, vyt::pByte msg, vyt::vytsize msgSize);
void UserLoginHandler(CWnd *wnd, vyt::pByte msg, vyt::vytsize msgSize);
void UserRegisHandler(CWnd *wnd, vyt::pByte msg, vyt::vytsize msgSize);
void LobbyJoinHandler(CWnd *wnd, vyt::pByte msg, vyt::vytsize msgSize);
void LobbyChatHandler(CWnd *wnd, vyt::pByte msg, vyt::vytsize msgSize);
void FriendOnlineHandler(CWnd *wnd, vyt::pByte msg, vyt::vytsize msgSize);
void FriendOfflineHandler(CWnd *wnd, vyt::pByte msg, vyt::vytsize msgSize);
void FriendAddHandler(CWnd *wnd, vyt::pByte msg, vyt::vytsize msgSize);
void FriendDelHandler(CWnd *wnd, vyt::pByte msg, vyt::vytsize msgSize);
void FriendChatHandler(CWnd *wnd, vyt::pByte msg, vyt::vytsize msgSize);
void FriendFileHandler(CWnd *wnd, vyt::pByte msg, vyt::vytsize msgSize);
void FriendVideoHandler(CWnd *wnd, vyt::pByte msg, vyt::vytsize msgSize);

extern std::map<vyt::command, std::map<vyt::command, Handler>> handlers;

inline void LogInvalidPacket(const vyt::Packet &packet)
{
	CString error;
	error.Format(_T("未定义的操作码，主操作码为 %d ，副操作码为 %d ，本消息包的内容将被抛弃\n"), packet->getOpCommand(), packet->getSubCommand());
	OutputDebugString(error);
}

inline void HandlePacket(CWnd *wnd, const vyt::Packet &packet)
{
	if (handlers.end() != handlers.find(packet->getOpCommand()))
	{
		auto subHandlers = handlers[packet->getOpCommand()];
		if (subHandlers.end() != subHandlers.find(packet->getSubCommand()))
			subHandlers[packet->getSubCommand()](wnd, packet->getMessage(), packet->getMessageSize());
		else
			LogInvalidPacket(packet);
	}
	else
		LogInvalidPacket(packet);
}

#endif