#pragma once
#ifndef _BEHAVIOUR_LIST_H
#define _BEHAVIOUR_LIST_H

#include "Utility/Utility.h"
#include "Utility/MetaHelper.h"
#include "Utility/MetaAlgorithms.h"

#if EDITOR

#else
#include "../Resource/Behaviours/BehavioursScripts/ChangeAudio.h"
#include "../Resource/Behaviours/BehavioursScripts/CharacterController.h"
#include "../Resource/Behaviours/BehavioursScripts/CombatBox.h"
#include "../Resource/Behaviours/BehavioursScripts/DeleteSelf.h"
#include "../Resource/Behaviours/BehavioursScripts/FollowTarget.h"
#include "../Resource/Behaviours/BehavioursScripts/ForceTOne.h"
#include "../Resource/Behaviours/BehavioursScripts/ForceTTwo.h"
#include "../Resource/Behaviours/BehavioursScripts/FormTTwo.h"
#include "../Resource/Behaviours/BehavioursScripts/FormTOne.h"
#include "../Resource/Behaviours/BehavioursScripts/Goblin.h"
#include "../Resource/Behaviours/BehavioursScripts/GoblinCombat.h"
#include "../Resource/Behaviours/BehavioursScripts/Parallax.h"
#include "../Resource/Behaviours/BehavioursScripts/SkillManager.h"
#include "../Resource/Behaviours/BehavioursScripts/SplashScreen.h"
#include "../Resource/Behaviours/BehavioursScripts/SceneManager.h"
#include "../Resource/Behaviours/BehavioursScripts/MenuManager.h"
#include "../Resource/Behaviours/BehavioursScripts/Health.h"
#include "../Resource/Behaviours/BehavioursScripts/FormTThree.h"
#include "../Resource/Behaviours/BehavioursScripts/ForceTThree.h"
#include "../Resource/Behaviours/BehavioursScripts/SplashScreen.h"

#endif

#if EDITOR

#else

namespace Dystopia
{
	using BehaviourList = typename Ut::MetaAutoIndexer<
		class ChangeAudio,
		class CharacterController,
		class CombatBox,
		class DeleteSelf,
		class FollowTarget,
		class ForceTOne,
		class ForceTTwo,
		class FormTTwo,
		class FormTOne,
		class Goblin,
		//class GoblinCombat,
		class Parallax,
		class SkillManager,
		class SplashScreen,
		class SceneManager,
		class MenuManager,
		class Health,
		class FormTThree,
		class ForceTThree,
		class SplashScreen
		/*Add Behaviours Here*/
	>::result;
}






#endif





#endif



