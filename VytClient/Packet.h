#ifndef PACKET_H_INCLUDED
#define PACKET_H_INCLUDED

#include "Buffer.h"
#include <afxwin.h>

namespace vyt
{
	struct __Packet
	{
		Buffer m_buffer = nullptr;
	public:
		__Packet(command OpCommand, command SubCommand, BufferPair &&srcs)
		{
			vytsize commandSize = sizeof(command);
			srcs.push_front({ &SubCommand, commandSize });
			srcs.push_front({ &OpCommand, commandSize });
			vytsize bestSize = __Buffer::GetBestSize(srcs), sizesize = sizeof(vytsize);
			srcs.push_front({ &bestSize, sizesize });
			m_buffer = std::make_shared<__Buffer>(std::move(srcs), bestSize + sizesize);
		}
		command getOpCommand() const
		{
			return *(const command*)(m_buffer->m_buffer + sizeof(vytsize));
		}
		command getSubCommand() const
		{
			return *(const command*)(m_buffer->m_buffer + sizeof(vytsize) + sizeof(command));
		}
		const pByte getMessage() const
		{
			return (m_buffer->m_buffer + sizeof(vytsize) + sizeof(command) * 2);
		}
		vytsize getMessageSize() const
		{
			return (m_buffer->m_size - sizeof(vytsize) - sizeof(command) * 2);
		}
		pByte getBuffer()
		{
			return m_buffer->m_buffer;
		}
		vytsize getBufferSize()
		{
			return m_buffer->m_size;
		}

	private:
		template <typename T>
		static void DecodeBuffer(T *pointer, pByte &pBuffer)
		{
			if (nullptr != pointer)
				*pointer = *(T*)pBuffer;
			pBuffer += sizeof(T);
		}
		template <>
		static void DecodeBuffer(CString *pointerString, pByte &pBuffer)
		{
			vytsize stringLength = *(vytsize*)pBuffer;
			pBuffer += sizeof(vytsize);
			TCHAR &cache = *(TCHAR*)(pBuffer + stringLength);
			TCHAR tmp = cache;
			cache = _T('\0');
			if (nullptr != pointerString)
				pointerString->SetString((LPCTSTR)pBuffer);
			cache = tmp;
			pBuffer += stringLength;

		}
		template <typename T>
		static void DecodeBuffer(va_list &args, pByte &pBuffer)
		{
			DecodeBuffer(va_arg(args, T*), pBuffer);
		}
		template <typename T>
		static void DecodeBuffer(pByte &target, pByte &pBuffer)
		{
			DecodeBuffer((T*)target, pBuffer);
			target += sizeof(T);
		}
		template <typename T>
		static void DecodeBuffers(va_list &args, pByte &pBuffer)
		{
			T *&pointerObjects = *va_arg(args, T**);
			vytsize objectCount = va_arg(args, vytsize);
			for (vytsize i = 0; i < objectCount; ++i)
				DecodeBuffer(&pointerObjects[i], pBuffer);
		}
		template <typename T>
		static void DecodeBuffers(pByte &target, pByte &pBuffer)
		{
			DecodeBuffer((T*)target, pBuffer);
			target += sizeof(T);
		}
		template <typename TArg>
		static void DecodeBuffer(char format, TArg &args, pByte &pBuffer)
		{
			switch (format)
			{
			case 's': DecodeBuffer<CString>(args, pBuffer); break;
			case 'b': DecodeBuffer<bool>(args, pBuffer); break;
			case 'i': DecodeBuffer<int>(args, pBuffer); break;
			case 'h': DecodeBuffer<short>(args, pBuffer); break;
			case 'f': DecodeBuffer<float>(args, pBuffer); break;
			case 'd': DecodeBuffer<double>(args, pBuffer); break;
			case 'S': DecodeBuffers<CString>(args, pBuffer); break;
			case 'B': DecodeBuffers<bool>(args, pBuffer); break;
			case 'I': DecodeBuffers<int>(args, pBuffer); break;
			case 'H': DecodeBuffers<short>(args, pBuffer); break;
			case 'F': DecodeBuffers<float>(args, pBuffer); break;
			case 'D': DecodeBuffers<double>(args, pBuffer); break;
			default:
				throw std::invalid_argument("Unsolved format");
			}
		}
	public:
		vytsize Decode(const char *szFormat, ...)
		{
			auto pBuffer = this->getMessage();
			va_list args;
			va_start(args, szFormat);
			while (0 != *szFormat)
				DecodeBuffer(*szFormat++, args, pBuffer);
			va_end(args);
			return vytsize(pBuffer - this->getMessage());
		}
		template <typename TStruct>
		void DecodeStruct(const char *step, const char *szFormat, TStruct *pStructs, vytsize count = 1)
		{
			auto pBuffer = this->getMessage();
			pByte crashBytes = nullptr;
			while (0 != *step)
				DecodeBuffer(*step++, crashBytes, pBuffer);
			for (vytsize i = 0; i < count; ++i)
			{
				pByte pField = (pByte)&pStructs[i];
				while (0 != *szFormat)
					DecodeBuffer(*szFormat++, pField, pBuffer);
			}
		}
	};

	inline Buffer _Buffer(const char *szFormat = "", ...)
	{
		BufferPair srcs;
		std::deque<CString> cacheStrings;
		std::deque<vytsize> cacheStringSizes;
		va_list args;
		va_start(args, szFormat);
		while (0 != *szFormat)
		{
			switch (*szFormat)
			{
			case 's':
			{
				cacheStrings.push_back(CString());
				cacheStringSizes.push_back(vytsize());
				cacheStrings.back().SetString(va_arg(args, CString));
				cacheStringSizes.back() = vytsize(cacheStrings.back().GetLength() * sizeof(TCHAR));
				srcs.push_back({ &cacheStringSizes.back(), vytsize(sizeof(vytsize)) });
				srcs.push_back({ cacheStrings.back().GetBuffer(), cacheStringSizes.back() });
				break;
			}
			case 'b': srcs.push_back({ &va_arg(args, bool), vytsize(sizeof(bool)) }); break;
			case 'i': srcs.push_back({ &va_arg(args, int), vytsize(sizeof(int)) }); break;
			case 'h': srcs.push_back({ &va_arg(args, short), vytsize(sizeof(short)) }); break;
			case 'f': srcs.push_back({ &va_arg(args, float), vytsize(sizeof(float)) }); break;
			case 'd': srcs.push_back({ &va_arg(args, double), vytsize(sizeof(double)) }); break;
			default:
				throw std::invalid_argument("Unsolved format");
			}
			++szFormat;
		}
		va_end(args);
		return std::make_shared<vyt::__Buffer>(std::move(srcs));
	}

	using Packet = std::shared_ptr<__Packet>;
	inline Packet _Packet(command OpCommand, command SubCommand, BufferPair &&srcs)
	{
		return std::make_shared<__Packet>(OpCommand, SubCommand, std::move(srcs));
	}
	inline Packet _Packet(command OpCommand, command SubCommand, const char *szFormat = "", ...)
	{
		static const vytsize intSize = sizeof(int), sizesize = sizeof(vytsize);
		BufferPair srcs;
		std::deque<CString> cacheStrings;
		std::deque<vytsize> cacheStringSizes;
		va_list args;
		va_start(args, szFormat);
		while (0 != *szFormat)
		{
			switch (*szFormat)
			{
			case 's':
			{
				cacheStrings.push_back(CString());
				cacheStringSizes.push_back(vytsize());
				cacheStrings.back().SetString(va_arg(args, CString));
				cacheStringSizes.back() = vytsize(cacheStrings.back().GetLength() * sizeof(TCHAR));
				srcs.push_back({ &cacheStringSizes.back(), sizesize });
				srcs.push_back({ cacheStrings.back().GetBuffer(), cacheStringSizes.back() });
				break;
			}
			case 'b': srcs.push_back({ &va_arg(args, bool), vytsize(sizeof(bool)) }); break;
			case 'i': srcs.push_back({ &va_arg(args, int), vytsize(sizeof(int)) }); break;
			case 'h': srcs.push_back({ &va_arg(args, short), vytsize(sizeof(short)) }); break;
			case 'f': srcs.push_back({ &va_arg(args, float), vytsize(sizeof(float)) }); break;
			case 'd': srcs.push_back({ &va_arg(args, double), vytsize(sizeof(double)) }); break;
			default:
				throw std::invalid_argument("Unsolved format");
			}
			++szFormat;
		}
		va_end(args);
		return _Packet(OpCommand, SubCommand, std::move(srcs));
	}
}

#endif