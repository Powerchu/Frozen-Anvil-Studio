/* HEADER *********************************************************************************/
/*!
\file	FallingPillar.cpp
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "FallingPillar.h"
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
#include "System/Graphics/GraphicsSystem.h"
#include "System/Graphics/Shader.h"
#include "System/Graphics/ShaderSystem.h"
#include "System/Physics/PhysicsSystem.h"
#include "Component/Convex.h"
#include "Component/RigidBody.h"
#include "Math/MathLib.h"

namespace Dystopia
{
	namespace FallingPillar_MSG
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
	FallingPillar::FallingPillar()
		: mfFallDuration{},
		mfShakeDuration{},
		mfShakeIntensity{},
		mbShake{false},
		mbFall{false},
		mfAccuDt{0.f},
		mfFrequency{},
		mfIntensityCounter{},
		mfDurationCounter{},
        mbEnded{false},
        mfFallSpeed{1.f}
	{
	}

	FallingPillar::~FallingPillar()
	{
	}

	void FallingPillar::Awake()
	{
		mbShake = mbEnded = mbFall = false;
	}

	void FallingPillar::Init()
	{
		mbShake = mbEnded = mbFall = false;
		HashString database{"Interactable"};
		auto db =  EngineCore::Get<DatabaseSystem>();
		if (db->OpenData(database))
		{
			mfShakeIntensity = db->GetDatabase(database)->GetElement<float>(HashString{"Falling_Pillar_Intensity"});
			mfShakeDuration = db->GetDatabase(database)->GetElement<float>(HashString{"Falling_Pillar_Shake_Duration"});
			mfFrequency = db->GetDatabase(database)->GetElement<float>(HashString{"Falling_Pillar_Frequency"});
			mfFallDuration = db->GetDatabase(database)->GetElement<float>(HashString{"Falling_Pillar_Fall_Duration"});
		}
		mfFallSpeed = 1.f;
	}

	void FallingPillar::Update(const float _fDeltaTime)
	{
		if (mbFall && !mbEnded)
			FallingUpdate(_fDeltaTime);
		if (mbShake)
			ShakingUpdate(_fDeltaTime);
	}
	
	void FallingPillar::GameObjectDestroy(void)
	{
		Behaviour::GameObjectDestroy();
	}
	
	void Dystopia::FallingPillar::OnCollisionEnter(const CollisionEvent& _colEvent)
	{
		if (_colEvent.mCollidedWith && _colEvent.mCollidedWith->HasTag("PillarStopper"))
			if (auto parent = GetOwner()->GetComponent<Transform>()->GetParent())
				if (auto parentRig = parent->GetOwner()->GetComponent<RigidBody>())
				{
                    if (auto rig = _colEvent.mCollidedWith->GetComponent<RigidBody>())
                        rig->Set_IsStatic(true);
                    parentRig->SetAngularVel({0,0,0});
                    parentRig->SetLinearVel({0,0,0});
					if (auto c = GetOwner()->GetComponent<Convex>())
						c->SetScale({0.89f, 0.55f,1.f});
					mbEnded = true;
					if (auto mpInput = EngineCore::GetInstance()->GetSystem<InputManager>())
						mpInput->InvokeVibration(1.f, 40.f, 10.f, 0.1f);
            		if (auto mpCamShaker = EngineCore::Get<SceneSystem>()->FindGameObject("Main Camera"))
                		FallingPillar_MSG::SendExternalMessage(mpCamShaker, "InvokeShake", .6f, 40.0f, 32.0f, 4.0f);
                }
	}

	void Dystopia::FallingPillar::OnCollisionStay(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::FallingPillar::OnCollisionExit(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::FallingPillar::OnTriggerEnter(GameObject * const _obj)
	{
	}

	void Dystopia::FallingPillar::OnTriggerStay(GameObject * const _obj)
	{
	}

	void Dystopia::FallingPillar::OnTriggerExit(GameObject * const _obj)
	{
	}

	FallingPillar * FallingPillar::Duplicate() const
	{
		return new FallingPillar{*this};
	}

	const char * const FallingPillar::GetBehaviourName() const
	{
		return FallingPillar::BehaviourName;
	}
	
	TypeErasure::TypeEraseMetaData FallingPillar::GetMetaData()
	{
		static MetaData<Dystopia::FallingPillar> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const FallingPillar::GetMetaData() const
	{
		static MetaData<Dystopia::FallingPillar> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn; 
	}
	
	void FallingPillar::ExecuteMechanic(void)
	{
		if (!mbFall && !mbEnded)
		{
			mfAccuDt = 0.f;
			mbFall = true; 
            //if (auto parent = GetOwner()->GetComponent<Transform>()->GetParent())
            //    if (auto parentRig = parent->GetOwner()->GetComponent<RigidBody>())
            //    {
            //        parentRig->AddLinearImpulse({0.f,0,0});
            //        parentRig->ApplyAngularImpulse({0,0,-1.f * parentRig->GetMass() * 0.1f * mfFallSpeed});
            //    }
		}
	}
		
	void FallingPillar::TakeDamage(float)
	{
		if (!mbShake && !mbFall)
		{
			mfAccuDt = 0.f;
			mfIntensityCounter = mfShakeIntensity;
			mfDurationCounter = mfShakeDuration;
			mbShake = true;
		}
	}
	
	void FallingPillar::TakeForce(float, Math::Vec2)
	{
		if (!mbShake && !mbFall)
		{
			mfAccuDt = 0.f;
			mfIntensityCounter = mfShakeIntensity;
			mfDurationCounter = mfShakeDuration;
			mbShake = true;
		}
	}
	
	void FallingPillar::FallingUpdate(float _dt)
	{
		mfAccuDt += _dt;
		//float alphaDone = Math::Clamp(mfAccuDt / mfFallDuration, 0.f, 1.f);
		float speed = mfFallSpeed;
		//if (mfAccuDt >= mfFallDuration)
			speed *= 2.f * (mfAccuDt / mfFallDuration);
        //{
        //    //mbEnded = true;
        //}
        if (auto parent = GetOwner()->GetComponent<Transform>()->GetParent())
            if (auto parentRig = parent->GetOwner()->GetComponent<RigidBody>())
                parentRig->ApplyAngularImpulse({0,0,-1.f * parentRig->GetMass() * _dt * speed});
	}
	
	void FallingPillar::ShakingUpdate(float _dt)
	{
		mfAccuDt += (_dt);
		if (mfAccuDt > 2.f)
			mfAccuDt -= 2.f;
		
		if (mfIntensityCounter > 0.f && mfDurationCounter > 0.f)
		{
			mfDurationCounter -= _dt;
			mfIntensityCounter = Math::Lerp( 0.f, mfShakeIntensity, mfDurationCounter / mfShakeDuration);
		}
		else 
			mbShake = false;
		
		if (auto sr = GetOwner()->GetComponent<SpriteRenderer>())
		{
			if (auto shader = CORE::Get<ShaderSystem>()->GetShader("Shake Quad"))
			{
				sr->SetShader(shader);
				sr->SetManualShaderOverride("Intensity", mfIntensityCounter);
				sr->SetManualShaderOverride("Time", mfAccuDt * mfFrequency);
			}
		}
	}
}




