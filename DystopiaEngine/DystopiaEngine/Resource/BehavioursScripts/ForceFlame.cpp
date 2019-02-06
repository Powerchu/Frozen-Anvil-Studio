/* HEADER *********************************************************************************/
/*!
\file	ForceFlame.cpp
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "ForceFlame.h"
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
	namespace ForceFlame_MSG
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
	ForceFlame::ForceFlame()
		: mfCastDamage{1.f},
		mfCastKnockback{20.f},
		mfAnimSpeed{0.048f},
		mfApplyTiming{0.1f},
		mbAppliedAtk{false}, 
		mpAnim{nullptr},
		mArrFoundEnemies{},
		mForceDir{ 1.f, 0.f },
		mbActivate{false}
	{
	}

	ForceFlame::~ForceFlame()
	{
	}

	void ForceFlame::Awake()
	{
		mpAnim = nullptr;
		mbActivate = false;
	}

	void ForceFlame::Init()
	{
		mpAnim = nullptr;
		mbActivate = false;
		HashString database{"Force_Skills.ddata"};
		auto db =  EngineCore::Get<DatabaseSystem>();
		if (db->OpenData(database))
		{
			mfCastDamage = db->GetDatabase(database)->GetElement<float>(HashString{"Flame_Damage"});
			mfCastKnockback = db->GetDatabase(database)->GetElement<float>(HashString{"Flame_Knockback"});
		}
	} 

	void ForceFlame::Update(const float _fDeltaTime)
	{
		if (mbActivate && !mbAppliedAtk)
		{
			mfApplyTiming -= _fDeltaTime;
			if (mfApplyTiming <= 0.f)
			{
				mbAppliedAtk = true;
				SendDamage();
			}
		}
	}

	void ForceFlame::GameObjectDestroy(void)
	{
		Behaviour::GameObjectDestroy();
	}

	void Dystopia::ForceFlame::OnTriggerEnter(GameObject * const _obj)
	{
		if (_obj && _obj->HasTag("Enemy"))
		{
			for (const auto& elem : mArrFoundEnemies)
				if (elem == _obj->GetID())
					return;
			mArrFoundEnemies.Insert(_obj->GetID());
		}
	}

	void Dystopia::ForceFlame::OnTriggerStay(GameObject * const)
	{
	}

	void Dystopia::ForceFlame::OnTriggerExit(GameObject * const _obj)
	{
		if (_obj && _obj->HasTag("Enemy"))
		{
			for (size_t i = 0; i < mArrFoundEnemies.size(); i++)
			{
				if (mArrFoundEnemies[i] ==  _obj->GetID())
				{
					mArrFoundEnemies.FastRemove(i);
					return;
				}
			}
		}
	}

	ForceFlame * ForceFlame::Duplicate() const
	{
		return new ForceFlame{*this};
	}

	const char * const ForceFlame::GetBehaviourName() const
	{
		return ForceFlame::BehaviourName;
	}

	TypeErasure::TypeEraseMetaData ForceFlame::GetMetaData()
	{
		static MetaData<Dystopia::ForceFlame> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const ForceFlame::GetMetaData() const
	{
		static MetaData<Dystopia::ForceFlame> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	
	void ForceFlame::SetKnockDirection(Math::Vec2 _dir)
	{
		mForceDir = _dir;
	}
	
	void ForceFlame::StartSkill(void)
	{
		mbActivate = true;
		mpAnim = GetOwner()->GetComponent<SpriteRenderer>();
		if (mpAnim)
		{
			mpAnim->Stop();
			mpAnim->SetAnimation(static_cast<unsigned>(0));
			mpAnim->SetSpeed(mfAnimSpeed);
			mpAnim->Play();
			const auto& res = EngineCore::Get<SceneSystem>()->FindGameObjectByTag("Player");
			if (res.size() == 1)
				ForceFlame_MSG::SendExternalMessage(res[0], "TakeForce", 2.f * mfCastKnockback, -mForceDir);
		}
	}

	void ForceFlame::SetFacing(bool _right)
	{
		auto scale = GetOwner()->GetComponent<Transform>()->GetGlobalScale();
		if (_right && scale.x < 0.f)
			GetOwner()->GetComponent<Transform>()->SetScale({scale.x * -1.f, scale.y, scale.z});
		else if (!_right && scale.x > 0.f)
			GetOwner()->GetComponent<Transform>()->SetScale({scale.x * -1.f, scale.y, scale.z});
	}
	
	void ForceFlame::SendDamage(void)
	{
		for (size_t i = 0; i < mArrFoundEnemies.size(); i++)
		{
			GameObject * obj = EngineCore::Get<SceneSystem>()->FindGameObject(mArrFoundEnemies[i]);
			if (obj && !(obj->GetFlags() & FLAG_REMOVE))
			{
				ForceFlame_MSG::SendExternalMessage(obj, "TakeDamage", mfCastDamage);
				ForceFlame_MSG::SendExternalMessage(obj, "TakeForce", 3.f * mfCastKnockback, mForceDir);
			}
			else
				mArrFoundEnemies.FastRemove(i--);
		}
	}
	
}



