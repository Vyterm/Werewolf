#ifndef VYTERM_BUFFER_H_INCLUDED
#define VYTERM_BUFFER_H_INCLUDED

#include <memory>
#include <deque>

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
		__Buffer(BufferPair &&srcs, vytsize presize = 0)
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
	using Buffer = std::shared_ptr<vyt::__Buffer>;
}

#endif