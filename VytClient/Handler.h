#ifndef VYTERM_NET_HANDLER_H_INCLUDED
#define VYTERM_NET_HANDLER_H_INCLUDED

#include "Packet.h"
#include <map>
#include <afxwin.h>

namespace vyt
{
	class IHandler
	{
		vyt::command m_opCommand;
		vyt::command m_subCommand;
	protected:
		IHandler(vyt::command opCommand, vyt::command subCommand);
		virtual ~IHandler();
	public:
		virtual void HandlePacket(vyt::Packet &packet) = NULL {};
	};

	class NetHandler
	{
		inline void LogInvalidPacket(const vyt::Packet &packet);
	private:
		NetHandler() {}
		static NetHandler m_instance;
	public:
		static NetHandler& Get() { return m_instance; }
	private:
		std::map<vyt::command, std::map<vyt::command, IHandler*>> m_handlers;
	public:
		void DispatchPacket(Packet &packet);
		void RegisterHandler(vyt::command opCommand, vyt::command subCommand, IHandler &handler);
		void UnregisterHandler(vyt::command opCommand, vyt::command subCommand, IHandler &handler);
	};
}

#endif