/* HEADER *********************************************************************************/
/*!
\file	MetaAlgorithms.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Compile time algorithms.

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _METAALGO_H_
#define _METAALGO_H_

#include "Utility\Meta.h"
#include "Utility\MetaHelper.h"
#include "Utility\MetaDataStructures.h"

namespace Utility
{
	// =========================================== COMPILE TIME FIND =========================================== //


	template <typename Ty, typename ... Arr>
	struct MetaFind
	{
		using result = typename Helper::Finder<Ty, Arr...>::result;
	};

	template <typename Ty, template <typename...> typename Set, typename ...T>
	struct MetaFind<Ty, Set<T...>>
	{
		using result = typename MetaFind<Ty, T...>::result;
	};

	template <typename Ty, typename ... T, unsigned ... vals>
	struct MetaFind<Ty, Indexer<vals,T>...>
	{
		using result = typename Helper::Finder<Ty, Indexer<vals, T>...>::result;
	};

	template <typename Ty, typename ...Arr>
	using MetaFind_t = typename MetaFind<Ty, Arr...>::result;


	// ======================================= COMPILE TIME PARTITIONING ======================================= //


	template <template <typename U, U, U> typename Op, typename Pivot, typename T>
	struct MetaPartitionValue;

	template <typename T, template <typename U, U, U> typename Op, template <typename, T ...> typename Set, T pivot, T ... vals>
	struct MetaPartitionValue<Op, Set<T, pivot>, Set<T, vals...>>
	{
		using result = typename Helper::MetaPartitionerValue<T, Op, pivot, Set<T, vals...>>::result;
	};


	template <template <typename, typename> typename Op, typename ... T>
	struct MetaPartitionType;

	template <template <typename, typename> typename Op, template <typename...> typename Set, typename pivot, typename ... Ty>
	struct MetaPartitionType<Op, pivot, Set<Ty...>>
	{
		using result = typename Helper::MetaPartitionerType<Op, pivot, Ty...>::result;
	};

	template <template <typename, typename> typename Op, typename pivot, typename ... Ty>
	struct MetaPartitionType<Op, pivot, Ty...>
	{
		using result = typename Helper::MetaPartitionerType<Op, pivot, Ty...>::result;
	};


	// =========================================== COMPILE TIME SORT =========================================== //


	template <typename Op, typename ... T>
	struct MetaSort;

	template <typename Op, template <typename...> typename Set, typename ...T>
	struct MetaSort<Op, Set<T...>>
	{
		using result = typename Helper::MetaSorter<Op, T...>::result;
	};

	template <typename Op, typename T>
	using MetaSort_t = typename MetaSort<Op, T>::result;


	// ============================================ MAKE TYPE LIST ============================================ //

	template <typename ... Ty>
	struct MakeTypeList;

	template <template <typename...> typename Set, typename ... Ty>
	struct MakeTypeList <Set<Ty...>>
	{
		using type = typename Helper::TypeListMaker<Ty...>::type;
	};

	template <typename ... T>
	using MakeTypeList_t = typename MakeTypeList<T...>::type;
}



#endif		// INCLUDE GUARD

