/* HEADER *********************************************************************************/
/*!
\file	Function.h
\author Tan Jie Wei Jacky (%), Keith (%)
\par    email: t.jieweijacky\@digipen.edu
\par    email: <keithemailhere>\@digipen.edu
\brief
	Type erased function

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _FUNCTION_H
#define _FUNCTION_H

#include "Globals.h"
#include "Utility/Meta.h"
#include "Utility/Utility.h"

#include "Allocator/DefaultAlloc.h"


/* NOT DEFINED */
template<typename T>
struct _DLL_EXPORT Function;

template<typename Ret_t, typename ... Param_t>
struct _DLL_EXPORT Function<Ret_t (Param_t...)>
{
public:
	// ====================================== CONSTRUCTORS ======================================= // 
	
	template<typename T, typename = Ut::EnableIf_t <!Ut::IsSame <Ut::Decay_t<T>, Function>::value>>
	Function(T&&);
	
	Function(Function&&);
	Function(const Function&);

	~Function(void) noexcept;


	// =================================== CONTAINER FUNCTIONS =================================== // 

	template<typename ... T>
	Ret_t operator()(T&&...);


	// ======================================== OPERATORS ======================================== // 

	explicit operator bool() const;

	template <typename T>
	auto operator = (T&&) -> Ut::EnableIf_t <!Ut::IsSame <Ut::Decay_t<T>, Function>::value, Function&>;

	Function& operator = (Function&&);
	Function& operator = (const Function&);

private:

	inline void Clear(void) noexcept;

	template <typename T>
	inline void Assign(T&&);

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
			return Dystopia::DefaultAllocator<Wrapper>::ConstructAlloc(*this);
		};
		virtual Ret_t operator()(Param_t... _params) override
		{
			return mObj(Ut::Forward<Param_t>(_params)...);
		};

		Ut::RemoveRef_t<T> mObj;
	};

	template <size_t N>
	struct CopyHelper
	{
		char DUMMY[N];
	};

	template <typename T, typename, typename, typename ... R>
	struct MemberFinderReal : public MemberFinderReal <T, Ut::MetaExtract_t<0, R...>, R...>
	{};

	template <typename T, typename _1, typename _2>
	struct MemberFinderReal<T, _1, _2>;

	template <typename T, typename Ty, typename ... R>
	struct MemberFinderReal<T, Ut::Type_t<decltype(static_cast<Ty>(&T::operator()))>, Ty, R...>
	{
		using type = Ty;
	};

	template <typename T, typename F, typename ... R>
	struct MemberFinder : public MemberFinderReal<T, F, F, R...>
	{

	};

	using SmallType = Ret_t(*)(Param_t...);
	using RealType = Concept*;

	alignas(Ut::MetaMax<size_t, sizeof(RealType), sizeof(SmallType)>::value)
		char buffer[Ut::Constant<size_t, sizeof(RealType) * 2>::value];

	constexpr static auto info_bit = sizeof(buffer) - 1;
};






// ============================================ FUNCTION DEFINITIONS ============================================ // 


template <typename Ret_t, typename ... Param_t>
template <typename T, typename>
Function<Ret_t (Param_t...)>::Function(T&& _func)
	: buffer{}
{
	Assign(Ut::Forward<T>(_func));
}

template<typename Ret_t, typename ...Param_t>
inline Function<Ret_t(Param_t...)>::Function(Function&& _rhs)
	: buffer{}
{
	*this = Ut::Move(_rhs);
}

template<typename Ret_t, typename ...Param_t>
inline Function<Ret_t(Param_t...)>::Function(const Function& _rhs)
	: buffer{ }
{
	*this = _rhs;
}


template <typename Ret_t, typename ... Param_t>
inline Function<Ret_t (Param_t...)>::~Function(void) noexcept
{
	Clear();
}


template <typename Ret_t, typename ... Param_t>
template <typename ... T>
inline Ret_t Function<Ret_t(Param_t...)>::operator()(T&& ..._args)
{
	if (!buffer[info_bit])
	{
		return reinterpret_cast<SmallType const&>(buffer)(Ut::Forward<T>(_args)...);
	}
	else if (2 == buffer[info_bit])
	{
		return (reinterpret_cast<Concept&>(buffer))(Ut::Forward<T>(_args)...);
	}

	return (*reinterpret_cast<RealType const&>(buffer))(Ut::Forward<T>(_args)...);
}



// ============================================ OPERATOR OVERLOADING ============================================ // 


template <typename Ret_t, typename ... Param_t>
Function<Ret_t(Param_t...)>::operator bool() const
{
	return !!reinterpret_cast<void const * const&>(buffer);
}


template <typename Ret_t, typename ... Param_t>
template <typename T>
inline auto Function<Ret_t(Param_t...)>::operator = (T&& _func) -> Ut::EnableIf_t <!Ut::IsSame <Ut::Decay_t<T>, Function>::value, Function&>
{
	Clear();
	Assign(Ut::Forward<T>(_func));

	return *this;
}

template <typename Ret_t, typename ... Param_t>
inline Function<Ret_t(Param_t...)>& Function<Ret_t (Param_t...)>::operator = (Function&& _rhs)
{
	Ut::Swap(
		reinterpret_cast<CopyHelper<sizeof(buffer)>&>(buffer),
		reinterpret_cast<CopyHelper<sizeof(buffer)>&>(_rhs.buffer)
	);

	return *this;
}

template <typename Ret_t, typename ... Param_t>
inline Function<Ret_t (Param_t...)>& Function<Ret_t (Param_t...)>::operator = (const Function& _rhs)
{
	if (this != &_rhs)
	{
		Clear();

		if (!_rhs.buffer[_rhs.info_bit])
		{
			reinterpret_cast<SmallType&>(buffer) = reinterpret_cast<SmallType const&>(_rhs.buffer);
		}
		else
		{
			buffer[info_bit] = 1;

			if (2 == _rhs.buffer[_rhs.info_bit])
				reinterpret_cast<RealType&>(buffer) = reinterpret_cast<Concept const*>(_rhs.buffer)->Duplicate();
			else
				reinterpret_cast<RealType&>(buffer) = reinterpret_cast<RealType const&>(_rhs.buffer)->Duplicate();
		}
	}

	return *this;
}


template <typename Ret_t, typename ... Param_t>
inline void Function<Ret_t (Param_t...)>::Clear(void) noexcept
{
	if (!buffer[info_bit])
		return;

	else if (2 == buffer[info_bit])
	{
		reinterpret_cast<Concept&>(buffer).~Concept();
	}
	else
	{
		Dystopia::DefaultAllocator<Concept>::DestructFree(reinterpret_cast<RealType&>(buffer));
		reinterpret_cast<RealType&>(buffer) = static_cast<RealType>(nullptr);
	}

	buffer[info_bit] = 0;
}

template <typename Ret_t, typename ... Param_t> template <typename T>
inline void Function<Ret_t(Param_t...)>::Assign(T&& _func)
{
	// Plain function pointer or lambdas with no captures
	if constexpr (Ut::IsConvertible<T, SmallType>::value)
	{
		reinterpret_cast<SmallType&>(buffer) = static_cast<SmallType>(Ut::Forward<T>(_func));
	}

	// Struct or class with no members!
	else if constexpr (Ut::IsEmptyClass<T>::value)
	{
		using MemPtr_t  = Ret_t(T::*)(Param_t...);
		using MemPtrC_t = Ret_t(T::*)(Param_t...) const;
		using Result_t = typename MemberFinder<Ut::Decay_t<T>, MemPtr_t, MemPtrC_t>::type;

		// Sadly, we cannot forward the params into the lambda
		reinterpret_cast<SmallType&>(buffer) = static_cast<SmallType>([](Param_t ..._args) -> Ret_t {
			constexpr static auto pRealFunc = static_cast<Result_t>(&T::operator());
			return (static_cast<T*>(nullptr)->*pRealFunc)(_args...);
		});

		_func;
	}

	// Object fits within our buffer
	else if constexpr (8 + sizeof(T) < sizeof(buffer))
	{
		buffer[info_bit] = 2;
		
		::new (buffer) Wrapper<T> { Ut::Forward<T>(_func) };
	}

	// The ugly and slow case
	else
	{
		buffer[info_bit] = 1;

		reinterpret_cast<RealType&>(buffer) = Dystopia::DefaultAllocator<Wrapper<T>>::ConstructAlloc(Ut::Forward<T>(_func));
	}
}



#endif		// INCLUDE GUARD

