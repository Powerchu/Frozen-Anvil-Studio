/* HEADER *********************************************************************************/
/*!
\file	UniquePointer.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Wrapper class for pointers

\warning
	The default constructor allocates. For no allocation, construct with nullptr

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the 
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _UNIQUEPOINTER_H_
#define _UNIQUEPOINTER_H_

#include "Utility/Utility.h"
#include "Utility/MetaAlgorithms.h"
#include "Allocator/DefaultAlloc.h"	// DefaultAllocator

template <class T, class Alloc = Dystopia::DefaultAllocator<T>>
class UniquePtr
{
	template <typename ... T>
	struct AnyUniquePtr;

public:
	// ====================================== CONSTRUCTORS ======================================= // 

	template <typename ... Ps, typename = Ut::EnableIf_t<!(AnyUniquePtr<Ut::Decay_t<Ps>...>::value && (1 == sizeof...(Ps)))>>
	UniquePtr(Ps&& ...);
	constexpr explicit UniquePtr(T* const) noexcept;
	template <typename U, typename = Ut::EnableIf<Ut::IsConvertible<U, T>::value>>
	UniquePtr(UniquePtr<U>&&) noexcept;

	~UniquePtr(void);


	// ======================================== OPERATORS ======================================== // 

	UniquePtr& operator=(std::nullptr_t);
	UniquePtr& operator=(UniquePtr&&) noexcept;

	T& operator*  (void) { return *mpObj; }
	T* operator-> (void) { return  mpObj; }
	T& operator[] (long long _nI) { return mpObj[_nI]; }

	const T& operator*  (void) const { return *mpObj; }
	const T* operator-> (void) const { return  mpObj; }
	const T& operator[] (long long _nI) const { return mpObj[_nI]; }

	inline explicit operator bool(void) const { return !!mpObj; }

	inline T* GetRaw(void) const;

private:

	T* mpObj;

	UniquePtr(const UniquePtr&) = delete;

	template <typename ... T>
	struct AnyUniquePtr
	{
		static constexpr bool value = false;
	};

	template <typename T, typename ... R>
	struct AnyUniquePtr<T, R...>
	{
		static constexpr bool value = AnyUniquePtr<R...>::value;
	};

	template <typename T, typename ... R>
	struct AnyUniquePtr<UniquePtr<T>, R...>
	{
		static constexpr bool value = true;
	};
};






// ============================================ FUNCTION DEFINITIONS ============================================ // 


/*!
  \warning
    The default constructor allocates
*/
template <class T, class A> template <typename ... Ps, typename>
UniquePtr<T, A>::UniquePtr(Ps&& ... _Args)
	: mpObj{ A::ConstructAlloc(Ut::Forward<Ps>(_Args)...) }
{
}

template <class T, class A> template <class U, typename>
UniquePtr<T, A>::UniquePtr(UniquePtr<U>&& _p) noexcept
	: UniquePtr{ static_cast<T*>(_p.mpObj) }
{
	_p.mpObj = nullptr;
}

template <class T, class A>
constexpr UniquePtr<T, A>::UniquePtr(T* _p) noexcept
	: mpObj{ _p }
{

}

template <class T, class A>
UniquePtr<T, A>::~UniquePtr(void)
{
	if (mpObj)
	{
		A::DestructFree(mpObj);
		mpObj = nullptr;
	}
}

template <class T, class A>
inline T* UniquePtr<T, A>::GetRaw(void) const
{
	return mpObj;
}



// ============================================ OPERATOR OVERLOADING ============================================ // 


template <class T, class A>
UniquePtr<T, A>& UniquePtr<T, A>::operator = (std::nullptr_t)
{
	~UniquePointer();
	return *this;
}

template <class T, class A>
UniquePtr<T, A>& UniquePtr<T, A>::operator = (UniquePtr<T, A>&& _ptr) noexcept
{
	Ut::Swap(mpObj, _ptr.mpObj);
	return *this;
}

template <class Ty, class A>
inline bool operator == (const UniquePtr<Ty, A>& _p, std::nullptr_t)
{
	return nullptr == _p;
}

template <class Ty, class A>
inline bool operator == (std::nullptr_t, const UniquePtr<Ty, A>& _p)
{
	return nullptr == _p.GetRaw();
}

template <class Ty, class A>
inline bool operator != (const UniquePtr<Ty, A>& _p, std::nullptr_t)
{
	return nullptr != _p;
}
template <class Ty, class A>
inline bool operator != (std::nullptr_t, const UniquePtr<Ty, A>& _p)
{
	return (nullptr != p.GetRaw());
}

template <typename Ty1, typename Ty2, typename A>
inline bool operator == (const UniquePtr<Ty1, A>& _pL, const UniquePtr<Ty2, A>& _pR)
{
	return _pL.GetRaw() == _pR.GetRaw();
}

template <typename Ty1, typename A1, typename Ty2, typename A2>
inline bool operator != (const UniquePtr<Ty1, A1>& _pL, const UniquePtr<Ty2, A2>& _pR)
{
	return  false;
}

template <typename Ty, typename A>
inline bool operator! (const UniquePtr<Ty, A>& _ptr)
{
	return !_ptr.GetRaw();
}

template <typename Ty, typename A, typename T>
inline auto operator - (const UniquePtr<Ty, A>& _lhs, T* const _rhs)
{
	return _lhs.GetRaw() - _rhs;
}

template <typename Ty, typename A, typename T>
inline auto operator - (T* const _lhs, const UniquePtr<Ty, A>& _rhs)
{
	return _lhs - _rhs.GetRaw();
}

template <typename Ty1, typename A1, typename Ty2, typename A2>
inline auto operator - (const UniquePtr<Ty1, A1>& _lhs, const UniquePtr<Ty2, A2>& _rhs)
{
	return _lhs.GetRaw() - _rhs.GetRaw();
}



#endif		// INCLUDE GUARD

