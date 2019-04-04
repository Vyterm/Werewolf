#ifndef VYTERM_BASE64_H_INCLUDED
#define VYTERM_BASE64_H_INCLUDED

#include "Buffer.h"

namespace vyt
{
	class Base64
	{
	public:
		Base64();
	private:
		static const pByte m_base64Codes;
	public:
		Buffer Encrypt(Buffer src);
		Buffer Decrypt(Buffer src);
	};
}

#endif