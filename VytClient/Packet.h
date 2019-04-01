#ifndef PACKET_H_INCLUDED
#define PACKET_H_INCLUDED

#include <afxwin.h>
#include <deque>
#include <memory>

namespace vyt
{
	using byte = unsigned char;
	using pByte = byte * ;
	using vytsize = unsigned long;
	using command = long;

	using BufferPair = std::deque<std::pair<void*, vytsize>>;

	struct __Buffer
	{
		pByte m_buffer = nullptr;
		vytsize m_size = 0;
		__Buffer(vytsize size)
		{
			m_size = size;
			m_buffer = new byte[m_size]();
		}
		__Buffer(BufferPair &&srcs, vytsize presize = -1)
		{
			m_size = presize <= 0 ? GetBestSize(srcs) : presize;
			m_buffer = new byte[m_size];
			pByte pBuffer = m_buffer;
#ifdef _DEBUG
			vytsize index = 0;
			for (auto &bps : srcs)
			{
				if (index + bps.second > m_size)
					throw std::invalid_argument("The buffer size less than presize, check the size!");
				MemoryCopy(pBuffer, bps.first, bps.second);
			}
#else
			for (auto &bps : srcs)
				MemoryCopy(pBuffer, bps.first, bps.second);
#endif
		}
		__Buffer(const __Buffer &buffer)
		{
			m_size = buffer.m_size;
			m_buffer = new byte[m_size];
			memcpy_s(m_buffer, m_size, buffer.m_buffer, buffer.m_size);
		}
		__Buffer(__Buffer &&buffer)
		{
			m_size = buffer.m_size;
			m_buffer = buffer.m_buffer;
			buffer.m_size = 0;
			buffer.m_buffer = nullptr;
		}
		__Buffer& operator=(const __Buffer& buffer) = delete;
		__Buffer& operator=(__Buffer &&buffer) = delete;
		~__Buffer()
		{
			if (nullptr != m_buffer)
			{
				delete[] m_buffer;
				m_buffer = nullptr;
			}
		}
		static vytsize GetBestSize(const BufferPair &srcs)
		{
			vytsize bestSize = 0;
			for (auto &bps : srcs)
				bestSize += bps.second;
			return bestSize;
		}
	private:
		static void MemoryCopy(pByte &dest, const void *src, vytsize size)
		{
			memcpy_s(dest, size, src, size);
			dest += size;
		}
	};
	using Buffer = std::shared_ptr<__Buffer>;

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
		static void SetBuffer(va_list &args, pByte &pBuffer)
		{
			*va_arg(args, T*) = *(T*)pBuffer;
			pBuffer += sizeof(T);
		}
	public:
		void Decode(const char *szFormat, ...)
		{
			auto pBuffer = this->getMessage();
			va_list args;
			va_start(args, szFormat);
			while (0 != *szFormat)
			{
				switch (*szFormat)
				{
				case 's':
				{
					vytsize stringLength = *(vytsize*)pBuffer;
					pBuffer += sizeof(vytsize);
					TCHAR &cache = *(TCHAR*)(pBuffer+stringLength);
					TCHAR tmp = cache;
					cache = _T('\0');
					va_arg(args, CString*)->SetString((LPCTSTR)pBuffer);
					cache = tmp;
					pBuffer += stringLength;
					break;
				}
				case 'b': SetBuffer<bool>(args, pBuffer); break;
				case 'i': SetBuffer<int>(args, pBuffer); break;
				case 'h': SetBuffer<short>(args, pBuffer); break;
				case 'f': SetBuffer<float>(args, pBuffer); break;
				case 'd': SetBuffer<double>(args, pBuffer); break;
				default:
					throw std::invalid_argument("Unsolved format");
				}
				++szFormat;
			}
			va_end(args);
		}
	};

	using Packet = std::shared_ptr<__Packet>;
	inline Packet _Packet(command OpCommand, command SubCommand, BufferPair &&srcs)
	{
		return std::make_shared<__Packet>(OpCommand, SubCommand, std::move(srcs));
	}
	inline Packet _Packet(command OpCommand, command SubCommand, const char *szFormat, ...)
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