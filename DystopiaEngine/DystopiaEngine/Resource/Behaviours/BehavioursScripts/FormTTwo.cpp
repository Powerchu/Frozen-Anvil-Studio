/* HEADER *********************************************************************************/
/*!
\file	FormTTwo.cpp
\author Dan Kang (100%)
\par    email: dan.kang\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "FormTTwo.h"
#include "System/Input/InputSystem.h"
#include "System/Input/InputMap.h"
#include "System/Driver/Driver.h"
#include "System/Behaviour/BehaviourSystem.h"

#include "System/Scene/SceneSystem.h"
#include "System/Collision/CollisionEvent.h"
#include "System/Collision/CollisionSystem.h"

#include "Object/ObjectFlags.h"
#include "Object/GameObject.h"
#include "Component/RigidBody.h"
#include "Component/Transform.h"

#include "Editor/EGUI.h"
#include "Utility/DebugAssert.h"



namespace Dystopia
{
	namespace FormTTwo_MSG
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
	FormTTwo::FormTTwo()
		: rBody(nullptr)
		, firingDirection(0)
	{
	}

	FormTTwo::~FormTTwo()
	{
	}

	void FormTTwo::Load()
	{
	}

	void FormTTwo::Awake()
	{
		SetFlags(FLAG_ACTIVE);
		if(GetOwner())
			rBody = GetOwner()->GetComponent<RigidBody>();
	}

	void FormTTwo::Init()
	{
		SetFlags(FLAG_ACTIVE);
		if(GetOwner())
			rBody = GetOwner()->GetComponent<RigidBody>();
	}

	void FormTTwo::Update(const float )
	{
	}

	void FormTTwo::FixedUpdate(const float )
	{
	}
	
	void FormTTwo::PostUpdate(void)
	{
	}

	void FormTTwo::GameObjectDestroy(void)
	{
	}

	void FormTTwo::Unload(void)
	{
	}

	void Dystopia::FormTTwo::OnCollisionEnter(const CollisionEvent& _colEvent)
	{
		if (_colEvent.mCollidedWith != nullptr)
		{
			name = _colEvent.mCollidedWith->GetNamePtr();

			if (!strcmp(name, "Goblin"))
			{
				DEBUG_PRINT(eLog::MESSAGE, "Knock Enemy!!!");
				FormTTwo_MSG::SendExternalMessage(_colEvent.mCollidedWith, "TakeDamage", 10);
				FormTTwo_MSG::SendExternalMessage(_colEvent.mCollidedWith, "Knock", 150, firingDirection);
			}
		}
	}

	void Dystopia::FormTTwo::OnCollisionStay(const CollisionEvent& )
	{

	}

	void Dystopia::FormTTwo::OnCollisionExit(const CollisionEvent& )
	{

	}

	void Dystopia::FormTTwo::OnTriggerEnter(GameObject * const )
	{
	}

	void Dystopia::FormTTwo::OnTriggerStay(GameObject * const )
	{
	}

	void Dystopia::FormTTwo::OnTriggerExit(GameObject * const )
	{
	}

	FormTTwo * FormTTwo::Duplicate() const
	{
		return new FormTTwo{*this};
	}

	void FormTTwo::Serialise(TextSerialiser& ) const
	{
	}

	void FormTTwo::Unserialise(TextSerialiser& )
	{
	}


	const char * const FormTTwo::GetBehaviourName() const
	{
		return FormTTwo::BehaviourName;
	}

	void FormTTwo::EditorUI(void) noexcept
	{
		
		
	}
	
	TypeErasure::TypeEraseMetaData FormTTwo::GetMetaData()
	{
		static MetaData<Dystopia::FormTTwo> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const FormTTwo::GetMetaData() const
	{
		static MetaData<Dystopia::FormTTwo> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}

	void FormTTwo::SetDirection(int _direction)
	{
		if(!rBody)
		{
			DEBUG_PRINT(eLog::MESSAGE, "FormTTwo has no Rigidbody. This message came from SetDirection()"); 
			return;
		}
		auto theMass = rBody->GetMass();
		DEBUG_PRINT(eLog::MESSAGE, "MyOwner Position %f", static_cast<float>(GetOwner()->GetComponent<Transform>()->GetGlobalPosition().x));
		DEBUG_PRINT(eLog::MESSAGE, "MyOwner %p", GetOwner());
		DEBUG_PRINT(eLog::MESSAGE, "MyOwnerID %u", GetOwnerID()); 

		//stone is flinging towards the right
		if (_direction == 2)
		{
			rBody->AddLinearImpulse({200 * theMass, -150 * theMass, 0});
			firingDirection = _direction;
		}

		if (_direction == 1)
		{
			rBody->AddLinearImpulse({-200 * theMass, -150 * theMass, 0});
			firingDirection = _direction;
		}
		DEBUG_PRINT(eLog::MESSAGE, "MyOwner Position after AddLinearImpluse %f", static_cast<float>(GetOwner()->GetComponent<Transform>()->GetGlobalPosition().x));
	}

	void FormTTwo::Knock(int _amt, int _direction)
	{
		DEBUG_PRINT(eLog::MESSAGE, "received Knock");
		
		auto theMass = rBody->GetMass();
		if (_direction == 0) //knock up
			rBody->AddLinearImpulse({0, _amt * theMass, 0});

		if (_direction == 1) //knock left
			rBody->AddLinearImpulse({ -(_amt * theMass), 0, 0 });
		
		if (_direction == 2) //knock right
			rBody->AddLinearImpulse({ _amt * theMass, 0, 0 });
	}
}




