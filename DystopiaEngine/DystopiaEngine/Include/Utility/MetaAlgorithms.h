/* HEADER *********************************************************************************/
/*!
\file	MetaAlgorithms.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Compile time algorithms.

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
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
		using result = typename Helper::MetaFinder<Ty, Arr...>::result;
	};

	template <typename Ty, template <typename...> typename Set, typename ...T>
	struct MetaFind<Ty, Set<T...>>
	{
		using result = typename MetaFind<Ty, T...>::result;
	};

	template <typename Ty, typename ... T, unsigned ... vals>
	struct MetaFind<Ty, Indexer<vals,T>...>
	{
		using result = typename Helper::MetaFinder<Ty, Indexer<vals, T>...>::result;
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


	template <template <typename, typename> typename Op, typename ... T>
	struct MetaSortType;

	template <template <typename, typename> typename Op, template <typename...> typename Set, typename ...T>
	struct MetaSortType<Op, Set<T...>>
	{
		using result = typename Helper::MetaSorterT<Op, Set<T...>>::result;
	};

	template <template <typename T, T, T> typename Op, typename ... Tys>
	struct MetaSortValue;

	template <typename T, template <typename, T, T> typename Op, template <typename, T...> typename Set, T ...vals>
	struct MetaSortValue<Op, Set<T, vals...>>
	{
		using result = typename Helper::MetaSorterV<Op, Set<T, vals...>>::result;
	};

	template <template <typename, typename> typename Op, typename T>
	using MetaSortT_t = typename MetaSortType<Op, T>::result;


	// ============================================ MAKE TYPE LIST ============================================ //


	template <template <typename...> typename ret_t, typename ... Ty>
	struct MakeTypeList;

	template <template <typename...> typename ret_t, template <typename...> typename Set, typename ... Ty>
	struct MakeTypeList <ret_t, Set<Ty...>>
	{
		using type = typename Helper::TypeListMaker<ret_t, Ty...>::type;
	};

	template <template <typename...> typename ret_t, typename ... T>
	using MakeTypeList_t = typename MakeTypeList<ret_t, T...>::type;


	// ============================================ META EXTRACTOR ============================================ //


	template <unsigned N, typename T, typename ... Ty>
	struct MetaExtract
	{
	private:
		using curr = typename Helper::MetaExtractor<N, T>::result;
		using rest = typename MetaExtract<N, Ty...>::result;

	public:

		using result = typename MetaConcat<curr, rest>::type;
	};

	template <unsigned N, typename T>
	struct MetaExtract<N, T>
	{
		using result = typename Helper::MetaExtractor<N, T>::result;
	};

	template <unsigned N, typename T>
	struct MetaExtractV
	{
		static constexpr auto value = typename Helper::MetaExtractorV<N, T>::value;
	};

	template <unsigned N, typename ... T>
	using MetaExtract_t = typename MetaExtract<N, T...>::result;
}



#endif		// INCLUDE GUARD
