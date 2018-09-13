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

#include <Utility\MetaAlgorithms.h>


namespace
{
	template <typename T, typename U>
	struct TupleBase;

	template <typename T>
	struct TupleLeaf
	{
		T value;
	};

	template <template <unsigned ...> class R, unsigned ... Ns, template <typename ...> class T, typename ... Ty>
	struct TupleBase <R<Ns...>, T<Ty...>> : public TupleLeaf<Ty> ...
	{};
}

template <typename ... T>
class Tuple : public TupleBase<Utility::MetaMakeRange_t<sizeof...(T)>, Tuple<T...>>
{
	using Range_t = Utility::MetaMakeRange_t<sizeof...(T)>;

public:


	// =================================== CONTAINER FUNCTIONS =================================== // 

	template <unsigned _Index>
	inline decltype(auto) Get(void) noexcept;
};






// ============================================ FUNCTION DEFINITIONS ============================================ // 


template <typename ... T> template <unsigned _Index>
inline decltype(auto) Tuple<T...>::Get(void) noexcept
{
	static_assert(_Index < sizeof...(T), "Tuple: Get Index out of Range!");
	return (static_cast<TupleLeaf<Utility::MetaExtract_t<_Index, Tuple<T...>>>&>(*this).value);
}



#endif		// INCLUDE GUARD

