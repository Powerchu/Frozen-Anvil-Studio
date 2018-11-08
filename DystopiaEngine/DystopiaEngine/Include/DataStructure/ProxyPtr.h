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

#include <exception>


template <class T>
class ProxyPtr
{
public:
	// ====================================== CONSTRUCTORS ======================================= // 

	constexpr ProxyPtr(T*) noexcept;
	ProxyPtr(ProxyPtr&&) noexcept;
	ProxyPtr(const ProxyPtr&) noexcept;


	// ======================================== OPERATORS ======================================== // 

	ProxyPtr& operator=(std::nullptr_t);
	ProxyPtr& operator=(ProxyPtr&&) noexcept;
	ProxyPtr& operator=(const ProxyPtr&) noexcept;

	T& operator*  (void) { Check(); return *mpObj; }
	T* operator-> (void) { Check(); return  mpObj; }
	T& operator[] (long long _nI) { Check();  return mpObj[_nI]; }

	const T& operator*  (void) const { Check(); return *mpObj; }
	const T* operator-> (void) const { Check(); return  mpObj; }
	const T& operator[] (long long _nI) const { Check(); return mpObj[_nI]; }

	inline explicit operator bool(void) const { return !!mpObj; }

	inline T* GetRaw(void) const;

private:

	T* mpObj;

	void Check(void) const;
	ProxyPtr(const ProxyPtr&) = delete;
};






// ============================================ FUNCTION DEFINITIONS ============================================ // 


template <class T>
constexpr ProxyPtr<T>::ProxyPtr(T* _p) noexcept
	: mpObj{ _p }
{

}

template <class T>
ProxyPtr<T>::ProxyPtr(ProxyPtr&& _p) noexcept
	: ProxyPtr{ _p.mpObj }
{
	_p.mpObj = nullptr;
}

template <class T>
ProxyPtr<T>::ProxyPtr(const ProxyPtr& _p) noexcept
	: ProxyPtr{ _p.mpObj }
{

}


template <class T>
inline T* ProxyPtr<T>::GetRaw(void) const
{
	return mpObj;
}

template<class T>
inline void ProxyPtr<T>::Check(void) const
{
#if defined(SAFE_PTRS)
	if (nullptr == mpObj)
		throw std::exception("Null pointer exception!");
#endif
}



// ============================================ OPERATOR OVERLOADING ============================================ // 


template <class T>
ProxyPtr<T>& ProxyPtr<T>::operator = (std::nullptr_t)
{
	~ProxyPtr();
	return *this;
}

template <class T>
ProxyPtr<T>& ProxyPtr<T>::operator = (ProxyPtr<T>&& _p) noexcept
{
	Ut::Swap(mpObj, _p.mpObj);
	return *this;
}

template <class T>
ProxyPtr<T>& ProxyPtr<T>::operator = (const ProxyPtr<T>& _p) noexcept
{
	mpObj = _p;
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
	return !(_pL == _pR);
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

