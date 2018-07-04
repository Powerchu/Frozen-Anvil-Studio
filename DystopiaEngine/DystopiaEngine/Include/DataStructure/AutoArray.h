/* HEADER *********************************************************************************/
/*!
\file	AutoArray.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Run-time auto resizing array.
	Now 100% neater!

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef DYNAMIC_ARRAY
#define DYNAMIC_ARRAY

#if defined(DEBUG) | defined(_DEBUG)
#include "Utility\DebugAssert.h"
#endif // Debug only includes

#include "Utility\Utility.h"			// Move
#include "Math\MathUtility.h"			// phi
#include "Utility\Meta.h"
#include "Utility\MetaDataStructures.h" // TypeList

#include <new>     // operator new

template<class T>
class AutoArray
{
public:
	// ====================================== CONSTRUCTORS ======================================= // 

	AutoArray(void) noexcept;
	explicit AutoArray(unsigned _nSize);
	AutoArray(const AutoArray<T>& _other);
	AutoArray(AutoArray<T>&& _other) noexcept;

	~AutoArray(void);


	// =================================== CONTAINER FUNCTIONS =================================== // 

	// Gets the begin iterator
	inline T* begin(void) const;

	// Gets the end iterator
	inline T* end(void) const;

	// Gets the last element
	inline T& back(void) const;

	// Gets the current number of elements in the array
	inline unsigned size(void) const;

	// Synonymous to Insert
	inline void push_back(const T& _obj);

	// Synonymous to Remove
	inline void pop_back(void);

	// Empties the array
	inline void clear(void) noexcept;

	// Minimises the memory used by the array
	inline void shrink(void);

	// Ensures that there are the specified amount of unused slots
	inline void reserve(unsigned _nSize);

	// Inserts an element to the back of the array
	void Insert(const T& _obj);

	// Insert an element to the specified position of the array
	void Insert(const T& _obj, const unsigned _nIndex);

	// In-place insert an element to the back of the array
	template<typename ...Args>
	void EmplaceBack(Args &&...args);

	// Removes the last element of the array
	inline void Remove(void);

	// Remove all matching elements
	inline void Remove(const T&);

	// Remove an element at the specified position of the array
	// Preserves the current order of the array
	void Remove(const unsigned _nIndex);

	// Remove an element at the specified position of the array
	// Destroys the current order of the array
	inline void FastRemove(const unsigned _nIndex);

	inline bool IsEmpty(void) const noexcept;

	// Sorts the array using Insertion Sort.
	// Defaults to sorting in ascending order based on the '<' operator
	template<typename Comparator>
	void Sort(Comparator _pTest = [](T& lhs, T& rhs)->bool { return lhs < rhs; });


	// ======================================== OPERATORS ======================================== // 

	T& operator[] (const unsigned _nIndex);
	const T& operator[] (const unsigned _nIndex) const;

	AutoArray<T>& operator= (const AutoArray<T>& _other);
	AutoArray<T>& operator= (AutoArray<T>&& _other) noexcept;

private:

	T* mpArray;
	unsigned mnLast;
	unsigned mnSize;

	void GrowArray(int = 0);

	inline void ArrayCopy(T*);
	inline void ArrayMove(T*);

	inline T* Construct(unsigned);
	inline void Destruct(T*);

	/* Not required if operator new is not overloaded by any class
		- Bug : Requires all other classes to also overload operator new

	template <typename = void>
	inline auto Construct(unsigned) -> decltype(Utility::EnableIf<Utility::HasMember<T>::value, T*>::type{});
	template <typename = void>
	inline auto Construct(unsigned) -> decltype(Utility::EnableIf<!Utility::HasMember<T>::value, T*>::type{});

	template <typename = void>
	inline auto Destruct(T*) -> decltype(Utility::EnableIf<Utility::HasMember<T>::value, void>::type());
	template <typename = void>
	inline auto Destruct(T*) -> decltype(Utility::EnableIf<!Utility::HasMember<T>::value, void>::type());
	*/

	inline void Destroy(T&);
	inline void DestroyArray(T*, T*);
};






// ============================================ FUNCTION DEFINITIONS ============================================ // 


template <class T>
AutoArray<T>::AutoArray(void) noexcept : 
	mnSize{ 0 }, mnLast{ 0 }, mpArray{ nullptr }
{
}

template <class T>
AutoArray<T>::AutoArray(unsigned _nSize) :
	mnSize{ _nSize }, mnLast{ 0 }, mpArray{ Construct(_nSize) }
{
}

template <class T>
AutoArray<T>::AutoArray(const AutoArray<T>& _other) :
	mnSize{ _other.mnSize }, mnLast{ _other.mnLast }, mpArray{ Construct(_other.mnSize * sizeof(T)) }
{
	memset(mpArray, 0, mnSize * sizeof(T));
	ArrayCopy(_other.mpArray);
}

template <class T>
AutoArray<T>::AutoArray(AutoArray<T>&& _other) noexcept :
	mnSize{ _other.mnSize }, mnLast{ _other.mnLast }, mpArray{ _other.mpArray }
{
	_other.mpArray = nullptr;
	_other.mnSize = _other.mnLast = 0;
}

template <class T>
AutoArray<T>::~AutoArray(void)
{
	clear();
	Destruct(mpArray);
}


// Gets the pointer to the first element
template <class T>
inline T* AutoArray<T>::begin(void) const
{
	return mpArray;
}

// Gets the pointer to the end of the array
template <class T>
inline T* AutoArray<T>::end(void) const
{
	return mpArray + mnLast;
}

// Gets the last element
template <class T>
inline T& AutoArray<T>::back(void) const
{
	return *(mpArray + mnLast - 1);
}

// Gets the current number of elements in the array
template <class T>
inline unsigned AutoArray<T>::size(void) const
{
	return mnLast;
}

// Inserts to the back of the array. Synonymous to Insert
template <class T>
inline void AutoArray<T>::push_back(const T& _obj)
{
	Insert(_obj);
}

// Removes the last element of the array. Synonymous to RemoveLast
template <class T>
inline void AutoArray<T>::pop_back(void)
{
	Remove();
}

// Empties the array
template <class T>
inline void AutoArray<T>::clear(void) noexcept
{
	DestroyArray(mpArray, mpArray + mnLast);

	mnLast = 0;
}

// Minimises the memory used by the array
template <class T>
inline void AutoArray<T>::shrink(void)
{
	if (!mnLast)
	{
		Destruct(mpArray);
		mnSize = 0;
		return;
	}

	GrowArray(mnLast);
}

// Ensures that there are _nSize amounts of empty slots
template <class T>
inline void AutoArray<T>::reserve(unsigned _nSize)
{
	if(mnLast + _nSize > mnSize)
		GrowArray(mnLast + _nSize);
}


// Inserts an element to the back of the array
template <class T>
void AutoArray<T>::Insert(const T& _obj)
{
	if (mnLast == mnSize)
	{
		GrowArray();
	}

	new (mpArray + mnLast) T{ _obj };
	++mnLast;
}

// Insert an element to the specified position
template <class T>
void AutoArray<T>::Insert(const T& _obj, const unsigned _nIndex)
{
#if _DEBUG
	if (!(_nIndex < mnLast))
	{
		DEBUG_PRINT("DynamicArray Error: Array index out of range!\n");
		__debugbreak();
	}
#endif

	if (mnLast == mnSize)
		GrowArray();

	unsigned n = mnLast;
	while (n-- > _nIndex)
	{
		mpArray[n + 1] = mpArray[n];
	}

	mpArray[n] = _obj;
}

// In-place insert an element to the back of the array
template <class T> template<typename ...Args>
void AutoArray<T>::EmplaceBack(Args &&...args)
{
	if (mnLast == mnSize)
		GrowArray();

	new (mpArray + mnLast) T{ static_cast<Args&&>(args)... };
	++mnLast;
}


// Removes the last element of the array
template <class T>
inline void AutoArray<T>::Remove(void)
{
#if _DEBUG
	DEBUG_LOG(0 == mnLast, "DynamicArray Error: Attempted remove from empty!\n");
	if (0 == mnLast) return;
#endif

	--mnLast;
	Destroy(*(mpArray + mnLast));
}

// Removes all matching elements
template<class T>
inline void AutoArray<T>::Remove(const T& _obj)
{
	T* start = begin();
	T* target = begin();
	T* goal = end();

	while (target != goal)
	{
		if (_obj == *target)
		{
			++target;
			continue;
		}

		*start = (start != target) ? Utility::Move(*target) : *start;
		++target;
		++start;
	}

	auto count = goal - start;
	while (count--)
		Remove();
}

// Remove an element at the specified position of the array
// Maintains the current order of the array
template <class T>
void AutoArray<T>::Remove(const unsigned _nIndex)
{
#if _DEBUG
	DEBUG_LOG(0 == mnLast, "DynamicArray Error: Attempted remove from empty!\n");
	if (0 == mnLast) return;
#endif
	unsigned nIndex = _nIndex;
	while (nIndex < mnLast)
	{
		*(mpArray + nIndex) = Utility::Move(*(mpArray + nIndex + 1));
		++nIndex;
	}

	Remove();
}

// Remove an element at the specified position of the array
// Destroys the current order of the array
template <class T>
inline void AutoArray<T>::FastRemove(const unsigned _nIndex)
{

#if _DEBUG
	DEBUG_LOG(0 == mnLast, "DynamicArray Error: Attempted remove from empty!\n");
	if (0 == mnLast) return;


	if (_nIndex < mnLast - 1)
	{
		DEBUG_PRINT("AutoArray Error: Invalid Remove Index!\n");
		return;
	}
#endif

	Utility::Swap(*(mpArray + _nIndex), *(mpArray + mnLast));
	Remove();
}

template <class T>
inline bool AutoArray<T>::IsEmpty(void) const noexcept
{
	return 0 == mnLast;
}


// Sorts the array.
// Defaults to sorting in ascending order based on the '<' operator
template <class T> template<typename Comparator>
void AutoArray<T>::Sort(Comparator _pTest)
{
	T temp;

	for (unsigned n2, n1 = 1; n1 < mnLast; ++n1)
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

#include <utility>

template <class T>
void AutoArray<T>::GrowArray(int _newSize)
{
#if _DEBUG
	DEBUG_LOG(_newSize < 0, "DynamicArray Error: Array size is negative!\n");
#endif

	unsigned nNewSize = _newSize == 0 ? static_cast<unsigned>((mnSize>0?mnSize:1) * Math::phi + .5f) : _newSize;
	T* pNewArray = Construct(nNewSize);
	T* temp = mpArray;

	mpArray = pNewArray;
	mnSize = nNewSize;

	ArrayMove(temp);

	DestroyArray(temp, temp + mnLast);
	Destruct(temp);
}

template <class T>
inline void AutoArray<T>::ArrayCopy(T* _other)
{
	for (unsigned n = 0; n < mnLast; ++n)
	{
		*(mpArray + n) = *(_other + n);
	}
}

template <class T>
inline void AutoArray<T>::ArrayMove(T* _other)
{
	for (unsigned n = 0; n < mnLast; ++n)
	{
		new (mpArray + n) T{ Utility::Move(*(_other + n)) };
	}
}

template <typename T>
inline T* AutoArray<T>::Construct(unsigned _nSize)
{
	T* ptr = static_cast<T*>(operator new[](_nSize * sizeof(T)));
//	memset(ptr, 0, _nSize * sizeof(T));

	return static_cast<T*> (ptr);
}

template <typename T>
inline void AutoArray<T>::Destruct(T* _arr)
{
	::operator delete[](static_cast<void*>(_arr));
}

/*
template <typename T> template <typename>
inline auto AutoArray<T>::Construct(unsigned _nSize) -> decltype(Utility::EnableIf<Utility::HasMember<T>::value, T*>::type{})
{
	return static_cast<T*> (T::operator new[](_nSize * sizeof(T)));
}

template <typename T> template <typename>
inline auto AutoArray<T>::Construct(unsigned _nSize) -> decltype(Utility::EnableIf<!Utility::HasMember<T>::value, T*>::type{})
{
	return static_cast<T*> (::operator new[](_nSize * sizeof(T)));
}

template <typename T> template <typename>
inline auto AutoArray<T>::Destruct(T* _arr) -> decltype(Utility::EnableIf<Utility::HasMember<T>::value, void>::type())
{
	T::operator delete[] (_arr);
}

template <typename T> template <typename>
inline auto AutoArray<T>::Destruct(T* _arr) -> decltype(Utility::EnableIf<!Utility::HasMember<T>::value, void>::type())
{
	::operator delete[] (static_cast<void*>(_arr));
}
*/

template <class T>
inline void AutoArray<T>::Destroy(T& _obj)
{
	_obj; // Stop unreferenced parameter warnings from types that do not have dtors
	_obj.~T();
}

template <class T>
inline void AutoArray<T>::DestroyArray(T* _pBegin, T* _pEnd)
{
	// Destroy (or at least attempt to) in reverse order of creation
	while (_pBegin != _pEnd)
	{
		--_pEnd;
		Destroy(*_pEnd);
	}
}



// ============================================ OPERATOR OVERLOADING ============================================ // 


template <class T>
T& AutoArray<T>::operator[] (const unsigned _nIndex)
{
#if _DEBUG
	if (!(_nIndex < mnSize))
	{
		DEBUG_PRINT("AutoArray Error: Array index out of range!\n");
		__debugbreak();
	}
#endif

	return *(mpArray + _nIndex);
}

template <class T>
const T& AutoArray<T>::operator[] (const unsigned _nIndex) const
{
#if _DEBUG
	if (!(_nIndex < mnSize))
	{
		DEBUG_PRINT("AutoArray Error: Array index out of range!\n");
		__debugbreak();
	}
#endif

	return *(mpArray + _nIndex);
}

template <class T>
AutoArray<T>& AutoArray<T>::operator= (const AutoArray<T>& _other)
{
	if (mnSize < _other.mnSize)
		GrowArray(_other.mnSize);

	mnSize = _other.mnSize;
	mnLast = _other.mnLast;
	ArrayCopy(_other.mpArray);

	return *this;
}

template <class T>
AutoArray<T>& AutoArray<T>::operator= (AutoArray<T>&& _other) noexcept
{
	Utility::Swap(mpArray, _other.mpArray);
	Utility::Swap(mnLast, _other.mnLast);
	Utility::Swap(mnSize, _other.mnSize);

	return *this;
}



#endif		// INCLUDE GUARD

