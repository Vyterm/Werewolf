#include "ClientPeer.h"
#include "Base64.h"

namespace vyt
{
	static Base64 cipher;

	ClientPeer *ClientPeer::m_instance;

	Packet ClientPeer::Recv(SOCKET & socket)
	{
		vytsize messageSize = 0;
		int len = 0;
		len = ::recv(socket, (char*)&messageSize, sizeof(vytsize), MSG_WAITALL);
		// 官方文档说连接正常断开后返回0，但调试时异常断开总是返回-1
		if (-1 == len || 0 == len) return nullptr;
		Buffer buffer = std::make_shared<__Buffer>(messageSize);
		len = ::recv(socket, (char*)buffer->m_buffer, int(buffer->m_size), MSG_WAITALL);
		if (-1 == len || 0 == len) return nullptr;
		buffer = cipher.Decrypt(buffer);
		command OpCommand = *(command*)buffer->m_buffer, SubCommand = *(command*)(buffer->m_buffer + sizeof(command));
		vytsize commandSize = sizeof(command);
		Packet packet(_Packet(OpCommand, SubCommand,
			{ {buffer->m_buffer + commandSize * 2, buffer->m_size - commandSize * 2} }));
		auto opcmd = packet->getOpCommand(), subcmd = packet->getSubCommand();
		auto msgSize = packet->getMessageSize();
		auto msg = packet->getMessage();
		return packet;
	}

	DWORD ClientPeer::RecvProc(LPVOID pArg)
	{
		ClientPeer &client = *(ClientPeer*)pArg;
		while (true)
		{
			auto packet = Recv(client.m_socket);
			if (nullptr == packet) break;
			guard g(client.m_packetMutex);
			client.m_packets.push_back(packet);
		}
		return 0;
	}

	bool ClientPeer::Connect(CString address, short port)
	{
		sockaddr_in addr = { };
		addr.sin_family = AF_INET;
		//addr.sin_addr.S_un.S_addr = inet_addr(address);
		InetPton(AF_INET, address, &addr.sin_addr.s_addr);
		addr.sin_port = htons(port);
		bool success = 0 == ::connect(m_socket, (sockaddr*)&addr, sizeof(sockaddr_in));
		if (success)
			CreateThread(nullptr, 0, &RecvProc, this, 0, nullptr);
		return success;
	}

	bool ClientPeer::Send(Packet packet)
	{
		return SOCKET_ERROR != ::send(m_socket, (char*)packet->getBuffer(), int(packet->getBufferSize()), 0);
	}

	Packet ClientPeer::Recv()
	{
		guard g(m_packetMutex);
		if (m_packets.empty()) return nullptr;
		auto packet = m_packets.front();
		m_packets.pop_front();
		return packet;
	}
}