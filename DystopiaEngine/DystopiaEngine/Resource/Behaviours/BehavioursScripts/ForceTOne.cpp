/* HEADER *********************************************************************************/
/*!
\file	ForceTOne.cpp
\author Dan Kang (100%)
\par    email: dan.kang\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "ForceTOne.h"
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
	namespace ForceTOne_MSG
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
	ForceTOne::ForceTOne()
		: firingDirection(0)
	{
	}

	ForceTOne::~ForceTOne()
	{
	}

	void ForceTOne::Load()
	{
	}

	void ForceTOne::Awake()
	{
		SetFlags(FLAG_ACTIVE);
	}

	void ForceTOne::Init()
	{
		SetFlags(FLAG_ACTIVE);
	}

	void ForceTOne::Update(const float )
	{
	}

	void ForceTOne::FixedUpdate(const float )
	{
	}
	
	void ForceTOne::PostUpdate(void)
	{
	}

	void ForceTOne::GameObjectDestroy(void)
	{
	}

	void ForceTOne::Unload(void)
	{
	}

	void Dystopia::ForceTOne::OnCollisionEnter(const CollisionEvent& )
	{

	}

	void Dystopia::ForceTOne::OnCollisionStay(const CollisionEvent& )
	{

	}

	void Dystopia::ForceTOne::OnCollisionExit(const CollisionEvent& )
	{

	}

	void Dystopia::ForceTOne::OnTriggerEnter(GameObject * const _obj)
	{
		auto * ptr = EngineCore::GetInstance()->Get<SceneSystem>()->FindGameObject(_obj->GetID());

		if (ptr)
		{
			name = ptr->GetNamePtr();


			if (!strcmp(name, "Goblin"))
			{
				DEBUG_PRINT(eLog::MESSAGE, "Flamestrike");
				ForceTOne_MSG::SendExternalMessage(_obj, "TakeDamage", 10);
				ForceTOne_MSG::SendExternalMessage(_obj, "Knock", 500, firingDirection);
			}
		}
	}

	void Dystopia::ForceTOne::OnTriggerStay(GameObject * const )
	{
	}

	void Dystopia::ForceTOne::OnTriggerExit(GameObject * const )
	{
	}

	ForceTOne * ForceTOne::Duplicate() const
	{
		return new ForceTOne{*this};
	}

	void ForceTOne::Serialise(TextSerialiser& ) const
	{
	}

	void ForceTOne::Unserialise(TextSerialiser& )
	{
	}


	const char * const ForceTOne::GetBehaviourName() const
	{
		return ForceTOne::BehaviourName;
	}

	void ForceTOne::EditorUI(void) noexcept
	{
		
		
	}

	TypeErasure::TypeEraseMetaData ForceTOne::GetMetaData()
	{
		static MetaData<Dystopia::ForceTOne> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const ForceTOne::GetMetaData() const
	{
		static MetaData<Dystopia::ForceTOne> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}

	void ForceTOne::SetDirection(int _directionToSet)
	{
		firingDirection = _directionToSet;
	}
}




