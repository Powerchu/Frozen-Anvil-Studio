/* HEADER *********************************************************************************/
/*!
\file	ForceTTwo.cpp
\author Dan Kang (100%)
\par    email: dan.kang\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "ForceTTwo.h"
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
	namespace ForceTTwo_MSG
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
	ForceTTwo::ForceTTwo()
		: firingDirection(0)
	{
	}

	ForceTTwo::~ForceTTwo()
	{
	}

	void ForceTTwo::Load()
	{
	}

	void ForceTTwo::Awake()
	{
		SetFlags(FLAG_ACTIVE);
	}

	void ForceTTwo::Init()
	{
		SetFlags(FLAG_ACTIVE);
	}

	void ForceTTwo::Update(const float )
	{
	}

	void ForceTTwo::FixedUpdate(const float )
	{
	}
	
	void ForceTTwo::PostUpdate(void)
	{
	}

	void ForceTTwo::Unload(void)
	{
	}

	void Dystopia::ForceTTwo::OnCollisionEnter(const CollisionEvent& )
	{

	}

	void Dystopia::ForceTTwo::OnCollisionStay(const CollisionEvent& )
	{

	}

	void Dystopia::ForceTTwo::OnCollisionExit(const CollisionEvent& )
	{

	}

	void Dystopia::ForceTTwo::OnTriggerEnter(GameObject * const _obj)
	{
		auto * ptr = EngineCore::GetInstance()->Get<SceneSystem>()->FindGameObject(_obj->GetID());
		if (ptr)
		{
			name = ptr->GetNamePtr();

			if (!strcmp(name, "Goblin"))
			{
				ForceTTwo_MSG::SendExternalMessage(_obj, "TakeDamage", 10);
				ForceTTwo_MSG::SendExternalMessage(_obj, "Knock", 1000, firingDirection);
			}
			else if (!strcmp(name, "FormSlamTwo"))
			{
				ForceTTwo_MSG::SendExternalMessage(_obj, "Knock", 2500, firingDirection);
			}
		}
	}

	void Dystopia::ForceTTwo::OnTriggerStay(GameObject * const)
	{
	}

	void Dystopia::ForceTTwo::OnTriggerExit(GameObject * const )
	{

	}

	ForceTTwo * ForceTTwo::Duplicate() const
	{
		return new ForceTTwo{*this};
	}

	void ForceTTwo::Serialise(TextSerialiser& ) const
	{
	}

	void ForceTTwo::Unserialise(TextSerialiser& )
	{
	}


	const char * const ForceTTwo::GetBehaviourName() const
	{
		return ForceTTwo::BehaviourName;
	}

	void ForceTTwo::EditorUI(void) noexcept
	{
		
		
	}
	
	TypeErasure::TypeEraseMetaData ForceTTwo::GetMetaData()
	{
		static MetaData<Dystopia::ForceTTwo> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const ForceTTwo::GetMetaData() const
	{
		static MetaData<Dystopia::ForceTTwo> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}

	void ForceTTwo::SetDirection(int _directionToSet)
	{
		firingDirection = _directionToSet;
	}
}




