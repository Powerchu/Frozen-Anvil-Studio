/* HEADER *********************************************************************************/
/*!
\file	Heap.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Implementation of a Heap defaulted to using AutoArray.

	You can replace the default container with any container, hopefully, that 
	returns a RandomAccessIterator and supports the following functions:
		-reserve
		-begin
		-end
		-push_back
		-pop_back
		-clear
		-size

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _HEAP_H_
#define _HEAP_H_

#include "Globals.h"
#include "DataStructure\AutoArray.h"	// AutoArray

template<typename T, class Container = AutoArray<T>>
class _DLL_EXPORT Heap
{
	Container mData;

	template <typename _T = decltype(mData.begin())> class HeapIterator;

public:
	// ====================================== CONSTRUCTORS ======================================= // 

	Heap(void) noexcept;
	explicit Heap(unsigned _nSize);

	~Heap(void);


	// =================================== CONTAINER FUNCTIONS =================================== // 

	inline HeapIterator<> begin() const;
	inline HeapIterator<> end() const;

	template <typename Comp>
	void Insert(const T&, Comp&&);
	template <typename Comp>
	void Pop(Comp&&);

	inline void Insert(T&&);
	inline void Pop(void);
	void Clear(void);

	unsigned Size(void) const;
	bool IsEmpty(void) const;

	T& First(void) const;
	
	template <typename _T>
	class _DLL_EXPORT HeapIterator
	{
	public:

		using iterator_category = std::bidirectional_iterator_tag;

		explicit HeapIterator(_T) noexcept;

		inline HeapIterator& operator++(void) noexcept;
		inline HeapIterator  operator++(int) noexcept;
		inline HeapIterator& operator--(void) noexcept;
		inline HeapIterator  operator--(int) noexcept;

		inline bool operator== (const HeapIterator&) noexcept;
		inline bool operator!= (const HeapIterator&) noexcept;

		inline T& operator* (void);
		inline T* operator->(void);

	private:
		_T mpRaw;
	};

private:

	template <typename Comp>
	void SiftUp(unsigned, Comp);
	template <typename Comp>
	void SiftDown(unsigned, Comp);

	struct _DLL_EXPORT DefaultCompare
	{
		inline bool operator ()(const T&, const T&);
	};

	inline unsigned Left(unsigned) const;
	inline unsigned Right(unsigned) const;
	inline unsigned Child(unsigned) const;
	inline unsigned Parent(unsigned) const;
};






// ============================================ FUNCTION DEFINITIONS ============================================ // 


template<typename T, class Container>
Heap<T, Container>::Heap(void) noexcept : mData{}
{

}
	
template<typename T, class Container>
Heap<T, Container>::Heap(unsigned _nSize) : Heap{}
{
	mData.reserve(_nSize);
}

template<typename T, class Container>
Heap<T, Container>::~Heap(void)
{
}

template<typename T, class Container>
inline typename Heap<T, Container>::HeapIterator<> Heap<T, Container>::begin(void) const
{
	return HeapIterator<>{ mData.begin() };
}

template<typename T, class Container>
inline typename Heap<T,Container>::HeapIterator<> Heap<T, Container>::end(void) const
{
	return HeapIterator<>{ mData.end() };
}

template<typename T, class Container>
inline void Heap<T, Container>::Insert(T&& _obj)
{
	Insert(Utility::Forward<T>(_obj), DefaultCompare{});
}

template<typename T, class Container>
inline void Heap<T, Container>::Pop(void)
{
	Pop(DefaultCompare{});
}

template<typename T, class Container> template <typename Comp>
void Heap<T, Container>::Insert(const T& _obj, Comp&& _compare)
{
	unsigned n = mData.size();
	mData.push_back(_obj);
	SiftUp(n, Utility::Forward<Comp>(_compare));
}

template<typename T, class Container> template <typename Comp>
void Heap<T, Container>::Pop(Comp&& _compare)
{
	// Remove the first element by swapping it with the last
	// Then popping the last element off the data structure
	Utility::Swap(*mData.begin(), *(mData.end() - 1));
	mData.pop_back();

	SiftDown(0, Utility::Forward<Comp>(_compare));
}

template<typename T, class Container>
inline void Heap<T, Container>::Clear(void)
{
	mData.clear();
}

template<typename T, class Container>
unsigned Heap<T, Container>::Size(void) const
{
	return static_cast<unsigned>(mData.size());
}

template<typename T, class Container>
inline bool Heap<T, Container>::IsEmpty(void) const
{
	return 0 == Size();
}

template<typename T, class Container>
T& Heap<T, Container>::First(void) const
{
#if defined(DEBUG) | defined(_DEBUG)
	DEBUG_ASSERT(Size() == 0, "Heap Error: Array index out of range! \n");
#endif

	return *mData.begin();
}
/*
template<typename T, class Container>
inline void Heap<T, Container>::SiftUp(unsigned _nIndex)
{
	SiftUp(_nIndex, [](const T& rhs, const T& lhs)
	{
		return rhs < lhs;
	});
}

template<typename T, class Container>
inline void Heap<T, Container>::SiftDown(unsigned _nIndex)
{
	SiftDown(_nIndex, [](const T& rhs, const T& lhs)
	{
		return rhs < lhs;
	});
} 
*/
template<typename T, class Container> template <typename Comp>
void Heap<T, Container>::SiftUp(unsigned _nIndex, Comp _compare)
{
	auto First = mData.begin();
	auto toSift = First + _nIndex;

	unsigned father = Parent(_nIndex);

	// While the obj is strictly smaller than it's parent
	while (_compare(*toSift, *(First + father)))
	{
		Utility::Swap(*toSift, *(First + father));
		toSift = First + father;

		father = Parent(father);
	}
}

template<typename T, class Container> template <typename Comp>
void Heap<T, Container>::SiftDown(unsigned _nIndex, Comp _compare)
{
	auto First = mData.begin();
	const unsigned size = Size();

	unsigned LChild = Left(_nIndex);
	unsigned RChild = Right(_nIndex);
	unsigned Path = ((RChild < size) && _compare(*(First + RChild), *(First + LChild))) ? RChild : LChild;

	while ((LChild < size) && _compare(*(First + Path), *(First + _nIndex)))
	{
		Utility::Swap(*(First + Path), *(First + _nIndex));

		_nIndex = Path;
		LChild = Left(_nIndex);
		RChild = Right(_nIndex);

		Path = ((RChild < size) && _compare(*(First + RChild), *(First + LChild))) ? RChild : LChild;
	}
}

template<typename T, class Container>
inline unsigned Heap<T, Container>::Left(unsigned _nIndex) const
{
	return Right(_nIndex) - 1;
}

template<typename T, class Container>
inline unsigned Heap<T, Container>::Right(unsigned _nIndex) const
{
	return Child(_nIndex);
}

template<typename T, class Container>
inline unsigned Heap<T, Container>::Child(unsigned _nIndex) const
{
	return ++_nIndex << 1;
}

template<typename T, class Container>
inline unsigned Heap<T, Container>::Parent(unsigned _nIndex) const
{
	return (_nIndex == 0) ? 0 : --_nIndex >> 1;
}

template <typename T, class Container>
inline bool Heap<T, Container>::DefaultCompare::operator ()(const T& _rhs, const T& _lhs)
{
	return _rhs < _lhs;
}



// ================================================ HEAP ITERATOR =============================================== // 


template<typename T, class Container> template <typename _T>
Heap<T, Container>::HeapIterator<_T>::HeapIterator(_T _pRaw) noexcept :
	mpRaw{ _pRaw }
{

}

template<typename T, class Container> template <typename _T>
inline typename Heap<T, Container>::HeapIterator<_T>& Heap<T, Container>::HeapIterator<_T>::operator++(void) noexcept
{
	++mpRaw;
	return *this;
}

template<typename T, class Container> template <typename _T>
inline typename  Heap<T, Container>::HeapIterator<_T> Heap<T, Container>::HeapIterator<_T>::operator++(int) noexcept
{
	HeapIterator ret{ *this };
	++mpRaw;
	return ret;
}

template<typename T, class Container> template <typename _T>
inline typename Heap<T, Container>::HeapIterator<_T>& Heap<T, Container>::HeapIterator<_T>::operator--(void) noexcept
{
	--mpRaw;
	return *this;
}

template<typename T, class Container> template <typename _T>
inline typename Heap<T, Container>::HeapIterator<_T> Heap<T, Container>::HeapIterator<_T>::operator--(int) noexcept
{
	HeapIterator ret{ *this };
	--mpRaw;
	return ret;
}

template<typename T, class Container> template <typename _T>
inline bool Heap<T, Container>::HeapIterator<_T>::operator== (const typename Heap<T, Container>::HeapIterator<_T>& _rhs) noexcept
{
	return mpRaw == _rhs.mpRaw;
}

template<typename T, class Container> template <typename _T>
inline bool Heap<T, Container>::HeapIterator<_T>::operator!= (const typename Heap<T, Container>::HeapIterator<_T>& _rhs) noexcept
{
	return !(*this == _rhs);
}

template<typename T, class Container> template <typename _T>
inline T& Heap<T, Container>::HeapIterator<_T>::operator* (void)
{
	return *mpRaw;
}

template<typename T, class Container> template <typename _T>
inline T* Heap<T, Container>::HeapIterator<_T>::operator->(void)
{
	return &*mpRaw;
}



#endif		// INCLUDE GUARD

