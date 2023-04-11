constexpr int MAX_BUF_SIZE = 512;
class ExpOver
{
public:
	WSAOVERLAPPED	m_overlap;
	IOCP_OP_CODE	m_opCode;
	char			m_buffer[MAX_BUF_SIZE];//여기에 roomId를 담자
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
		short size = 0;
		memcpy(&size, p, 2);		
		m_wsaBuf.len = size;
		m_wsaBuf.buf = m_buffer;
		m_opCode = OP_SEND;
		memcpy(m_buffer, p, size);
	}
};
