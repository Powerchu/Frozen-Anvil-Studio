/* HEADER *********************************************************************************/
/*!
\file	EventSystem.h
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _EVENTSYSTEM_H_
#define _EVENTSYSTEM_H_
#include "DataStructure\AutoArray.h"
#include <utility>
#include <tuple>

typedef unsigned int EventID;
constexpr EventID FNV_PRIME = 16777619u;
constexpr EventID OFFSET_BASIS = 2166136261u;
EventID EventHash(const char* _eventName);

template<unsigned int N>
constexpr EventID EventHashC(const char(&_eventName)[N], unsigned int I = N)
{
	return I == 1 ? (OFFSET_BASIS ^ _eventName[0]) * FNV_PRIME :
					(EventHashC(_eventName, I - 1) ^ (_eventName[I - 1])) * FNV_PRIME;
}

template<typename A>
struct FunctionWrapper
{};

template<typename ReturnType, typename ... Args>
struct FunctionWrapper<ReturnType(&)(Args...)>
{
	FunctionWrapper(ReturnType(&rhs)(Args...))
		: fcallback{ rhs }
	{}

	ReturnType operator()(Args ... params)
	{
		return fcallback(params ...);
	}

	ReturnType(&fcallback)(Args...);
};

template<typename ReturnType, typename C, typename ... Args>
struct FunctionWrapper<ReturnType(C::*)(Args...)>
{
	FunctionWrapper(ReturnType(C::*mfp)(Args...), C*  user)
		: fcallback{ mfp }, caller{ user }
	{}

	ReturnType operator()(Args ... params)
	{
		return (caller->*fcallback)(params ...);
	}

	C* caller;
	ReturnType(C::*fcallback)(Args...);
};

template<typename ReturnType, typename ... Args>
auto Bind(ReturnType(&f)(Args...))
{
	FunctionWrapper<ReturnType(&)(Args...)> func = f;
	return func;
}

template<typename ReturnType, typename C, typename ... Args>
auto Bind(ReturnType(C::*f)(Args...), C* user)
{
	FunctionWrapper<ReturnType(C::*)(Args...)> func = { f, user };
	return func;
}

class EventCallback
{
public:

	EventCallback(void(&rhs)(void))
		: pModelEvent{ }
	{}

	template<class Caller>
	EventCallback(void(Caller::*rhs)(void), Caller* const caller)
		: pModelEvent{}
	{}

	~EventCallback()
	{
		delete pModelEvent;
	}

	void Fire() const
	{
		pModelEvent->Fire();
	}

private:
	struct Concept
	{
		virtual void Fire() const;
		virtual ~Concept() {}
	};

	Concept * const pModelEvent;
};

class Event
{
public:

private:

};

class EventSystem
{
public:

private:

};






#endif //_EVENTSYSTEM_H_

