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

#include "Utility\Meta.h"		// IsSame

// Copied size_t out of microsoft header
// Just in case it is not defined in any of the includes
namespace std {
#ifdef _WIN64
	typedef unsigned __int64 size_t;
#else
	typedef unsigned int     size_t;
#endif
}

#if defined(DEBUG) | defined(_DEBUG)
#include "DebugAssert.h"

#include <typeinfo>
#endif // Debug only includes

template <class T>
class SharedPtr
{
public:
	constexpr SharedPtr(void) noexcept 
		: mpObj{ nullptr }, mnRefCount{ nullptr }
	{

	}
	SharedPtr(const SharedPtr& _pPointer) noexcept
		: mpObj{ _pPointer.mpObj }, mnRefCount{ _pPointer.mnRefCount }
	{
		if (_pPointer.mpObj)
			++*mnRefCount;
	}
	SharedPtr(SharedPtr&& _pPointer) noexcept
		: mpObj{ _pPointer.mpObj }, mnRefCount{ _pPointer.mnRefCount }
	{
		_pPointer.mpObj = nullptr;
		_pPointer.mnRefCount = nullptr;
	}

	~SharedPtr(void)
	{
		RemoveReference();
	}

	T* GetPtr(void) const
	{
		return mpObj;
	}

	SharedPtr& operator=(std::nullptr_t)
	{
		RemoveReference();
		return *this;
	}
	SharedPtr& operator=(const SharedPtr<T>& _pPointer)
	{
		if (_pPointer.mpObj)
			++*_pPointer.mnRefCount;

		RemoveReference();

		mpObj = _pPointer.mpObj;
		mnRefCount = _pPointer.mnRefCount;

		return *this;
	}
	SharedPtr& operator=(SharedPtr<T>&& _pPointer) noexcept
	{
		RemoveReference();

		mpObj = _pPointer.mpObj;
		mnRefCount = _pPointer.mnRefCount;

		_pPointer.mpObj = nullptr;
		_pPointer.mnRefCount = nullptr;

		return *this;
	}

	// Overloaded Operators

	T& operator*  (void) { return *mpObj; }
	T* operator-> (void) { return  mpObj; }
	const T& operator*  (void) const { return *mpObj; }
	const T* operator-> (void) const { return  mpObj; }

	inline explicit operator bool(void) const { return !!mpObj; }

	// Disallow pointers to SharedPtr
	static void* operator new (std::size_t) = delete;

private:
	SharedPtr(T* _pObj)
		: mpObj(_pObj), mnRefCount(new unsigned{ 1 })
	{
	}

	void RemoveReference(void)
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
	static_assert(!Utility::IsSame<Nest<Type>, SharedPtr<Type>>::value, 
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
	static_assert(!Utility::IsSame<Nest<Type>, SharedPtr<Type>>::value, 
		"SharedPtr Error: Cannot create SharedPtr to type SharedPtr.");

	return SharedPtr<Nest<Type>>(_pObj);
}



// ============================================ OPERATOR OVERLOADING ============================================ // 


template<typename Type>
inline bool operator== (const SharedPtr<Type>& _p, std::nullptr_t)
{
	return nullptr == _p.GetPtr();
}
template<typename Type>
inline bool operator== (std::nullptr_t, const SharedPtr<Type>& _p)
{
	return _p == nullptr;
}

template<typename Type>
inline bool operator!= (const SharedPtr<Type>& _p, std::nullptr_t)
{
	return _p.GetPtr() != nullptr;
}
template<typename Type>
inline bool operator!= (std::nullptr_t, const SharedPtr<Type>& _p)
{
	return _p != nullptr;
}

template<typename TypeA, typename TypeB>
inline bool operator== (const SharedPtr<TypeA>& _pL, const SharedPtr<TypeB>& _pR)
{
	return _pL.GetPtr() == _pR.GetPtr();
}

template<typename TypeA, typename TypeB>
inline bool operator!= (const SharedPtr<TypeA>& _pL, const SharedPtr<TypeB>& _pR)
{
	return _pL.GetPtr() != _pR.GetPtr();
}

template<typename Type>
inline bool operator! (const SharedPtr<Type>& _pL)
{
	return !_pL.GetPtr();
}



#endif		// INCLUDE GUARD

