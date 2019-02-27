/* HEADER *********************************************************************************/
/*!
\file	Tuple.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Why am I even doing this

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _TUPLE_H_
#define _TUPLE_H_

#include "Globals.h"
#include "Utility/MetaAlgorithms.h"


namespace
{
	template <typename T, typename U>
	struct _DLL_EXPORT TupleBase;

	template <size_t, typename T>
	struct _DLL_EXPORT TupleLeaf
	{
		T value;
	};

	template <template <unsigned ...> class R, unsigned ... Ns, template <typename ...> class T, typename ... Ty>
	struct _DLL_EXPORT TupleBase <R<Ns...>, T<Ty...>> : public TupleLeaf<Ns, Ty> ...
	{};
}

template <typename ... T>
class Tuple : public TupleBase<Ut::MetaMakeRange_t<sizeof...(T)>, Tuple<T...>>
{
	using Range_t = Ut::MetaMakeRange_t<sizeof...(T)>;

public:


	// =================================== CONTAINER FUNCTIONS =================================== // 

	template <size_t _Index>
	inline decltype(auto) Get(void) noexcept;

	// For C++ language features
	template <size_t _Idx>
	inline decltype(auto) get(void) noexcept;
};






// ============================================ FUNCTION DEFINITIONS ============================================ // 


template <typename ... T> template <size_t _Idx>
inline decltype(auto) Tuple<T...>::Get(void) noexcept
{
	static_assert(_Idx < sizeof...(T), "Tuple: Get Index out of Range!");
	return (static_cast<TupleLeaf<_Idx, Ut::MetaExtract_t<_Idx, Tuple<T...>>>&>(*this).value);
}

template <typename ... T> template <size_t _Idx>
inline decltype(auto) Tuple<T...>::get(void) noexcept
{
	return Get<_Idx>();
}


// ============================================== LANGAUGE SUPPORT ============================================== // 

_STD_BEGIN
	template <typename ... T>
	struct tuple_size<::Tuple<T...>> :
		integral_constant<size_t, sizeof ... (T)>
	{ };

	template <typename ... T, size_t N>
	struct tuple_element<N, ::Tuple<T...>>
	{
		using type = decltype(declval<::Tuple<T...>>().Get<N>());
	};

	template <typename ... T, size_t N>
	struct tuple_element<N, const ::Tuple<T...>>
	{
		using _Mybase = tuple_element<N, ::Tuple<T...>>; // use std::convention just in case
		using type = add_const_t<typename _Mybase::type>;
	};
_STD_END



#endif		// INCLUDE GUARD

