constexpr int MAX_BUF_SIZE = 512;

enum IOCP_OP_CODE
{
	OP_NONE,
	OP_ACCEPT,
	OP_RECV,
	OP_SEND
	//추가 명령어
};

class ExpOver
{
public:
	WSAOVERLAPPED	m_overlap;
	IOCP_OP_CODE	m_opCode;
	char			m_buffer[MAX_BUF_SIZE];
	WSABUF			m_wsaBuf;
public:
	ExpOver()
	{
		m_opCode = OP_RECV;
		m_wsaBuf.buf = m_buffer;
		m_wsaBuf.len = MAX_BUF_SIZE;
	}
	ExpOver(char* p)
	{
		ZeroMemory(&m_overlap, sizeof(m_overlap));
		m_wsaBuf.len = p[0];
		m_wsaBuf.buf = m_buffer;
		m_opCode = OP_SEND;
		memcpy(m_buffer, p, p[0]);
	}
};
