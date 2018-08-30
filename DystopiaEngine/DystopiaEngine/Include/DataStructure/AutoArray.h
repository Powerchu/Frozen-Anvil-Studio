/* HEADER *********************************************************************************/
/*!
\file	AutoArray.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Run-time auto resizing array.
	Now 100% neater!

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _DYNAMIC_ARRAY_
#define _DYNAMIC_ARRAY_

#if defined(DEBUG) | defined(_DEBUG)
#include "Utility\DebugAssert.h"
#endif // Debug only includes

#include "Utility\Utility.h"			// Move, CopyUninit
#include "Math\MathUtility.h"			// phi
#include "Utility\Meta.h"				// Decay, IsSame, EnableIf, IsIntegral
#include "Utility\MetaDataStructures.h" // TypeList
#include "Allocator\DefaultAlloc.h"		// DefaultAllocator

#include <new>				// operator new
#include <cstring>			// memcpy
#include <initializer_list> // init-list


template<class T, class Alloc = Dystopia::DefaultAllocator<T[]>>
class AutoArray
{
public:
	// ==================================== CONTAINER DEFINES ==================================== // 

	using Itor_t = T*;
	using Val_t  = T;
	using Ptr_t  = T*;
	using Sz_t   = unsigned;


	// ====================================== CONSTRUCTORS ======================================= // 

	AutoArray(void) noexcept;
	explicit AutoArray(Sz_t _nSize);
	AutoArray(const AutoArray& _other);
	AutoArray(AutoArray&& _other) noexcept;
	template <typename U, typename = 
		Utility::EnableIf_t<
			 Utility::IsSame<Utility::Decay_t<U>, T>::value &&
			!Utility::IsIntegral<Utility::Decay_t<U>>::value
	>>
	AutoArray(std::initializer_list<U>);

	~AutoArray(void);


	// =================================== CONTAINER FUNCTIONS =================================== // 

	// Gets the begin iterator
	inline Itor_t begin(void) const noexcept;

	// Gets the end iterator
	inline Itor_t end(void) const noexcept;

	// Gets the last element
	inline T& back(void) const noexcept;

	// Gets the current number of elements in the array
	inline Sz_t size(void) const noexcept;

	// Synonymous to Insert
	inline void push_back(const T& _obj);

	// Synonymous to Remove
	inline void pop_back(void) noexcept;

	// Empties the array
	inline void clear(void) noexcept;

	// Minimises the memory used by the array
	inline void shrink(void);

	// Ensures that there are the specified amount of unused slots
	inline void reserve(Sz_t _nSize);

	// Inserts an element to the back of the array
	void Insert(const T& _obj);

	// Insert an element to the specified position of the array
	void Insert(const T& _obj, const Sz_t _nIndex);

	// In-place insert an element to the back of the array
	template<typename ...Args>
	void EmplaceBack(Args &&...args);

	// Removes the last element of the array
	inline void Remove(void) noexcept;

	// Remove all matching elements
	void Remove(const T&) noexcept;

	// Remove an element at the specified position of the array
	// Preserves the current order of the array
	inline void Remove(const Sz_t _nIndex);

	// Remove an element pointed by the iterator to the array
	// Preserves the current order of the array
	void Remove(const Itor_t _pObj);

	// Remove an element at the specified position of the array
	inline void FastRemove(const Sz_t _nIndex);

	// Remove an element pointed by the iterator to the array
	inline void FastRemove(const Itor_t& _pObj);

	inline bool IsEmpty(void) const noexcept;

	inline Sz_t Cap(void) const noexcept;

	// Sorts the array using Insertion Sort.
	// Defaults to sorting in ascending order based on the '<' operator
	template<typename Comparator>
	void Sort(Comparator _pTest = [](T& lhs, T& rhs)->bool { return lhs < rhs; });


	// ======================================== OPERATORS ======================================== // 

	T& operator[] (const Sz_t _nIndex);
	const T& operator[] (const Sz_t _nIndex) const;

	AutoArray& operator= (const AutoArray& _other);
	AutoArray& operator= (AutoArray&& _other) noexcept;


private:

	Ptr_t mpArray;
	Ptr_t mpEnd;
	Ptr_t mpLast;

	void GrowArray(Sz_t = 0);

	static inline Itor_t ArrayCopy(Itor_t _src_beg, Itor_t _src_end, Itor_t _dest);
	static inline void ArrayMove(Ptr_t _src, Sz_t _sz, Ptr_t _dest);

	static inline Ptr_t Allocate(Sz_t);
	static inline void Deallocate(Itor_t);

	static inline void Destroy(T&);
	static inline void DestroyArray(Itor_t, Itor_t);
};






// ============================================ FUNCTION DEFINITIONS ============================================ // 


template <class T, class A>
AutoArray<T, A>::AutoArray(void) noexcept : 
	mpArray{ nullptr }, mpEnd{ nullptr }, mpLast{ nullptr }
{
}

template <class T, class A>
AutoArray<T, A>::AutoArray(Sz_t _nSize) :
	mpArray{ Allocate(_nSize) }, mpEnd{ mpArray + _nSize }, mpLast{ mpArray }
{
}

template <class T, class A>
AutoArray<T, A>::AutoArray(const AutoArray<T, A>& _other) :
	mpArray{ Allocate(_other.size()) }, mpEnd{ mpArray + _other.size() },
	mpLast{ ArrayCopy(_other.mpArray, _other.mpLast, mpArray) }
{
}

template <class T, class A>
AutoArray<T, A>::AutoArray(AutoArray<T, A>&& _other) noexcept :
	mpArray{ _other.mpArray }, mpEnd{ _other.mpEnd }, mpLast{ _other.mpLast }
{
	_other.mpArray = _other.mpEnd = _other.mpLast = nullptr;
}

template <class T, class A> template <typename U, typename>
AutoArray<T, A>::AutoArray(std::initializer_list<U> _il) :
	AutoArray(static_cast<Sz_t>(_il.size()))
{
	for (auto& e : _il)
		EmplaceBack(Utility::Move(e));
}

template <class T, class A>
AutoArray<T, A>::~AutoArray(void)
{
	clear();
	Deallocate(mpArray);
}


// Gets the pointer to the first element
template <class T, class A>
inline typename AutoArray<T, A>::Itor_t AutoArray<T, A>::begin(void) const noexcept
{
	return mpArray;
}

// Gets the pointer to the end of the array
template <class T, class A>
inline typename AutoArray<T, A>::Itor_t AutoArray<T, A>::end(void) const noexcept
{
	return mpLast;
}

// Gets the last element
template <class T, class A>
inline T& AutoArray<T, A>::back(void) const noexcept
{
#if _DEBUG
	DEBUG_LOG(mpArray == mpLast, "DynamicArray Error: Array is empty!\n");
	if (mpArray == mpLast) __debugbreak();
#endif

	return *(mpLast - 1);
}

// Gets the current number of elements in the array
template <class T, class A>
inline typename AutoArray<T, A>::Sz_t AutoArray<T, A>::size(void) const noexcept
{
	return static_cast<Sz_t>(mpLast - mpArray);
}

template<class T, class A>
inline typename AutoArray<T, A>::Sz_t AutoArray<T, A>::Cap(void) const noexcept
{
	return static_cast<Sz_t>(mpEnd - mpArray);
}

// Inserts to the back of the array. Synonymous to Insert
template <class T, class A>
inline void AutoArray<T, A>::push_back(const T& _obj)
{
	Insert(_obj);
}

// Removes the last element of the array. Synonymous to RemoveLast
template <class T, class A>
inline void AutoArray<T, A>::pop_back(void) noexcept
{
	Remove();
}

// Empties the array
template <class T, class A>
inline void AutoArray<T, A>::clear(void) noexcept
{
	DestroyArray(mpArray, mpLast);
	mpLast = mpArray;
}

// Minimises the memory used by the array
template <class T, class A>
inline void AutoArray<T, A>::shrink(void)
{
	Sz_t sz = size();
	if (!sz)
	{
		Deallocate(mpArray);
		return;
	}

	GrowArray(sz);
}

// Ensures that there are _nSize amounts of empty slots
template <class T, class A>
inline void AutoArray<T, A>::reserve(Sz_t _nSize)
{
	Sz_t sz = size();
	if(sz + _nSize > mpEnd - mpArray)
		GrowArray(sz + _nSize);
}


// Inserts an element to the back of the array
template <class T, class A>
void AutoArray<T, A>::Insert(const T& _obj)
{
	if (mpLast == mpEnd)
	{
		GrowArray();
	}

	new (mpLast) T{ _obj };
	++mpLast;
}

// Insert an element to the specified position
template <class T, class A>
void AutoArray<T, A>::Insert(const T& _obj, const Sz_t _nIndex)
{
#if _DEBUG
	if (!(_nIndex < size()))
	{
		DEBUG_PRINT("DynamicArray Error: Array index out of range!\n");
		__debugbreak();
	}
#endif

	if (mpLast == mpEnd)
		GrowArray();

	Itor_t at = mpArray + _nIndex;
	Itor_t j = mpLast;
	Itor_t i = ++mpLast;

	while (i != at)
	{
		*i = Utility::Move(*j);
		--i; --j;
	}

	*at = _obj;
}

// In-place insert an element to the back of the array
template <class T, class A> template<typename ...Args>
void AutoArray<T, A>::EmplaceBack(Args &&...args)
{
	if (mpLast == mpEnd)
		GrowArray();

	new (mpLast) T{ Utility::Forward<Args>(args)... };
	++mpLast;
}


// Removes the last element of the array
template <class T, class A>
inline void AutoArray<T, A>::Remove(void) noexcept
{
#if _DEBUG
	DEBUG_LOG(mpArray == mpLast, "DynamicArray Error: Attempted remove from empty!\n");
	if (mpArray == mpLast) __debugbreak();
#endif

	--mpLast;
	Destroy(*mpLast);
}

// Removes all matching elements
template<class T, class A>
void AutoArray<T, A>::Remove(const T& _obj) noexcept
{
	Itor_t start  = begin();
	Itor_t target = start;
	Itor_t goal   = end();

	while (target != goal)
	{
		if (_obj == *target)
		{
			++target;
			continue;
		}

		*start = Utility::Move(*target);
		++target;
		++start;
	}

	auto count = goal - start;
	while (count--)
		Remove();
}

// Remove an element at the specified position of the array
// Maintains the current order of the array
template <class T, class A>
inline void AutoArray<T, A>::Remove(const Sz_t _nIndex)
{
	Remove(mpArray + _nIndex);
}

template<class T, class A>
inline void AutoArray<T, A>::Remove(Itor_t _pObj)
{
#if _DEBUG
	DEBUG_LOG(mpArray == mpLast, "DynamicArray Error: Attempted remove from empty!\n");
	if (mpArray == mpLast) __debugbreak();
#endif

	Itor_t nxt = _pObj + 1;
	while (nxt != mpLast)
	{
		*(_pObj) = Utility::Move(*nxt);
		++_pObj; ++nxt;
	}

	mpLast = _pObj;
	Destroy(*nxt);
}

// Remove an element at the specified position of the array
// Destroys the current order of the array
template <class T, class A>
inline void AutoArray<T, A>::FastRemove(const Sz_t _nIndex)
{
	FastRemove(mpArray + _nIndex);
}

template<class T, class A>
inline void AutoArray<T, A>::FastRemove(const Itor_t& _pObj)
{
#if _DEBUG
	DEBUG_LOG(mpArray == mpLast, "DynamicArray Error: Attempted remove from empty!\n");
	if (mpArray == mpLast) return __debugbreak();


	if (!(_pObj < mpLast))
	{
		DEBUG_PRINT("AutoArray Error: Invalid Remove Index!\n");
		return;
	}
#endif

	--mpLast;
	Utility::Swap(*_pObj, *mpLast);
	Destroy(*mpLast);
}

template <class T, class A>
inline bool AutoArray<T, A>::IsEmpty(void) const noexcept
{
	return mpArray == mpLast;
}


// Sorts the array.
// Defaults to sorting in ascending order based on the '<' operator
template <class T, class A> template<typename Comparator>
void AutoArray<T, A>::Sort(Comparator _pTest)
{
	const Sz_t last = mpLast - mpArray;
	T temp;

	for (Sz_t n2, n1 = 1; n1 < last; ++n1)
	{
		temp = Utility::Move(mpArray[n1]);

		n2 = n1;
		while (n2-- > 0 && _pTest(temp, mpArray[n2]))
		{
			mpArray[n2 + 1] = Utility::Move(mpArray[n2]);
		}

		mpArray[n2 + 1] = Utility::Move(temp);
	}
}

template <class T, class A>
void AutoArray<T, A>::GrowArray(Sz_t _newSize)
{
	Sz_t cap_old = Cap(), sz = size();
	Sz_t cap_new = !_newSize ? static_cast<Sz_t>(cap_old > 1 ? cap_old * Math::phi : 2) : _newSize;
	Ptr_t pNewArray = Allocate(cap_new);

	ArrayMove(mpArray, sz, pNewArray);
//	DestroyArray(mpArray, mpLast);
	Deallocate(mpArray);

	mpArray = pNewArray;
	mpEnd   = mpArray + cap_new;
	mpLast  = mpArray + sz;
}

template <class T, class A>
inline typename AutoArray<T, A>::Itor_t AutoArray<T, A>::ArrayCopy(Itor_t _src_beg, Itor_t _src_end, Itor_t _dest)
{
	return Utility::CopyInit(_src_beg, _src_end, _dest);
}

template <class T, class A>
inline void AutoArray<T, A>::ArrayMove(Ptr_t _src, Sz_t _sz, Ptr_t _dest)
{
	std::memcpy(_dest, _src, sizeof(T) * _sz);
}

template <typename T, class A>
inline T* AutoArray<T, A>::Allocate(Sz_t _nSize)
{
//	return static_cast<T*>(::operator new[](_nSize * sizeof(T)));
	return A::Alloc(_nSize);
}

template <typename T, class A>
inline void AutoArray<T, A>::Deallocate(Itor_t _arr)
{
//	::operator delete[] (static_cast<void*>(_arr));
	A::Free(_arr);
}

template <class T, class A>
inline void AutoArray<T, A>::Destroy(T& _obj)
{
	_obj; // Stop unreferenced parameter warnings for basic types
	_obj.~T();
}

template <class T, class A>
inline void AutoArray<T, A>::DestroyArray(Itor_t _pBegin, Itor_t _pEnd)
{
	// Destroy (or at least attempt to) in reverse order of creation
	while (_pBegin != _pEnd)
	{
		Destroy(*--_pEnd);
	}
}



// ============================================ OPERATOR OVERLOADING ============================================ // 


template <class T, class A>
T& AutoArray<T, A>::operator[] (const Sz_t _nIndex)
{
	return const_cast<T&>(static_cast<AutoArray const &>(*this)[_nIndex]);
}

template <class T, class A>
const T& AutoArray<T, A>::operator[] (const Sz_t _nIndex) const
{
#if _DEBUG
	if (!(mpArray + _nIndex < mpLast))
	{
		DEBUG_PRINT("AutoArray Error: Array index out of range!\n");
		__debugbreak();
	}
#endif

	return *(mpArray + _nIndex);
}

template <class T, class A>
AutoArray<T, A>& AutoArray<T, A>::operator= (const AutoArray<T, A>& _other)
{
	Sz_t sz = _other.size();
	if (size() < sz)
		GrowArray(sz);

	ArrayCopy(_other.mpArray, _other.mpLast, mpArray);
	return *this;
}

template <class T, class A>
AutoArray<T, A>& AutoArray<T, A>::operator= (AutoArray<T, A>&& _other) noexcept
{
	Utility::Swap(mpEnd  , _other.mpEnd  );
	Utility::Swap(mpLast , _other.mpLast );
	Utility::Swap(mpArray, _other.mpArray);

	return *this;
}



#endif		// INCLUDE GUARD

