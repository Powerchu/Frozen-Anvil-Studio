/* HEADER *********************************************************************************/
/*!
\file	FormSpikeChild.cpp
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "FormSpikeChild.h"
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
#include "System/File/FileSystem.h"
#include "Math/MathUtility.h"

namespace Dystopia
{
	namespace FormSpikeChild_MSG
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
	FormSpikeChild::FormSpikeChild()
		: mfCastDamage{1.f},
		mfCastKnockback{20.f},
		mfAnimSpeed{0.048f}, 
		mpSprite{nullptr},
		mArrFoundEnemies{},
		mForceDir{ 1.f, 0.f },
		mbActivate{false},
		mnSpikeID{0},
		mfSpawnSpeed{1},
		mfSpawnCounter{},
		mfStartY{0},
		mfEndY{0},
		mfDeathCountdown{-1.f},
        mbFacing{true},
        mbReverse{false},
		mbPeaked{false}
	{
	}

	FormSpikeChild::~FormSpikeChild()
	{
	}
	
	void FormSpikeChild::Awake()
	{
		mpSprite = nullptr;
		mbActivate = mbPeaked = false;
        mbFacing = true;
        mbReverse = false;
		mnSpikeID = 0;
	}

	void FormSpikeChild::Init()
	{
		mpSprite = nullptr;
		mbActivate = mbPeaked = false;
		HashString database{"Form_Skills.ddata"};
		auto db =  EngineCore::Get<DatabaseSystem>();
		if (db->OpenData(database))
		{
			mfCastDamage = db->GetDatabase(database)->GetElement<float>(HashString{"Spike_Damage"});
			mfCastKnockback = db->GetDatabase(database)->GetElement<float>(HashString{"Spike_Knockback"});
			mfSpawnSpeed = db->GetDatabase(database)->GetElement<float>(HashString{"Spike_Spawn_Speed"});
		}
		mnSpikeID = 0;
		mfSpawnCounter = 0;
        mbReverse = false;
	}

	void FormSpikeChild::Update(const float _fDeltaTime)
	{
		if (mbActivate)
		{
			if (!mbReverse)
			{
				if (mfSpawnCounter < mfSpawnSpeed)
				{
					auto pos = GetOwner()->GetComponent<Transform>()->GetGlobalPosition();
					
					mfSpawnCounter += _fDeltaTime;
					//float v = Math::Lerp(mfStartY, mfEndY, Math::Clamp(mfSpawnCounter / mfSpawnSpeed, 0.f, 1.f));
					//GetOwner()->GetComponent<Transform>()->SetGlobalPosition({pos.x, v, pos.z});
					mbPeaked = false;
				}
				else
					mbPeaked = true;
			}
            
			if (mfDeathCountdown > 0.f)
			{
				mfDeathCountdown -= _fDeltaTime;
				if (mfDeathCountdown <= 0.f)
                {
                    //mbReverse = true;
                    //mfSpawnCounter = 0.f;
                    GetOwner()->Destroy();
                }
			}
            
            //if (mbReverse && (mfSpawnCounter < mfSpawnSpeed))
            //{
			//	auto pos = GetOwner()->GetComponent<Transform>()->GetGlobalPosition();
			//	
			//	mfSpawnCounter += _fDeltaTime;
			//	float v = Math::Lerp(mfEndY, mfStartY, Math::Clamp(mfSpawnCounter / mfSpawnSpeed, 0.f, 1.f));
			//	GetOwner()->GetComponent<Transform>()->SetGlobalPosition({pos.x, v, pos.z});
            //    if (mfSpawnCounter >= mfSpawnSpeed)
            //    {
            //        GetOwner()->Destroy();
            //        mbActivate = false;
            //    }
            //}
		}
	}

	void FormSpikeChild::GameObjectDestroy(void)
	{
		Behaviour::GameObjectDestroy();
	}

	void Dystopia::FormSpikeChild::OnCollisionEnter(const CollisionEvent& )
	{

	}

	void Dystopia::FormSpikeChild::OnCollisionStay(const CollisionEvent& )
	{

	}

	void Dystopia::FormSpikeChild::OnCollisionExit(const CollisionEvent& )
	{

	}

	void Dystopia::FormSpikeChild::OnTriggerEnter(GameObject * const _obj)
	{
		if (!mbPeaked && _obj && _obj->HasTag("Enemy") && mnSpikeID > 0 && mbActivate)
		{
			for (const auto& elem : mArrFoundEnemies)
				if (elem == _obj->GetID())
					return;
				
			mArrFoundEnemies.Insert(_obj->GetID());
			SendDamage(_obj);
		}
	}

	void Dystopia::FormSpikeChild::OnTriggerStay(GameObject * const)
	{
	}

	void Dystopia::FormSpikeChild::OnTriggerExit(GameObject * const )
	{
	}

	FormSpikeChild * FormSpikeChild::Duplicate() const
	{
		return new FormSpikeChild{*this};
	}
	
	const char * const FormSpikeChild::GetBehaviourName() const
	{
		return FormSpikeChild::BehaviourName;
	}

	TypeErasure::TypeEraseMetaData FormSpikeChild::GetMetaData()
	{
		static MetaData<Dystopia::FormSpikeChild> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const FormSpikeChild::GetMetaData() const
	{
		static MetaData<Dystopia::FormSpikeChild> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	
	void FormSpikeChild::SetKnockDirection(Math::Vec2 _dir)
	{
		mForceDir = _dir;
	}
	
	void FormSpikeChild::StartSkill(void)
	{
		mbActivate = true;
		mpSprite = GetOwner()->GetComponent<SpriteRenderer>();
		if (mpSprite && mnSpikeID > 0)
		{
			// auto t = EngineCore::Get<GraphicsSystem>()->LoadTexture(mnSpikeID == 1 ? 
			// 	EngineCore::Get<FileSystem>()->GetFromResource("Form_Spike_1.png").c_str(): mnSpikeID == 2 ? 
			// 	EngineCore::Get<FileSystem>()->GetFromResource("Form_Spike_2.png").c_str(): 
			// 	EngineCore::Get<FileSystem>()->GetFromResource("Form_Spike_3.png").c_str());
				
			//if (t)
			//{
			//	mpSprite->SetTexture(t);
				mpSprite->Stop();
				
				mpSprite->SetAnimation(mnSpikeID - 1);
                auto db =  EngineCore::Get<DatabaseSystem>();
                HashString database{"Form_Skills.ddata"};
                if (db->OpenData(database))
                {
                    mpSprite->ResizeToFit(
                    db->GetDatabase(database)->GetElement<float>("Spike_Universal_Scale"),
                    db->GetDatabase(database)->GetElement<float>("Spike_Universal_Scale")
                    );
                }
                else
                    mpSprite->ResizeToFit();
                    
                auto s = GetOwner()->GetComponent<Transform>()->GetGlobalScale();
                if ((mbFacing && s.x < 0.f ) || (!mbFacing && s.x > 0.f))
                    GetOwner()->GetComponent<Transform>()->SetScale({s.x * -1.f, s.y, s.z});
				
				mpSprite->SetSpeed(mfSpawnSpeed);
				mpSprite->Play();
			//}
            
			auto pos = GetOwner()->GetComponent<Transform>()->GetGlobalPosition();
			auto scale = GetOwner()->GetComponent<Transform>()->GetGlobalScale();
			mfStartY = pos.y + (scale.y * 0.5f);
			//mfEndY = mfStartY + (scale.y * 1.f);
			GetOwner()->GetComponent<Transform>()->SetGlobalPosition({pos.x, mfStartY, pos.z});
		}
	}

	void FormSpikeChild::SetFacing(bool _right)
	{
        mbFacing = _right;
	}
	
	void FormSpikeChild::SendDamage(GameObject * const _obj)
	{
		if (_obj && !(_obj->GetFlags() & FLAG_REMOVE))
		{
			FormSpikeChild_MSG::SendExternalMessage(_obj, "TakeDamage", mfCastDamage);
            float growth = static_cast<float>(mnSpikeID == 1 ? 0.6 : mnSpikeID == 2 ? 0.8f : mnSpikeID == 3 ? 1.f : 0.f);
			FormSpikeChild_MSG::SendExternalMessage(_obj, "TakeForce", mfCastKnockback * growth, mForceDir);
		}
	}
	
	void FormSpikeChild::SetSpikeID(int _i)
	{
		mnSpikeID = _i;
	}	
	
	void FormSpikeChild::SetDeathCountdown(float _time)
	{
		mfDeathCountdown = _time;
	}
	
}




