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

#include <memory> // std::addressof


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
	explicit Delegate(C*, R_t(Ut::Decay_t<C>::*)(P...)) noexcept;
	template <typename C>
	explicit Delegate(C*, R_t(Ut::Decay_t<C>::*)(P...) const) noexcept;
	template <typename C>
	explicit Delegate(C&&, R_t(Ut::Decay_t<C>::*)(P...)) noexcept;
	template <typename C>
	explicit Delegate(C&&, R_t(Ut::Decay_t<C>::*)(P...) const) noexcept;

	Delegate(Delegate&&) noexcept;
	Delegate(const Delegate&) = delete;

	~Delegate(void) noexcept;


	// =================================== CONTAINER FUNCTIONS =================================== // 

	template<typename ... Ty>
	R_t operator()(Ty&&...);


	// ======================================== OPERATORS ======================================== // 

	explicit operator bool() const;

	template <typename Ty>
	auto operator = (Ty&&) -> Ut::EnableIf_t <!Ut::IsSame <Ut::Decay_t<Ty>, Delegate>::value, Delegate&>;

	Delegate& operator = (Delegate&&) noexcept;
	Delegate& operator = (const Delegate&) = delete;


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

	void(*mAlloc)(D*);
	D *mDel = nullptr, *mObj;
	Func_t mFunc;

	template <typename Ty>
	inline auto DeduceFunc(void) noexcept;

	template <typename Ty>
	inline auto GetObject(Ty&&) noexcept(Ut::IsLvalueRef<Ty>::value || Ut::IsEmptyClass<Ut::Decay_t<Ty>>::value);
	template <typename Ty>
	inline decltype(auto) DeduceObject(Ty&&) noexcept;

	template <typename To, typename From, typename Ty>
	decltype(auto) MemPtrCast(From&&, Ty&&) noexcept;
};






// ============================================ FUNCTION DEFINITIONS ============================================ //

template <typename R_t, typename ... P> template <typename Ty, typename>
inline Delegate<R_t (P...)>::Delegate(Ty&& _f) noexcept
	: Delegate{ DeduceObject(Ut::Fwd<Ty>(_f)), DeduceFunc<Ty>() }
{
}

template <typename R_t, typename ... P> template <typename C>
inline Delegate<R_t (P...)>::Delegate(C* _obj, R_t(Ut::Decay_t<C>::* _f)(P...)) noexcept
	: mAlloc{ nullptr }, mObj{ reinterpret_cast<D*>(_obj) }, mFunc{ MemPtrCast<Func_t>(_f, *_obj) }
{
}

template <typename R_t, typename ... P> template <typename C>
inline Delegate<R_t (P...)>::Delegate(C* _obj, R_t(Ut::Decay_t<C>::* _f)(P...) const) noexcept
	: mAlloc{ nullptr }, mObj{ reinterpret_cast<D*>(_obj) }, mFunc{ MemPtrCast<Func_t>(_f, *_obj) }
{
}

template <typename R_t, typename ... P> template <typename C>
inline Delegate<R_t (P...)>::Delegate(C&& _obj, R_t(Ut::Decay_t<C>::* _f)(P...)) noexcept
	: mAlloc{ nullptr }, mObj{ GetObject(Ut::Fwd<C>(_obj)) }, mFunc{ MemPtrCast<Func_t>(_f, Ut::Fwd<C>(_obj)) }
{
}

template <typename R_t, typename ... P> template <typename C>
inline Delegate<R_t (P...)>::Delegate(C&& _obj, R_t(Ut::Decay_t<C>::* _f)(P...) const) noexcept
	: mAlloc{ nullptr }, mObj{ GetObject(Ut::Fwd<C>(_obj)) }, mFunc{ MemPtrCast<Func_t>(_f, Ut::Fwd<C>(_obj)) }
{
}

template <typename R_t, typename ... P>
inline Delegate<R_t (P...)>::Delegate(Delegate&& _rhs) noexcept
	: mAlloc{ _rhs.mAlloc }, mDel{ Ut::Move(_rhs.mDel) }, mObj{ Ut::Move(_rhs.mObj) }, mFunc{ Ut::Move(_rhs.mFunc) }
{
	_rhs.mAlloc = nullptr;
	_rhs.mDel   = nullptr;
	_rhs.mObj   = nullptr;
	_rhs.mFunc  = nullptr;
}

template<typename R_t, typename ...P>
inline Delegate<R_t(P...)>::~Delegate(void) noexcept
{
	if (mAlloc) mAlloc(mDel);
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
	using T = Ut::Decay_t<Ty>;

	if constexpr(Ut::IsEmptyClass<T>::value)
	{
		UNUSED_PARAMETER(_obj);

		// We cheat the 'this' pointer if there are no lambda captures
		// or if the functor has no member variables, prevents allocation
		return nullptr;
	}
	else
	{
		if constexpr (Ut::IsLvalueRef<Ty>::value)
		{
			// std::addressof uses compiler instrinsic, can't get around it
			return const_cast<D*>(reinterpret_cast<D const*>(std::addressof(_obj)));
		}

		// Small value optimisation...
		else if constexpr (sizeof(mDel) >= sizeof(T))
		{
			::new (&mDel) T{ Ut::Fwd<Ty>(_obj) };
			mAlloc = reinterpret_cast<void(*)(D*)>(static_cast<void(*)(T*)>([](T* _p) { reinterpret_cast<T*>(&_p)->~T(); }));

			return reinterpret_cast<D*>(&mDel);
		}

		else
		{
			mDel = reinterpret_cast<D*>(Dystopia::DefaultAllocator<Ty>::ConstructAlloc(Ut::Fwd<Ty>(_obj)));
			mAlloc = reinterpret_cast<void(*)(D*)>(&Dystopia::DefaultAllocator<Ty>::DestructFree);

			static_assert(false);
			return mDel;
		}
	}
}

template <typename R_t, typename ... P> template <typename To, typename From, typename Ty>
inline decltype(auto) Delegate<R_t (P...)>::MemPtrCast(From&& _f, Ty&&) noexcept
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
	else if constexpr (fptr_sz + sizeof(void*) == item_sz)
	{
		struct Internal {
			Func_t ptr;
			int a, b;
		};
		auto p = Ut::pun_cast<Internal>(Ut::Fwd<From>(_f));

		mObj = reinterpret_cast<D*>(reinterpret_cast<uintptr_t>(mObj) + p.a);

		return p.ptr;
	}
	else
	{
		static_assert(false, "Please notify the programmer in charge.");
	}
}



template<typename R_t, typename ...P>
inline Delegate<R_t(P...)>::operator bool() const
{
	return !!mFunc;
}

template <typename R_t, typename ... P> template <typename Ty>
inline auto Delegate<R_t(P...)>::operator=(Ty&& _obj) -> Ut::EnableIf_t<!Ut::IsSame<Ut::Decay_t<Ty>, Delegate>::value, Delegate&>
{
	~Delegate();
	mAlloc = nullptr;
	mDel   = nullptr;

	mObj   = GetObject(DeduceObject(Ut::Fwd<Ty>(_obj)));
	mFunc  = MemPtrCast(DeduceFunc<Ty>(), Ut::Fwd<Ty>(_obj));
		   
	return *this;
}

template <typename R_t, typename ... P>
inline Delegate<R_t(P...)>& Delegate<R_t(P...)>::operator = (Delegate&& _rhs) noexcept
{
	Ut::Swap(mAlloc, _rhs.mAlloc);
	Ut::Swap(mDel  , _rhs.mDel  );
	Ut::Swap(mObj  , _rhs.mObj  );
	Ut::Swap(mFunc , _rhs.mFunc );

	return *this;
}



#endif		// INCLUDE GUARD

