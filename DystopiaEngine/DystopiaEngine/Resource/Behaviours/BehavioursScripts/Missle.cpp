/* HEADER *********************************************************************************/
/*!
\file	Missle.cpp
\author DAN (100%)
\par    email: dan\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "Missle.h"
#include "System/Input/InputSystem.h"
#include "System/Input/InputMap.h"
#include "System/Driver/Driver.h"
#include "Editor/EGUI.h"
#include "Utility/DebugAssert.h"
#include "System/Scene/SceneSystem.h"
#include "Object/GameObject.h"
#include "System/Collision/CollisionEvent.h"
#include "Component/SpriteRenderer.h"

namespace Dystopia
{
	Missle::Missle()
	{
	}

	Missle::~Missle()
	{
	}

	void Missle::Load()
	{
	}

	void Missle::Init()
	{
		GetOwner()->GetComponent<SpriteRenderer>()->SetAnimation("Missles");
	}

	void Missle::Update(const float _fDeltaTime)
	{
		//auto * ptr = EngineCore::GetInstance()->Get<SceneSystem>()->FindGameObject(_colEvent.mOtherID);
		if(EngineCore::GetInstance()->Get<InputManager>()->IsKeyTriggered(eButton::KEYBOARD_V))
		{

		}
		if (GetOwner())
		{
			if(GetOwner()->GetComponent<SpriteRenderer>())
			{
				if(GetOwner()->GetComponent<SpriteRenderer>()->AnimationFinished())
				{
					GetOwner()->Destroy();
				}
			}
		}
		
	}

	void Missle::FixedUpdate(const float _fDeltaTime)
	{
	}
	
	void Missle::PostUpdate(void)
	{
	}

	void Missle::GameObjectDestroy(void)
	{
	}

	void Missle::Unload(void)
	{
	}

	void Dystopia::Missle::OnCollisionEnter(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::Missle::OnCollisionStay(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::Missle::OnCollisionExit(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::Missle::OnTriggerEnter(GameObject * const _obj)
	{
	}

	void Dystopia::Missle::OnTriggerStay(GameObject * const _obj)
	{
	}

	void Dystopia::Missle::OnTriggerExit(GameObject * const _obj)
	{
	}

	Missle * Missle::Duplicate() const
	{
		return new Missle{};
	}

	void Missle::Serialise(TextSerialiser& _ser) const
	{
	}

	void Missle::Unserialise(TextSerialiser& _ser)
	{
	}


	const char * const Missle::GetBehaviourName() const
	{
		return Missle::BehaviourName;
	}

	void Missle::EditorUI(void) noexcept
	{
		
		
	}
	
	TypeErasure::TypeEraseMetaData Missle::GetMetaData()
	{
		/*TO DO*/
		/*REMEMBER TO RETURN YOUR REFLECTED DATA HERE*/
		static MetaData<Dystopia::Missle> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const Missle::GetMetaData() const
	{
		/*TO DO*/
		/*REMEMBER TO RETURN YOUR REFLECTED DATA HERE*/
		static MetaData<Dystopia::Missle> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
}




