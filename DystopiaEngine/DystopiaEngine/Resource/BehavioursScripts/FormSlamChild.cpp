/* HEADER *********************************************************************************/
/*!
\file	FormSlamChild.cpp
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "FormSlamChild.h"
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
#include "Component/RigidBody.h"
#include "Component/Collider.h"
#include "System/Physics/PhysicsSystem.h"
#include "System/Tag/Tags.h"
#include "System/Database/DatabaseSystem.h"

namespace Dystopia
{
	namespace FormSlamChild_MSG
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
	FormSlamChild::FormSlamChild()
		: mfCastDamage{1.f},
		mfCastKnockback{20.f},
		mfApplyTiming{0.1f},
		mfLaunchForce{10.f},
		mbAppliedAtk{false},
		mForceDir{ 1.f, 0.f }, 
		mArrFoundEnemies{},
		mbActivate{false},
		mpSprite{nullptr},
		mpBody{nullptr},
		mfInternalMultiplier{1000.f},
		mfTimer{1.0f},
		mpCamShaker{nullptr},
		mpInput{nullptr},
		mpPlayer{nullptr}
	{
	}

	FormSlamChild::~FormSlamChild()
	{
	}

	void FormSlamChild::Awake()
	{
		mpSprite = nullptr;
		mpBody = nullptr;
		mfTimer = 1.0f;

		
	}

	void FormSlamChild::Init()
	{
		mpSprite = nullptr;
		mbActivate = false;
        mpCamShaker = EngineCore::Get<SceneSystem>()->FindGameObject("Camera Shake");
		mpPlayer = EngineCore::Get<SceneSystem>()->FindGameObject("CharacterController");
        mpInput = EngineCore::GetInstance()->GetSystem<InputManager>();
		mpBody = GetOwner()->GetComponent<RigidBody>();

		auto& allchild = GetOwner()->GetComponent<Transform>()->GetAllChild();
        for (auto& c : allchild)
        {
            if (c->GetOwner()->GetName() == HashString{"Slam_HitBox"})
                mpSlamHitbox = c->GetOwner();
        } 

		if (mpBody)
			mpBody->SetActive(false);
		
		HashString database{"Form_Skills.ddata"};
		auto db =  EngineCore::Get<DatabaseSystem>();
		if (db->OpenData(database))
		{
			mfCastDamage = db->GetDatabase(database)->GetElement<float>(HashString{"Slam_Damage"});
			mfCastKnockback = db->GetDatabase(database)->GetElement<float>(HashString{"Slam_Knockback"});
			mfLaunchForce = db->GetDatabase(database)->GetElement<float>(HashString{"Slam_Launch_Force"});
		}

		if (mpPlayer)
			FormSlamChild_MSG::SendExternalMessage(mpPlayer, "FormT2Audio", "FormT2_FlingOnly.wav");
	}

	void FormSlamChild::Update(const float _fDeltaTime)
	{
		mfTimer -= _fDeltaTime;
	}

	void FormSlamChild::GameObjectDestroy(void)
	{
	}

	void Dystopia::FormSlamChild::OnCollisionEnter(const CollisionEvent& _colEvent)
	{
		if (mfTimer < 0.0f) return;
		GameObject * obj = _colEvent.mCollidedWith;
		if (obj && !(obj->GetFlags() & FLAG_REMOVE))
		{
			if(_colEvent.mCollidedWith->GetComponent<Collider>()->GetColLayer() == LAYER_GLOBAL && !mbAppliedAtk)
			{
				if(mpInput)
					mpInput->InvokeVibration(1.f, 30.f, 4.8f, -0.3f);
            	if (mpCamShaker)
                	FormSlamChild_MSG::SendExternalMessage(mpCamShaker, "InvokeShake", mfLaunchForce, 80.0f, 40.0f, 4.0f);
				mbAppliedAtk = true;
				if (mpSlamHitbox)
				{
					FormSlamChild_MSG::SendExternalMessage(mpSlamHitbox, "InvokeAudio", "FormT2_Slam_01.wav");
					FormSlamChild_MSG::SendExternalMessage(mpSlamHitbox, "SendForce", mfCastKnockback * 10.f, Math::Vec2{0,1});
					FormSlamChild_MSG::SendExternalMessage(mpSlamHitbox, "SendDamage", mfCastDamage/2);
				}
			}
			if (mpBody)
				if (mpBody->GetLinearVelocity().y < -200.0f)
				{
					FormSlamChild_MSG::SendExternalMessage(obj, "TakeDamage", mfCastDamage);
					FormSlamChild_MSG::SendExternalMessage(obj, "TakeForce", mfCastKnockback * 3000000.f, mForceDir);
				}
		}
	}

	void Dystopia::FormSlamChild::OnCollisionStay(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::FormSlamChild::OnCollisionExit(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::FormSlamChild::OnTriggerEnter(GameObject * const _obj)
	{
	}

	void Dystopia::FormSlamChild::OnTriggerStay(GameObject * const _obj)
	{
	}

	void Dystopia::FormSlamChild::OnTriggerExit(GameObject * const _obj)
	{
	}

	FormSlamChild * FormSlamChild::Duplicate() const
	{
		return new FormSlamChild{*this};
	}

	const char * const FormSlamChild::GetBehaviourName() const
	{
		return FormSlamChild::BehaviourName;
	}

	TypeErasure::TypeEraseMetaData FormSlamChild::GetMetaData()
	{
		static MetaData<Dystopia::FormSlamChild> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	
	TypeErasure::TypeEraseMetaData const FormSlamChild::GetMetaData() const
	{
		static MetaData<Dystopia::FormSlamChild> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	
	void FormSlamChild::SetKnockDirection(const Math::Vec2& _dir)
	{
		mForceDir = _dir;
	}
	
	void FormSlamChild::SetFacing(bool _right)
	{
		mbFaceRight = _right;
		auto scale = GetOwner()->GetComponent<Transform>()->GetGlobalScale();
		if (_right && scale.x < 0.f)
			GetOwner()->GetComponent<Transform>()->SetScale({scale.x * -1.f, scale.y, scale.z});
		else if (!_right && scale.x > 0.f)
			GetOwner()->GetComponent<Transform>()->SetScale({scale.x * -1.f, scale.y, scale.z});
	}
	
	void FormSlamChild::StartSkill(void)
	{
		mbActivate = true;
		mpBody = GetOwner()->GetComponent<RigidBody>();
		if (mpBody)
		{
			mpBody->SetActive(true);
			mpBody->AddLinearImpulse({mbFaceRight ? mpBody->GetMass() * mfLaunchForce * mfInternalMultiplier : 
									                mpBody->GetMass() * mfInternalMultiplier * mfLaunchForce * -1.f, 0.f, 0.f });
		}
	}
	
	void FormSlamChild::SendDamage(GameObject * const)
	{
		
	}
	
}




