/* HEADER *********************************************************************************/
/*!
\file	Queue.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Fixed size Queue

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _QUEUE_H_
#define _QUEUE_H_

#include "Utility\Utility.h"	// LoopIncrement

#include <iterator>				// std::input_iterator_tag

template<typename T>
class Queue
{
public:
	struct QueueIterator;
	// ====================================== CONSTRUCTORS ======================================= // 

	explicit Queue(unsigned _nSize);
	Queue(Queue&&);
	Queue(const Queue&) = delete;

	~Queue(void);


	// =================================== CONTAINER FUNCTIONS =================================== // 

	// Gets the iterator to the start of the Queue
	inline QueueIterator begin() const;

	// Gets the iterator to the end of the Queue
	inline QueueIterator end() const;

	// Synonymous with First
	inline const T& front(void) const;

	// Synonymous with Insert
	inline void push_back(const T&);

	// Synonymous with Remove
	inline void pop_front(void);

	void clear(void);

	// Gets the first element of the Queue
	inline const T& First(void) const;

	// Inserts an element to the back of the Queue
	void Insert(const T&);

	// Removes the front most element of the Queue
	void Remove(void);

	// Gets the number of elements in the Queue
	inline unsigned Size(void) const;

	// Returns true if the Queue is full
	// Returns false otherwise
	inline bool IsFull(void) const;

	// Returns true if there are no elements in the Queue
	// Returns false otherwise
	inline bool IsEmpty(void) const;

	struct QueueIterator
	{
		explicit QueueIterator(const T*, unsigned, unsigned) noexcept;

		QueueIterator  operator++(int) noexcept;
		QueueIterator& operator++(void) noexcept;

		const T& operator* (void);
		const T* operator->(void);

		bool operator == (const QueueIterator&) const noexcept;
		bool operator != (const QueueIterator&) const noexcept;

		using iterator_category = std::input_iterator_tag;

	private:

		const T* mpArray;
		unsigned mnPos, mnLimit;
	};

private:

	T* mpArray;
	unsigned mnFront, mnBack, mnSize, mnCap;


	// ======================================== OPERATORS ======================================== // 

	Queue& operator = (const Queue&) = delete;
};






// ============================================ FUNCTION DEFINITIONS ============================================ // 


template <typename T>
Queue<T>::Queue(unsigned _nSize) :
	mpArray{ new T[_nSize] }, mnFront{ 0 }, mnBack{ 0 }, mnSize{ 0 }, mnCap{ _nSize }
{

}

template <typename T>
Queue<T>::Queue(Queue<T>&& _o) :
	mpArray{ _o.mpArray }, mnFront{ _o.mnFront }, mnBack{ _o.mnBack }, mnSize{ _o.mnSize }, mnCap{ _o.mnCap }
{

}

template <typename T>
Queue<T>::~Queue(void)
{
	delete[] mpArray;
}

template <typename T>
inline typename Queue<T>::QueueIterator Queue<T>::begin(void) const
{
	return QueueIterator{ mpArray, mnFront, mnCap };
}

template <typename T>
inline typename Queue<T>::QueueIterator Queue<T>::end(void) const
{
	return QueueIterator{ mpArray, mnBack, mnCap };
}

template <typename T>
inline const T& Queue<T>::front(void) const
{
	return First();
}

template <typename T>
inline void Queue<T>::push_back(const T& _obj)
{
	Insert(_obj);
}

template <typename T>
inline void Queue<T>::pop_front(void)
{
	Remove();
}

template<typename T>
inline void Queue<T>::clear(void)
{
	while (!IsEmpty()) Remove();
}

template <typename T>
const T& Queue<T>::First(void) const
{
	return mpArray[mnFront];
}

template <typename T>
void Queue<T>::Insert(const T& _obj)
{
	mpArray[mnBack] = _obj;
	mnBack = Utility::LoopIncrement(mnBack, mnCap);
}

template <typename T>
void Queue<T>::Remove(void)
{
	--mnSize;
	mnFront = Utility::LoopIncrement(mnFront, mnCap);
}

template <typename T>
inline unsigned Queue<T>::Size(void) const
{
	return mnSize;
}

template<typename T>
inline bool Queue<T>::IsFull(void) const
{
	return mnSize == mnCap;
}

template <typename T> 
inline bool Queue<T>::IsEmpty(void) const
{
	return 0 == mnSize;
}



// =============================================== QUEUE ITERATOR =============================================== // 


template <typename T>
Queue<T>::QueueIterator::QueueIterator(const T* _pArr, unsigned _nPos, unsigned _nLimit) noexcept :
	mpArray{ _pArr }, mnPos{ _nPos }, mnLimit{ _nLimit }
{

}

template <typename T>
typename Queue<T>::QueueIterator& Queue<T>::QueueIterator::operator++ (void) noexcept
{
	mnPos = Utility::LoopIncrement(mnPos, mnLimit);
	return *this;
}

template <typename T>
typename Queue<T>::QueueIterator Queue<T>::QueueIterator::operator++ (int) noexcept
{
	QueueIterator ret = *this;
	mnPos = Utility::LoopIncrement(mnPos, mnLimit);
	return ret;
}

template <typename T>
const T& Queue<T>::QueueIterator::operator* (void)
{
	return mpArray[mnPos];
}

template <typename T>
const T* Queue<T>::QueueIterator::operator->(void)
{
	return mpArray + mnPos;
}

template <typename T>
bool Queue<T>::QueueIterator::operator == (const QueueIterator& _rhs) const noexcept
{
	return (mnPos == _rhs.mnPos) && (mpArray == _rhs.mpArray);
}

template <typename T>
bool Queue<T>::QueueIterator::operator != (const QueueIterator& _rhs) const noexcept
{
	return !(*this == _rhs);
}



#endif		// INCLUDE GUARD

