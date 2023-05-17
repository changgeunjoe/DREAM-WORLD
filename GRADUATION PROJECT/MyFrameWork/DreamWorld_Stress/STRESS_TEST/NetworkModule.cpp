#include"stdafx.h"
//#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma warning(disable: 4996)
using namespace std;
using namespace chrono;

extern HWND		hWnd;

const static int MAX_TEST = 20000;
const static int MAX_CLIENTS = MAX_TEST * 2;
const static int INVALID_ID = -1;
const static int MAX_PACKET_SIZE = 512;
const static int MAX_BUFF_SIZE = 512;

#pragma comment (lib, "ws2_32.lib")

#include "protocol_2022.h"

HANDLE g_hiocp;



high_resolution_clock::time_point last_connect_time;

struct OverlappedEx {
	WSAOVERLAPPED over;
	WSABUF wsabuf;
	unsigned char IOCP_buf[MAX_BUFF_SIZE];
	OPTYPE event_type;
	int event_target;
};

struct CLIENT {
	int id;
	int x;
	int y;
	atomic_bool connected;

	bool isMove = false;
	bool isInGameRoom = false;

	SOCKET client_socket;
	OverlappedEx recv_over;
	unsigned char packet_buf[MAX_PACKET_SIZE];
	short prev_packet_data;
	short curr_packet_size;
	high_resolution_clock::time_point last_move_time;
	high_resolution_clock::time_point last_move_start_time;
};

array<int, MAX_CLIENTS> client_map;
array<CLIENT, MAX_CLIENTS> g_clients;
atomic_int num_connections;
atomic_int client_to_close;
atomic_int active_clients;

int			global_delay;				// ms단위, 1000이 넘으면 클라이언트 증가 종료

vector <thread*> worker_threads;
thread test_thread;

float point_cloud[MAX_TEST * 2];

// 나중에 NPC까지 추가 확장 용
struct ALIEN {
	int id;
	int x, y;
	int visible_count;
};

void error_display(const char* msg, int err_no)
{
	WCHAR* lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	std::cout << msg;
	std::wcout << L"에러" << lpMsgBuf << std::endl;

	MessageBox(hWnd, lpMsgBuf, L"ERROR", 0);
	LocalFree(lpMsgBuf);
	// while (true);
}

void DisconnectClient(int ci)
{
	bool status = true;
	if (true == atomic_compare_exchange_strong(&g_clients[ci].connected, &status, false)) {
		closesocket(g_clients[ci].client_socket);
		active_clients--;
	}
	// cout << "Client [" << ci << "] Disconnected!\n";
}

void SendPacket(int cl, void* packet)
{
	short psize = 0;// = reinterpret_cast<unsigned char*>(packet)[0];
	memcpy(&psize, packet, 2);
	int ptype = reinterpret_cast<unsigned char*>(packet)[2];
	OverlappedEx* over = new OverlappedEx;
	over->event_type = OP_SEND;
	memcpy(over->IOCP_buf, packet, psize);
	ZeroMemory(&over->over, sizeof(over->over));
	over->wsabuf.buf = reinterpret_cast<CHAR*>(over->IOCP_buf);
	over->wsabuf.len = psize;
	int ret = WSASend(g_clients[cl].client_socket, &over->wsabuf, 1, NULL, 0,
		&over->over, NULL);
	if (0 != ret) {
		int err_no = WSAGetLastError();
		if (WSA_IO_PENDING != err_no)
			error_display("Error in SendPacket:", err_no);
	}
	// std::cout << "Send Packet [" << ptype << "] To Client : " << cl << std::endl;
}

void ProcessPacket(int ci, unsigned char packet[])
{
	switch (packet[2]) {
	case SERVER_PACKET::LOGIN_OK:
	{
		//매치 패킷
		active_clients++;
		CLIENT_PACKET::MatchPacket matchPacket;
		matchPacket.Role = pow(2, (ci % 4));
		matchPacket.size = sizeof(CLIENT_PACKET::MatchPacket);
		matchPacket.type = CLIENT_PACKET::MATCH;
		SendPacket(ci, &matchPacket);
	}
	break;
	case SERVER_PACKET::ADD_PLAYER:
	{
		//cout << "AddPlayer[" << ci << "]" << endl;
	}
	break;
	case SERVER_PACKET::MOVE_KEY_DOWN:
	case SERVER_PACKET::MOVE_KEY_UP:
	{

	}
	break;
	case SERVER_PACKET::INTO_GAME:
	{
		g_clients[ci].isInGameRoom = true;
		cout << "InGame[" << ci << "]" << endl;
	}
	break;
	case SERVER_PACKET::GAME_END:
	{
		g_clients[ci].isInGameRoom = false;
		CLIENT_PACKET::GameEndPacket sendPacket;
		sendPacket.size = sizeof(CLIENT_PACKET::GameEndPacket);
		sendPacket.type = CLIENT_PACKET::GAME_END_OK;
		SendPacket(ci, &sendPacket);

		CLIENT_PACKET::MatchPacket matchPacket;
		matchPacket.Role = pow(2, (ci % 4));
		matchPacket.size = sizeof(CLIENT_PACKET::MatchPacket);
		matchPacket.type = CLIENT_PACKET::MATCH;
		SendPacket(ci, &matchPacket);
	}
	break;
	case SERVER_PACKET::BOSS_CHANGE_STATE_MOVE_DES:
	{

	}
	break;
	case SERVER_PACKET::GAME_STATE:
	{
		SERVER_PACKET::GameState* recvPacket = reinterpret_cast<SERVER_PACKET::GameState*>(packet);
		utc_clock::time_point currentUTC_Time = utc_clock::now();
		double gameStateDurationTime = duration_cast<milliseconds>(currentUTC_Time - recvPacket->time).count();
		if (global_delay < gameStateDurationTime) global_delay++;
		else if (global_delay > gameStateDurationTime) global_delay--;
	}
	break;
	default: MessageBox(hWnd, L"Unknown Packet Type", L"ERROR", 0);
		while (true);
	}
}

void Worker_Thread()
{
	while (true) {
		DWORD io_size;
		unsigned long long ci;
		OverlappedEx* over;
		BOOL ret = GetQueuedCompletionStatus(g_hiocp, &io_size, &ci,
			reinterpret_cast<LPWSAOVERLAPPED*>(&over), INFINITE);
		// std::cout << "GQCS :";
		int client_id = static_cast<int>(ci);
		if (FALSE == ret) {
			int err_no = WSAGetLastError();
			if (64 == err_no) DisconnectClient(client_id);
			else {
				// error_display("GQCS : ", WSAGetLastError());
				DisconnectClient(client_id);
			}
			if (OP_SEND == over->event_type) delete over;
		}
		if (0 == io_size) {
			DisconnectClient(client_id);
			continue;
		}
		if (OP_RECV == over->event_type) {
			//std::cout << "RECV from Client :" << ci;
			//std::cout << "  IO_SIZE : " << io_size << std::endl;
			unsigned char* buf = g_clients[ci].recv_over.IOCP_buf;
			unsigned short psize = g_clients[ci].curr_packet_size;
			unsigned short pr_size = g_clients[ci].prev_packet_data;
			while (io_size > 0) {
				if (0 == psize) {
					short shortPacketSize;
					memcpy(&shortPacketSize, buf, 2);
					psize = shortPacketSize;
				}
				if (io_size + pr_size >= psize) {
					// 지금 패킷 완성 가능
					unsigned char packet[MAX_PACKET_SIZE];
					memcpy(packet, g_clients[ci].packet_buf, pr_size);
					memcpy(packet + pr_size, buf, psize - pr_size);
					ProcessPacket(static_cast<int>(ci), packet);
					io_size -= psize - pr_size;
					buf += psize - pr_size;
					psize = 0; pr_size = 0;
				}
				else {
					memcpy(g_clients[ci].packet_buf + pr_size, buf, io_size);
					pr_size += io_size;
					io_size = 0;
				}
			}
			g_clients[ci].curr_packet_size = psize;
			g_clients[ci].prev_packet_data = pr_size;
			DWORD recv_flag = 0;
			int ret = WSARecv(g_clients[ci].client_socket,
				&g_clients[ci].recv_over.wsabuf, 1,
				NULL, &recv_flag, &g_clients[ci].recv_over.over, NULL);
			if (SOCKET_ERROR == ret) {
				int err_no = WSAGetLastError();
				if (err_no != WSA_IO_PENDING)
				{
					//error_display("RECV ERROR", err_no);
					DisconnectClient(client_id);
				}
			}
		}
		else if (OP_SEND == over->event_type) {
			if (io_size != over->wsabuf.len) {
				// std::cout << "Send Incomplete Error!\n";
				DisconnectClient(client_id);
			}
			delete over;
		}
		else if (OP_DO_MOVE == over->event_type) {
			// Not Implemented Yet
			delete over;
		}
		else {
			std::cout << "Unknown GQCS event!\n";
			while (true);
		}
	}
}

constexpr int DELAY_LIMIT = 100;
constexpr int DELAY_LIMIT2 = 150;
constexpr int ACCEPT_DELY = 50;

void Adjust_Number_Of_Client()
{
	static int delay_multiplier = 1;
	static int max_limit = MAXINT;
	static bool increasing = true;

	if (active_clients >= MAX_TEST) return;
	if (num_connections >= MAX_CLIENTS) return;

	auto duration = high_resolution_clock::now() - last_connect_time;
	if (ACCEPT_DELY * delay_multiplier > duration_cast<milliseconds>(duration).count()) return;

	int t_delay = global_delay;
	if (DELAY_LIMIT2 < t_delay) {
		if (true == increasing) {
			max_limit = active_clients;
			increasing = false;
		}
		if (100 > active_clients) return;
		if (ACCEPT_DELY * 10 > duration_cast<milliseconds>(duration).count()) return;
		last_connect_time = high_resolution_clock::now();
		DisconnectClient(client_to_close);
		client_to_close++;
		return;
	}
	else
		if (DELAY_LIMIT < t_delay) {
			delay_multiplier = 10;
			return;
		}
	if (max_limit - (max_limit / 20) < active_clients) return;

	increasing = true;
	last_connect_time = high_resolution_clock::now();
	g_clients[num_connections].client_socket = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	SOCKADDR_IN ServerAddr;
	ZeroMemory(&ServerAddr, sizeof(SOCKADDR_IN));
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(9000);
	ServerAddr.sin_addr.s_addr = inet_addr("127.0.0.1");


	int Result = WSAConnect(g_clients[num_connections].client_socket, (sockaddr*)&ServerAddr, sizeof(ServerAddr), NULL, NULL, NULL, NULL);
	if (0 != Result) {
		error_display("WSAConnect : ", GetLastError());
	}

	g_clients[num_connections].curr_packet_size = 0;
	g_clients[num_connections].prev_packet_data = 0;
	ZeroMemory(&g_clients[num_connections].recv_over, sizeof(g_clients[num_connections].recv_over));
	g_clients[num_connections].recv_over.event_type = OP_RECV;
	g_clients[num_connections].recv_over.wsabuf.buf =
		reinterpret_cast<CHAR*>(g_clients[num_connections].recv_over.IOCP_buf);
	g_clients[num_connections].recv_over.wsabuf.len = sizeof(g_clients[num_connections].recv_over.IOCP_buf);

	DWORD recv_flag = 0;
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(g_clients[num_connections].client_socket), g_hiocp, num_connections, 0);

	//login Packet send
	CLIENT_PACKET::LoginPacket loginPacket;
	int temp = num_connections;
	sprintf_s(loginPacket.id, "module%d", temp);
	loginPacket.size = sizeof(CLIENT_PACKET::LoginPacket);
	loginPacket.type = CLIENT_PACKET::LOGIN;
	SendPacket(num_connections, &loginPacket);

	int ret = WSARecv(g_clients[num_connections].client_socket, &g_clients[num_connections].recv_over.wsabuf, 1,
		NULL, &recv_flag, &g_clients[num_connections].recv_over.over, NULL);
	if (SOCKET_ERROR == ret) {
		int err_no = WSAGetLastError();
		if (err_no != WSA_IO_PENDING)
		{
			error_display("RECV ERROR", err_no);
			goto fail_to_connect;
		}
	}
	num_connections++;
fail_to_connect:
	return;
}

void Test_Thread()
{
	while (true) {
		//Sleep(max(20, global_delay));
		Adjust_Number_Of_Client();

		for (int i = 0; i < num_connections; ++i) {
			CLIENT_PACKET::MovePacket sendPacket;
			sendPacket.size = sizeof(CLIENT_PACKET::MovePacket);
			if (!g_clients[i].connected) continue;
			if (!g_clients[i].isInGameRoom) continue;
			if (!g_clients[i].isMove) {
				if (g_clients[i].last_move_time + 2s < high_resolution_clock::now()) {
					g_clients[i].isMove = true;
					g_clients[i].last_move_time = g_clients[i].last_move_start_time = high_resolution_clock::now();
					switch (rand() % 4) {
					case 0: sendPacket.direction = DIRECTION::FRONT; break;
					case 2: sendPacket.direction = DIRECTION::BACK; break;
					case 1: sendPacket.direction = DIRECTION::LEFT; break;
					case 3: sendPacket.direction = DIRECTION::RIGHT; break;
					}
					sendPacket.t = std::chrono::utc_clock::now();
					sendPacket.type = CLIENT_PACKET::MOVE_KEY_DOWN;
					g_clients[i].last_move_time = high_resolution_clock::now();
					SendPacket(i, &sendPacket);
				}
			}
			else if (g_clients[i].last_move_start_time + 3s < high_resolution_clock::now()) {
				g_clients[i].isMove = false;
				g_clients[i].last_move_time = high_resolution_clock::now();
				sendPacket.type = CLIENT_PACKET::MOVE_KEY_UP;
				sendPacket.t = std::chrono::utc_clock::now();
				sendPacket.direction = DIRECTION::FRONT;
				SendPacket(i, &sendPacket);
			}
		}
	}
}

void InitializeNetwork()
{
	for (auto& cl : g_clients) {
		cl.connected = false;
		cl.id = INVALID_ID;
	}

	for (auto& cl : client_map) cl = -1;
	num_connections = 0;
	last_connect_time = high_resolution_clock::now();

	WSADATA	wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);

	g_hiocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, NULL, 0);

	for (int i = 0; i < 6; ++i)
		worker_threads.push_back(new std::thread{ Worker_Thread });

	test_thread = thread{ Test_Thread };
}

void ShutdownNetwork()
{
	test_thread.join();
	for (auto pth : worker_threads) {
		pth->join();
		delete pth;
	}
}

void Do_Network()
{
	return;
}

void GetPointCloud(int* size, float** points)
{
	int index = 0;
	for (int i = 0; i < num_connections; ++i)
		if (true == g_clients[i].connected) {
			point_cloud[index * 2] = static_cast<float>(g_clients[i].x);
			point_cloud[index * 2 + 1] = static_cast<float>(g_clients[i].y);
			index++;
		}

	*size = index;
	*points = point_cloud;
}

