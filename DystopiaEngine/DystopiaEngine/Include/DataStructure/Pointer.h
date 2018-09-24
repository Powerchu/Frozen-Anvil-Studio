/* HEADER *********************************************************************************/
/*!
\file	Pointer.h
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
#ifndef _GENERICPOINTER_H_
#define _GENERICPOINTER_H_

#include "Utility/Utility.h"
#include "Utility/MetaAlgorithms.h"
#include "Allocator/DefaultAlloc.h"	// DefaultAllocator

template <class T, class Alloc = Dystopia::DefaultAllocator<T>>
class Pointer
{
public:
	// ====================================== CONSTRUCTORS ======================================= // 

	template <typename ... Ps, typename 
		= Utility::EnableIf_t<!Utility::MetaFind<Pointer, Utility::Decay_t<Ps>...>::value>>
	Pointer(Ps&& ...);
	constexpr explicit Pointer(T* const _pObj) noexcept;
	Pointer(Pointer&& _pPointer) noexcept;

	~Pointer(void);


	// ======================================== OPERATORS ======================================== // 

	Pointer& operator=(std::nullptr_t);
	Pointer& operator=(Pointer&& _pPointer) noexcept;

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

	Pointer(const Pointer&) = delete;
};






// ============================================ FUNCTION DEFINITIONS ============================================ // 


/*!
  \warning
    The default constructor allocates
*/
template <class T, class A> template <typename ... Ps, typename>
Pointer<T, A>::Pointer(Ps&& ... _Args)
	: mpObj{ A::ConstructAlloc(Utility::Forward<Ps>(_Args)...) }
{

}

template <class T, class A>
Pointer<T, A>::Pointer(Pointer&& _pPointer) noexcept
	: Pointer{ _pPointer.mpObj }
{
	_pPointer.mpObj = nullptr;
}

template <class T, class A>
constexpr Pointer<T, A>::Pointer(T* _pObj) noexcept
	: mpObj{ _pObj }
{

}

template <class T, class A>
Pointer<T, A>::~Pointer(void)
{
	if (mpObj)
	{
		A::DestructFree(mpObj);
		mpObj = nullptr;
	}
}

template <class T, class A>
inline T* Pointer<T, A>::GetRaw(void) const
{
	return mpObj;
}



// ============================================ OPERATOR OVERLOADING ============================================ // 


template <class T, class A>
Pointer<T, A>& Pointer<T, A>::operator = (std::nullptr_t)
{
	~Pointer();
	return *this;
}

template <class T, class A>
Pointer<T, A>& Pointer<T, A>::operator = (Pointer<T, A>&& _ptr) noexcept
{
	Utility::Swap(mpObj, _ptr.mpObj);
	return *this;
}

template <class Ty, class A>
inline bool operator == (const Pointer<Ty, A>& _p, std::nullptr_t)
{
	return nullptr == _p;
}

template <class Ty, class A>
inline bool operator == (std::nullptr_t, const Pointer<Ty, A>& _p)
{
	return nullptr == _p.GetRaw();
}

template <class Ty, class A>
inline bool operator != (const Pointer<Ty, A>& _p, std::nullptr_t)
{
	return nullptr != _p;
}
template <class Ty, class A>
inline bool operator != (std::nullptr_t, const Pointer<Ty, A>& _p)
{
	return (nullptr != p.GetRaw());
}

template <typename Ty1, typename Ty2, typename A>
inline bool operator == (const Pointer<Ty1, A>& _pL, const Pointer<Ty2, A>& _pR)
{
	return _pL.GetRaw() == _pR.GetRaw();
}

template <typename Ty1, typename A1, typename Ty2, typename A2>
inline bool operator != (const Pointer<Ty1, A1>& _pL, const Pointer<Ty2, A2>& _pR)
{
	return  false;
}

template <typename Ty, typename A>
inline bool operator! (const Pointer<Ty, A>& _ptr)
{
	return !_ptr.GetRaw();
}

template <typename Ty, typename A, typename T>
inline auto operator - (const Pointer<Ty, A>& _lhs, T* const _rhs)
{
	return _lhs.GetRaw() - _rhs;
}

template <typename Ty, typename A, typename T>
inline auto operator - (T* const _lhs, const Pointer<Ty, A>& _rhs)
{
	return _lhs - _rhs.GetRaw();
}

template <typename Ty1, typename A1, typename Ty2, typename A2>
inline auto operator - (const Pointer<Ty1, A1>& _lhs, const Pointer<Ty2, A2>& _rhs)
{
	return _lhs.GetRaw() - _rhs.GetRaw();
}



#endif		// INCLUDE GUARD

