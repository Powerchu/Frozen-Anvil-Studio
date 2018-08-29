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
#include "Utility\Utility.h"


template <typename T, size_t SIZE>
class Array
{
public:
	// ==================================== CONTAINER DEFINES ==================================== // 

	using Itor_t      = T *;
	using ConstItor_t = T const *;
	using Val_t       = T;
	using Ptr_t       = T *;
	using Sz_t        = size_t;

	static constexpr Sz_t mnSize = SIZE;


private:
	template <typename ... Us>
	struct IsAnyArray
	{
		static constexpr bool value = false;
	};
	template <typename U, typename ... Us>
	struct IsAnyArray<U, Us...>
	{
		static constexpr bool value = IsAnyArray<Us...>::value;
	};
	template <typename U, size_t SZ, typename ... Us>
	struct IsAnyArray<Array<U, SZ>, Us...>
	{
		static constexpr bool value = true;
	};
public:


	// ====================================== CONSTRUCTORS ======================================= // 

	template <typename ... U, typename =
		Utility::EnableIf_t<!IsAnyArray<Utility::Decay_t<U>...>::value>>
	Array(U&& ...) noexcept;
	Array(const Array&) = default;
	Array(Array&&) noexcept = default;

	// For some reason the dtor disappears even if I "= default" it.
	// Also the defaulted one might crash Microsoft Compiler
	~Array(void) noexcept {}


	// =================================== CONTAINER FUNCTIONS =================================== // 

	inline Itor_t      end(void) noexcept;
	inline ConstItor_t end(void) const noexcept;
	inline Itor_t      begin(void) noexcept;
	inline ConstItor_t begin(void) const noexcept;

	constexpr inline Sz_t size(void) const noexcept;



	// ======================================== OPERATORS ======================================== // 

	Val_t& operator[] (Sz_t _nIndex);
	Val_t const& operator[] (Sz_t _nIndex) const;

	template<typename U>
	auto operator= (const Array<U, SIZE>&) -> Utility::EnableIf_t<!Utility::IsSame<T, U>::value, Array&>;
	Array& operator= (const Array&) = default;
	Array& operator= (Array&&) noexcept = default;
	

private:

	T mArray[SIZE];
};






// ============================================ FUNCTION DEFINITIONS ============================================ // 


namespace Ctor
{
	template <typename T, typename ... Us>
	constexpr Array<T, sizeof...(Us)> MakeArray(Us&&... args) noexcept
	{
		return Array<T, sizeof...(Us)>{ Utility::Forward<Us>(args)... };
	}
}

template <typename T, size_t Sz> template <typename ... U, typename>
Array<T, Sz>::Array(U&& ... _Args) noexcept
	: mArray { Utility::Forward<U>(_Args)... }
{
}

template <typename T, size_t Sz>
inline typename Array<T, Sz>::Itor_t Array<T, Sz>::begin(void) noexcept
{
	return mArray;
}

template <typename T, size_t Sz>
inline typename Array<T, Sz>::ConstItor_t Array<T, Sz>::begin(void) const noexcept
{
	return mArray;
}

template <typename T, size_t Sz>
inline typename Array<T, Sz>::Itor_t Array<T, Sz>::end(void) noexcept
{
	return mArray + Sz;
}

template <typename T, size_t Sz>
inline typename Array<T, Sz>::ConstItor_t Array<T, Sz>::end(void) const noexcept
{
	return mArray + Sz;
}

template <typename T, size_t Sz>
constexpr inline typename Array<T, Sz>::Sz_t Array<T, Sz>::size(void) const noexcept
{
	return Sz;
}



// ============================================ OPERATOR OVERLOADING ============================================ // 


template <typename T, size_t Sz>
inline typename Array<T, Sz>::Val_t& Array<T, Sz>::operator[] (const Sz_t _nIndex)
{
	return mArray[_nIndex];
}

template <typename T, size_t Sz>
inline typename Array<T, Sz>::Val_t const& Array<T, Sz>::operator[] (const Sz_t _nIndex) const
{
	return mArray[_nIndex];
}

template <typename T, size_t Sz> template<typename U>
auto Array<T, Sz>::operator= (const Array<U, Sz>& _other) -> Utility::EnableIf_t<!Utility::IsSame<T, U>::value, Array&>
{
	Itor_t dest = mArray;

	for (auto& e : _other)
		*dest++ = e;

	return *this;
}



#endif		// INCLUDE GUARD

