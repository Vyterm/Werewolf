#ifndef COMMANDS_H_INCLUDED
#define COMMANDS_H_INCLUDED

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
	Verify = 2,
	GetName = 3,
	SetName = 4,
	GetInfo = 5,
	Rename = 6,
};


enum class LobbyCommand
{
	Refresh = 0,
	Create = 1,
	Join = 2,
	Leave = 3,
	Chat = 4,
	Verify = 5,
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
	List = 7,
	Access = 8,
};

#endif