/* HEADER *********************************************************************************/
/*!
\file	PrimativeEnemy.cpp
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "PrimativeEnemy.h"
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
#include "System/Physics/PhysicsSystem.h"
#include "System/Graphics/GraphicsSystem.h"
#include "System/Graphics/Shader.h"
#include "System/Graphics/ShaderSystem.h"
#include "Component/SpriteRenderer.h"
#include "Component/RigidBody.h"
#include "Component/Collider.h"
#include "Component/AudioSource.h"

#define A_GOBLIN_FLINCH "goblin_flinch.wav"
#define A_GOBLIN_HIT "EnemyHit.wav"

namespace Dystopia
{
	namespace PrimativeEnemy_MSG
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
	PrimativeEnemy::PrimativeEnemy()
		:mbIsDead{false},
		mfMaximumHealth{100},
		mfCurrentHealth{100},
		mfForceTakenAmp{ 1000.f },
        mbUsePrimitiveUI{true},
        mfPrimitiveFull{0.47f},
        mfPrimitiveEmpty{0.53f},
        mpPrimitiveUI{nullptr},
		mpPlayer{nullptr}
	{
	}

	PrimativeEnemy::~PrimativeEnemy()
	{
	}

	void PrimativeEnemy::Awake()
	{
        mpPrimitiveUI = nullptr;
	}
 
	void PrimativeEnemy::Init()
	{
		mfCurrentHealth = mfMaximumHealth;
		mbIsDead = mfCurrentHealth <= 0.f;
		mpAudioSrc = GetOwner()->GetComponent<AudioSource>();
		mpPlayer = EngineCore::Get<SceneSystem>()->FindGameObject("CharacterController");
        if (mbUsePrimitiveUI)
        {
            auto& children = GetOwner()->GetComponent<Transform>()->GetAllChild();
            for (auto& c : children)
            {
                if (c->GetOwner()->GetName() == "PrimitiveHealthUI")
                {
                    mpPrimitiveUI = c->GetOwner();
                    break;
                }
            }
        }
	}

	void PrimativeEnemy::Update(const float _f)
	{
        if (mbUsePrimitiveUI)
            UpdatePrimitiveUI(_f);
	}

	void PrimativeEnemy::GameObjectDestroy(void)
	{
		Behaviour::GameObjectDestroy();
	}
	
	void Dystopia::PrimativeEnemy::OnCollisionEnter(const CollisionEvent& _colEvent)
	{
		// Hit Player
		if (_colEvent.mCollidedWith->GetComponent<Collider>()->GetColLayer() & LAYER_2 && _colEvent.mCollidedWith->GetComponent<Collider>()->GetColLayer() != LAYER_GLOBAL)
        {
			PrimativeEnemy_MSG::SendExternalMessage(_colEvent.mCollidedWith, "TakeDamage", mfAttackDamage/2, 1.f);
		}
	
	}

	void Dystopia::PrimativeEnemy::OnCollisionStay(const CollisionEvent&)
	{

	}

	void Dystopia::PrimativeEnemy::OnCollisionExit(const CollisionEvent&)
	{

	}

	void Dystopia::PrimativeEnemy::OnTriggerEnter(GameObject * const obj)
	{
		// Hit Player
		if (obj->GetComponent<Collider>()->GetColLayer() & LAYER_2 && obj->GetComponent<Collider>()->GetColLayer() != LAYER_GLOBAL)
        {
			PrimativeEnemy_MSG::SendExternalMessage(obj, "TakeDamage", mfAttackDamage/2, 1.f);
		}
	}

	void Dystopia::PrimativeEnemy::OnTriggerStay(GameObject * const)
	{
	}

	void Dystopia::PrimativeEnemy::OnTriggerExit(GameObject * const)
	{
	}

	PrimativeEnemy * PrimativeEnemy::Duplicate() const
	{
		return new PrimativeEnemy{*this};
	}

	const char * const PrimativeEnemy::GetBehaviourName() const
	{
		return PrimativeEnemy::BehaviourName;
	}
	
	TypeErasure::TypeEraseMetaData PrimativeEnemy::GetMetaData()
	{
		static MetaData<Dystopia::PrimativeEnemy> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const PrimativeEnemy::GetMetaData() const
	{
		static MetaData<Dystopia::PrimativeEnemy> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	
	void PrimativeEnemy::TakeDamage(float _amount)
	{
		if (mbIsDead) return;
		
		mfCurrentHealth -= _amount;

		if (mpAudioSrc)
		{
			mpAudioSrc->Stop();

			mpAudioSrc->SetVolume(1.0f);
			mpAudioSrc->ChangeAudio(A_GOBLIN_FLINCH);

			mpAudioSrc->Play();
		}

		if (mfCurrentHealth <= 0.f)
		{
			mfCurrentHealth = 0.f;
			mbIsDead = true;
			GetOwner()->Destroy();
		}

		if (mpPlayer)
			PrimativeEnemy_MSG::SendExternalMessage(mpPlayer, "IncreaseCombo");
	}
	
	void PrimativeEnemy::TakeForce(float _force, Math::Vec2 _dir)
	{
		RigidBody * pBody = GetOwner()->GetComponent<RigidBody>();
		if (pBody)
		{
			float f = _force * mfForceTakenAmp;
			pBody->SetLinearVel({0.f,0.f,0.f});
			pBody->AddLinearImpulse(Math::Vector3D{_dir.x * f, _dir.y * f, 0.f});
		}
	}
    
    void PrimativeEnemy::UpdatePrimitiveUI(float)
    {
        if (mpPrimitiveUI)
        {
			if (auto trans = mpPrimitiveUI->GetComponent<Transform>())
			{
				auto scale = trans->GetGlobalScale();
				trans->SetGlobalScale(Math::Abs(static_cast<float>(scale.x)), scale.y, scale.z);
			}
            if (auto sr = mpPrimitiveUI->GetComponent<SpriteRenderer>())
            {
                if (auto shader = CORE::Get<ShaderSystem>()->GetShader("Loading"))
                {
                    float alpha = Math::Clamp(mfCurrentHealth / mfMaximumHealth, 0.f, 1.f);
                    float perc = mfPrimitiveEmpty - (alpha * (mfPrimitiveEmpty - mfPrimitiveFull));
                    sr->SetShader(shader);
                    sr->SetManualShaderOverride("fRatio", Math::Clamp(perc, mfPrimitiveFull, mfPrimitiveEmpty));
                    sr->SetManualShaderOverride("fPivotX", 0.5f);
                    sr->SetManualShaderOverride("fPivotY", -2.f);
                }
            }                
        }
    }
}




