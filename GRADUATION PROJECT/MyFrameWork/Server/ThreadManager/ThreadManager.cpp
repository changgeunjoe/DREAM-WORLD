#include "stdafx.h"
#include "ThreadManager.h"

ThreadManager::ThreadManager()
{
	spdlog::info("ThreadManager::ThreadManager() - ThreadManager Constructor");
	m_threads.reserve(12);
}

ThreadManager::~ThreadManager()
{
	spdlog::info("ThreadManager::~ThreadManager() - ThreadManager Destructor");
}

void ThreadManager::CreateThread(std::thread&& th)
{
	spdlog::debug("ThreadManager::CreateThread() - Create Thread");
	m_threads.push_back(std::move(th));
}

void ThreadManager::Join()
{
	for (auto& th : m_threads) {
		if (th.joinable())
			th.join();
	}
	spdlog::debug("ThreadManager::Join() - threads.join()");
}
