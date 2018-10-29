/* HEADER *********************************************************************************/
/*!
\file	FollowTarget.cpp
\author d (100%)
\par    email: d\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "FollowTarget.h"
#include "System/Input/InputSystem.h"
#include "System/Input/InputMap.h"
#include "System/Driver/Driver.h"
#include "System/Scene/SceneSystem.h"
#include "System/Collision/CollisionEvent.h"
#include "Editor/EGUI.h"

#include "Utility/DebugAssert.h"
#include "Math/Mathlib.h"

#include "Component/Transform.h"
namespace Dystopia
{
	FollowTarget::FollowTarget()
	{
	}

	FollowTarget::~FollowTarget()
	{
	}

	void FollowTarget::Load()
	{
	}

	void FollowTarget::Init()
	{
		mpTarget = EngineCore::GetInstance()->Get<SceneSystem>()->FindGameObject_cstr("Player_Hero");
	}

	void FollowTarget::Update(const float _fDeltaTime)
	{
		const auto currPos = GetOwner()->GetComponent<Transform>()->GetGlobalPosition();
		const auto newPos  = mpTarget->GetComponent<Transform>()->GetGlobalPosition();
		const auto lerpPos = Math::Lerp(currPos, newPos, 0.02F);
		GetOwner()->GetComponent<Transform>()->SetGlobalPosition(lerpPos);
	}

	void FollowTarget::FixedUpdate(const float _fDeltaTime)
	{
	}
	
	void FollowTarget::PostUpdate(void)
	{
	}

	void FollowTarget::GameObjectDestroy(void)
	{
	}

	void FollowTarget::Unload(void)
	{
	}

	void Dystopia::FollowTarget::OnCollisionEnter(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::FollowTarget::OnCollisionStay(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::FollowTarget::OnCollisionExit(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::FollowTarget::OnTriggerEnter(const GameObject * _obj)
	{
	}

	void Dystopia::FollowTarget::OnTriggerStay(const GameObject * _obj)
	{
	}

	void Dystopia::FollowTarget::OnTriggerExit(const GameObject * _obj)
	{
	}

	FollowTarget * FollowTarget::Duplicate() const
	{
		return new FollowTarget{};
	}

	void FollowTarget::Serialise(TextSerialiser& _ser) const
	{
	}

	void FollowTarget::Unserialise(TextSerialiser& _ser)
	{
	}


	const char * const FollowTarget::GetBehaviourName() const
	{
		return FollowTarget::BehaviourName;
	}

	void FollowTarget::EditorUI(void) noexcept
	{
		
		
	}
	
	TypeErasure::TypeEraseMetaData FollowTarget::GetMetaData()
	{
		/*TO DO*/
		/*REMEMBER TO RETURN YOUR REFLECTED DATA HERE*/
		static MetaData<Dystopia::FollowTarget> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const FollowTarget::GetMetaData() const
	{
		/*TO DO*/
		/*REMEMBER TO RETURN YOUR REFLECTED DATA HERE*/
		MetaData<FollowTarget> mMetaData;
		return TypeErasure::TypeEraseMetaData{mMetaData};
	}
}




