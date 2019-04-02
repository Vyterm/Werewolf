from vyterm.conversion import *
from vyterm.sqladapter import SqlAdapter
from vyterm.utils import singleton, get_instance
from vyterm.cryptography import *
from vyterm.api3_rd import aliyunapi
from abc import abstractmethod
from enum import Enum
from threading import Lock
from random import randint
from datetime import *


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
    Verify = 2


class LobbyCommand(Enum):
    Join = 0
    Chat = 1
    Leave = 2


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
        self.mysql = SqlAdapter(db="werewolf", password="1JXahbu230x1Zehim88t")
        self.dbPlayers = {}
        self.friends = {}
        for player in self.mysql.select("select `user_number`, `user_name`, `user_pass` from `user_table`;"):
            phone, name, md5_pass = player
            self.dbPlayers[str(phone)] = md5_pass
            self.friends[str(phone)] = []

        for connection in self.mysql.select("select * from `friend_table`;"):
            uid, fuid = connection
            uphone, = self.mysql.select("select `user_number` from `user_table` where `user_id` = %d" % uid)
            fphone, = self.mysql.select("select `user_number` from `user_table` where `user_id` = %d" % fuid)
            if fphone not in self.friends[uphone]:
                self.friends[uphone].append(fphone)
            if uphone not in self.friends[fphone]:
                self.friends[fphone].append(uphone)

    def contains(self, phone: str):
        return phone in self.dbPlayers

    def append_user(self, phone: str, password: str):
        if phone in self.dbPlayers:
            return False
        self.dbPlayers[phone] = md5str(password)
        assert self.mysql.execute("insert into `user_table`(user_number, user_name, user_pass) values "
                                  "(%s, '%s', '%s')" % (phone, phone, self.dbPlayers[phone]))
        return True

    def remove_user(self, phone: str):
        if phone not in self.dbPlayers:
            return False
        self.dbPlayers.pop(phone)
        assert self.mysql.execute("delete from `user_table` where `user_number`=%s" % (phone,))
        return True

    def match(self, phone: str, password: str):
        password = md5str(password)
        return self.dbPlayers[phone] == password

    def friend_names(self, userphone):
        return self.friends[userphone]

    def get_user_name(self, userphone):
        return self.mysql.select("select `user_name` from `user_table` where `user_number` = %s" % userphone)[0][0]

    def get_user_info(self, userphone):
        count_row, = self.mysql.select(
            "select `win_count`, `lose_count`, `run_count`, `achievement` from `user_table`"
            " where `user_id` = (select `user_id` from `user_table` where `user_number` = %d);" % (userphone,))
        win, lose, run, achieve = count_row
        return win, lose, run, achieve

    def search_user_by_phone(self, userphone):
        if userphone in self.dbPlayers:
            return userphone

    def search_user_by_name(self, username):
        userphone = self.mysql.select("select `user_number` from `user_table` where `user_name` = '%s'" % username)
        return userphone[0][0] if userphone else None


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
    def __init__(self):
        self.verifyphones = {}

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

    def regis(self, client, packet):
        # 解包，获取用户名及密码
        username, password = bytes_to_strings(packet)
        # 判断是否为合法的用户名
        if not username.isdigit() or len(username) != 11:
            client.send(OpCommand.User.value, UserCommand.Regis.value, struct.pack('B', 2))
        # 判断该用户名是否已注册
        elif Caches.get().contains(username):
            client.send(OpCommand.User.value, UserCommand.Regis.value, struct.pack('B', 1))
        # 若没有注册则向客户端发送注册成功的消息
        else:
            code = randint(100000, 999999)
            # MARK1：保存用户名，验证码，密码，创建时间
            self.verifyphones[username] = (str(code), password, datetime.now())
            # 发送验证码短信
            aliyunapi.AliyunAdapter().SendSms(username, 'Vyterm', 'SMS_150577820', code)
            # 给客户端发送验证码短信已发送的消息
            client.send(OpCommand.User.value, UserCommand.Regis.value, struct.pack('B', 0))

    def verify(self, client, packet):
        # 将超过五分钟的验证码从列表中删除 #MARK1：保存的验证码第三项为创建时间，即t[2]
        invalidphones = [p for p, t in self.verifyphones.items() if t[2] - datetime.now() > timedelta(minutes=5)]
        for phone in invalidphones:
            self.verifyphones.pop(phone)
        # 解包获取要验证的手机号与验证码
        phone, code = bytes_to_strings(packet)
        # 判断手机号及验证码是否有效
        if not phone.isdigit() or not code.isdigit():
            client.send(OpCommand.User.value, UserCommand.Verify.value, struct.pack('B', 1))
        # 如果手机号不存在说明验证码 #MARK1：保存的验证码第一项为用户名，即t[1]
        elif phone not in self.verifyphones or self.verifyphones[phone][0] != code:
            client.send(OpCommand.User.value, UserCommand.Verify.value, struct.pack('B', 1))
        # 添加新用户并给客户端发送注册成功的消息 #MARK1：保存的验证码第二项为用户名，即t[1]
        else:
            Caches().append_user(phone, self.verifyphones[phone][1])
            client.send(OpCommand.User.value, UserCommand.Verify.value, struct.pack('B', 0))

    @property
    def handlers(self):
        return {
            UserCommand.Login.value: self.login,
            UserCommand.Regis.value: self.regis,
            UserCommand.Verify.value: self.verify
        }

    def logout(self, client):
        if client in _Client2Player:
            _Player2Client.pop(_Client2Player[client])
            _Client2Player.pop(client)


class LobbyHandler(Handler):
    def join(self, client, packet):
        if client not in _Client2Player:
            return
        lobby_id, = struct.unpack('i', packet)
        if lobby_id not in self.Lobbys:
            self.Lobbys[lobby_id] = []
        player = _Client2Player[client]
        if player not in self.Lobbys[lobby_id]:
            for p in self.Lobbys[lobby_id]:
                _Player2Client[p].send(OpCommand.Lobby.value, LobbyCommand.Join.value, string_to_bytes(player))
            self.Lobbys[lobby_id].append(player)
        else:
            client.send(OpCommand.Lobby.value, LobbyCommand.Join.value, struct.pack('B', 1))
        pass

    def leave(self, client, packet):
        if client not in _Client2Player:
            return
        lobby_id, = struct.unpack('i', packet)
        if lobby_id not in self.Lobbys:
            return
        player = _Client2Player[client]
        if player not in self.Lobbys[lobby_id]:
            return
        self.Lobbys[lobby_id].remove(player)
        for p in self.Lobbys[lobby_id]:
            _Player2Client[p].send(OpCommand.Lobby.value, LobbyCommand.Leave.value, string_to_bytes(player))
        pass

    def lobby_chat(self, client, packet):
        if client not in _Client2Player:
            return
        player = _Client2Player[client]
        lobby_id, = struct.unpack('i', packet[:4])
        if lobby_id not in self.Lobbys:
            return
        if player not in self.Lobbys[lobby_id]:
            return
        chat = bytes_to_string(packet[4:])
        for p in self.Lobbys[lobby_id]:
            _Player2Client[p].send(OpCommand.Lobby.value, LobbyCommand.Chat.value, struct.pack('i', lobby_id) +
                                   strings_to_bytes(player, chat))
        pass

    def __init__(self):
        self.Lobbys = {}

    @property
    def handlers(self):
        return {
            LobbyCommand.Join.value: self.join,
            LobbyCommand.Leave.value: self.leave,
            LobbyCommand.Chat.value: self.lobby_chat,
        }

    def logout(self, client):
        if client not in _Client2Player:
            return
        player = _Client2Player[client]
        for lobby in self.Lobbys.values():
            if player in lobby:
                lobby.remove(player)
        pass


class FriendHandler(Handler):
    def online(self, client, packet):
        pass

    @staticmethod
    def add_friend(client, packet):
        if client not in _Client2Player:
            return
        sender = _Client2Player[client]
        is_direct = struct.unpack('B', packet[:1])
        user_tag = string_to_bytes(packet[1:])
        if is_direct:
            player = Caches().search_user_by_phone(user_tag)
        else:
            player = Caches().search_user_by_name(user_tag)
        if player in _Player2Client:
            _Player2Client[player].send(OpCommand.Friend.value, FriendCommand.Add.value,
                                        struct.pack('B', 0) + string_to_bytes(sender))
            client.send(OpCommand.Friend.value, FriendCommand.Add.value, struct.pack('B', 0) + string_to_bytes(player))
        else:
            client.send(OpCommand.Friend.value, FriendCommand.Add.value, struct.pack('B', 1))
        pass

    def del_friend(self, client, packet):
        pass

    def private_chat(self, client, packet):
        pass

    def file_trans(self, client, packet):
        pass

    def video_trans(self, client, packet):
        print(id(self), packet)
        client.send(OpCommand.Friend.value, FriendCommand.Video.value,
                    struct.pack('=Bifd', True, 123456, 3.14, 3.141597)
                    + strings_to_bytes("Test", "Video"))
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
    OpCommand.Friend.value: FriendHandler(),
    OpCommand.Lobby.value: LobbyHandler(),
    OpCommand.User.value: UserHandler(),
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
    # print(type(Caches.get))
    # assert id(Caches.get()) == id(Caches())
    assert id(Caches.get()) == id(Caches.get())
    # print(Caches.get().get_user_info(18986251734))
    # assert not Caches().remove_user(str(17708807700))
    # assert Caches().append_user(str(17708807700), 'Crazy')
    # assert not Caches().append_user(str(17708807700), 'Crazy')
    # assert Caches().match(str(17708807700), 'Crazy')
    # assert Caches().remove_user(str(17708807700))
    # print(Caches().search_user_by_name('18986251734'))
    Caches().friend_names('18986251734')
    print("All tests of werewolf.Handlers passed.")
else:
    Caches.get()
