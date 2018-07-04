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

	template <typename ... Ty>
	struct Collection;


	// =========================================== COMPILE TIME FIND =========================================== //


	template <typename Ty, typename ... Arr>
	struct Find
	{
		using result_t = typename Helper::Finder<Ty, Arr...>::result_t;
	};

	template <typename Ty, template <typename...> typename Set, typename ...T>
	struct Find<Ty, Set<T...>>
	{
		using result_t = typename Find<Ty, T...>::result_t;
	};

	template <typename Ty, typename ... T, unsigned ... vals>
	struct Find<Ty, Indexer<vals,T>...>
	{
		using result_t = typename Helper::Finder<Ty, Indexer<vals, T>...>::result_t;
	};

	template <typename Ty, typename ...Arr>
	using Find_t = typename Find<Ty, Arr...>::result_t;


	// =========================================== COMPILE TIME SORT =========================================== //

	template <typename T>
	struct Sort;
}



#endif		// INCLUDE GUARD

