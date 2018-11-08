/* HEADER *********************************************************************************/
/*!
\file	SharedPtr.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
Step 1: Use CreateShared to make a new pointer to type
Eg. Ctor::CreateShared<GameObject>();
Step 2: Use as if it is a normal pointer
All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _SHAREDPTR_H_
#define _SHAREDPTR_H_

#include "Globals.h"              // _DLL_EXPORT
#include "Utility/Meta.h"		  // IsSame
#include "Utility/DebugAssert.h"
#include "Allocator/DefaultAlloc.h"


template <class Ty>
class SharedPtr;

namespace Ctor
{
	template<typename T>
	SharedPtr<T> CreateShared(T*, void(*)(void*) = [](void* _p) { delete static_cast<T*>(_p); });
	template<typename T, typename ... P>
	SharedPtr<T> CreateShared(P&& ...);
	template<typename T, template<typename> class Nest>
	SharedPtr<Nest<T>> CreateShared(Nest<T>* _pObj, void(*)(void*) = [](void* _p) { delete static_cast<Nest<T>*>(_p); });
	template<typename T, template<typename> class Nest, typename ... P>
	SharedPtr<Nest<T>> CreateShared(P&& ...);
}

namespace
{
	struct Control
	{
		size_t mnRefs;
		void(*mpDeleter)(void*);

		Control(void(*_pDeleter)(void*)) :
			mnRefs{ 1 }, mpDeleter{ _pDeleter }
		{}

		void AddRef(void) noexcept;
		void RemoveRef(void*) noexcept;
	};
}

template <class Ty>
class SharedPtr
{
public:
	// ====================================== CONSTRUCTORS ======================================= // 

	constexpr SharedPtr(void) noexcept;
	explicit SharedPtr(Ty*, void(*)(void*) = [](void* _p) { delete static_cast<Ty*>(_p); });

	template <typename U, typename = Ut::EnableIf_t<Ut::IsConvertible<U*, Ty*>::value>>
	SharedPtr(const SharedPtr<U>&) noexcept;
	template <typename U, typename = Ut::EnableIf_t<Ut::IsConvertible<U*, Ty*>::value>>
	SharedPtr(SharedPtr<U>&&) noexcept;

	SharedPtr(SharedPtr&&) noexcept;
	SharedPtr(const SharedPtr&) noexcept;

	~SharedPtr(void);


	// ======================================== OPERATORS ======================================== // 

	SharedPtr& operator = (std::nullptr_t) noexcept;
	SharedPtr& operator = (SharedPtr&&) noexcept;
	template <typename U, typename = Ut::EnableIf_t<Ut::IsConvertible<U*, Ty*>::value>>
	SharedPtr& operator = (const SharedPtr<U>&) noexcept;

	SharedPtr& operator = (const SharedPtr&) noexcept;

	Ty& operator*  (void) { return *mpObj; }
	Ty* operator-> (void) noexcept { return  mpObj; }
	const Ty& operator*  (void) const { return *mpObj; }
	const Ty* operator-> (void) const noexcept { return  mpObj; }

	inline explicit operator bool(void) const noexcept { return !!mpObj; }

	inline Ty* GetRaw(void) const noexcept;
	inline Control* GetCtrl(void) const noexcept;

	inline void _Clean(void) noexcept;

private:

	template <typename T>
	using Alloc_t = Dystopia::DefaultAllocator<T>;


	template <typename T>
	struct ControlAux : Control
	{
		T mObj;

		template <typename ... U>
		explicit ControlAux(void(*_p)(void*), U&& ..._args) :
			Control{ _p }, mObj{ Ut::Forward<U>(_args)... }
		{}
	};


	template <typename ...P> SharedPtr(void(*)(void*), P&& ...);


	void RemoveReference(void) noexcept;


	template <typename T, typename U>
	friend SharedPtr<T> DynamicPtrCast(const SharedPtr<U>&) noexcept;

	template<typename T, typename ... P>
	friend SharedPtr<T> Ctor::CreateShared(P&& ...);
	template<typename T, template<typename> class Nest, typename ... P>
	friend SharedPtr<Nest<T>> Ctor::CreateShared(P&& ...);
	template<typename T>
	friend SharedPtr<T> Ctor::CreateShared(T*, void(*)(void*));
	template<typename T, template<typename> class Nest>
	friend SharedPtr<Nest<T>> Ctor::CreateShared(Nest<T>* _pObj, void(*)(void*));


	Control* mpCtrl;
	Ty* mpObj;
};






// ============================================ FUNCTION DEFINITIONS ============================================ // 


template <class Ty>
constexpr SharedPtr<Ty>::SharedPtr(void) noexcept
	: mpCtrl{ nullptr }, mpObj{ nullptr }
{

}

template <class Ty>
SharedPtr<Ty>::SharedPtr(Ty* _pObj, void(*_del)(void*)) :
	mpCtrl{ Alloc_t<ControlAux<Ty*>>::ConstructAlloc(_del, _pObj) },
	mpObj{ _pObj }
{

}

template <class Ty> template <class U, typename>
SharedPtr<Ty>::SharedPtr(const SharedPtr<U>& _p) noexcept
	: mpCtrl{ _p.GetCtrl() }, mpObj{ static_cast<Ty*>(_p.GetRaw()) }
{
	mpCtrl->AddRef();
}
template <class Ty>
SharedPtr<Ty>::SharedPtr(const SharedPtr& _p) noexcept
	: mpCtrl{ _p.mpCtrl }, mpObj{ _p.mpObj }
{
	mpCtrl->AddRef();
}

template <class Ty> template <class U, typename>
SharedPtr<Ty>::SharedPtr(SharedPtr<U>&& _p) noexcept
	: mpCtrl{ _p.GetCtrl() }, mpObj{ static_cast<Ty*>(_p.GetRaw()) }
{
	_p._Clean();
}

template <class Ty>
SharedPtr<Ty>::SharedPtr(SharedPtr&& _p) noexcept
	: mpCtrl{ _p.mpCtrl }, mpObj{ _p.mpObj }
{
	_p.mpObj  = nullptr;
	_p.mpCtrl = nullptr;
}

template <class Ty> template <typename ... P>
SharedPtr<Ty>::SharedPtr(void(*_del)(void*), P&& ... _args) :
	mpCtrl{ Alloc_t<ControlAux<Ty>>::ConstructAlloc(_del, Ut::Forward<P>(_args)...) },
	mpObj{ &(static_cast<ControlAux<Ty>*>(mpCtrl)->mObj) }
{

}

template <class Ty>
SharedPtr<Ty>::~SharedPtr(void)
{
	RemoveReference();
}


template <class Ty>
inline Ty* SharedPtr<Ty>::GetRaw(void) const noexcept
{
	return mpObj;
}

template<class Ty>
inline Control* SharedPtr<Ty>::GetCtrl(void) const noexcept
{
	return mpCtrl;
}

template<class Ty>
inline void SharedPtr<Ty>::_Clean(void) noexcept
{
	RemoveReference();
}

template <class Ty>
inline void SharedPtr<Ty>::RemoveReference(void) noexcept
{
	if (mpCtrl) mpCtrl->RemoveRef(mpObj);

	mpObj  = nullptr;
	mpCtrl = nullptr;
}



// ============================================ OPERATOR OVERLOADING ============================================ // 


template <class Ty>
SharedPtr<Ty>& SharedPtr<Ty>::operator = (std::nullptr_t) noexcept
{
	RemoveReference();
	return *this;
}

template <class Ty> template <class U, typename>
SharedPtr<Ty>& SharedPtr<Ty>::operator = (const SharedPtr<U>& _p) noexcept
{
	if (_p.GetCtrl())
		_p.GetCtrl()->AddRef();

	RemoveReference();
	mpObj = static_cast<Ty*>(_p.GetRaw());
	mpCtrl = _p.GetCtrl();

	return *this;
}

template <class Ty>
SharedPtr<Ty>& SharedPtr<Ty>::operator = (const SharedPtr<Ty>& _p) noexcept
{
	if (_p.mpCtrl)
		_p.mpCtrl->AddRef();

	RemoveReference();
	mpObj  = _p.mpObj;
	mpCtrl = _p.mpCtrl;

	return *this;
}

template <class Ty>
SharedPtr<Ty>& SharedPtr<Ty>::operator = (SharedPtr<Ty>&& _p) noexcept
{
	Ut::Swap(mpObj, _p.mpObj);
	Ut::Swap(mpCtrl, _p.mpCtrl);

	return *this;
}

template <typename Ty>
inline bool operator == (const SharedPtr<Ty>& _p, std::nullptr_t) noexcept
{
	return nullptr == _p.GetRaw();
}

template <typename Ty>
inline bool operator == (std::nullptr_t, const SharedPtr<Ty>& _p) noexcept
{
	return _p == nullptr;
}

template <typename Ty>
inline bool operator != (const SharedPtr<Ty>& _p, std::nullptr_t) noexcept
{
	return _p.GetRaw() != nullptr;
}
template <typename Ty>
inline bool operator != (std::nullptr_t, const SharedPtr<Ty>& _p) noexcept
{
	return _p != nullptr;
}

template <typename LHS, typename RHS>
inline bool operator == (const SharedPtr<LHS>& _pL, const SharedPtr<RHS>& _pR) noexcept
{
	return _pL.GetRaw() == _pR.GetRaw();
}

template <typename LHS, typename RHS>
inline bool operator != (const SharedPtr<LHS>& _pL, const SharedPtr<RHS>& _pR) noexcept
{
	return _pL.GetRaw() != _pR.GetRaw();
}

template <typename Ty>
inline bool operator! (const SharedPtr<Ty>& _pL)
{
	return !_pL.GetRaw();
}



// ============================================ SHARED POINTER MAKER ============================================ // 


template<typename T, typename ... P>
inline SharedPtr<T> Ctor::CreateShared(P&& ..._args)
{
	return SharedPtr<T>{ [](void* _p){ static_cast<T*>(_p)->~T(); }, Ut::Forward<P>(_args)... };
}

template<typename T, template<typename> class Nest, typename ... P>
inline SharedPtr<Nest<T>> Ctor::CreateShared(P&& ..._args)
{
	static_assert(!Ut::IsSame<Nest<T>, SharedPtr<T>>::value,
		"SharedPtr Error: Cannot create SharedPtr to type SharedPtr.");

	return SharedPtr<T>{ Ut::Forward<P>(_args)... };
}


template<typename T>
inline SharedPtr<T> Ctor::CreateShared(T* _pObj, void(*_del)(void*))
{
	return SharedPtr<T>{ _pObj, _del };
}

template<typename Type, template<typename> class Nest>
inline SharedPtr<Nest<Type>> Ctor::CreateShared(Nest<Type>* _pObj, void(*_del)(void*))
{
	static_assert(!Ut::IsSame<Nest<Type>, SharedPtr<Type>>::value,
		"SharedPtr Error: Cannot create SharedPtr to type SharedPtr.");

	return SharedPtr<Nest<Type>>{ _pObj, _del };
}

template <typename T, typename U>
inline SharedPtr<T> DynamicPtrCast(const SharedPtr<U>& _pPtr) noexcept
{
	if (dynamic_cast<T>(_pPtr.GetRaw()))
	{
		return SharedPtr<T> { _pPtr };
	}

	return SharedPtr<T>{};
}



// ================================================ NESTED TYPES ================================================ // 


inline void Control::AddRef(void) noexcept
{
	++mnRefs;
}

inline void Control::RemoveRef(void* _p) noexcept
{
	if (!--mnRefs)
	{
		if (mpDeleter) mpDeleter(_p);

		Dystopia::DefaultAllocator<void>::Free(this);
	}
}



#endif		// INCLUDE GUARD
