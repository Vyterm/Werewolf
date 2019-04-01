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
		if (subHandlers.end() != subHandlers.find(packet->getSubCommand()))
		{
			for (auto &pHandler : subHandlers[packet->getSubCommand()])
				pHandler->HandlePacket(packet);
		}
		else
			LogInvalidPacket(packet);
	}
	else
		LogInvalidPacket(packet);
}

void vyt::NetHandler::RegisterHandler(vyt::command opCommand, vyt::command subCommand, IHandler & handler)
{
	m_handlers[opCommand][subCommand].emplace(&handler);
}

void vyt::NetHandler::UnregisterHandler(vyt::command opCommand, vyt::command subCommand, IHandler & handler)
{
	auto &handlers = m_handlers[opCommand][subCommand];
	auto pHandler = handlers.find(&handler);
	if (handlers.end() != pHandler)
		handlers.erase(pHandler);
}
