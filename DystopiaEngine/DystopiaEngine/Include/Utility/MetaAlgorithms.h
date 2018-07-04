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
	struct Find
	{
		using result = typename Helper::Finder<Ty, Arr...>::result;
	};

	template <typename Ty, template <typename...> typename Set, typename ...T>
	struct Find<Ty, Set<T...>>
	{
		using result = typename Find<Ty, T...>::result;
	};

	template <typename Ty, typename ... T, unsigned ... vals>
	struct Find<Ty, Indexer<vals,T>...>
	{
		using result = typename Helper::Finder<Ty, Indexer<vals, T>...>::result;
	};

	template <typename Ty, typename ...Arr>
	using Find_t = typename Find<Ty, Arr...>::result;


	// =========================================== COMPILE TIME SORT =========================================== //


	template <typename Pred, typename T>
	struct Sort;

//	template <typename Pred, typename T>
//	using Sort_t = typename Sort<Pred, T>::type;


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

