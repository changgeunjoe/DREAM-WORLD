#pragma once

class Session;
class SessionObject
{
protected:
	Session* m_session;
private:
	//	member
public:
	SessionObject(Session* session);
	virtual ~SessionObject();
};
