/* HEADER *********************************************************************************/
/*!
\file	SharedPtr.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Step 1: Use CreateShared to make a new pointer to type
	Eg. CreateShared<GameObject>();
	Step 2: Use as if it is a normal pointer

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the 
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _SHAREDPTR_H_
#define _SHAREDPTR_H_

#include "Utility/Meta.h"		// IsSame
#include "Globals.h"            // _DLL_EXPORT

#if defined(DEBUG) | defined(_DEBUG)
#include "Utility/DebugAssert.h"

#include <typeinfo>
#endif // Debug only includes


template <class T>
class SharedPtr
{
public:
	// ====================================== CONSTRUCTORS ======================================= // 

	constexpr SharedPtr(void) noexcept;
	SharedPtr(const SharedPtr& _pPointer) noexcept;
	SharedPtr(SharedPtr&& _pPointer) noexcept;

	~SharedPtr(void);


	// ======================================== OPERATORS ======================================== // 

	SharedPtr& operator=(std::nullptr_t);
	SharedPtr& operator=(const SharedPtr<T>& _pPointer);
	SharedPtr& operator=(SharedPtr<T>&& _pPointer) noexcept;

	T& operator*  (void) { return *mpObj; }
	T* operator-> (void) { return  mpObj; }
	const T& operator*  (void) const { return *mpObj; }
	const T* operator-> (void) const { return  mpObj; }

	inline explicit operator bool(void) const { return !!mpObj; }

	inline T* GetRaw(void) const;

private:

	SharedPtr(T* _pObj);

	void RemoveReference(void);

	template <typename T, typename U>
	friend SharedPtr<T> DynamicPtrCast(const SharedPtr<U>&);

	template<typename Type>
	friend SharedPtr<Type> CreateShared(Type*);
	template<typename Type, template<typename> class Nest>
	friend SharedPtr<Nest<Type>> CreateShared(Nest<Type>* _pObj);
	template<typename Type, typename ...Params>
	friend SharedPtr<Type> CreateShared(Params&& ...);
	template<typename Type, template<typename> class Nest, typename ...Params>
	friend SharedPtr<Nest<Type>> CreateShared(Params&& ...);

	T* mpObj;
	unsigned* mnRefCount;
};

template<typename Type, typename ...Params>
inline SharedPtr<Type> CreateShared(Params&& ...args)
{
	return SharedPtr<Type>(new Type{ static_cast<Params&&>(args)... });
}

template<typename Type, template<typename> class Nest, typename ...Params>
inline SharedPtr<Nest<Type>> CreateShared(Params&& ...args)
{
	static_assert(!Ut::IsSame<Nest<Type>, SharedPtr<Type>>::value, 
		"SharedPtr Error: Cannot create SharedPtr to type SharedPtr.");

	return SharedPtr<Type>(new Type{ static_cast<Params&&>(args)... });
}


template<typename Type>
inline SharedPtr<Type> CreateShared(Type* _pObj)
{
	return SharedPtr<Type>(_pObj);
}

template<typename Type, template<typename> class Nest>
inline SharedPtr<Nest<Type>> CreateShared(Nest<Type>* _pObj)
{
	static_assert(!Ut::IsSame<Nest<Type>, SharedPtr<Type>>::value, 
		"SharedPtr Error: Cannot create SharedPtr to type SharedPtr.");

	return SharedPtr<Nest<Type>>(_pObj);
}

template <typename T, typename U>
inline SharedPtr<T> DynamicPtrCast(const SharedPtr<U>& _pPtr)
{
	SharedPtr<T> ret{};

	ret.mpObj = dynamic_cast<T>(_pPtr.GetRaw());

	if (ret.mpObj)
	{
		ret.mnRefCount = _pPtr.mnRefCount;
	}

	return ret;
}






// ============================================ FUNCTION DEFINITIONS ============================================ // 


template <class T>
constexpr SharedPtr<T>::SharedPtr (void) noexcept
	: mpObj{ nullptr }, mnRefCount{ nullptr }
{

}

template <class T>
SharedPtr<T>::SharedPtr(const SharedPtr& _pPointer) noexcept
	: mpObj{ _pPointer.mpObj }, mnRefCount{ _pPointer.mnRefCount }
{
	if (_pPointer.mpObj)
		++*mnRefCount;
}

template <class T>
SharedPtr<T>::SharedPtr(SharedPtr&& _pPointer) noexcept
	: mpObj{ _pPointer.mpObj }, mnRefCount{ _pPointer.mnRefCount }
{
	_pPointer.mpObj = nullptr;
	_pPointer.mnRefCount = nullptr;
}

template <class T>
SharedPtr<T>::SharedPtr(T* _pObj) :
	mpObj(_pObj), mnRefCount(new unsigned{ 1 })
{

}

template <class T>
SharedPtr<T>::~SharedPtr(void)
{
	RemoveReference();
}

template <class T>
inline T* SharedPtr<T>::GetRaw(void) const
{
	return mpObj;
}

template <class T>
void SharedPtr<T>::RemoveReference(void)
{
	if (mpObj)
	{
		if (0 == --*mnRefCount)
		{
		#if _DEBUG
			DEBUG_LOG(mpObj, "SharedPtr freeing object \"%s\".\n", typeid(mpObj).name());
		#endif

			delete mpObj;
			delete mnRefCount;
		}

		mpObj = nullptr;
		mnRefCount = nullptr;
	}
}



// ============================================ OPERATOR OVERLOADING ============================================ // 


template <class T>
 SharedPtr<T>& SharedPtr<T>::operator = (std::nullptr_t)
{
	RemoveReference();
	return *this;
}

template <class T>
 SharedPtr<T>& SharedPtr<T>::operator = (const SharedPtr<T>& _pPointer)
{
	if (_pPointer.mpObj)
		++*_pPointer.mnRefCount;

	RemoveReference();

	mpObj = _pPointer.mpObj;
	mnRefCount = _pPointer.mnRefCount;

	return *this;
}

template <class T>
 SharedPtr<T>& SharedPtr<T>::operator = (SharedPtr<T>&& _pPointer) noexcept
{
	RemoveReference();

	mpObj = _pPointer.mpObj;
	mnRefCount = _pPointer.mnRefCount;

	_pPointer.mpObj = nullptr;
	_pPointer.mnRefCount = nullptr;

	return *this;
}

template <typename Type>
inline bool operator== (const SharedPtr<Type>& _p, std::nullptr_t)
{
	return nullptr == _p.GetRaw();
}

template <typename Type>
inline bool operator== (std::nullptr_t, const SharedPtr<Type>& _p)
{
	return _p == nullptr;
}

template <typename Type>
inline bool operator!= (const SharedPtr<Type>& _p, std::nullptr_t)
{
	return _p.GetRaw() != nullptr;
}
template <typename Type>
inline bool operator!= (std::nullptr_t, const SharedPtr<Type>& _p)
{
	return _p != nullptr;
}

template <typename TypeA, typename TypeB>
inline bool operator== (const SharedPtr<TypeA>& _pL, const SharedPtr<TypeB>& _pR)
{
	return _pL.GetRaw() == _pR.GetRaw();
}

template <typename TypeA, typename TypeB>
inline bool operator!= (const SharedPtr<TypeA>& _pL, const SharedPtr<TypeB>& _pR)
{
	return _pL.GetRaw() != _pR.GetRaw();
}

template <typename Type>
inline bool operator! (const SharedPtr<Type>& _pL)
{
	return !_pL.GetRaw();
}



#endif		// INCLUDE GUARD

