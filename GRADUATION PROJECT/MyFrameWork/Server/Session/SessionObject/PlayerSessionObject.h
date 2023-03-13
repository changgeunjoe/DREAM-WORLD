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

private:
	ExpOver				m_exOver;
	SOCKET				m_socket;
	int					m_prevBufferSize;
	ROLE				m_InGameRole = ROLE::NONE_SELECT;
private:
	std::wstring m_roomId;
public:
	PlayerSessionObject(Session* session, SOCKET& sock);
	virtual	~PlayerSessionObject();

public:
	void SetName(std::wstring& inputWst)
	{
		m_playerName = inputWst;
	}
public:
	void Recv();
	void Send(void* p);
	void ConstructPacket(int ioByte);
public:
	bool AdjustPlayerInfo(DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& rotate);
public:
	virtual void AutoMove() override;
	virtual void StartMove(DIRECTION d) override;
	virtual void StopMove() override;
	virtual void ChangeDirection(DIRECTION d) override;
public:
	virtual void Rotate(ROTATE_AXIS axis, float angle) override;
public:
	virtual const DirectX::XMFLOAT3 GetPosition() override;
	virtual const DirectX::XMFLOAT3 GetRotation() override;
public:
	char* GetPlayerInfo();
};
