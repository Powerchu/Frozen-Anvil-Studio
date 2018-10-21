/* HEADER *********************************************************************************/
/*!
\file	ParentToHero.cpp
\author aaron (100%)
\par    email: m.chu\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "ParentToHero.h"
#include "Math/Vector2.h"
#include "System/Input/InputSystem.h"
#include "System/Input/InputMap.h"
#include "System/Driver/Driver.h"
#include "Editor/EGUI.h"
#include "Utility/DebugAssert.h"
#include "Object/GameObject.h"
#include "Component/Transform.h"

#include "Object/ObjectFlags.h"
#include "System/Scene/SceneSystem.h"
#include "System/Scene/Scene.h"

#include "Math/Vector4.h"


namespace Dystopia
{
	ParentToHero::ParentToHero()
	{
	}

	ParentToHero::~ParentToHero()
	{
	}

	void ParentToHero::Load()
	{
	}

	void ParentToHero::Init()
	{
		//mpTarget = EngineCore::GetInstance()->GetSystem<SceneSystem>()->FindGameObject("Hero");
		//mpOwner  = GetOwner();
		
	}

	void ParentToHero::Update(const float _dt)
	{
		const auto currPos = this->GetOwner()->GetComponent<Transform>()->GetGlobalPosition();
		const auto nextPos = EngineCore::GetInstance()->GetSystem<SceneSystem>()->FindGameObject("Hero")->GetComponent<Transform>()->GetGlobalPosition();
		const float lerpx = (float(nextPos.x) - float(currPos.x)) * .02F;
		const float lerpy = (float(nextPos.y) - float(currPos.y)) * .02F;
		this->GetOwner()->GetComponent<Transform>()->SetGlobalPosition(currPos.x + lerpx, currPos.y + lerpy, 0);
		//this->GetOwner()->GetComponent<Transform>()->SetParent(EngineCore::GetInstance()->GetSystem<SceneSystem>()->FindGameObject("Hero")->GetComponent<Transform>());
	}

	void ParentToHero::FixedUpdate(const float)
	{
	}
	
	void ParentToHero::PostUpdate(void)
	{
	}

	void ParentToHero::GameObjectDestroy(void)
	{
	}

	void ParentToHero::Unload(void)
	{
	}

	ParentToHero * ParentToHero::Duplicate() const
	{
		return new ParentToHero{};
	}

	void ParentToHero::Serialise(TextSerialiser&) const
	{
	}

	void ParentToHero::Unserialise(TextSerialiser&)
	{
	}


	const char * const ParentToHero::GetBehaviourName() const
	{
		return ParentToHero::BehaviourName;
	}

	void ParentToHero::EditorUI(void) noexcept
	{
		
		
	}
}




