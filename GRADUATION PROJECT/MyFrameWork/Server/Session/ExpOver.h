constexpr int MAX_BUF_SIZE = 512;

enum IOCP_OP_CODE
{
	OP_NONE,
	OP_ACCEPT,
	OP_RECV,
	OP_SEND
	//추가 명령어
};

struct ExpOver
{
	WSAOVERLAPPED	m_overlap;
	IOCP_OP_CODE	m_opCode;
	char			m_buffer[MAX_BUF_SIZE];
	WSABUF			m_wsaBuf;
	SOCKET			m_socket;
};