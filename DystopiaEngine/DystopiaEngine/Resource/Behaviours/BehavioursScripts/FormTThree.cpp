/* HEADER *********************************************************************************/
/*!
\file	FormTThree.cpp
\author Dan Kang (100%)
\par    email: dan.kang\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "FormTThree.h"
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
	namespace FormTThree_MSG
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
	FormTThree::FormTThree()
		: rBody(nullptr)
		, firingDirection(0)
	{
	}

	FormTThree::~FormTThree()
	{
	}

	void FormTThree::Load()
	{
	}

	void FormTThree::Awake()
	{
		SetFlags(FLAG_ACTIVE);
		if(GetOwner())
			rBody = GetOwner()->GetComponent<RigidBody>();
	}

	void FormTThree::Init()
	{
		SetFlags(FLAG_ACTIVE);
		if(GetOwner())
			rBody = GetOwner()->GetComponent<RigidBody>();
	}

	void FormTThree::Update(const float )
	{
	}

	void FormTThree::FixedUpdate(const float )
	{
	}
	
	void FormTThree::PostUpdate(void)
	{
	}

	void FormTThree::Unload(void)
	{
	}

	void Dystopia::FormTThree::OnCollisionEnter(const CollisionEvent& )
	{

	}

	void Dystopia::FormTThree::OnCollisionStay(const CollisionEvent& )
	{

	}

	void Dystopia::FormTThree::OnCollisionExit(const CollisionEvent& )
	{

	}

	void Dystopia::FormTThree::OnTriggerEnter(GameObject * const _obj)
	{
		auto * ptr = EngineCore::GetInstance()->Get<SceneSystem>()->FindGameObject(_obj->GetID());

		if (ptr)
		{
			name = ptr->GetNamePtr();

			if (!strcmp(name, "Goblin"))
			{
				DEBUG_PRINT(eLog::MESSAGE, "KNOCK!");
				FormTThree_MSG::SendExternalMessage(_obj, "TakeDamage", 10);
				FormTThree_MSG::SendExternalMessage(_obj, "Knock", 150, firingDirection);
			}
		}
	}

	void Dystopia::FormTThree::OnTriggerStay(GameObject * const )
	{
	}

	void Dystopia::FormTThree::OnTriggerExit(GameObject * const )
	{
	}

	FormTThree * FormTThree::Duplicate() const
	{
		return new FormTThree{*this};
	}

	void FormTThree::Serialise(TextSerialiser& ) const
	{
	}

	void FormTThree::Unserialise(TextSerialiser& )
	{
	}


	const char * const FormTThree::GetBehaviourName() const
	{
		return FormTThree::BehaviourName; 
	}

	void FormTThree::EditorUI(void) noexcept
	{
		
		
	}
	
	TypeErasure::TypeEraseMetaData FormTThree::GetMetaData()
	{
		static MetaData<Dystopia::FormTThree> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const FormTThree::GetMetaData() const
	{
		static MetaData<Dystopia::FormTThree> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	
	void FormTThree::SetDirection(int _directionToSet)
	{
		firingDirection = _directionToSet;
		
		auto theMass = rBody->GetMass();

		if(firingDirection == 2)
			rBody->AddLinearImpulse({30 * theMass, 0, 0}); 

		else
			rBody->AddLinearImpulse({-30 * theMass, 0, 0});
	}
}




