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
#include "../Resource/BehavioursScripts/AiDirector.h"
#include "../Resource/BehavioursScripts/BossAI.h"
#include "../Resource/BehavioursScripts/CamAi.h"
#include "../Resource/BehavioursScripts/CamShake.h"
#include "../Resource/BehavioursScripts/CharacterController.h"
#include "../Resource/BehavioursScripts/CharacterHitBox.h"
#include "../Resource/BehavioursScripts/EnvironmentalResponse.h"
#include "../Resource/BehavioursScripts/Fader.h"
#include "../Resource/BehavioursScripts/FallingPillar.h"
#include "../Resource/BehavioursScripts/ForceBlast.h"
#include "../Resource/BehavioursScripts/ForceFlame.h"
#include "../Resource/BehavioursScripts/FormSlam.h"
#include "../Resource/BehavioursScripts/FormSlamChild.h"
#include "../Resource/BehavioursScripts/FormSpike.h"
#include "../Resource/BehavioursScripts/FormSpikeChild.h"
#include "../Resource/BehavioursScripts/GoblinAI.h"
//#include "../Resource/BehavioursScripts/PlayerUIController.h"
#endif

#if EDITOR

#else

namespace Dystopia
{
	using BehaviourList = typename Ut::MetaAutoIndexer<
		/*Add Behaviours Here*/
		class MainMenu,
		class PauseMenu,
		class SplashManager,
		class AiDirector,
		class BossAI,
		class CamAi,
		class CamShake,
		class CharacterController,
		class CharacterHitBox,
		class EnvironmentalResponse,
		class Fader,
		class FallingPillar,
		class ForceBlast,
		class ForceFlame,
		class FormSlam,
		class FormSlamChild,
		class FormSpike,
		class FormSpikeChild,
		class GoblinAI
		//class PlayerUIController
		
	>::result;
}






#endif





#endif



