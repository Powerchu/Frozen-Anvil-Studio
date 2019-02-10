/* HEADER *********************************************************************************/
/*!
\file	FormSlam.cpp
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "FormSlam.h"
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

// custom includes
#include "Component/SpriteRenderer.h"
#include "System/Tag/Tags.h"
#include "System/Database/DatabaseSystem.h"

namespace Dystopia
{
	namespace FormSlam_MSG
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
	FormSlam::FormSlam()
		: mfAnimSpeed{0.024f},
		mpAnim{nullptr},
		mForceDir{ 1.f, 0.f },
		mbActivate{false},
		mbPrepped{false},
		mbSpawned{false},
		mbFaceRight{false}
	{
	}

	FormSlam::~FormSlam()
	{
	}
	
	void FormSlam::Awake()
	{
		mpAnim = nullptr;
		mbFaceRight = mbActivate = mbPrepped = mbSpawned = false;
	}

	void FormSlam::Init()
	{
		mpAnim = nullptr;
		mbFaceRight = mbActivate = mbPrepped = mbSpawned = false;
		mpAnim = GetOwner()->GetComponent<SpriteRenderer>();
		if (mpAnim)
			mpAnim->SetActive(false);
	}

	void FormSlam::Update(const float )
	{
		if (mbActivate && mpAnim)
		{
			if (!mpAnim->IsPlaying() && mbPrepped && !mbSpawned)
			{
				mbSpawned = true;
				mpAnim->SetActive(false);
				SpawnChild();
			}		
			
			if (!mbPrepped && mpAnim && mpAnim->IsPlaying())
				mbPrepped = true;
		}
	}

	void FormSlam::GameObjectDestroy(void)
	{
		Behaviour::GameObjectDestroy();
	}

	FormSlam * FormSlam::Duplicate() const
	{
		return new FormSlam{*this};
	}

	const char * const FormSlam::GetBehaviourName() const
	{
		return FormSlam::BehaviourName;
	}
	
	TypeErasure::TypeEraseMetaData FormSlam::GetMetaData()
	{
		static MetaData<Dystopia::FormSlam> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const FormSlam::GetMetaData() const
	{
		static MetaData<Dystopia::FormSlam> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	
	void FormSlam::SetKnockDirection(const Math::Vec2& _dir)
	{
		mForceDir = _dir;
	}
	
	void FormSlam::StartSkill(void)
	{
		mbActivate = true;
		mpAnim = GetOwner()->GetComponent<SpriteRenderer>();
		if (mpAnim)
		{
			mpAnim->SetActive(true);
			mpAnim->Stop();
			mpAnim->SetAnimation(static_cast<unsigned>(0));
			mpAnim->SetSpeed(mfAnimSpeed);
			mpAnim->Play();
		}
	}

	void FormSlam::SetFacing(bool _right)
	{
		mbFaceRight = _right;
		auto scale = GetOwner()->GetComponent<Transform>()->GetGlobalScale();
		if (_right && scale.x < 0.f)
			GetOwner()->GetComponent<Transform>()->SetScale({scale.x * -1.f, scale.y, scale.z});
		else if (!_right && scale.x > 0.f)
			GetOwner()->GetComponent<Transform>()->SetScale({scale.x * -1.f, scale.y, scale.z});
	}
	
	void FormSlam::SpawnChild(void)
	{
		auto transform = GetOwner()->GetComponent<Transform>();
		auto spawnPt = transform->GetGlobalPosition(); 
		spawnPt.y = spawnPt.y + (transform->GetGlobalScale().y * 0.266f);
		spawnPt.z = spawnPt.z - 0.5f;
 
		auto& allFormSlamRocks = CORE::Get<SceneSystem>()->FindGameObjectByTag("Form_Slam_Rock");
		for (auto& rocks : allFormSlamRocks)
			rocks->Destroy();
		
		auto inst = EngineCore::Get<SceneSystem>()->Instantiate("Form_Slam_Child.dobj", spawnPt);
		if (inst)
		{
			FormSlam_MSG::SendExternalMessage(inst, "SetKnockDirection", mForceDir);
			FormSlam_MSG::SendExternalMessage(inst, "SetFacing", mbFaceRight);
			FormSlam_MSG::SendExternalMessage(inst, "StartSkill");
		}
	}
}




