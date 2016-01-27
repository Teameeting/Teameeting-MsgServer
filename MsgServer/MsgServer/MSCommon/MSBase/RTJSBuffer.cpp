#include "RTJSBuffer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int	kRequestBufferSizeInBytes = 2048;
void RTJSBuffer::writeShort(char** pptr, unsigned short anInt)
{
	**pptr = (char)(anInt / 256);
	(*pptr)++;
	**pptr = (char)(anInt % 256);
	(*pptr)++;
}

unsigned short RTJSBuffer::readShort(char** pptr)
{
	char* ptr = *pptr;
	unsigned short len = 256 * ((unsigned char)(*ptr)) + (unsigned char)(*(ptr + 1));
	*pptr += 2;
	return len;
}

RTJSBuffer::RTJSBuffer()
	: m_nBufOffset(0)
{
	m_nBufLen = kRequestBufferSizeInBytes;
	m_pBuffer = new char[m_nBufLen];
	m_nParseBufLen = kRequestBufferSizeInBytes;
	m_pParseBuf = new char[m_nParseBufLen];
}


RTJSBuffer::~RTJSBuffer()
{
	delete[] m_pBuffer;
	m_pBuffer = NULL;
	delete[] m_pParseBuf;
	m_pParseBuf = NULL;
}

void RTJSBuffer::RecvData(const char*data, int size)
{
	{//* 1,将接收到的数据放入缓存中
		while ((m_nBufOffset + size) > m_nBufLen)
		{
			int newLen = m_nBufLen + kRequestBufferSizeInBytes;
			if (size > newLen)
				newLen = m_nBufLen + size;
			char* temp = new char[newLen];
			if (temp == NULL)
				continue;
			memcpy(temp, m_pBuffer, m_nBufLen);
			delete[] m_pBuffer;
			m_pBuffer = temp;
			m_nBufLen = newLen;
		}

		memcpy(m_pBuffer + m_nBufOffset, data, size);
		m_nBufOffset += size;
	}

	while (m_nBufOffset > 3)
	{//* 2,解压包
		int parsed = 0;
		if (m_pBuffer[0] != '$')
		{// Hase error!
            printf("+++++=====-----====>>>RTJSBuffer::RecvData has error, m_pBuffer:%s\n\n", m_pBuffer);
			parsed = m_nBufOffset;
		}
		else
		{
			char*pptr = m_pBuffer + 1;
			int packLen = readShort(&pptr);
			if ((packLen + 3) <= m_nBufOffset)
			{
				ParseMessage(pptr, packLen);
				parsed = packLen + 3;
			}
			else
			{
				break;
			}
		}

		if (parsed > 0 && m_pBuffer != NULL)
		{
			m_nBufOffset -= parsed;
			if (m_nBufOffset == 0)
			{
				memset(m_pBuffer, 0, m_nBufLen);
			}
			else
			{
				memmove(m_pBuffer, m_pBuffer + parsed, m_nBufOffset);
			}
		}
	}
}

void RTJSBuffer::ParseMessage(const char*message, int nLen)
{
	if (nLen >= m_nParseBufLen)
	{
		m_nParseBufLen = nLen + 1;
		delete[] m_pParseBuf;
		m_pParseBuf = new char[m_nParseBufLen];
	}
	memcpy(m_pParseBuf, message, nLen);
	m_pParseBuf[nLen] = '\0';
	OnRecvMessage(m_pParseBuf, nLen);
}