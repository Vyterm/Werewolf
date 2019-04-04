#include "Base64.h"

using namespace vyt;

const pByte Base64::m_base64Codes = (const pByte)"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

struct indexforbit
{
public:
	unsigned long byteIndex = 0;
	unsigned long bitIndex = 0;
	indexforbit& operator+=(unsigned long offset)
	{
		bitIndex += offset;
		byteIndex += bitIndex / 8;
		bitIndex %= 8;
		return *this;
	}
};


Base64::Base64()
{
}

Buffer vyt::Base64::Encrypt(Buffer src)
{
	const pByte original = src->m_buffer;
	vytsize length = src->m_size;
	auto cipherText = new unsigned char[length / 3 * 4 + 4]();
	unsigned long oi = 0;
	indexforbit i;
	while (i.byteIndex < length)
	{
		int codeIndex;
		if (i.bitIndex == 0)
			codeIndex = original[i.byteIndex] >> 2;
		else
		{
			codeIndex = (original[i.byteIndex] & (0xFF >> i.bitIndex)) << (i.bitIndex - 2);
			if (i.byteIndex + 1 < length)
				codeIndex += original[i.byteIndex + 1] >> (10 - i.bitIndex);
		}
		cipherText[oi++] = m_base64Codes[codeIndex];
		i += 6;
	}
	return Buffer(new __Buffer({ { cipherText, oi } }));
}

Buffer vyt::Base64::Decrypt(Buffer buffer)
{
	const pByte src = buffer->m_buffer;
	unsigned long length = buffer->m_size;
	unsigned long size = length * 3 / 4;
	auto origin = new unsigned char[size + 4]();
	indexforbit bi;
	for (unsigned long fori = 0; fori < length; ++fori)
	{
		int i = 0;
		while (src[fori] != m_base64Codes[i])
			++i;
		if (0 == bi.bitIndex)
			origin[bi.byteIndex] = i << 2;
		else if (6 == bi.bitIndex)
		{
			origin[bi.byteIndex] += i >> 4;
			if (bi.byteIndex + 1 < size)
				origin[bi.byteIndex + 1] = (i & 0xFF >> 4) << 4;
		}
		else if (4 == bi.bitIndex)
		{
			origin[bi.byteIndex] += i >> 2;
			if (bi.byteIndex + 1 < size)
				origin[bi.byteIndex + 1] = (i & 0xFF >> 6) << 6;
		}
		else if (2 == bi.bitIndex)
			origin[bi.byteIndex] += i;
		bi += 6;
	}
	return Buffer(new __Buffer({ { origin, size } }));
}
