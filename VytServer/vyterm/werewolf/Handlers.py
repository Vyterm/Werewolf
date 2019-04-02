from vyterm.werewolf.kernel import *
from vyterm.conversion import *
from vyterm.api3_rd import aliyunapi
from threading import Lock
from random import randint
from functools import reduce
from datetime import *

_Client2Player = {}
_Player2Client = {}
_handleLock = Lock()


def player_to_namebytes(player):
    player = Caches().get_user_name(player)
    return string_to_bytes(player) if player else bytes()


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
            _Client2Player[client] = phone
            _Player2Client[phone] = client
            Caches().append_user(phone, self.verifyphones[phone][1])
            client.send(OpCommand.User.value, UserCommand.Verify.value, struct.pack('B', 0))

    @staticmethod
    def getname(client, packet):
        assert len(packet) == 0
        if client not in _Client2Player:
            return
        client.send(OpCommand.User.value, UserCommand.GetName.value, player_to_namebytes(_Client2Player[client]))
        pass

    def setname(self, client, packet):
        if client not in _Client2Player:
            return
        oldname = Caches().get_user_name(_Client2Player[client])
        newname = bytes_to_string(packet)
        if newname.isdigit() and len(newname) == 11 or not Caches.username_isvalid(newname):
            client.send(OpCommand.User.value, UserCommand.SetName.value, struct.pack('B', 1))
        else:
            Caches().update_user_name(oldname, newname)
            LobbyHandler().brocast_name_changed(_Client2Player[client], oldname, newname)
            client.send(OpCommand.User.value, UserCommand.SetName.value, struct.pack('B', 0))
            self.getname(client, bytes())
        pass

    @staticmethod
    def getinfo(client, packet):
        assert len(packet) == 0
        if client not in _Client2Player:
            return
        client.send(OpCommand.User.value, UserCommand.GetInfo.value,
                    struct.pack('iiii', *Caches().get_user_info(_Client2Player[client])))
        pass

    @property
    def handlers(self):
        return {
            UserCommand.Login.value: self.login,
            UserCommand.Regis.value: self.regis,
            UserCommand.Verify.value: self.verify,
            UserCommand.GetName.value: self.getname,
            UserCommand.SetName.value: self.setname,
            UserCommand.GetInfo.value: self.getinfo,
        }

    def logout(self, client):
        if client in _Client2Player:
            _Player2Client.pop(_Client2Player[client])
            _Client2Player.pop(client)


@get_instance
@singleton
class LobbyHandler(Handler):
    def brocast_name_changed(self, player, oldname, newname):
        for lobby in self.Lobbys.values():
            if player in lobby:
                for p in lobby:
                    _Player2Client[p].send(OpCommand.Lobby.value, LobbyCommand.Rename.value,
                                           strings_to_bytes(oldname, newname))

    def join(self, client, packet):
        if client not in _Client2Player:
            return
        lobby_id, = struct.unpack('i', packet)
        if lobby_id not in self.Lobbys:
            self.Lobbys[lobby_id] = []
        player = _Client2Player[client]
        if player in self.Lobbys[lobby_id]:
            return
        for p in self.Lobbys[lobby_id]:
            _Player2Client[p].send(OpCommand.Lobby.value, LobbyCommand.Join.value, player_to_namebytes(player))
            client.send(OpCommand.Lobby.value, LobbyCommand.Join.value, player_to_namebytes(p))
        client.send(OpCommand.Lobby.value, LobbyCommand.Join.value, player_to_namebytes(player))
        self.Lobbys[lobby_id].append(player)
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
            _Player2Client[p].send(OpCommand.Lobby.value, LobbyCommand.Leave.value, player_to_namebytes(player))
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
                                   player_to_namebytes(player) + string_to_bytes(chat))
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
        for lid, lobby in self.Lobbys.items():
            if player in lobby:
                self.leave(client, struct.pack('i', lid))
        pass


class FriendHandler(Handler):
    def __init__(self):
        self.access_connections = {}

    def create_connection(self, sp, tp):
        if sp not in self.access_connections:
            self.access_connections[sp] = {}
        if tp not in self.access_connections:
            self.access_connections[tp] = {}
        self.access_connections[sp][Caches().get_user_name(tp)] = tp
        self.access_connections[tp][Caches().get_user_name(sp)] = sp

    def delete_connection(self, player):
        if player in self.access_connections:
            self.access_connections.pop(player)
        player = Caches().get_user_name(player)
        for connection in self.access_connections:
            if player in connection:
                connection.pop(player)

    def get_connection(self, sp, tpn):
        return self.access_connections[sp][tpn] \
            if sp in self.access_connections and tpn in self.access_connections[sp] else None

    def online(self, client, packet):
        pass

    @staticmethod
    def add_friend(client, packet):
        if client not in _Client2Player:
            return
        sender = _Client2Player[client]
        user_tag = bytes_to_string(packet)
        is_direct = user_tag.isdigit() and len(user_tag) == 11
        if is_direct:
            player = Caches().search_user_by_phone(user_tag)
        else:
            player = Caches().search_user_by_name(user_tag)
        if player not in _Player2Client:
            client.send(OpCommand.Friend.value, FriendCommand.Add.value, struct.pack('B', 1))
        elif sender == player:
            client.send(OpCommand.Friend.value, FriendCommand.Add.value, struct.pack('B', 2))
        elif Caches().is_friend(sender, player):
            client.send(OpCommand.Friend.value, FriendCommand.Add.value, struct.pack('B', 3))
        else:
            Caches().create_connection(sender, player)
            _Player2Client[player].send(OpCommand.Friend.value, FriendCommand.Add.value,
                                        struct.pack('B', 0) + player_to_namebytes(sender))
            client.send(OpCommand.Friend.value, FriendCommand.Add.value,
                        struct.pack('B', 0) + player_to_namebytes(player))
        pass

    def del_friend(self, client, packet):
        if client not in _Client2Player:
            return
        player = _Client2Player[client]
        name = bytes_to_string(packet)
        if player in self.access_connections and name in self.access_connections[player]:
            target = self.access_connections[player][name]
            self.access_connections[player].pop(name)
            self.access_connections[target].pop(Caches().get_user_name(player))
        else:
            target = Caches().search_user_by_name(name)
            if target is None:
                return
        if Caches().is_friend(player, target):
            Caches().delete_connection(player, target)
            client.send(OpCommand.Friend.value, FriendCommand.Del.value, player_to_namebytes(target))
            if target in _Player2Client:
                _Player2Client[target].send(OpCommand.Friend.value, FriendCommand.Del.value,
                                            player_to_namebytes(player))
        pass

    def private_chat(self, client, packet):
        if client not in _Client2Player:
            return
        player = _Client2Player[client]
        name, chat = bytes_to_strings(packet)
        target = self.get_connection(player, name)
        if target is None:
            return
        _Player2Client[target].send(OpCommand.Friend.value, FriendCommand.Chat.value,
                                    player_to_namebytes(player) + string_to_bytes(chat))

    def file_trans(self, client, packet):
        pass

    def video_trans(self, client, packet):
        print(id(self), packet)
        client.send(OpCommand.Friend.value, FriendCommand.Video.value,
                    struct.pack('=Bifd', True, 123456, 3.14, 3.141597)
                    + strings_to_bytes("Test", "Video"))
        pass

    @staticmethod
    def friend_list(client, packet):
        assert len(packet) == 0
        friends = Caches().friend_names(_Client2Player[client])
        count = struct.pack('i', len(friends))
        friends = [player_to_namebytes(friend) for friend in friends]
        friends = reduce(lambda l, r: l + r, [bytes()] + friends)
        client.send(OpCommand.Friend.value, FriendCommand.List.value,
                    count + friends)
        pass

    def access_chat(self, client, packet):
        if client not in _Client2Player:
            return
        player = _Client2Player[client]
        tag, = struct.unpack('i', packet[:4])
        name = bytes_to_string(packet[4:])
        target = Caches().search_user_by_name(name)
        if not target:
            client.send(OpCommand.Friend.value, FriendCommand.Access.value, struct.pack('B', 1))
        elif not Caches().is_friend(player, target):
            client.send(OpCommand.Friend.value, FriendCommand.Access.value, struct.pack('B', 2))
        elif target not in _Player2Client:
            client.send(OpCommand.Friend.value, FriendCommand.Access.value, struct.pack('B', 3))
        else:
            self.create_connection(player, target)
            client.send(OpCommand.Friend.value, FriendCommand.Access.value, struct.pack('Bi', 0, tag))
            _Player2Client[target].send(OpCommand.Friend.value, FriendCommand.Access.value, struct.pack('Bi', 0, tag))
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
            FriendCommand.List.value: self.friend_list,
            FriendCommand.Access.value: self.access_chat,
        }

    def logout(self, client):
        if client not in _Client2Player:
            return
        player = _Client2Player[client]
        self.delete_connection(player)
        for friend in Caches().friend_names(player):
            if friend in _Player2Client:
                _Player2Client[friend].send(OpCommand.Friend.value, FriendCommand.Offline.value,
                                            player_to_namebytes(player))
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
