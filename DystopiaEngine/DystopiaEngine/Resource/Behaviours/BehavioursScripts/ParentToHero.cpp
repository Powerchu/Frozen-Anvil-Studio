/* HEADER *********************************************************************************/
/*!
\file	ParentToHero.cpp
\author aaron (100%)
\par    email: m.chu\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
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
#include "System/Logger/LoggerSystem.h"


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
		//// mpTarget is a "reference" to game object

		//// mpObj is A game object
		//mpTarget = EngineCore::GetInstance()->GetSystem<SceneSystem>()->FindGameObject("Hero");
		////mpOwner  = GetOwner();

		//mfHealth = 100.0F; // "asd", 2, 'c'		
	}

	void ParentToHero::Update(const float)
	{
		//std::string targetName{ "Hero" };
		const auto currPos = this->GetOwner()->GetComponent<Transform>()->GetPosition();
		const auto sceneSys = EngineCore::GetInstance()->GetSystem<SceneSystem>();
		auto heroObj = sceneSys->FindGameObject(18446744072606142976);

		if (nullptr == heroObj)
		{
			heroObj = sceneSys->FindGameObject("Hero");	
		}

		if (nullptr != heroObj)
		{
			const auto newPos = heroObj->GetComponent<Transform>()->GetPosition();
			const auto lerp = Math::Lerp(currPos, newPos, 0.02F);
			this->GetOwner()->GetComponent<Transform>()->SetPosition(lerp.x, lerp.y, 0);
		}
		

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




