/* HEADER *********************************************************************************/
/*!
\file	MetSet.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Mathematical Set

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _META_SET_H_
#define _META_SET_H_

#include "Utility/Meta.h"
#include "Utility/MetaAlgorithms.h"


namespace Ut
{
	template <typename ... Ty>
	struct MetaSet
	{
		using type = typename MetaUnique<Ty...>::type;
	};
}



#endif		// INCLUDE GUARD

