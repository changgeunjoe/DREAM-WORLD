#include "stdafx.h"
#include "UserSession.h"
#include "../ExpOver/ExpOver.h"
#include "../../../../Server/Network/protocol/protocol.h"
#include "../IocpEvent/IocpEventManager.h"
#include "../UserManager/UserManager.h"
#include "../NetworkModule/NetworkModule.h"

int Network::UserSession::GetInitUserId()
{
	static int InitUserId = 0;
	return InitUserId++;
}

void Network::UserSession::Disconnect()
{
	m_isConnect = false;
	UserManager::GetInstance().InsertDisconnectId(m_id);
	DreamWorld::StressTestNetwork::GetInstance().DisconnectClient();
}

void Network::UserSession::DoRecv(ExpOver*& expOver)
{
	if (!m_isConnect) return;
	//wsaBuf length 길이 재 설정
	m_recvDataStorage.m_wsabuf.len = MAX_RECV_BUF_SIZE - m_recvDataStorage.m_remainDataLength;
	DWORD immediateRecvByte = 0;
	DWORD recvFlag = 0;
	int recvRes = WSARecv(m_socket, &m_recvDataStorage.m_wsabuf, 1, nullptr, &recvFlag, expOver, nullptr);
	if (recvRes != 0) {
		int errCode = WSAGetLastError();
		if (WSA_IO_PENDING != errCode) {
			Disconnect();
		}
	}
}

void Network::UserSession::ConstructPacket(const DWORD& ioSize)
{
	int remainSize = ioSize + m_recvDataStorage.m_remainDataLength;
	char* bufferPosition = m_recvDataStorage.m_buffer;
	while (remainSize > sizeof(PacketHeader::size)) {
		PacketHeader* currentPacket = reinterpret_cast<PacketHeader*>(bufferPosition);
		if (currentPacket->size > remainSize) {
			//완성된 패킷이 만들어지지 않음.
			break;
		}
		//완성된 패킷
		ExecutePacket(currentPacket);
		//남은 퍼버 크기 최신화, 현재 버퍼 위치 다음 패킷 시작 위치로
		remainSize -= currentPacket->size;
		bufferPosition = bufferPosition += currentPacket->size;
	}
	//현재 남은 데이터 크기 저장
	m_recvDataStorage.m_remainDataLength = remainSize;
	//남은 패킷 데이터가 있다면, 맨 앞으로 당기기
	if (remainSize > 0)
		std::memcpy(m_recvDataStorage.m_buffer, bufferPosition, remainSize);
	//wsaBuf의 buf 위치를 바꿈
	m_recvDataStorage.m_wsabuf.buf = m_recvDataStorage.m_buffer + remainSize;
}

Network::UserSession::UserSession() :m_id(GetInitUserId()), m_socket(NULL), m_recvDataStorage(RecvDataStorage()), m_isConnect(false)
{
}

Network::UserSession::~UserSession()
{
	if (m_socket) {
		closesocket(m_socket);
		m_socket = NULL;
	}
}

void Network::UserSession::Execute(Network::ExpOver* expOver, const DWORD& ioByte, const ULONG_PTR& key)
{
	const auto& opCode = expOver->GetOpCode();
	if (IOCP_OP_CODE::OP_RECV == opCode) {
		if (0 == ioByte) {
			Disconnect();
		}
		ConstructPacket(ioByte);
		DoRecv(expOver);
		return;
	}
	else {
		std::cout << "error UserSession::Execute() - Invalid OpCode: " << (int)opCode << std::endl;
	}
}

void Network::UserSession::Fail(ExpOver* expOver)
{
	Disconnect();
	IocpEventManager::GetInstance().DeleteExpOver(expOver);
}

void Network::UserSession::Connect(SOCKET connectSocket)
{
	if (m_socket)
		closesocket(m_socket);
	m_socket = connectSocket;
	m_isConnect = true;
	StartRecv();
}

void Network::UserSession::StartRecv()
{
	m_recvDataStorage.Reset();
	auto expOver = IocpEventManager::GetInstance().CreateExpOver(IOCP_OP_CODE::OP_RECV, shared_from_this());
	DoRecv(expOver);
}

void Network::UserSession::Send(PacketHeader* sendPacketHeader)
{
	if (!m_isConnect) return;
	IocpEventManager::GetInstance().Send(m_socket, sendPacketHeader);
}

void Network::UserSession::Send(std::shared_ptr<PacketHeader> sendPacketHeader)
{
	if (!m_isConnect) return;
	IocpEventManager::GetInstance().Send(m_socket, sendPacketHeader.get());
}
