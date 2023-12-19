#pragma once
#include "../../SingletonBase.h"


class ExpOver;
class ExpOverBuffer;
class ExpOverWsaBuffer;

class ExpOverManager : public SingletonBase< ExpOverManager>
{
	friend SingletonBase;
	//send, DB, Event overlapped包访 磊丰备炼 包府
private:
	ExpOverManager();
	~ExpOverManager();

	concurrency::concurrent_queue<ExpOverBuffer*> m_bufferQueue;
	concurrency::concurrent_queue<ExpOverWsaBuffer*> m_wsaBufferQueue;
	concurrency::concurrent_queue<ExpOver*> m_expOverQueue;
public:
	void Initailize();

	ExpOver* CreateExpOver(const IOCP_OP_CODE&& opCode);
	ExpOver* CreateExpOverBuffer(const IOCP_OP_CODE&& opCode, char* data);
	ExpOver* CreateExpOverBuffer(const IOCP_OP_CODE&& opCode, char* data, int dataSize);
	ExpOverWsaBuffer* CreateExpOverWsaBuffer(const IOCP_OP_CODE&& opCode, const char* data);

	void DeleteExpOver(ExpOver* delExpOver);
	void DeleteExpOverBuffer(ExpOverBuffer* delExpOver);
	void DeleteExpOverWsaBuffer(ExpOverWsaBuffer* delExpOver);
};
