/* HEADER *********************************************************************************/
/*!
\file	Fireball.cpp
\author keith.goh (100%)
\par    email: keith.goh\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "Fireball.h"
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
	Fireball::Fireball()
	{
	}

	Fireball::~Fireball()
	{
	}

	void Fireball::Load()
	{
	}

	void Fireball::Init()
	{
	}

	void Fireball::Update(const float _fDeltaTime)
	{
	}

	void Fireball::FixedUpdate(const float _fDeltaTime)
	{
	}
	
	void Fireball::PostUpdate(void)
	{
	}

	void Fireball::GameObjectDestroy(void)
	{
	}

	void Fireball::Unload(void)
	{
	}

	void Dystopia::Fireball::OnCollisionEnter(const CollisionEvent& _colEvent)
	{
		auto * ptr = EngineCore::GetInstance()->Get<SceneSystem>()->FindGameObject(_colEvent.mOtherID);
		if(ptr)
		{
			//const char * name = ptr->Get
			const char * name = ptr->GetNamePtr();
			if(strcmp(name,"PlayerAttackTrig") &&
			   strcmp(name, "Fireball")&& strcmp(name, "Missle")
			   && strcmp(name, "AudioTrig"))
			{
				GetOwner()->Destroy();
			}
		}
	}

	void Dystopia::Fireball::OnCollisionStay(const CollisionEvent& _colEvent)
	{
	
	}

	void Dystopia::Fireball::OnCollisionExit(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::Fireball::OnTriggerEnter(const GameObject * _obj)
	{
		auto * ptr = EngineCore::GetInstance()->Get<SceneSystem>()->FindGameObject(_obj->GetID());
		if(ptr)
		{
			//const char * name = ptr->Get
			const char * name = ptr->GetNamePtr();
			if(strcmp(name,"PlayerAttackTrig") &&
			   strcmp(name, "Fireball")&& strcmp(name, "Missle")
			   && strcmp(name, "AudioTrig"))
			{
				GetOwner()->Destroy();
			}
		}
	}

	void Dystopia::Fireball::OnTriggerStay(const GameObject * _obj)
	{
	
	}

	void Dystopia::Fireball::OnTriggerExit(const GameObject * _obj)
	{
	}

	Fireball * Fireball::Duplicate() const
	{
		return new Fireball{};
	}

	void Fireball::Serialise(TextSerialiser& _ser) const
	{
	}

	void Fireball::Unserialise(TextSerialiser& _ser)
	{
	}


	const char * const Fireball::GetBehaviourName() const
	{
		return Fireball::BehaviourName;
	}

	void Fireball::EditorUI(void) noexcept
	{
		
		
	}
	
	TypeErasure::TypeEraseMetaData Fireball::GetMetaData()
	{
		/*TO DO*/
		/*REMEMBER TO RETURN YOUR REFLECTED DATA HERE*/
		static MetaData<Dystopia::Fireball> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const Fireball::GetMetaData() const
	{
		/*TO DO*/
		/*REMEMBER TO RETURN YOUR REFLECTED DATA HERE*/
		return TypeErasure::TypeEraseMetaData{};
	}
}




