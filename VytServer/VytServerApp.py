import socket
import threading
# Python 会自动对齐到四字节，因此如果要发送单个字节的话，需要在格式字符串前加上=取消对齐
import struct
from vyterm.werewolf.Handlers import handle_packet, handle_logout

class ClientPeer:
    @staticmethod
    def SendProc(*args, **kwargs):
        peer = kwargs['peer']
        while True:
            try:
                peer.sendlock.acquire()
                packets = peer.packets
                peer.packets = []
                peer.sendlock.release()
                for packet in packets:
                    peer.socket.send(packet)
            except WindowsError:
                print("SendProc catch winerror")
                return
            except Exception as e:
                print("SendProc catch except exception:", e)
                return


    def __init__(self, socket):
        self.socket = socket
        self.packets = []
        self.sendlock = threading.Lock()
        sendThread = threading.Thread(name="Send Thread", target=ClientPeer.SendProc, kwargs={'peer':self})
        sendThread.setDaemon(True)
        sendThread.start()

    def send(self, opcmd: int, subcmd: int, packet: bytes):
        packet = struct.pack('ii', opcmd, subcmd) + packet
        packet = struct.pack('i', len(packet)) + packet
        self.sendlock.acquire()
        self.packets.append(packet)
        self.sendlock.release()

    def recv(self, size):
        return self.socket.recv(size)


def RecvProc(*args, **kwargs):
    client = ClientPeer(kwargs['socket'])
    ip, port = kwargs['addr']
    while True:
        try:
            packetsize = client.recv(4)
            packetsize, = struct.unpack('i', packetsize)
            packet = client.recv(packetsize)
            opCommand,subCommand, = struct.unpack('ii', packet[:8])
            packet = packet[8:]
            print("收到来自客户端的数据包，大小为" + str(packetsize)
                  + "\n主操作码为：" + str(opCommand)
                  + "\n副操作码为：" + str(subCommand))
            handle_packet(client, opCommand, subCommand, packet)
        except ConnectionResetError:
            # 客户端断开连接后会出现异常
            print("用户 " + ip + " 已断开连接")
            handle_logout(client)
            return


def AcceptProc(*args, **kwargs):
    server = kwargs['socket']
    while True:
        client, addr = server.accept()
        ip, port = addr
        print("用户IP：" + ip + "   " + "端口号：" + str(port))
        # 建立接收客户端消息线程
        recvThread = threading.Thread(name='Recv Thread', target=RecvProc, kwargs={'socket': client, 'addr': addr})
        recvThread.setDaemon(True)
        # 开启线程
        recvThread.start()


if __name__ == '__main__':
    # 创建套接字
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM, socket.IPPROTO_TCP)
    # 绑定IP及端口
    server.bind(("localhost", 38564))
    # 开始监听连接
    server.listen(socket.SOMAXCONN)
    # 创建主线程
    acceptThread = threading.Thread(name='Accept Thread', target=AcceptProc, kwargs={'socket':server})
    acceptThread.setDaemon(True)
    # 启动主线程
    acceptThread.start()

    while ("Quit" != input(">>> ")):
        continue


    print("Exit Done")
