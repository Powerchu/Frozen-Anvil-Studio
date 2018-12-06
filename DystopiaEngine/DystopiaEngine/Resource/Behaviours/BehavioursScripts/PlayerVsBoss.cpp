/* HEADER *********************************************************************************/
/*!
\file	PlayerVsBoss.cpp
\author Dan (100%)
\par    email: dan.kang\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "PlayerVsBoss.h"
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



namespace Dystopia
{
	namespace PlayerVsBoss_MSG
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
	PlayerVsBoss::PlayerVsBoss()
	{
	}

	PlayerVsBoss::~PlayerVsBoss()
	{
	}

	void PlayerVsBoss::Load()
	{
	}

	void PlayerVsBoss::Awake()
	{
	}

	void PlayerVsBoss::Init()
	{
		SetFlags(eObjFlag::FLAG_ACTIVE);
	}

	void PlayerVsBoss::Update(const float )
	{
	}

	void PlayerVsBoss::FixedUpdate(const float )
	{
	}
	
	void PlayerVsBoss::PostUpdate(void)
	{
	}

	void PlayerVsBoss::GameObjectDestroy(void)
	{
	}

	void PlayerVsBoss::Unload(void)
	{
	}

	void Dystopia::PlayerVsBoss::OnCollisionEnter(const CollisionEvent& _colEvent)
	{
		if (!strcmp(_colEvent.mCollidedWith->GetName().c_str(), "BossAttackArea"))
		{
			DEBUG_PRINT(eLog::MESSAGE, "Sending player take damage");
			PlayerVsBoss_MSG::SendInternalMessage(this, "TakeDamage", 10.f);
		}

		if (!strcmp(_colEvent.mCollidedWith->GetName().c_str(), "Flame"))
		{
			DEBUG_PRINT(eLog::MESSAGE, "Sending player take damage");
			PlayerVsBoss_MSG::SendInternalMessage(this, "TakeDamage", 20.f);
		}
	}

	void Dystopia::PlayerVsBoss::OnCollisionStay(const CollisionEvent& )
	{
		
	}

	void Dystopia::PlayerVsBoss::OnCollisionExit(const CollisionEvent& )
	{

	}

	void Dystopia::PlayerVsBoss::OnTriggerEnter(GameObject * const )
	{
		
	}

	void Dystopia::PlayerVsBoss::OnTriggerStay(GameObject * const )
	{
		
	}

	void Dystopia::PlayerVsBoss::OnTriggerExit(GameObject * const )
	{
	}

	PlayerVsBoss * PlayerVsBoss::Duplicate() const
	{
		return new PlayerVsBoss{*this};
	}

	void PlayerVsBoss::Serialise(TextSerialiser& ) const
	{
	}

	void PlayerVsBoss::Unserialise(TextSerialiser& )
	{
	}


	const char * const PlayerVsBoss::GetBehaviourName() const
	{
		return PlayerVsBoss::BehaviourName;
	}

	void PlayerVsBoss::EditorUI(void) noexcept
	{
		
		
	}
	
	TypeErasure::TypeEraseMetaData PlayerVsBoss::GetMetaData()
	{
		static MetaData<Dystopia::PlayerVsBoss> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const PlayerVsBoss::GetMetaData() const
	{
		static MetaData<Dystopia::PlayerVsBoss> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
}




