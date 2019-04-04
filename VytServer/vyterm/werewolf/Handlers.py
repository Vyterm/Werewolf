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
            for friendName in Caches.get().get_friend_phones(username):
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
            namepack = strings_to_bytes(oldname, newname)
            for c in _Client2Player:
                c.send(OpCommand.User.value, UserCommand.Rename.value, namepack)
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


class RoomInfo(object):
    def __init__(self, idname: str, password: str = str(), description: str = str(), mode: int = 0):
        self.idname = idname
        self.password = password
        self.description = description
        self.mode = mode
        # 目前默认模式房间最大人数都为15，如果之后要加入自定义模式则不能再采用默认模式的默认最大人数
        assert self.mode == 0
        self.maxplayer = 15

    @property
    def has_password(self) -> bool:
        return self.password != str()


class LobbyHandler(Handler):
    def refresh(self, client, packet):
        assert len(packet) == 0
        count = 0
        for info in self.LobbyInfos.values():
            if len(self.Lobbys[info.idname]) != 0:
                count += 1
                lobbyname = string_to_bytes(info.idname)
                haspassword = struct.pack('B', info.has_password)
                creator = player_to_namebytes(self.Lobbys[info.idname][0])
                description = string_to_bytes(info.description)
                playercount = struct.pack('iii', len(self.Lobbys[info.idname]), info.maxplayer, info.mode)
                packet += lobbyname + haspassword + creator + description + playercount
        packet = struct.pack('i', count) + packet
        client.send(OpCommand.Lobby.value, LobbyCommand.Refresh.value, packet)
        pass

    def create(self, client, packet):
        if client not in _Client2Player:
            return
        lobby_id, size = bytes_to_string_with_size(packet)
        if lobby_id in self.Lobbys:
            client.send(OpCommand.Lobby.value, LobbyCommand.Create.value, struct.pack('B', 1))
        else:
            password = bytes_to_string(packet[size:]) if len(packet) != size else str()
            self.Lobbys[lobby_id] = []
            self.LobbyInfos[lobby_id] = RoomInfo(idname=lobby_id, password=password)
            client.send(OpCommand.Lobby.value, LobbyCommand.Create.value,
                        struct.pack('B', 0) + string_to_bytes(lobby_id))

    def join(self, client, packet):
        if client not in _Client2Player:
            return
        lobby_id, size = bytes_to_string_with_size(packet)
        if lobby_id not in self.Lobbys:
            return
        player = _Client2Player[client]
        if player in self.Lobbys[lobby_id]:
            return
        lidpack = string_to_bytes(lobby_id)
        playerpack = lidpack + player_to_namebytes(player)
        for p in self.Lobbys[lobby_id]:
            _Player2Client[p].send(OpCommand.Lobby.value, LobbyCommand.Join.value, playerpack)
            client.send(OpCommand.Lobby.value, LobbyCommand.Join.value, lidpack + player_to_namebytes(p))
        client.send(OpCommand.Lobby.value, LobbyCommand.Join.value, playerpack)
        self.Lobbys[lobby_id].append(player)
        pass

    def leave(self, client, packet):
        if client not in _Client2Player:
            return
        lobby_id = bytes_to_string(packet)
        if lobby_id not in self.Lobbys:
            return
        player = _Client2Player[client]
        if player not in self.Lobbys[lobby_id]:
            return
        playerpack = string_to_bytes(lobby_id) + player_to_namebytes(player)
        self.Lobbys[lobby_id].remove(player)
        for p in self.Lobbys[lobby_id]:
            _Player2Client[p].send(OpCommand.Lobby.value, LobbyCommand.Leave.value, playerpack)
        if len(self.Lobbys[lobby_id]) == 0 and lobby_id in self.LobbyInfos:
            self.Lobbys.pop(lobby_id)
            self.LobbyInfos.pop(lobby_id)

    def lobby_chat(self, client, packet):
        if client not in _Client2Player:
            return
        player = _Client2Player[client]
        lobby_id, chat = bytes_to_strings(packet)
        if lobby_id not in self.Lobbys:
            return
        if player not in self.Lobbys[lobby_id]:
            return
        for p in self.Lobbys[lobby_id]:
            _Player2Client[p].send(OpCommand.Lobby.value, LobbyCommand.Chat.value, string_to_bytes(lobby_id) +
                                   player_to_namebytes(player) + string_to_bytes(chat))
        pass

    def __init__(self):
        self.Lobbys = {str(): []}
        self.LobbyInfos = {}

    @property
    def handlers(self):
        return {
            LobbyCommand.Refresh.value: self.refresh,
            LobbyCommand.Create.value: self.create,
            LobbyCommand.Join.value: self.join,
            LobbyCommand.Leave.value: self.leave,
            LobbyCommand.Chat.value: self.lobby_chat,
        }

    def logout(self, client):
        if client not in _Client2Player:
            return
        player = _Client2Player[client]
        lobbyids = [lid for lid in self.Lobbys]
        for lid in lobbyids:
            if player in self.Lobbys[lid]:
                self.leave(client, string_to_bytes(lid))
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
        # 判断发起请求的玩家是否在线
        if client not in _Client2Player:
            return
        sender = _Client2Player[client]
        # 解包获取要添加的好友标识
        user_tag = bytes_to_string(packet)
        # 搜索缓存中的好友信息
        is_direct = user_tag.isdigit() and len(user_tag) == 11
        if is_direct:
            player = Caches().search_user_by_phone(user_tag)
        else:
            player = Caches().search_user_by_name(user_tag)
        # 如果要添加为好友的玩家不在线，添加失败
        if player not in _Player2Client:
            client.send(OpCommand.Friend.value, FriendCommand.Add.value, struct.pack('B', 1))
        # 不能添加自身为好友
        elif sender == player:
            client.send(OpCommand.Friend.value, FriendCommand.Add.value, struct.pack('B', 2))
        # 不能重复添加对方为好友
        elif Caches().is_friend(sender, player):
            client.send(OpCommand.Friend.value, FriendCommand.Add.value, struct.pack('B', 3))
        # 成功添加好友
        else:
            # 在缓存中建立好友连接
            Caches().create_connection(sender, player)
            # 给双方发送添加好友成功的消息
            _Player2Client[player].send(OpCommand.Friend.value, FriendCommand.Add.value,
                                        struct.pack('B', 0) + player_to_namebytes(sender))
            client.send(OpCommand.Friend.value, FriendCommand.Add.value,
                        struct.pack('B', 0) + player_to_namebytes(player))
        pass

    def del_friend(self, client, packet):
        # 判断发起请求的玩家是否在线
        if client not in _Client2Player:
            return
        player = _Client2Player[client]
        # 解包获取要删除的好友名
        name = bytes_to_string(packet)
        # 如果已经建立连接则从连接中获取要删除的好友号码并断开连接
        if player in self.access_connections and name in self.access_connections[player]:
            # 获取要删除的好友号码
            target = self.access_connections[player][name]
            # 向客户端发送断开连接的消息
            _Player2Client[target].send(OpCommand.Friend.value, FriendCommand.Offline.value,
                                        player_to_namebytes(player))
            client.send(OpCommand.Friend.value, FriendCommand.Offline.value, name)
            # 删除好友间的连接
            self.access_connections[player].pop(name)
            self.access_connections[target].pop(Caches().get_user_name(player))
        # 否则从缓存中读取要删除的好友号码
        else:
            target = Caches().search_user_by_name(name)
            if target is None:
                return
        # 判断自身与其是否为好友
        if Caches().is_friend(player, target):
            # 删除数据库及缓存中的好友关系
            Caches().delete_connection(player, target)
            # 向客户端发送删除好友成功的消息
            client.send(OpCommand.Friend.value, FriendCommand.Del.value, player_to_namebytes(target))
            # 如果好友在线，则向好友客户端也发送删除好友的消息
            if target in _Player2Client:
                _Player2Client[target].send(OpCommand.Friend.value, FriendCommand.Del.value,
                                            player_to_namebytes(player))
        pass

    def private_chat(self, client, packet):
        # 判断发起请求的玩家是否在线
        if client not in _Client2Player:
            return
        player = _Client2Player[client]
        # 解包获取要发给的好友名及私聊消息
        name, chat = bytes_to_strings(packet)
        # 判断是否有发送的权限
        target = self.get_connection(player, name)
        if target is None:
            return
        # 给目标好友发送消息，客户端会自身完成私聊信息的刷新，因此无需给发送者发私聊消息
        _Player2Client[target].send(OpCommand.Friend.value, FriendCommand.Chat.value,
                                    player_to_namebytes(player) + string_to_bytes(chat))

    def direct_verify_trans(self, client, packet):
        if client not in _Client2Player:
            return
        player = _Client2Player[client]
        name, namesize = bytes_to_string_with_size(packet)
        target = self.get_connection(player, name)
        if target is None:
            return
        _Player2Client[target].send(OpCommand.Friend.value, FriendCommand.File.value,
                                    player_to_namebytes(player) + packet[namesize:])

    def file_trans(self, client, packet):
        self.direct_verify_trans(client, packet)

    def video_trans(self, client, packet):
        self.direct_verify_trans(client, packet)

    @staticmethod
    def friend_list(client, packet):
        assert len(packet) == 0
        friends = Caches().get_friend_phones(_Client2Player[client])
        count = struct.pack('i', len(friends))
        friends = [player_to_namebytes(friend) for friend in friends]
        friends = reduce(lambda l, r: l + r, [bytes()] + friends)
        client.send(OpCommand.Friend.value, FriendCommand.List.value,
                    count + friends)
        pass

    def access_chat(self, client, packet):
        # 判断发起请求的玩家是否在线
        if client not in _Client2Player:
            return
        player = _Client2Player[client]
        # 获取客户端的请求标识信息(初始设计为对私聊文件及视频分配不同的权限，目前都是全权限，因此此参数作废)
        tag, = struct.unpack('i', packet[:4])
        assert tag == 0
        # 解包获取要发送的好友名
        name = bytes_to_string(packet[4:])
        # 在缓存中搜索要发送的好友号码
        target = Caches().search_user_by_name(name)
        # 如果没有找到，则请求发送权限失败
        if not target:
            client.send(OpCommand.Friend.value, FriendCommand.Access.value, struct.pack('B', 1))
        # 如果与目标玩家不是好友，则请求发送权限失败
        elif not Caches().is_friend(player, target):
            client.send(OpCommand.Friend.value, FriendCommand.Access.value, struct.pack('B', 2))
        # 如果好友未在线，则请求发送权限失败
        elif target not in _Player2Client:
            client.send(OpCommand.Friend.value, FriendCommand.Access.value, struct.pack('B', 3))
        # 成功建立发送权限
        else:
            # 在自身的缓存中建立连接
            self.create_connection(player, target)
            # 给双方客户端发送建立聊天配置成功的消息
            client.send(OpCommand.Friend.value, FriendCommand.Access.value,
                        struct.pack('B', 0) + string_to_bytes(name))
            _Player2Client[target].send(OpCommand.Friend.value, FriendCommand.Access.value,
                                        struct.pack('B', 0) + player_to_namebytes(player))
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
        for friend in Caches().get_friend_phones(player):
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
    print("All tests of werewolf.Handlers passed.")
else:
    Caches.get()
