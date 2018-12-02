/* HEADER *********************************************************************************/
/*!
\file	AiController.cpp
\author AARON CHU MING SAN (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
Wrapper class for AI Tree functions.

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "Component/AiController.h"		// File header

#include "System/AI/AISystem.h"
#include "System/Driver/Driver.h"
#include "System/Scene/SceneSystem.h"
#include "System/Scene/Scene.h"

#include "Object/GameObject.h"
#include "Object/ObjectFlags.h"

#include "Math/MathUtility.h"

#if EDITOR
#include "Editor/EGUI.h"
#endif

namespace Dystopia
{
	AiController::AiController(void)
	{

	}

	/*void AiController::Load()
	{
	}*/

	void AiController::Awake()
	{
	}

	void AiController::Init()
	{
	}

	/*void AiController::Unload()
	{
	}*/

	AiController* AiController::Duplicate() const
	{
		const auto cc = EngineCore::GetInstance()->GetSystem<AISystem>()->RequestComponent(*this);
		//cc->SetOwner(GetOwner());
		//cc->Init();
		return cc;
	}

	void AiController::Serialise(TextSerialiser&) const
	{
	}

	void AiController::Unserialise(TextSerialiser&)
	{
	}

	void AiController::Update(float)
	{
		if (bTree.IsValidTree())
		{
			bTree.Update();
		}
	}

	void AiController::LateUpdate(float) const
	{

	}

	BehaviourTree& AiController::GetTreeAsRef()
	{
		return bTree;
	}

	BehaviourTree::Ptr AiController::GetTreeAsPtr()
	{
		return Ctor::CreateShared<BehaviourTree>(bTree);
	}
}

