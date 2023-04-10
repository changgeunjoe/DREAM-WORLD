#pragma once
#include "SessionObject.h"
#include "../ExpOver.h"


class PlayerSessionObject : public SessionObject
{
private:
	std::wstring m_playerName;
	short	m_level;
	short	m_exp;
	short	m_hp;
	short	m_maxHp;
	short	m_attackDamage;
	unsigned char m_mouseInput;

private:
	ExpOver				m_exOver;
	SOCKET				m_socket;
	int					m_prevBufferSize;
	ROLE				m_InGameRole = ROLE::NONE_SELECT;
private:
	std::string m_roomId;
public:
	PlayerSessionObject(Session* session, SOCKET& sock);
	virtual	~PlayerSessionObject();

public:
	void SetName(std::wstring& inputWst)
	{
		m_playerName = inputWst;
	}
	std::wstring& GetName() { return m_playerName; }
public:
	void Recv() override;
	void Send(void* p);
	void ConstructPacket(int ioByte);
public:
	bool AdjustPlayerInfo(DirectX::XMFLOAT3& position); // , DirectX::XMFLOAT3& rotate
public:
	virtual void AutoMove() override;
	virtual void StartMove(DIRECTION d) override;
	virtual void StopMove() override;
	virtual void ChangeDirection(DIRECTION d) override;
public:
	void Move(float fDistance);
	void SetDirection(DIRECTION d);
	void SetMouseInput(unsigned char mouseInput);
public:
	virtual void Rotate(ROTATE_AXIS axis, float angle) override;
public:
	virtual const DirectX::XMFLOAT3 GetPosition() override;
	virtual const DirectX::XMFLOAT3 GetRotation() override;
public:
	char* GetPlayerInfo();
public:
	void SetRole(ROLE r) { m_InGameRole = r; }
	void ResetRole() { m_InGameRole = ROLE::NONE_SELECT; }
	ROLE GetRole() {
		return m_InGameRole;
	}
	void SetRoomId(std::string& roomId) { m_roomId = roomId; }
	std::string GetRoomId() { return m_roomId; }
};
