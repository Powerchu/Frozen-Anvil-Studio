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
		:xOffset{0}, yOffset{0}, xWeight{0}, yWeight{0}
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
		mpTarget = EngineCore::GetInstance()->Get<SceneSystem>()->FindGameObject_cstr("CameraPivot");
		vPos = mpTarget->GetComponent<Transform>()->GetGlobalPosition();
	}

	void FollowTarget::Update(const float _fDeltaTime)
	{
		if (!mpTarget)
		{
			DEBUG_PRINT(eLog::MESSAGE, "No target to follow");
			return; 
		}
		
		auto newPos = mpTarget->GetComponent<Transform>()->GetGlobalPosition();
		if(Math::ApproxEq(newPos[0], vPos[0]) && Math::ApproxEq(newPos[1], vPos[1]))
		{
			DEBUG_PRINT(eLog::MESSAGE, "SKIPPING");
			return;
		}
		
		
		Math::Vec4 currPos = GetOwner()->GetComponent<Transform>()->GetGlobalPosition();
		newPos.x = newPos.x + xOffset;
		newPos.y = newPos.y + yOffset;
		
		currPos.x = Math::Lerp<float>(currPos.x, newPos.x, xWeight * _fDeltaTime);
		currPos.y = Math::Lerp<float>(currPos.y, newPos.y, yWeight * _fDeltaTime);
		
		//newPos = (newPos - currPos);
		//prev = (prev  * .7f + newPos.Magnitude() * .3f) * _fDeltaTime;
		//newPos.Normalise();
	    //
		//newPos *= prev;
		//
		//vPos.x += newPos.x * xWeight;
		//vPos.y += newPos.y * yWeight;
		
		//currPos.x = currPos.x + lerpC.x * _fDeltaTime;
		//currPos.y = currPos.y + lerpC.y * _fDeltaTime;
		GetOwner()->GetComponent<Transform>()->SetGlobalPosition(currPos);
		vPos = newPos;
		
		//prevV = lerpC;
		//auto lerpPos = Math::Lerp(currPos, newPos, 0.03F);
		//lerpPos.z = z;
		//if ((newPos - currPos).MagnitudeSqr() < 0.00001F)
		//{
		//	GetOwner()->GetComponent<Transform>()->SetGlobalPosition(newPos);
		//}
		//else
		//{
		//	GetOwner()->GetComponent<Transform>()->SetGlobalPosition(lerpPos);
		//}

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

	void Dystopia::FollowTarget::OnTriggerEnter(GameObject * const _obj)
	{
	}

	void Dystopia::FollowTarget::OnTriggerStay(GameObject * const _obj)
	{
	}

	void Dystopia::FollowTarget::OnTriggerExit(GameObject * const _obj)
	{
	}

	FollowTarget * FollowTarget::Duplicate() const
	{
		return new FollowTarget{*this};
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




