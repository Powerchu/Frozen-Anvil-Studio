/* HEADER *********************************************************************************/
/*!
\file	Milestone.cpp
\author aaron (100%)
\par    email: m.chu\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "Milestone.h"
#include "Math/Vector2.h"
#include "System/Input/InputSystem.h"
#include "System/Input/InputMap.h"
#include "System/Driver/Driver.h"
#include "Editor/EGUI.h"
#include "Utility/DebugAssert.h"
#include "Object/GameObject.h"
#include "Component/Transform.h"
#include "Component/RigidBody.h"

namespace Dystopia
{
	Milestone::Milestone()
	{
        
	}

	Milestone::~Milestone()
	{
        
	}

	void Milestone::Load()
	{
	}

	void Milestone::Init()
	{ 
		SetActive(true);
        this->GetOwner()->GetComponent<Transform>()->SetScale(32, 128, 1); 
	}

	void Milestone::Update(const float)
	{
	}  

	void Milestone::FixedUpdate(const float)
	{
	}
	
	void Milestone::PostUpdate(void)
	{
	} 
	void Milestone::OnTriggerEnter(const GameObject* other)
	{
		DEBUG_PRINT(eLog::MESSAGE, "Entering Trigger (%zu)", other->GetID());
	}

	void Milestone::OnTriggerStay(const GameObject* other)
	{
		const auto body = other->GetComponent<RigidBody>();

		if (nullptr != body)
		{
			body->AddLinearImpulse({0,500,0});
		}
	}

	void Milestone::OnTriggerExit(const GameObject* other)
	{
		
		DEBUG_PRINT(eLog::MESSAGE, "Exiting Trigger (%zu)", other->GetID());
	}

	void Milestone::GameObjectDestroy(void)
	{
	}

	void Milestone::Unload(void)
	{
	}

	Milestone * Milestone::Duplicate() const
	{
		return new Milestone{};
	}

	void Milestone::Serialise(TextSerialiser&) const
	{
	}

	void Milestone::Unserialise(TextSerialiser&)
	{
	}


	const char * const Milestone::GetBehaviourName() const
	{
		return Milestone::BehaviourName;
	}

	void Milestone::EditorUI(void) noexcept
	{
		
		
	}
}




