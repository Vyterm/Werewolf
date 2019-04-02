from abc import abstractmethod
from enum import Enum
from vyterm.sqladapter import SqlAdapter
from vyterm.utils import singleton, get_instance
from vyterm.cryptography import md5str


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
    GetName = 3
    SetName = 4
    GetInfo = 5


class LobbyCommand(Enum):
    Join = 0
    Chat = 1
    Leave = 2
    Rename = 3


class FriendCommand(Enum):
    Online = 0
    Offline = 1
    Add = 2
    Del = 3
    Chat = 4
    File = 5
    Video = 6
    List = 7


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

    def friend_names(self, userphone: str):
        return self.friends[userphone]

    def get_user_name(self, userphone: str):
        return self.mysql.select("select `user_name` from `user_table` where `user_number` = %s" % userphone)[0][0]

    def get_user_info(self, userphone: str):
        count_row, = self.mysql.select(
            "select `win_count`, `lose_count`, `run_count`, `achievement` from `user_table`"
            " where `user_id` = (select `user_id` from `user_table` where `user_number` = %s);" % (userphone,))
        win, lose, run, achieve = count_row
        return win, lose, run, achieve

    def search_user_by_phone(self, userphone: str) -> str:
        if userphone in self.dbPlayers:
            return userphone

    def search_user_by_name(self, username: str) -> str:
        userphone = self.mysql.select("select `user_number` from `user_table` where `user_name` = '%s'" % username)
        return str(userphone[0][0]) if userphone else None

    def update_user_name(self, oldname: str, newname: str) -> bool:
        return self.mysql.execute(
            "update `user_table` set `user_name`='%s' where `user_name`='%s'" % (newname, oldname))

    @staticmethod
    def username_isvalid(username: str):
        username = username.lower()
        for badchar in [' ', '：', ':', ';', "'", '"', '`', '@', '$', '&', '%', '/', '?', '=', '<', '>', '\n', 'system']:
            if badchar in username:
                return False
        return True


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


if __name__ == '__main__':
    pass
