/* HEADER *********************************************************************************/
/*!
\file	Pointer.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Wrapper class for pointers

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the 
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _GENERICPOINTER_H_
#define _GENERICPOINTER_H_

#include "Utility\Utility.h"

template <class T>
class Pointer
{
public:
	// ====================================== CONSTRUCTORS ======================================= // 

	constexpr Pointer(void) noexcept;
	explicit Pointer(T* _pObj) noexcept;
	Pointer(const Pointer&) = delete;
	Pointer(Pointer&& _pPointer) noexcept;

	~Pointer(void);


	// ======================================== OPERATORS ======================================== // 

	Pointer& operator=(std::nullptr_t);
	Pointer& operator=(Pointer<T>&& _pPointer) noexcept;

	T& operator*  (void) { return *mpObj; }
	T* operator-> (void) { return  mpObj; }
	const T& operator*  (void) const { return *mpObj; }
	const T* operator-> (void) const { return  mpObj; }

	inline explicit operator bool(void) const { return !!mpObj; }

	inline T* GetRaw(void) const;

	template <typename ... Param>
	static inline Pointer CreatePointer(Param&&...);

private:

	T * mpObj;

	template <typename Ty>
	struct Destroyer
	{
		static void Destroy(Ty* _pObj) { delete _pObj; }
	};
	template <typename Ty>
	struct Destroyer <Ty[]>
	{
		static void Destroy(Ty(*_pObj)[]) { delete[] _pObj; }
	};
};






// ============================================ FUNCTION DEFINITIONS ============================================ // 


template <class T>
constexpr Pointer<T>::Pointer(void) noexcept
	: mpObj{ nullptr }
{

}

template <class T>
Pointer<T>::Pointer(Pointer&& _pPointer) noexcept
	: Pointer{ _pPointer.mpObj }
{
	_pPointer.mpObj = nullptr;
}

template <class T>
Pointer<T>::Pointer(T* _pObj) noexcept
	: mpObj{ _pObj }
{

}

template <class T>
Pointer<T>::~Pointer(void)
{
	Destroyer<T>::Destroy(mpObj);
	mpObj = nullptr;
}

template <class T>
inline T* Pointer<T>::GetRaw(void) const
{
	return mpObj;
}



// ============================================ OPERATOR OVERLOADING ============================================ // 


template <class T>
Pointer<T>& Pointer<T>::operator = (std::nullptr_t)
{
	~Pointer();
	return *this;
}

template <class T>
Pointer<T>& Pointer<T>::operator = (Pointer<T>&& _ptr) noexcept
{
	Utility::Swap(mpObj, _ptr.mpObj);
	return *this;
}

template <typename Ty>
inline bool operator == (const Pointer<Ty>& _p, std::nullptr_t)
{
	return nullptr == _p;
}

template <typename Ty>
inline bool operator == (std::nullptr_t, const Pointer<Ty>& _p)
{
	return nullptr == _p.GetRaw();
}

template <typename Ty>
inline bool operator != (const Pointer<Ty>& _p, std::nullptr_t)
{
	return nullptr != _p;
}
template <typename Ty>
inline bool operator != (std::nullptr_t, const Pointer<Ty>& _p)
{
	return (nullptr != p.GetRaw());
}

template <typename LHS, typename RHS>
inline bool operator == (const Pointer<LHS>& _pL, const Pointer<RHS>& _pR)
{
	return _pL.GetRaw() == _pR.GetRaw();
}

template <typename LHS, typename RHS>
inline bool operator != (const Pointer<LHS>& _pL, const Pointer<RHS>& _pR)
{
	return  _pL.GetRaw() != _pR.GetRaw();;
}

template <typename Ty>
inline bool operator! (const Pointer<Ty>& _ptr)
{
	return !_ptr.GetRaw();
}

template<class T> template<typename ...Param>
inline Pointer<T> Pointer<T>::CreatePointer(Param&& ... Args)
{
	return Pointer{ new T { Utility::Forward<Param>(Args) ... } };
}



#endif		// INCLUDE GUARD

