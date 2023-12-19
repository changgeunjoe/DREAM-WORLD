#pragma once
#include "../../PCH/stdafx.h"
class ExpOver
{
protected:
	WSAOVERLAPPED m_overlapped;
	IOCP_OP_CODE m_opCode;
public:
	ExpOver(IOCP_OP_CODE opCode)
	{
		m_opCode = opCode;
		ZeroMemory(&m_overlapped, sizeof(WSAOVERLAPPED));
	}

	void ResetOverlapped()
	{
		ZeroMemory(&m_overlapped, sizeof(WSAOVERLAPPED));
	}

	void ResetOverlapped(IOCP_OP_CODE opCode)
	{
		m_opCode = opCode;
		ZeroMemory(&m_overlapped, sizeof(WSAOVERLAPPED));
	}

	IOCP_OP_CODE GetOpCode()
	{
		return m_opCode;
	}

};

class ExpOverBuffer :public ExpOver
{
protected:
	char m_buffer[MAX_BUF_SIZE] = { 0 };
public:
	ExpOverBuffer(IOCP_OP_CODE opCode) : ExpOver(opCode)
	{
		ZeroMemory(m_buffer, MAX_BUF_SIZE);
	}

	ExpOverBuffer(IOCP_OP_CODE opCode, char* data) : ExpOver(opCode)
	{
		ZeroMemory(m_buffer, MAX_BUF_SIZE);
		strcpy_s(m_buffer, data);
	}

	ExpOverBuffer(IOCP_OP_CODE opCode, char* data, const int& dataSize) : ExpOver(opCode)
	{
		ZeroMemory(m_buffer, MAX_BUF_SIZE);
		memcpy(m_buffer, data, dataSize);
	}

	char* GetBufferData()
	{
		return m_buffer;
	}

	virtual void SetData(const char* data)
	{
		strcpy_s(m_buffer, data);
	}

	void SetData(const char* data, const int& dataSize)
	{
		strncpy_s(m_buffer, data, dataSize);
	}
};

//send용
class ExpOverWsaBuffer : public ExpOverBuffer
{
protected:
	WSABUF m_wsaBuf;
public:
	ExpOverWsaBuffer(IOCP_OP_CODE opCode) : ExpOverBuffer(opCode)
	{
		ZeroMemory(m_buffer, MAX_BUF_SIZE);
		m_wsaBuf.len = MAX_BUF_SIZE;
		m_wsaBuf.buf = m_buffer;
	}

	ExpOverWsaBuffer(IOCP_OP_CODE opCode, const char* data) : ExpOverBuffer(opCode)
	{
		ZeroMemory(m_buffer, MAX_BUF_SIZE);
		//이 프로젝트는 short임
		m_wsaBuf.len = (unsigned char)data[0];
		m_wsaBuf.buf = m_buffer;
		memcpy(m_buffer, data, m_wsaBuf.len);
	}

	virtual void SetData(const char* data) override
	{
		ZeroMemory(m_buffer, MAX_BUF_SIZE);
		//이 프로젝트는 short임
		m_wsaBuf.len = (unsigned char)data[0];
		m_wsaBuf.buf = m_buffer;
		memcpy(m_buffer, data, m_wsaBuf.len);
	}

public:
	void DoSend(SOCKET& socket);
};

//Recv용
class RecvExpOverBuffer : public ExpOverWsaBuffer
{
protected:
	int m_remainData;
public:
	RecvExpOverBuffer() : ExpOverWsaBuffer(OP_RECV), m_remainData{ 0 } { }
	RecvExpOverBuffer(char* data) : ExpOverWsaBuffer(OP_RECV), m_remainData{ 0 } { }
public:
	void DoRecv(SOCKET& socket);
	void RecvPacket(const int& id, const int& roomId, const unsigned long& ioByte);
	void Clear();
};

