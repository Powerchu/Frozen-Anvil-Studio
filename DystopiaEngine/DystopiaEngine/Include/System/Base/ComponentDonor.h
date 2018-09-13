/* HEADER *********************************************************************************/
/*!
\file	ComponentDonor.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	For systems that give out components to inherit

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _COMPONENTDONOR_H_
#define _COMPONENTDONOR_H_

#include "Utility\Utility.h"
#include "DataStructure\MagicArray.h"


namespace Dystopia
{
	template <typename Ty, typename Settings = Ctor::MagicArrayBuilder<Ty>::SetBlockSize<64>>
	class ComponentDonor
	{
	public:
		using Component_t = Ty;

		template <typename ... U>
		Ty* RequestComponent(U&& ...);


	protected:

		typename Settings::type mComponents;
	};
}






// ============================================ FUNCTION DEFINITIONS ============================================ // 


template <typename Ty, typename S> template <typename ... U>
inline Ty* Dystopia::ComponentDonor<Ty, S>::RequestComponent(U&& ... _Args)
{
	return mComponents.Emplace(Utility::Forward<U>(_Args)...);
}



#endif		// INCLUDE GUARD

