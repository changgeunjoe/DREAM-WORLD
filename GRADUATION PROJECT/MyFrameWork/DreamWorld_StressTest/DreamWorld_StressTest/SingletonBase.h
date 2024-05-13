#pragma once
#include "PCH/stdafx.h"

template<class T>
class SingletonBase
{
protected:
	SingletonBase() = default;
	virtual ~SingletonBase() {}
	//�̱����̱� ������, ���簡 �Ͼ�� �� ��.
	SingletonBase(const SingletonBase&) = delete;
	SingletonBase& operator=(const SingletonBase&) = delete;
public:
	static T& GetInstance()
	{
		//thread-safe??
		static T instance;
		return instance;
	}
};
