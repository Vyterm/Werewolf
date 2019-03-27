from vyterm.conversion import *
from vyterm.sqladapter import SqlAdapter
from vyterm.utils import singleton, get_instance
from abc import abstractmethod
from enum import Enum
from threading import Lock


class OpCommand(Enum):
    Kernel = 0
    User = 1
    Lobby = 2
    Friend = 3


class KernelCommand(Enum):
    Version = 0


class UserCommand(Enum):
    Login = 0
    Regis = 1


class LobbyCommand(Enum):
    Join = 0
    Chat = 1


class FriendCommand(Enum):
    Online = 0
    Offline = 1
    Add = 2
    Del = 3
    Chat = 4
    File = 5
    Video = 6


_Client2Player = {}
_Player2Client = {}
_handleLock = Lock()


@get_instance
@singleton
class Caches(object):
    def __init__(self):
        self.mysql = SqlAdapter()
        # ToDo: load data from sql
        self.dbPlayers = {
            "Vyterm": "123456",
            "Fromex": "123456",
            "Yitoru": "123456",
        }
        self.friends = {
            "Vyterm": ["Fromex"],
            "Fromex": [],
            "Yitoru": [],
        }

    def __del__(self):
        # ToDo: push data to sql
        print("Caches release")
        pass

    def contains(self, username):
        return username in self.dbPlayers

    def match(self, username, password):
        return self.dbPlayers[username] == password

    def friend_names(self, username):
        return self.friends[username]


class Handler(object):
    @property
    @abstractmethod
    def handlers(self):
        pass

    def execute(self, client, command, packet):
        if command in self.handlers:
            self.handlers[command](client, packet)
            return True
        else:
            return False

    @abstractmethod
    def logout(self, client):
        pass


class UserHandler(Handler):
    @staticmethod
    def login(client, packet):
        # 解包，获取用户名及密码
        username, password = bytes_to_strings(packet)
        # 判断用户名是否存在
        if not Caches.get().contains(username):
            client.send(OpCommand.User.value, UserCommand.Login.value, struct.pack('B', 1))
        # 判断密码是否与用户名匹配
        elif not Caches.get().match(username, password):
            client.send(OpCommand.User.value, UserCommand.Login.value, struct.pack('B', 2))
        # 判断是否已登录
        elif username in _Player2Client:
            client.send(OpCommand.User.value, UserCommand.Login.value, struct.pack('B', 3))
        # 使玩家登录
        else:
            _Client2Player[client] = username
            _Player2Client[username] = client
            # 发送登录成功的消息
            client.send(OpCommand.User.value, UserCommand.Login.value, struct.pack('B', 0))
            # 给已在线上的好友发送上线的消息
            for friendName in Caches.get().friend_names(username):
                if friendName in _Player2Client:
                    _Player2Client[friendName].send(OpCommand.Friend.value, FriendCommand.Online.value, bytes(1))

    @staticmethod
    def regis(client, packet):
        # 解包，获取用户名及密码
        username, password = bytes_to_strings(packet)
        # 判断该用户名是否已注册
        if Caches.get().contains(username):
            client.send(OpCommand.User.value, UserCommand.Regis.value, struct.pack('B', 1))
        # 若没有注册则向客户端发送注册成功的消息
        else:
            client.send(OpCommand.User.value, UserCommand.Regis.value, struct.pack('B', 0))

    @property
    def handlers(self):
        return {
            UserCommand.Login.value: self.login,
            UserCommand.Regis.value: self.regis,
        }

    def logout(self, client):
        if client in _Client2Player:
            _Player2Client.pop(_Client2Player[client])
            _Client2Player.pop(client)


class LobbyHandler(Handler):
    def join(self, client, packet):
        pass

    def lobby_chat(self, client, packet):
        pass

    @property
    def handlers(self):
        return {
            LobbyCommand.Join.value: self.join,
            LobbyCommand.Chat.value: self.lobby_chat,
        }

    def logout(self, client):
        pass


class FriendHandler(Handler):
    def online(self, client, packet):
        pass

    def add_friend(self, client, packet):
        pass

    def del_friend(self, client, packet):
        pass

    def private_chat(self, client, packet):
        pass

    def file_trans(self, client, packet):
        pass

    def video_trans(self, client, packet):
        pass

    @property
    def handlers(self):
        return {
            FriendCommand.Online.value: self.online,
            FriendCommand.Add.value: self.add_friend,
            FriendCommand.Del.value: self.del_friend,
            FriendCommand.Chat.value: self.private_chat,
            FriendCommand.File.value: self.file_trans,
            FriendCommand.Video.value: self.video_trans,
        }

    def logout(self, client):
        pass


ActiveHandlers = {
    OpCommand.User.value: UserHandler(),
    OpCommand.Lobby.value: LobbyHandler(),
    OpCommand.Friend.value: FriendHandler(),
}


def handle_packet(client, opcmd, subcmd, packet):
    if opcmd in ActiveHandlers:
        try:
            _handleLock.acquire()
            success = ActiveHandlers[opcmd].execute(client, subcmd, packet)
            return success
        except Exception as e:
            print("Handle packet happend exception:", e)
        finally:
            _handleLock.release()
    else:
        print("Warning: The command %d-%d doesn't contains in active handlers, data maybe lost" % (opcmd, subcmd))
        return False


def handle_logout(client):
    for handler in ActiveHandlers.values():
        try:
            _handleLock.acquire()
            handler.logout(client)
        except Exception as e:
            print("Handle logout happend exception:", e)
        finally:
            _handleLock.release()


if __name__ == '__main__':
    # assert handlers[0].execute(None, 0, strings_to_bytes("Vyterm", "123456"))
    assert not handle_packet(None, -1, 0, 0)
    assert not handle_packet(None, 0, -1, 0)
    print(type(Caches.get))
    # assert id(Caches.get()) == id(Caches())
    assert id(Caches.get()) == id(Caches.get())
    print("All tests of werewolf.Handlers passed.")
else:
    Caches.get()
