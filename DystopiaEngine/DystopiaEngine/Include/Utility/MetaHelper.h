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

#include <type_traits>

namespace Utility
{
	// Forward Decl
	template <unsigned label, typename T>
	struct Indexer;

	template <typename T, typename U>
	struct IsSame;

	template <bool, typename true_t, typename false_t>
	struct IfElse;

	namespace Helper
	{
		// If it has a member, we can instantiate type of member pointer
		template <typename T>
		constexpr bool HasMember(T T::*)	{ return true; }

		// Force false overload to have the lowest rank so that
		// it will only be picked if substution for "true" overload fails
		template <typename T>
		constexpr bool HasMember(...)		{ return false; }


		template <typename Ty, typename ... Arr>
		struct Finder;

		template <typename Ty, typename T, typename ... Ts, unsigned val, unsigned ... vals>
		struct Finder<Ty, Indexer<val, T>, Indexer<vals, Ts>...>
		{
			using result = typename IfElse<
				IsSame<Ty, T>::value,
				Indexer<val, Ty>,
				Finder<Ty, Indexer<vals, Ts>...>
			>::type;
		};

		template <typename Ty, typename T, unsigned vals>
		struct Finder<Ty, Indexer<vals, T>>
		{
			static_assert(IsSame<Ty, T>::value, "Compile time search error.");

			using result = Indexer<vals, Ty>;
		};
	}
}



#endif		// INCLUDE GUARD

