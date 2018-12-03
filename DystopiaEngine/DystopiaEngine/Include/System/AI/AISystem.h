/* HEADER *********************************************************************************/
/*!
\file	Payloads.h
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief

If you need anything specificly, lemme know and I will create a user-friendly one for you.
Else, just hack it - include imgui.h and use more functions available at your own risk.

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _AI_SYSTEM_H_
#define _AI_SYSTEM_H_
#include "System/Base/ComponentDonor.h"
#include "System/Base/Systems.h"
#include "System/AI/Blackboard.h"
#include "System/AI/NeuralTree.h"
#include "System/AI/TreeBuilder.h"
#include "System/AI/Composite.h"
#include "System/AI/Decorator.h"
#include "System/AI/Task.h"

namespace Dystopia
{
	using namespace NeuralTree;
	class AiController;

	class AISystem : public Systems, public ComponentDonor<AiController>
	{
	public:
		AISystem();

		void PreInit(void) override;
		bool Init(void) override;
		void PostInit(void) override;

		void PreFixedUpdate(float) override;
		void FixedUpdate(float) override;
		void Update(float) override;
		void PostUpdate() override;

		void Shutdown(void) override;

		void LoadDefaults(void) override;
		void LoadSettings(DysSerialiser_t&) override;
		void SaveSettings(DysSerialiser_t&) override;

		void EditorUI(void);

#if EDITOR
		
#endif 

	
		virtual ~AISystem(void);


	};
}



#endif
