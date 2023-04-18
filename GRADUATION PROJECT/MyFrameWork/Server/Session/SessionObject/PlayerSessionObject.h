#pragma once
#include "SessionObject.h"
#include "../ExpOver.h"
#include "../../PCH/stdafx.h"

class PlayerSessionObject : public SessionObject
{
private:
	std::wstring m_playerName;
	/*short	m_level;
	short	m_exp;*/
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
	DIRECTION m_inputDirection = DIRECTION::IDLE;
	DIRECTION m_prevDirection = DIRECTION::IDLE;
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
	void SetRole(ROLE r) {
		m_InGameRole = r;
		switch (r)
		{
		case WARRIOR:
		{
			SetPosition(XMFLOAT3(300, 0, 100));
			m_maxHp = m_hp = 400;
			m_attackDamage = 150;
		}
		break;
		case PRIEST:
		{
			SetPosition(XMFLOAT3(270, 0, 80));
			m_maxHp = m_hp = 480;
			m_attackDamage = 30;
		}
		break;
		case TANKER:
		{
			SetPosition(XMFLOAT3(230, 0, 60));
			m_maxHp = m_hp = 600;
			m_attackDamage = 60;
		}
		break;
		case ARCHER:
		{
			SetPosition(XMFLOAT3(200, 0, 40));
			m_maxHp = m_hp = 250;
			m_attackDamage = 200;
		}
		break;
		default:
			break;
		}
	}
	void ResetRole() { m_InGameRole = ROLE::NONE_SELECT; }
	ROLE GetRole() {
		return m_InGameRole;
	}
	void SetRoomId(std::string& roomId) { m_roomId = roomId; }
	std::string GetRoomId() { return m_roomId; }
};
