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
typedef unsigned int EventCallbackID;
constexpr EventID FNV_PRIME = 16777619u;
constexpr EventID OFFSET_BASIS = 2166136261u;
EventID EventHash(const char* _eventName);

template<unsigned int N>
constexpr EventID EventHashC(const char(&_eventName)[N], unsigned int I = N)
{
	return I == 1 ? (OFFSET_BASIS ^ _eventName[0]) * FNV_PRIME :
					(EventHashC(_eventName, I - 1) ^ (_eventName[I - 1])) * FNV_PRIME;
}

template<typename A, typename ... Bs>
struct Function_Traits
{};

template<typename ReturnT, typename ... Args, Args ... params>
struct Function_Traits<ReturnT(&)(Args...), params ...>
{
	static constexpr bool is_mem_fn = false;
	using return_t = ReturnT;
	std::tuple arg_tuple = std::make_tuple<Args...>;
	std::tuple param_tuple = std::make_tuple<std::forward<Args>(params) ...>;
};

template<class Caller, typename ReturnT, typename ... Args, Args ... params>
struct Function_Traits<ReturnT(Caller::*)(Args...), params ...>
{
	static constexpr bool is_mem_fn = true;
	using return_t = ReturnT;
	using class_t = Caller;
	std::tuple arg_tuple = std::make_tuple<Args...>;
	std::tuple param_tuple = std::make_tuple<std::forward<Args>(params) ...>;
};

template<typename Fn>
struct FunctionWrapper
{};

template<typename ReturnType, typename ... Args>
struct FunctionWrapper<ReturnType(&)(Args...)>
{
	FunctionWrapper(ReturnType(&f)(Args...))
		: fn{ f }
	{}

	ReturnType operator()(Args ... params)
	{
		return fn(params ...);
	}
	ReturnType(&fn)(Args...);
};

template<typename ReturnType, typename Caller, typename ... Args>
struct FunctionWrapper<ReturnType(Caller::*)(Args...)>
{
	FunctionWrapper(ReturnType(Caller::*fn)(Args...), Caller const * const call)
		:pCaller{ call }, mfn{ fn }
	{}

	ReturnType operator()(Args ... params)
	{
		return (pCaller->*mfn)(params ...);
	}
	ReturnType(Caller::*mfn)(Args...);
	Caller* pCaller;
};


class EventCallback
{
public:

private:

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

