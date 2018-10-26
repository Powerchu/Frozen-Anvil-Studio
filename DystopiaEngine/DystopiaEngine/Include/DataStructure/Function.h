#pragma once
#ifndef _FUNCTION_H
#define _FUNCTION_H
#include "Utility/Meta.h"
#include "Utility/Utility.h"
#include "DataStructure/SharedPtr.h"
#include "Globals.h"
#include <type_traits>


/*NOT DEFINED*/
template<typename T>
struct _DLL_EXPORT Function;

template<typename Ret_t, typename ... Param_t>
struct _DLL_EXPORT Function<Ret_t (Param_t...)>
{
protected:
	struct Concept
	{
		virtual Concept* Duplicate() const   = 0;
		virtual Ret_t operator()(Param_t...) = 0;
		virtual ~Concept(){}
	};

	template<typename T>
	struct Wrapper : Concept
	{
		template<typename U>
		Wrapper(U && _obj)
			:mObj{Ut::Forward<U>(_obj)}
		{
			
		}
		virtual Wrapper* Duplicate() const override
		{
			return new Wrapper{ *this };
		};
		virtual Ret_t operator()(Param_t... _params) override
		{
			return mObj(std::forward<Param_t>(_params)...);
		};

		T mObj;
	};

public:
	template<typename ... Func, typename SFINAE = Ut::EnableIf_t<!Ut::IsSame<Ut::Decay_t<Func>,Function>::value && std::is_nothrow_invocable<Func,Param_t...>()>>
	Function(Func && _f)
		:mpWrapper{CreateShared(_f)}
	{
		
	}
	template<typename ... T>
	Ret_t operator()(T&&... _params);

	operator bool();

private:
	SharedPtr<Concept> mpWrapper;
};


template <typename Ret_t, typename ... Param_t>
template <typename ... T>
inline Ret_t Function<Ret_t(Param_t...)>::operator()(T&&... _params)
{
	return mpWrapper->operator()(std::forward<T>(_params));
}

template <typename Ret_t, typename ... Param_t>
Function<Ret_t(Param_t...)>::operator bool()
{
	return mpWrapper;
}


#endif
