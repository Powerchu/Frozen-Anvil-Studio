/* HEADER *********************************************************************************/
/*!
\file	MetaHelper.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Helper functions & structs for Meta.h

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _META_HELPER_H_
#define _META_HELPER_H_

#include "Utility/MetaDataStructures.h"

namespace Utility
{
	// Foward Decls

	template <bool, typename true_t, typename false_t>
	struct IfElse;

	template <typename T, typename U>
	struct IsSame;

	namespace Helper
	{
		// If it has a member, we can instantiate type of member pointer
		template <typename T>
		constexpr char HasMember(T T::*)	{ return 1; }

		// Force false overload to have the lowest rank so that
		// it will only be picked if substution for "true" overload fails
		template <typename T>
		constexpr int HasMember(...)		{ return 0; }


	// =========================================== COMPILE TIME FIND =========================================== // 
	// Make result be false instead of compile error?

		template <typename Ty, typename ... Arr>
		struct Finder;

		template <typename Ty, typename first, typename ... Arr>
		struct Finder<Ty, first, Arr...>
		{
			using type = typename IfElse<
				IsSame<Ty, first >::value,
				Finder<Ty, Ty> , Finder<Ty, Arr...>
			>::type;

			using result = typename type::result;
		};

		template <typename Ty, typename last>
		struct Finder<Ty, last>
		{
			static_assert(IsSame<Ty, last>::value, "Compile time search error: Type not found.");

			using result = last;
		};

		template <typename Ty, typename T, typename ... Ts, unsigned val, unsigned ... vals>
		struct Finder<Ty, Indexer<val, T>, Indexer<vals, Ts>...>
		{
			using type = typename IfElse<
				IsSame<Ty, T>::value,
				Finder<Ty, Indexer<val, Ty>>,
				Finder<Ty, Indexer<vals, Ts>...>
			>::type;

			using result = typename type::result;
		};

		template <typename Ty, typename T, unsigned val>
		struct Finder<Ty, Indexer<val, T>>
		{
			static_assert(IsSame<Ty, T>::value, "Compile time search error: Type not found.");

			using type = Finder;
			using result = Indexer<val, Ty>;
		};


	// =========================================== INITIALISER LIST ============================================ // 

		template <typename ... Ty>
		struct TypeListMaker
		{
			using type = TypeList<Ty...>;
		};

		template <unsigned ...vals, typename ... Ty>
		struct TypeListMaker<Indexer<vals, Ty>...>
		{
			using type = TypeList<Ty...>;
		};
	}
}



#endif		// INCLUDE GUARD

