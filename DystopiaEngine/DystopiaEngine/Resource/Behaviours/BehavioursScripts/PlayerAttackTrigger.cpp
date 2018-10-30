/* HEADER *********************************************************************************/
/*!
\file	PlayerAttackTrigger.cpp
\author keith (100%)
\par    email: keith.goh\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "PlayerAttackTrigger.h"
#include "System/Input/InputSystem.h"
#include "System/Input/InputMap.h"
#include "System/Driver/Driver.h"
#include "Editor/EGUI.h"
#include "Utility/DebugAssert.h"
#include "System/Scene/SceneSystem.h"
#include "Object/GameObject.h"
#include "System/Collision/CollisionEvent.h"
#include "Math/Mathlib.h"
namespace Dystopia
{
	PlayerAttackTrigger::PlayerAttackTrigger()
	{
	}

	PlayerAttackTrigger::~PlayerAttackTrigger()
	{
	}

	void PlayerAttackTrigger::Load()
	{
	}

	void PlayerAttackTrigger::Init()
	{
	}

	void PlayerAttackTrigger::Update(const float _fDeltaTime)
	{
		// auto * ptr = EngineCore::GetInstance()->Get<SceneSystem>()->FindGameObject_cstr("PlayerHero");
		// if(ptr)
		// {
			// auto scale = ptr->GetComponent<Transform>()->GetScale();
			// auto pos   = ptr->GetComponent<Transform>()->GetGlobalPosition();
			// GetOwner()->GetComponent<Transform>()->SetPosition(pos + Math::MakeVector3D(scale.x/2, 0,0));
		// }
	}

	void PlayerAttackTrigger::FixedUpdate(const float _fDeltaTime)
	{
	}
	
	void PlayerAttackTrigger::PostUpdate(void)
	{
	}

	void PlayerAttackTrigger::GameObjectDestroy(void)
	{
	}

	void PlayerAttackTrigger::Unload(void)
	{
	}

	void Dystopia::PlayerAttackTrigger::OnCollisionEnter(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::PlayerAttackTrigger::OnCollisionStay(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::PlayerAttackTrigger::OnCollisionExit(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::PlayerAttackTrigger::OnTriggerEnter(const GameObject * _obj)
	{
	}

	void Dystopia::PlayerAttackTrigger::OnTriggerStay(const GameObject * _obj)
	{
	}

	void Dystopia::PlayerAttackTrigger::OnTriggerExit(const GameObject * _obj)
	{
	}

	PlayerAttackTrigger * PlayerAttackTrigger::Duplicate() const
	{
		return new PlayerAttackTrigger{};
	}

	void PlayerAttackTrigger::Serialise(TextSerialiser& _ser) const
	{
	}

	void PlayerAttackTrigger::Unserialise(TextSerialiser& _ser)
	{
	}


	const char * const PlayerAttackTrigger::GetBehaviourName() const
	{
		return PlayerAttackTrigger::BehaviourName;
	}

	void PlayerAttackTrigger::EditorUI(void) noexcept
	{
		
		
	}
	
	TypeErasure::TypeEraseMetaData PlayerAttackTrigger::GetMetaData()
	{
		/*TO DO*/
		/*REMEMBER TO RETURN YOUR REFLECTED DATA HERE*/
		static MetaData<Dystopia::PlayerAttackTrigger> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const PlayerAttackTrigger::GetMetaData() const
	{
		/*TO DO*/
		/*REMEMBER TO RETURN YOUR REFLECTED DATA HERE*/
		static MetaData<Dystopia::PlayerAttackTrigger> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
}




