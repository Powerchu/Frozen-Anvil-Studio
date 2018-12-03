/* HEADER *********************************************************************************/
/*!
\file	GoblinCombat.cpp
\author Dan Kang (100%)
\par    email: dan.kang\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "GoblinCombat.h"
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
#include "Component/Transform.h" 


namespace Dystopia
{
	namespace GoblinCombat_MSG
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
	GoblinCombat::GoblinCombat()
		: canAttack(true)
	{
	}

	GoblinCombat::~GoblinCombat()
	{
	}

	void GoblinCombat::Load()
	{
	}

	void GoblinCombat::Awake()
	{
		SetFlags(FLAG_ACTIVE);
		//get my parent goblin
	}

	void GoblinCombat::Init()
	{
		SetFlags(FLAG_ACTIVE);
	}

	void GoblinCombat::Update(const float _fDeltaTime)
	{
		if (mp_target != nullptr && canAttack)
		{
			GoblinCombat_MSG::SendExternalMessage(GetOwner()->GetComponent<Transform>()->GetParent()->GetOwner(), "Attacking"); 
			canAttack = false; 
		}
	}

	void GoblinCombat::FixedUpdate(const float _fDeltaTime)
	{
	}
	
	void GoblinCombat::PostUpdate(void)
	{
	}

	void GoblinCombat::GameObjectDestroy(void)
	{
	}

	void GoblinCombat::Unload(void)
	{
	}

	void Dystopia::GoblinCombat::OnCollisionEnter(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::GoblinCombat::OnCollisionStay(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::GoblinCombat::OnCollisionExit(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::GoblinCombat::OnTriggerEnter(GameObject * const _obj)
	{
		if (_obj)
		{
			name = _obj->GetNamePtr(); 
 
			if (!strcmp(name, "Player"))
			{
				mp_target = (_obj);
				DEBUG_PRINT(eLog::MESSAGE, "WEW");
			}
		}
	}

	void Dystopia::GoblinCombat::OnTriggerStay(GameObject * const _obj)
	{
	}

	void Dystopia::GoblinCombat::OnTriggerExit(GameObject * const _obj)
	{
		if (_obj)
		{
			name = _obj->GetNamePtr();

			if (!strcmp(name, "Player"))
			{
				mp_target = nullptr;
			}
		}
	}

	GoblinCombat * GoblinCombat::Duplicate() const
	{
		return new GoblinCombat{*this};
	}

	void GoblinCombat::Serialise(TextSerialiser& _ser) const
	{
	}

	void GoblinCombat::Unserialise(TextSerialiser& _ser)
	{
	}


	const char * const GoblinCombat::GetBehaviourName() const
	{
		return GoblinCombat::BehaviourName;
	}

	void GoblinCombat::EditorUI(void) noexcept
	{
		
		
	}
	
	void GoblinCombat::DealDamage(float _dmg)
	{
		if (mp_target)
		{
			GoblinCombat_MSG::SendExternalMessage(mp_target, "TakeDamage", _dmg);	
		}
		canAttack = true; 
	}

	TypeErasure::TypeEraseMetaData GoblinCombat::GetMetaData()
	{
		/*TO DO*/
		/*REMEMBER TO RETURN YOUR REFLECTED DATA HERE*/
		static MetaData<Dystopia::GoblinCombat> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const GoblinCombat::GetMetaData() const
	{
		/*TO DO*/
		/*REMEMBER TO RETURN YOUR REFLECTED DATA HERE*/
		static MetaData<Dystopia::GoblinCombat> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
}




