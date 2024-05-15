//#pragma once
//#include "../../PCH/stdafx.h"
//#include "../SessionObject.h"
//
//
//class MeteoSessionObject :public SessionObject
//{
//public:
//	MeteoSessionObject();
//	~MeteoSessionObject();
//private:
//	bool m_active = false;
//	int m_id = -1;
//public:
//	void InitSetDamage();
//	void SetRoomId(int roomId);
//	void SetId(int id);
//	int GetId() { return m_id; }
//	void SetStart(float& speed, XMFLOAT3& srcPosition);
//public:
//	bool GetActive() { return m_active; }
//	void OnActive() { m_active = true; }
//public:
//	virtual bool Move(float elapsedTime);
//	void Rotate(ROTATE_AXIS axis, float angle) override {}
//	float GetSpeed() { return m_speed; }
//};
//
