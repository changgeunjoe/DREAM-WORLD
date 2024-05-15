#pragma once
#include "../PCH/stdafx.h"
#include <../SingletonBase.h>

class ThreadManager :public SingletonBase<ThreadManager>
{
	friend SingletonBase;
private:
	ThreadManager();
	~ThreadManager();
public:
	void CreateThread(std::thread&& th);
	void Join();
private:
	std::vector<std::thread> m_threads;
	//std::mutex m_threadLock;
};

//iocp workerThread
//Timer
//DB
//Matching
