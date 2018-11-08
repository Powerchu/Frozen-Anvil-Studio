/* HEADER *********************************************************************************/
/*!
\file	ProxyPtr.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Wrapper class for pointers that should not be deleted

\warning
	This class will never allocate a pointer

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

template <class T>
class ProxyPtr
{
public:
	// ====================================== CONSTRUCTORS ======================================= // 

	template <typename ... Ps, typename 
		= Ut::EnableIf_t<!Ut::MetaFind<Pointer, Ut::Decay_t<Ps>...>::value>>
	ProxyPtr(Ps&& ...);
	constexpr explicit ProxyPtr(T* const _pObj) noexcept;
	ProxyPtr(ProxyPtr&& _pPointer) noexcept;

	~ProxyPtr(void);


	// ======================================== OPERATORS ======================================== // 

	ProxyPtr& operator=(std::nullptr_t);
	ProxyPtr& operator=(ProxyPtr&& _pPointer) noexcept;

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

	ProxyPtr(const ProxyPtr&) = delete;
};






// ============================================ FUNCTION DEFINITIONS ============================================ // 


/*!
  \warning
    The default constructor allocates
*/
template <class T> template <typename ... Ps, typename>
ProxyPtr<T>::ProxyPtr(Ps&& ... _Args)
	: mpObj{ A::ConstructAlloc(Ut::Forward<Ps>(_Args)...) }
{

}

template <class T, class A>
ProxyPtr<T>::ProxyPtr(ProxyPtr&& _pPointer) noexcept
	: ProxyPtr{ _pPointer.mpObj }
{
	_pPointer.mpObj = nullptr;
}

template <class T, class A>
constexpr ProxyPtr<T, A>::ProxyPtr(T* _pObj) noexcept
	: mpObj{ _pObj }
{

}

template <class T, class A>
ProxyPtr<T, A>::~ProxyPtr(void)
{
	if (mpObj)
	{
		A::DestructFree(mpObj);
		mpObj = nullptr;
	}
}

template <class T, class A>
inline T* ProxyPtr<T, A>::GetRaw(void) const
{
	return mpObj;
}



// ============================================ OPERATOR OVERLOADING ============================================ // 


template <class T, class A>
ProxyPtr<T>& ProxyPtr<T, A>::operator = (std::nullptr_t)
{
	~ProxyPtr();
	return *this;
}

template <class T>
ProxyPtr<T>& ProxyPtr<T>::operator = (ProxyPtr<T>&& _ptr) noexcept
{
	Ut::Swap(mpObj, _ptr.mpObj);
	return *this;
}

template <class Ty>
inline bool operator == (const ProxyPtr<Ty>& _p, std::nullptr_t)
{
	return nullptr == _p;
}

template <class Ty>
inline bool operator == (std::nullptr_t, const ProxyPtr<Ty>& _p)
{
	return nullptr == _p.GetRaw();
}

template <class Ty>
inline bool operator != (const ProxyPtr<Ty>& _p, std::nullptr_t)
{
	return nullptr != _p;
}
template <class Ty>
inline bool operator != (std::nullptr_t, const ProxyPtr<Ty>& _p)
{
	return (nullptr != p.GetRaw());
}

template <typename Ty1, typename Ty2>
inline bool operator == (const ProxyPtr<Ty1>& _pL, const ProxyPtr<Ty2>& _pR)
{
	return _pL.GetRaw() == _pR.GetRaw();
}

template <typename Ty1, typename Ty2>
inline bool operator != (const ProxyPtr<Ty1>& _pL, const ProxyPtr<Ty2>& _pR)
{
	return  false;
}

template <typename Ty>
inline bool operator! (const ProxyPtr<Ty>& _ptr)
{
	return !_ptr.GetRaw();
}

template <typename Ty, typename T>
inline auto operator - (const ProxyPtr<Ty>& _lhs, T* const _rhs)
{
	return _lhs.GetRaw() - _rhs;
}

template <typename Ty, typename T>
inline auto operator - (T* const _lhs, const ProxyPtr<Ty>& _rhs)
{
	return _lhs - _rhs.GetRaw();
}

template <typename Ty1, typename Ty2>
inline auto operator - (const ProxyPtr<Ty1>& _lhs, const ProxyPtr<Ty2>& _rhs)
{
	return _lhs.GetRaw() - _rhs.GetRaw();
}



#endif		// INCLUDE GUARD

