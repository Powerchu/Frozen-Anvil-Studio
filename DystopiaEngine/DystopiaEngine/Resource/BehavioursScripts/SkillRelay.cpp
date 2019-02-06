/* HEADER *********************************************************************************/
/*!
\file	SkillRelay.cpp
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "SkillRelay.h"
#include "System/Input/InputSystem.h"
#include "System/Input/InputMap.h"
#include "System/Driver/Driver.h"
#include "System/Behaviour/BehaviourSystem.h"

#include "System/Scene/SceneSystem.h"
#include "System/Collision/CollisionEvent.h"
#include "System/Collision/CollisionSystem.h"

#include "Object/ObjectFlags.h"
#include "Object/GameObject.h"

#include "Editor/EGUI.h"
#include "Utility/DebugAssert.h"

//custom includes
#include "Component/Transform.h"

namespace Dystopia
{
	namespace SkillRelay_MSG
	{
		template<typename ... Ts>
		void SendInternalMessage(Behaviour * _ptr, const char * _FuncName, Ts ... _Params)
		{
			EngineCore::GetInstance()->Get<BehaviourSystem>()->SendInternalMessage(_ptr, _FuncName, _Params...);
		}
		
		template<typename ... Ts>
		void SendExternalMessage(uint64_t _ObjectID, const char * _FuncName, Ts ... _Params)
		{
			EngineCore::GetInstance()->Get<BehaviourSystem>()->SendExternalMessage(_ObjectID, _FuncName, _Params...);
		}
		template<typename ... Ts>
		void SendExternalMessage(const GameObject * _ptr, const char * _FuncName, Ts ... _Params)
		{
			EngineCore::GetInstance()->Get<BehaviourSystem>()->SendExternalMessage(_ptr, _FuncName, _Params...);
		}
		
		template<typename ... Ts>
		void SendExternalMessage(GameObject * _ptr, const char * _FuncName, Ts ... _Params)
		{
			EngineCore::GetInstance()->Get<BehaviourSystem>()->SendExternalMessage(_ptr, _FuncName, _Params...);
		}
		
		template<typename ... Ts>
		void SendAllMessage(const char * _FuncName, Ts ... _Params)
		{
			EngineCore::GetInstance()->Get<BehaviourSystem>()->SendAllMessage(_FuncName, _Params...);
		}
	}
	SkillRelay::SkillRelay()
		: mfLifespan{-1.f}, 
		mbStartCountdown{false}
	{
	}

	SkillRelay::~SkillRelay()
	{
	}

	void SkillRelay::Awake()
	{
		mfCountdown = mfLifespan;
		mbStartCountdown = false;
	}

	void SkillRelay::Init()
	{
		mfCountdown = mfLifespan;
		mbStartCountdown = false;
	}

	void SkillRelay::Update(const float _fDeltaTime)
	{
		if (mbStartCountdown)
		{
			mfCountdown -= _fDeltaTime;
			if (mfCountdown < 0.f)
				GetOwner()->Destroy();
		}
	}

	void SkillRelay::GameObjectDestroy(void)
	{
		Behaviour::GameObjectDestroy();
	}

	SkillRelay * SkillRelay::Duplicate() const
	{
		return new SkillRelay{*this};
	}

	const char * const SkillRelay::GetBehaviourName() const
	{
		return SkillRelay::BehaviourName;
	}
	
	TypeErasure::TypeEraseMetaData SkillRelay::GetMetaData()
	{
		static MetaData<Dystopia::SkillRelay> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const SkillRelay::GetMetaData() const
	{
		static MetaData<Dystopia::SkillRelay> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	
	void SkillRelay::SetKnockDirection(const Math::Vec2& _dir)
	{
		auto& allchildren = GetOwner()->GetComponent<Transform>()->GetAllChild();
		for (auto& child : allchildren)
			SkillRelay_MSG::SendExternalMessage(child->GetOwner(), "SetKnockDirection", _dir);
	}
	
	void SkillRelay::SetFacing(bool _right)
	{
		auto scale = GetOwner()->GetComponent<Transform>()->GetGlobalScale();
		if (_right && scale.x < 0.f)
			GetOwner()->GetComponent<Transform>()->SetScale({scale.x * -1.f, scale.y, scale.z});
		else if (!_right && scale.x > 0.f)
			GetOwner()->GetComponent<Transform>()->SetScale({scale.x * -1.f, scale.y, scale.z});
		auto& allchildren = GetOwner()->GetComponent<Transform>()->GetAllChild();
		for (auto& child : allchildren)
			SkillRelay_MSG::SendExternalMessage(child->GetOwner(), "SetFacing", _right);
	}
	
	void SkillRelay::StartSkill(void)
	{
		auto& allchildren = GetOwner()->GetComponent<Transform>()->GetAllChild();
		for (auto& child : allchildren)
			SkillRelay_MSG::SendExternalMessage(child->GetOwner(), "StartSkill");
		
		if (mfLifespan > 0.f && !mbStartCountdown)
			mbStartCountdown = true;
	}
}




