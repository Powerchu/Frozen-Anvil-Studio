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
#include "System/Physics/PhysicsSystem.h"

#include "System/Scene/SceneSystem.h"
#include "System/Collision/CollisionEvent.h"
#include "System/Collision/CollisionSystem.h"

#include "Object/ObjectFlags.h"
#include "Object/GameObject.h"

#include "Component/RigidBody.h"
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
		//rBody = GetOwner()->GetComponent<RigidBody>();
	}

	void FormTTwo::Init()
	{
		SetFlags(FLAG_ACTIVE);
		//rBody = GetOwner()->GetComponent<RigidBody>();
	}

	void FormTTwo::Update(const float _fDeltaTime)
	{
	}

	void FormTTwo::FixedUpdate(const float _fDeltaTime)
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
		/*if (_colEvent.mCollidedWith !=)
		{
			name = _colEvent.mCollidedWith->GetNamePtr();

			if (!strcmp(name, "Goblin"))
			{
				DEBUG_PRINT(eLog::MESSAGE, "Flamestrike");
				FormTTwo_MSG::SendExternalMessage(_obj, "TakeDamage", 10);
				FormTTwo_MSG::SendExternalMessage(_obj, "Knock", 150, firingDirection);
			}
		}*/
	}

	void Dystopia::FormTTwo::OnCollisionStay(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::FormTTwo::OnCollisionExit(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::FormTTwo::OnTriggerEnter(GameObject * const _obj)
	{
	}

	void Dystopia::FormTTwo::OnTriggerStay(GameObject * const _obj)
	{
	}

	void Dystopia::FormTTwo::OnTriggerExit(GameObject * const _obj)
	{
	}

	FormTTwo * FormTTwo::Duplicate() const
	{
		return new FormTTwo{};
	}

	void FormTTwo::Serialise(TextSerialiser& _ser) const
	{
	}

	void FormTTwo::Unserialise(TextSerialiser& _ser)
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
		if (_direction == 1)
		{
			//rBody->AddLinearImpulse({ -50 * rBody->GetMass(), 10 * rBody->GetMass(), 0 });
		}

		else
		{
			//rBody->AddLinearImpulse({ 50 * rBody->GetMass(), 10 * rBody->GetMass(), 0 });
		}
	}
}




