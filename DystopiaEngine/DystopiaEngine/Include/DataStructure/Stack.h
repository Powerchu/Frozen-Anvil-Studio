/* HEADER *********************************************************************************/
/*!
\file	Stack.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Fixed size stack

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _STACK_H_
#define _STACK_H_

#if defined(DEBUG) | defined(_DEBUG)
#include "Utility\DebugAssert.h"
#endif // Debug only includes

template <typename T>
class Stack
{
public:
	// ====================================== CONSTRUCTORS ======================================= // 

	explicit Stack(unsigned _nSize);

	~Stack(void);


	// =================================== CONTAINER FUNCTIONS =================================== // 

	void Push(const T&);
	void Pop(void);
	const T& Peek(void) const;

	void Clear(void);

	bool IsEmpty(void) const;

	// Please Pretend that this returns a Foward Iterator
	T* begin(void) const;
	// Please Pretend that this returns a Foward Iterator
	T* end(void) const;

private:

	T* mpArray;
	unsigned mnSize, mnCap;


	// ======================================== OPERATORS ======================================== // 

	Stack& operator = (const Stack&) = delete;
};






// ============================================ FUNCTION DEFINITIONS ============================================ // 


template <typename T>
Stack<T>::Stack(unsigned _nSize) :
	mpArray{ nullptr }, mnSize{ 0 }, mnCap{ _nSize }
{
	mpArray = new T[mnCap];
}

template <typename T>
Stack<T>::~Stack(void)
{
	delete[] mpArray;
}

template <typename T>
void Stack<T>::Push(const T& _obj)
{
#if defined(debug) | defined(_DEBUG)
	if (mnSize == mnCap)
	{
		assert(mnSize == mnCap && "Stack Error: Attempted push into full stack! \n");
		--mnSize;
		return;
	}
#endif

	mpArray[mnSize] = _obj;
	++mnSize;
}

template <typename T>
void Stack<T>::Pop(void)
{
#if defined(debug) | defined(_DEBUG)
	if (IsEmpty())
	{
		assert(IsEmpty() && "Stack Error: Attempted pop from empty stack! \n");
		--mnSize;
		return;
	}
#endif

	--mnSize;
}

template <typename T>
const T& Stack<T>::Peek(void) const
{
	return mpArray[mnSize - 1];
}

template <typename T>
void Stack<T>::Clear(void)
{
	mnSize = 0;
}

template<typename T>
inline bool Stack<T>::IsEmpty(void) const
{
	return 0 == mnSize;
}

template<typename T>
T* Stack<T>::begin(void) const
{
	return mpArray + (mnSize - 1);
}

template<typename T>
T* Stack<T>::end(void) const
{
	return mpArray - 1;
}



#endif		// INCLUDE GUARD

