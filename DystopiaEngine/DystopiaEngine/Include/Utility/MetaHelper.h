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

namespace Utility
{
	namespace Helper
	{
		// If it has a member, we can instantiate type of member pointer
		template <typename T>
		constexpr bool HasMember(T T::*)	{ return true; }

		// Force false overload to have the lowest rank so that
		// it will only be picked if substution for "true" overload fails
		template <typename T>
		constexpr bool HasMember(...)		{ return false; }
	}
}



#endif		// INCLUDE GUARD

