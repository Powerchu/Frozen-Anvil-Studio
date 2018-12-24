/* HEADER *********************************************************************************/
/*!
\file	Delegate.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Type erased member function

	Reference:
	https://www.codeproject.com/Articles/7150/Member-F

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _DELEGATE_H_
#define _DELEGATE_H_

#include "Utility/Meta.h"
#include "Utility/MetaAlgorithms.h"
#include "Utility/Utility.h"

#include "Allocator/DefaultAlloc.h"


template <typename>
class Delegate;

template <typename R_t, typename ... P>
class Delegate<R_t (P...)>
{
public:
	// ====================================== CONSTRUCTORS ======================================= // 

	template <typename Ty, typename = Ut::EnableIf_t<!Ut::IsSame<Ut::Decay_t<Ty>, Delegate>::value>>
	explicit Delegate(Ty&&) noexcept;

	template <typename C>
	explicit Delegate(C*, R_t(C::*)(P...)) noexcept;
	template <typename C>
	explicit Delegate(C*, R_t(C::*)(P...) const) noexcept;
	template <typename C>
	explicit Delegate(C&&, R_t(Ut::Decay_t<C>::*)(P...)) noexcept;
	template <typename C>
	explicit Delegate(C&&, R_t(Ut::Decay_t<C>::*)(P...) const) noexcept;

	Delegate(Delegate&&) noexcept;
	Delegate(const Delegate&) = delete;


	// =================================== CONTAINER FUNCTIONS =================================== // 

	template<typename ... Ty>
	R_t operator()(Ty&&...);


	// ======================================== OPERATORS ======================================== // 

	explicit operator bool() const;

	template <typename Ty>
	auto operator = (Ty&&) -> Ut::EnableIf_t <!Ut::IsSame <Ut::Decay_t<Ty>, Delegate>::value, Delegate&>;

	Delegate& operator = (Delegate&&);
	Delegate& operator = (const Delegate&);


private:

	struct __single_inheritance D;
	using Func_t = R_t(D::*)(P...);

	template <typename>
	struct Convertor;

	template <typename R, typename ... T>
	struct Convertor<R(T...)>
	{
		R operator() (T ... _args)
		{
			return static_cast<R(*)(T...)>(reinterpret_cast<void*>(this))(_args...);
		}
	};

	D*  mObj;
	Func_t mFunc;

	template <typename Ty>
	inline auto DeduceFunc(void) noexcept;

	template <typename Ty>
	inline auto GetObject(Ty&&) noexcept(Ut::IsLvalueRef<Ty>::value || Ut::IsEmptyClass<Ut::Decay_t<Ty>>::value);
	template <typename Ty>
	inline decltype(auto) DeduceObject(Ty&&) noexcept;

	template <typename To, typename From>
	decltype(auto) MemPtrCast(From&&) noexcept;
};






// ============================================ FUNCTION DEFINITIONS ============================================ //

template <typename R_t, typename ... P> template <typename Ty, typename>
inline Delegate<R_t (P...)>::Delegate(Ty&& _f) noexcept
	: Delegate{ DeduceObject(Ut::Fwd<Ty>(_f)), DeduceFunc<Ty>() }
{
}

template <typename R_t, typename ... P> template <typename C>
inline Delegate<R_t (P...)>::Delegate(C* _obj, R_t(C::*_f)(P...)) noexcept
	: mObj{ reinterpret_cast<D*>(_obj) }, mFunc{ MemPtrCast<Func_t>(_f) }
{
}

template <typename R_t, typename ... P> template <typename C>
inline Delegate<R_t (P...)>::Delegate(C* _obj, R_t(C::*_f)(P...) const) noexcept
	: mObj{ reinterpret_cast<D*>(_obj) }, mFunc{ MemPtrCast<Func_t>(_f) }
{
}

template <typename R_t, typename ... P> template <typename C>
inline Delegate<R_t (P...)>::Delegate(C&& _obj, R_t(Ut::Decay_t<C>::*_f)(P...)) noexcept
	: mObj{ GetObject(Ut::Fwd<C>(_obj)) }, mFunc{ MemPtrCast<Func_t>(_f) }
{
}

template <typename R_t, typename ... P> template <typename C>
inline Delegate<R_t (P...)>::Delegate(C&& _obj, R_t(Ut::Decay_t<C>::*_f)(P...) const) noexcept
	: mObj{ GetObject(Ut::Fwd<C>(_obj)) }, mFunc{ MemPtrCast<Func_t>(_f) }
{
}

template <typename R_t, typename ... P>
inline Delegate<R_t (P...)>::Delegate(Delegate&& _rhs) noexcept
	: mObj{ Ut::Move(_rhs.mObj) }, mFunc{ Ut::Move(_rhs.mFunc) }
{
	_rhs.mObj  = nullptr;
	_rhs.mFunc = nullptr;
}


template <typename R_t, typename ... P> template <typename ... Ty>
inline R_t Delegate<R_t (P...)>::operator()(Ty&& ... _args)
{
	return (mObj->*mFunc)(Ut::Fwd<Ty>(_args)...);
}


template <typename R_t, typename ... P> template <typename Ty>
inline auto Delegate<R_t (P...)>::DeduceFunc(void) noexcept
{
	if constexpr (Ut::IsSame<Ut::Decay_t<Ty>, R_t(*)(P...)>::value)
	{
		return &Convertor<R_t(P...)>::operator();
	}
	else
	{
		// Assume functor and try to get ptr to member
		using MemPtr  = R_t(Ut::Decay_t<Ty>::*)(P...);
		using MemPtrC = R_t(Ut::Decay_t<Ty>::*)(P...) const;
		using Result  = typename Ut::MemberFinder<Ut::Decay_t<Ty>, MemPtr, MemPtrC>::type;

		static_assert(Ut::IsSame<Result, MemPtrC>::value);

		return static_cast<Result>(&Ut::Decay_t<Ty>::operator());
	}
}

template <typename R_t, typename ... P> template <typename Ty>
inline decltype(auto) Delegate<R_t (P...)>::DeduceObject(Ty&& _f) noexcept
{
	if constexpr (Ut::IsSame<Ut::Decay_t<Ty>, R_t(*)(P...)>::value)
	{
		return reinterpret_cast<Convertor<R_t(P...)>*>(reinterpret_cast<void*>(static_cast<R_t(*)(P...)>(_f)));
	}
	else
	{
		return Ut::Fwd<Ty>(_f);
	}
}

template <typename R_t, typename ... P> template <typename Ty>
inline auto Delegate<R_t (P...)>::GetObject(Ty&& _obj) noexcept(Ut::IsLvalueRef<Ty>::value || Ut::IsEmptyClass<Ut::Decay_t<Ty>>::value)
{
	if constexpr (Ut::IsLvalueRef<Ty>::value)
	{
		return reinterpret_cast<D*>(&_obj);
	}
	else if constexpr(Ut::IsEmptyClass<Ut::Decay_t<Ty>>::value)
	{
		UNUSED_PARAMETER(_obj);

		// We cheat the 'this' pointer if there are no lambda captures
		// or if the functor has no member variables, prevents allocation
		return nullptr;
	}
	else
	{
    // Here we're ask ourselves do we want to support handling the lifetime of the object
    // If yes, we have to figure out if we're required to delete it
    // compilcating the implementation and adding the need to somehow store that information
    // If no, we keep our implementation much simpler
		static_assert(false);
		return reinterpret_cast<D*>(Dystopia::DefaultAllocator<Ty>::ConstructAlloc(Ut::Fwd<Ty>(_obj)));
	}
}

template <typename R_t, typename ... P> template <typename To, typename From>
inline decltype(auto) Delegate<R_t (P...)>::MemPtrCast(From&& _f) noexcept
{
	constexpr auto fptr_sz = sizeof(void(*)(void));
	constexpr auto item_sz = sizeof(Ut::Decay_t<From>);

  // We should actually never need this case
	if constexpr (Ut::IsSame<Ut::Decay_t<From>, To>::value)
	{
		return Ut::Fwd<From>(_f);
	}
  // Compiler allows the conversion, simply cast and return
	else if constexpr (fptr_sz == item_sz)
	{
		return reinterpret_cast<Func_t>(_f);
	}
  // Either 1. Virtual Function 2. Virtual Inheritance, 3. Multi Inheritance
	else if constexpr (fptr_sz + sizeof(int) == item_sz)
	{
		static_assert(false);
	}
  // Either 1. Unknown class type, 2. Has everything
	else if constexpr (fptr_sz + 2 * sizeof(int) == item_sz)
	{
		static_assert(false);
	}
}



#endif		// INCLUDE GUARD

