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
};


enum class LobbyCommand
{
	Join = 0,
	Chat = 1,
	Leave = 2,
	Rename = 3,
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
};

#endif