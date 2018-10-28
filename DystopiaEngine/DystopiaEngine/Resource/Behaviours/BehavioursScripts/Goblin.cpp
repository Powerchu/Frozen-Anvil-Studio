/* HEADER *********************************************************************************/
/*!
\file	Goblin.cpp
\author Dan (100%)
\par    email: Dan.Kang\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "Goblin.h"
#include "System/Input/InputSystem.h"
#include "System/Input/InputMap.h"
#include "System/Driver/Driver.h"
#include "Editor/EGUI.h"
#include "Utility/DebugAssert.h"
#include "System/Scene/SceneSystem.h"
#include "Object/GameObject.h"
#include "System/Collision/CollisionEvent.h"

namespace Dystopia
{
	Goblin::Goblin()
	{
	}

	Goblin::~Goblin()
	{
	}

	void Goblin::Load()
	{
	}

	void Goblin::Init()
	{
	}

	void Goblin::Update(const float _fDeltaTime)
	{
		//mHealth -=5;
		if(EngineCore::GetInstance()->Get<InputManager>()->IsKeyTriggered(eButton::KEYBOARD_X) && isColliding)
		{
			mHealth -= 5;
		}
        if(mHealth < 0)
        {
            GetOwner()->Destroy();
        }

	}

	void Goblin::FixedUpdate(const float _fDeltaTime)
	{
	}
	
	void Goblin::PostUpdate(void)
	{
		
	}

	void Goblin::GameObjectDestroy(void)
	{
	}

	void Goblin::Unload(void)
	{
	}

	void Dystopia::Goblin::OnCollisionEnter(const CollisionEvent& _colEvent)
	{
		auto * ptr = EngineCore::GetInstance()->Get<SceneSystem>()->FindGameObject(_colEvent.mOtherID);
		//if(EngineCore::GetInstance()->Get<InputManager>()->IsKeyPressed(eButton::KEYBOARD_RIGHT))
			
		if(ptr)
		{
			//const char * name = ptr->Get
			const char * name = ptr->GetNamePtr();
			if(!strcmp(name,"PlayerHero"))
			{
				isColliding = true;
			}
			else if(!strcmp(name,"Fireball"))
			{
				mHealth -= 5;
			}
		}
	}

	void Dystopia::Goblin::OnCollisionStay(const CollisionEvent& _colEvent)
	{
		auto * ptr = EngineCore::GetInstance()->Get<SceneSystem>()->FindGameObject(_colEvent.mOtherID);
		if(ptr)
		{
			//const char * name = ptr->Get
			const char * name = ptr->GetNamePtr();
			if(!strcmp(name,"PlayerHero"))
			{
				isColliding = true;
			}
		}
	}

	void Dystopia::Goblin::OnCollisionExit(const CollisionEvent& _colEvent)
	{
		auto * ptr = EngineCore::GetInstance()->Get<SceneSystem>()->FindGameObject(_colEvent.mOtherID);
		if(ptr)
		{
			isColliding = false;
		}
	}

	void Dystopia::Goblin::OnTriggerEnter(const GameObject * _obj)
	{
		//mHealth -= 5;
	}

	void Dystopia::Goblin::OnTriggerStay(const GameObject * _obj)
	{
		//mHealth -= 5;
	}

	void Dystopia::Goblin::OnTriggerExit(const GameObject * _obj)
	{
	}

	Goblin * Goblin::Duplicate() const
	{
		return new Goblin{};
	}

	void Goblin::Serialise(TextSerialiser& _ser) const
	{
	}

	void Goblin::Unserialise(TextSerialiser& _ser)
	{
	}


	const char * const Goblin::GetBehaviourName() const
	{
		return Goblin::BehaviourName;
	}

	void Goblin::EditorUI(void) noexcept
	{
		
		
	}
	
	TypeErasure::TypeEraseMetaData Goblin::GetMetaData()
	{
		/*TO DO*/
		/*REMEMBER TO RETURN YOUR REFLECTED DATA HERE*/
		static MetaData<Dystopia::Goblin> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const Goblin::GetMetaData() const
	{
		/*TO DO*/
		/*REMEMBER TO RETURN YOUR REFLECTED DATA HERE*/
		return TypeErasure::TypeEraseMetaData{};
	}
}




