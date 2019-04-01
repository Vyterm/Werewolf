#include "Handler.h"

vyt::IHandler::IHandler(vyt::command opCommand, vyt::command subCommand)
	: m_opCommand(opCommand), m_subCommand(subCommand)
{
	vyt::NetHandler::Get().RegisterHandler(m_opCommand, m_subCommand, *this);
}

vyt::IHandler::~IHandler()
{
	vyt::NetHandler::Get().UnregisterHandler(m_opCommand, m_subCommand, *this);
}

vyt::NetHandler vyt::NetHandler::m_instance;

inline void vyt::NetHandler::LogInvalidPacket(const vyt::Packet & packet)
{
	CString error;
	error.Format(_T("未定义的操作码，主操作码为 %d ，副操作码为 %d ，本消息包的内容将被抛弃\n"), packet->getOpCommand(), packet->getSubCommand());
	OutputDebugString(error);
}

void vyt::NetHandler::DispatchPacket(Packet & packet)
{
	if (m_handlers.end() != m_handlers.find(packet->getOpCommand()))
	{
		auto &subHandlers = m_handlers[packet->getOpCommand()];
		if (subHandlers.end() != subHandlers.find(packet->getSubCommand()) &&
			nullptr != subHandlers[packet->getSubCommand()])
			subHandlers[packet->getSubCommand()]->HandlePacket(packet);
		else
			LogInvalidPacket(packet);
	}
	else
		LogInvalidPacket(packet);
}

void vyt::NetHandler::RegisterHandler(vyt::command opCommand, vyt::command subCommand, IHandler & handler)
{
	m_handlers[opCommand][subCommand] = &handler;
}

void vyt::NetHandler::UnregisterHandler(vyt::command opCommand, vyt::command subCommand, IHandler & handler)
{
	if (m_handlers[opCommand][subCommand] == &handler)
		m_handlers[opCommand][subCommand] = nullptr;
}
