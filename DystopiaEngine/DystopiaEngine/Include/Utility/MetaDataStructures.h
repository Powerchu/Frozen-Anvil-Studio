/* HEADER *********************************************************************************/
/*!
\file	MetaDataStructures.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Contains useful metafunction "data structures" 
	that are also used by meta algorithms.

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
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



	// ========================================= CERTAIN USEFUL STRUCTS ======================================== //


	template <typename T>
	struct SizeofList;

	template <template <typename...> typename Set, typename ... T>
	struct SizeofList<Set<T...>>
	{
		static constexpr auto value = sizeof...(T);
	};
}



#endif		// INCLUDE GUARD

