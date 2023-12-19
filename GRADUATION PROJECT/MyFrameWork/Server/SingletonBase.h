#pragma once
#include "PCH/stdafx.h"

template<class T>
class SingletonBase
{
protected:
	SingletonBase() = default;
	virtual ~SingletonBase() {}
	SingletonBase(SingletonBase const&) = delete;
	SingletonBase& operator=(SingletonBase const&) = delete;
public:
	static T& GetInstance()
	{
		//thread-safe??
		static T instance;
		return instance;
	}
};
