#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS 1
#pragma comment(lib, "mswsock.lib")
#pragma comment(lib, "WS2_32.lib")

#include <WS2tcpip.h>
#include <MSWSock.h>

#include <Windows.h>

#include <chrono>

#include <mutex>

#include <vector>
#include <unordered_map>
#include <array>

#include <iostream>