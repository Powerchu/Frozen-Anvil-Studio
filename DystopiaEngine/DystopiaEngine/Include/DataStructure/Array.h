/* HEADER *********************************************************************************/
/*!
\file	Array.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Fixed size array

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _ARRAY_H_
#define _ARRAY_H_

#include "Utility\Meta.h"


template <typename T, size_t SIZE>
class Array
{
	// ==================================== CONTAINER DEFINES ==================================== // 

	using Itor_t = T * ;
	using Val_t = T;
	using Ptr_t = T * ;
	using Sz_t = size_t;

	static constexpr Sz_t mnSize = SIZE;


	// ====================================== CONSTRUCTORS ======================================= // 

	Array(void) noexcept;
	explicit Array(Sz_t _nSize);
	Array(const Array& _other);
	Array(Array&& _other) noexcept;
	template <typename ... U> Array(U&& ...);

	~Array(void);


	// =================================== CONTAINER FUNCTIONS =================================== // 

	inline Itor_t begin(void) const noexcept;
	inline Itor_t end(void) const noexcept;

	constexpr inline Sz_t size(void) const noexcept;

	//inline void push_back(const T& _obj);
	//inline void pop_back(void) noexcept;

	//inline void clear(void) noexcept;

	//void Insert(const T& _obj);
	//void Insert(const T& _obj, const Sz_t _nIndex);
	//void Insert(const T& _obj, const Itor_t _pPos);

	template<typename ...Args>
	Itor_t Emplace(Args &&...args);

	// Removes the last element of the array
	//inline void Remove(void);
	//inline void Remove(const T&);
	//void Remove(const Sz_t _nIndex);
	//void Remove(const Itor_t _pObj);

	inline bool IsEmpty(void) const noexcept;


	// ======================================== OPERATORS ======================================== // 

	Val_t& operator[] (Sz_t _nIndex);
	const Val_t& operator[] (Sz_t _nIndex) const;

	Array& operator= (const Array& _other);
	Array& operator= (Array&& _other) noexcept;
	

private:

	T[SIZE] mArray;
};






// ============================================ FUNCTION DEFINITIONS ============================================ // 


// here



// ============================================ OPERATOR OVERLOADING ============================================ // 


// here



#endif		// INCLUDE GUARD

