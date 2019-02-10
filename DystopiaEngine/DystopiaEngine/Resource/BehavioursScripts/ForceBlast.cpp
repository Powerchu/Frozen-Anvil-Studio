/* HEADER *********************************************************************************/
/*!
\file	ForceBlast.cpp
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "ForceBlast.h"
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
	namespace ForceBlast_MSG
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
	ForceBlast::ForceBlast()
		: mfCastDamage{1.f},
		mfCastKnockback{20.f},
		mfAnimSpeed{0.048f},
		mfApplyTiming{0.1f},
		mbAppliedAtk{false}, 
		mpAnim{nullptr},
		mArrFoundEnemies{},
		mArrBlastInteractable{},
		mForceDir{ 1.f, 0.f },
		mbActivate{false}
	{
	}

	ForceBlast::~ForceBlast()
	{
	}
	
	void ForceBlast::Awake()
	{
		mpAnim = nullptr;
		mbActivate = false;
	}

	void ForceBlast::Init()
	{
		mArrFoundEnemies.clear();
		mArrBlastInteractable.clear();
		mpAnim = nullptr;
		mbActivate = false;
		HashString database{"Force_Skills.ddata"};
		auto db =  EngineCore::Get<DatabaseSystem>();
		if (db->OpenData(database))
		{
			mfCastDamage = db->GetDatabase(database)->GetElement<float>(HashString{"Blast_Damage"});
			mfCastKnockback = db->GetDatabase(database)->GetElement<float>(HashString{"Blast_Knockback"});
		}
	}

	void ForceBlast::Update(const float _fDeltaTime)
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

	void ForceBlast::GameObjectDestroy(void)
	{
		Behaviour::GameObjectDestroy();
	}

	void Dystopia::ForceBlast::OnTriggerEnter(GameObject * const _obj)
	{
		if (_obj)
		{
			if (_obj->HasTag("Enemy"))
			{
				for (const auto& elem : mArrFoundEnemies)
					if (elem == _obj->GetID())
						return;
				mArrFoundEnemies.Insert(_obj->GetID());
			}
			if (_obj->HasTag("BlastInteractable"))
			{
				for (const auto& elem : mArrBlastInteractable)
					if (elem == _obj->GetID())
						return;
				mArrBlastInteractable.Insert(_obj->GetID());
			}
		}
	}

	void Dystopia::ForceBlast::OnTriggerStay(GameObject * const)
	{
	}

	void Dystopia::ForceBlast::OnTriggerExit(GameObject * const _obj)
	{
		if (_obj)
		{
			if (_obj->HasTag("Enemy"))
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
			if (_obj->HasTag("BlastInteractable"))
			{
				for (size_t i = 0; i < mArrBlastInteractable.size(); i++)
				{
					if (mArrBlastInteractable[i] ==  _obj->GetID())
					{
						mArrBlastInteractable.FastRemove(i);
						return;
					}
				}
			}
		}
	}

	ForceBlast * ForceBlast::Duplicate() const
	{
		return new ForceBlast{*this};
	}

	const char * const ForceBlast::GetBehaviourName() const
	{
		return ForceBlast::BehaviourName;
	}
	
	TypeErasure::TypeEraseMetaData ForceBlast::GetMetaData()
	{
		static MetaData<Dystopia::ForceBlast> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const ForceBlast::GetMetaData() const
	{
		static MetaData<Dystopia::ForceBlast> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	
	void ForceBlast::SetKnockDirection(Math::Vec2 _dir)
	{
		mForceDir = _dir;
	}
	
	void ForceBlast::StartSkill(void)
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
				ForceBlast_MSG::SendExternalMessage(res[0], "TakeForce", 4.f *mfCastKnockback, -mForceDir);
		}
	}

	void ForceBlast::SetFacing(bool _right)
	{
		auto scale = GetOwner()->GetComponent<Transform>()->GetGlobalScale();
		if (_right && scale.x < 0.f)
			GetOwner()->GetComponent<Transform>()->SetScale({scale.x * -1.f, scale.y, scale.z});
		else if (!_right && scale.x > 0.f)
			GetOwner()->GetComponent<Transform>()->SetScale({scale.x * -1.f, scale.y, scale.z});
	}
	
	void ForceBlast::SendDamage(void)
	{
		for (size_t i = 0; i < mArrFoundEnemies.size(); i++)
		{
			GameObject * obj = EngineCore::Get<SceneSystem>()->FindGameObject(mArrFoundEnemies[i]);
			if (obj && !(obj->GetFlags() & FLAG_REMOVE))
			{
				ForceBlast_MSG::SendExternalMessage(obj, "TakeDamage", mfCastDamage);
				ForceBlast_MSG::SendExternalMessage(obj, "TakeForce", mfCastKnockback*10.f, mForceDir);
			}
			else
				mArrFoundEnemies.FastRemove(i--);
		}
		for (size_t i = 0; i < mArrBlastInteractable.size(); i++)
		{
			GameObject * obj = EngineCore::Get<SceneSystem>()->FindGameObject(mArrBlastInteractable[i]);
			if (obj && !(obj->GetFlags() & FLAG_REMOVE))
				ForceBlast_MSG::SendExternalMessage(obj, "ExecuteMechanic");
			else
				mArrBlastInteractable.FastRemove(i--);
		}
	}
}




