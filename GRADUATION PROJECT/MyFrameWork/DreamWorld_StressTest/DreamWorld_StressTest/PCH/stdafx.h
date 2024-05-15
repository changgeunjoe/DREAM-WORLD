#pragma once

#include <WinSock2.h>
#include <winsock.h>
#include <Windows.h>
#include <WS2tcpip.h>
#include <iostream>
#include <thread>
#include <vector>
#include <unordered_set>
#include <mutex>
#include <atomic>
#include <chrono>
#include <queue>
#include <array>
#include <memory>
#include <random>

#include <string>
#include <string_view>

#include "tbb/concurrent_priority_queue.h"
#include "tbb/concurrent_queue.h"


#include <windows.h>		// Header File For Windows
#include <math.h>			// Header File For Windows Math Library
#include <stdio.h>			// Header File For Standard Input/Output
#include <stdarg.h>			// Header File For Variable Argument Routines
#include <gl\gl.h>			// Header File For The OpenGL32 Library
#include <gl\glu.h>			// Header File For The GLu32 Library
#include <atomic>
#include <memory>

#include <codecvt>
#include <atlconv.h>
#include <strsafe.h>
//#include <gl\glaux.h>		// Header File For The Glaux Library

#include <DirectXMath.h>
using namespace DirectX;
#pragma comment (lib, "ws2_32.lib")
#pragma comment (lib, "opengl32.lib")
#pragma comment (lib, "glu32.lib")
//#pragma warning(disable: 4996)
//#define _CRT_SECURE_NO_WARNINGS
//#define _WINSOCK_DEPRECATED_NO_WARNINGS

enum class IOCP_OP_CODE
{
	OP_NONE,
	OP_SEND,
	OP_RECV
};

enum PLAYER_STATE
{
	FREE,
	ALLOC,
	IN_GAME,
	IN_GAME_ROOM
};

enum DIRECTION : char
{
	IDLE = 0x00,
	FRONT = 0x01,
	RIGHT = 0x02,
	LEFT = 0x04,
	BACK = 0x08
};

enum ROTATE_AXIS :char
{
	X, Y, Z
};

enum ROLE :char {
	NONE_SELECT = 0x00,
	WARRIOR = 0x01,
	PRIEST = 0x02,
	TANKER = 0x04,
	ARCHER = 0x08
};
