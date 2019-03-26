#ifndef CLIENT_PEER_H_INCLUDED
#define CLIENT_PEER_H_INCLUDED

#include <afxwin.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib,"ws2_32.lib")
#include <map>
#include <mutex>
#include <deque>
using mutex = std::mutex;
using guard = std::lock_guard<mutex>;

#include "Packet.h"

namespace vyt
{
	class ClientPeer
	{
		mutex m_packetMutex;
		std::deque<Packet> m_packets;
		SOCKET m_socket;
	private:
		ClientPeer()
		{
			WSADATA wsaData;
			WSAStartup(MAKEWORD(2, 2), &wsaData);
			m_socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		}
		~ClientPeer()
		{
			::closesocket(m_socket);
			WSACleanup();
		}
		static ClientPeer *m_instance;
	public:
		static ClientPeer& Get()
		{
			if (nullptr == m_instance)
				m_instance = new ClientPeer;
			return *m_instance;
		}
	private:
		static Packet Recv(SOCKET &socket);
		static DWORD CALLBACK RecvProc(LPVOID pArg);
	public:
		bool Connect(CString address, short port);
		bool Send(__Packet &&packet);
		// This method return nullptr while empty in queue.
		const Packet Recv();
	};
}

#endif