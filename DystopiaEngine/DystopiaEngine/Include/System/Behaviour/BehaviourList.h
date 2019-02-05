/* HEADER *********************************************************************************/
/*!
\file	BehaviourList.h
\author Goh Kai Jie Keith (100%)
\par    email: keith.goh\@digipen.edu
\brief


All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#pragma once
#ifndef _BEHAVIOUR_LIST_H
#define _BEHAVIOUR_LIST_H

#include "Utility/Utility.h"
#include "Utility/MetaHelper.h"
#include "Utility/MetaAlgorithms.h"

#if EDITOR

#else
#include "../Resource/BehavioursScripts/MainMenu.h"
#include "../Resource/BehavioursScripts/PauseMenu.h"
#include "../Resource/BehavioursScripts/SplashManager.h"
#endif

#if EDITOR

#else

namespace Dystopia
{
	using BehaviourList = typename Ut::MetaAutoIndexer<
		/*Add Behaviours Here*/
		class MainMenu,
		class PauseMenu,
		class SplashManager
	>::result;
}






#endif





#endif



