/* HEADER *********************************************************************************/
/*!
\file	MetaDataStructures.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Contains useful metafunction "data structures" 
	that are also used by meta algorithms.

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _META_DATASTRUCTS_H_
#define _META_DATASTRUCTS_H_

#include "Utility\MetaHelper.h"

namespace Utility
{
	template <unsigned label, typename T>
	struct Indexer
	{
		using type = T;
		static constexpr unsigned value = label;
	};

	template <unsigned _label, typename T>
	using Indexer_t = typename Indexer<_label, T>::type;

	// Use this when instantiation is unwanted
	template <typename ... Ty>
	struct Collection;

	// Use this when you need to pass types into functions
	template <typename ... Ty>
	struct TypeList
	{};

	template <typename T, T ...>
	struct IntegralList;

	template <typename L, typename R>
	struct Duplex
	{
		using lhs = L;
		using rhs = R;
	};



	// ========================================= CERTAIN USEFUL STRUCTS ======================================== //


	template <typename T>
	struct SizeofList;

	template <template <typename...> typename Set, typename ... T>
	struct SizeofList<Set<T...>>
	{
		static constexpr auto value = sizeof...(T);
	};

	template <typename Ty, template <typename, Ty...> typename Set, Ty ... vals>
	struct SizeofList<Set<Ty, vals...>>
	{
		static constexpr auto value = sizeof...(vals);
	};



	template <typename ...Tys>
	struct MetaConcat;

	template <template <typename ...> typename Set, typename ... Tys>
	struct MetaConcat<Set<Tys...>>
	{
		using type = Set<Tys...>;
	};

	template <template <typename ...> typename Set, typename ... lhs, typename ... rhs>
	struct MetaConcat<Set<lhs...>, Set<rhs...>>
	{
		using type = Set<lhs..., rhs...>;
	};

	template <template <typename ...> typename Set, typename ... lhs, typename ... rhs, typename ... Rest>
	struct MetaConcat<Set<lhs...>, Set<rhs...>, Rest...>
	{
		using type = typename MetaConcat<
			Set<lhs..., rhs...>,
			typename MetaConcat<Rest...>::type
		>::type;
	};

	template <typename T, template <typename, T ...> typename Set, T ... vals>
	struct MetaConcat<Set<T, vals...>>
	{
		using type = Set<T, vals...>;
	};

	template <typename T, template <typename, T ...> typename Set, T ... LHS, T ... RHS>
	struct MetaConcat<Set<T, LHS...>, Set<T, RHS...>>
	{
		using type = Set<T, LHS..., RHS...>;
	};

	template <typename T, template <typename, T ...> typename Set, T ... LHS, T ... RHS, typename ... Rest>
	struct MetaConcat<Set<T, LHS...>, Set<T, RHS...>, Rest...>
	{
		using type = typename MetaConcat<
			Set<T, LHS..., RHS...>,
			typename MetaConcat<Rest...>::type
		>::type;
	};

	template <typename ... T>
	using MetaConcat_t = typename MetaConcat<T...>::type;
}



#endif		// INCLUDE GUARD

