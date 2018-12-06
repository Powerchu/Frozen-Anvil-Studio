/* HEADER *********************************************************************************/
/*!
\file	FormTOne.cpp
\author Dan Kang (100%)
\par    email: dan.kang\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "FormTOne.h"
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
	namespace FormTOne_MSG
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
	FormTOne::FormTOne()
		: firingDirection(0)
	{
	}

	FormTOne::~FormTOne()
	{
	}

	void FormTOne::Load()
	{
	}

	void FormTOne::Awake()
	{
		SetFlags(FLAG_ACTIVE);
	}

	void FormTOne::Init()
	{
		SetFlags(FLAG_ACTIVE);
	}

	void FormTOne::Update(const float )
	{
	}

	void FormTOne::FixedUpdate(const float )
	{
	}
	
	void FormTOne::PostUpdate(void)
	{
	}

	void FormTOne::GameObjectDestroy(void)
	{
	}

	void FormTOne::Unload(void)
	{
	}

	void Dystopia::FormTOne::OnCollisionEnter(const CollisionEvent& )
	{

	}

	void Dystopia::FormTOne::OnCollisionStay(const CollisionEvent& )
	{

	}

	void Dystopia::FormTOne::OnCollisionExit(const CollisionEvent& )
	{

	}

	void Dystopia::FormTOne::OnTriggerEnter(GameObject * const _obj)
	{
		auto * ptr = EngineCore::GetInstance()->Get<SceneSystem>()->FindGameObject(_obj->GetID());

		if (ptr)
		{
			name = ptr->GetNamePtr();

			if (!strcmp(name, "Goblin"))
			{
				DEBUG_PRINT(eLog::MESSAGE, "KNOCK!");
				FormTOne_MSG::SendExternalMessage(_obj, "TakeDamage", 10);
				FormTOne_MSG::SendExternalMessage(_obj, "Knock", 1200, firingDirection);
			}

			else if (!strcmp(name, "FormSlamTwo"))
			{
				FormTOne_MSG::SendExternalMessage(_obj, "Knock", 1500, 0);
			}
		}
	}

	void Dystopia::FormTOne::OnTriggerStay(GameObject * const )
	{
	}

	void Dystopia::FormTOne::OnTriggerExit(GameObject * const )
	{
	}

	FormTOne * FormTOne::Duplicate() const
	{
		return new FormTOne{*this};
	}

	void FormTOne::Serialise(TextSerialiser& ) const
	{
	}

	void FormTOne::Unserialise(TextSerialiser& )
	{
	}


	const char * const FormTOne::GetBehaviourName() const
	{
		return FormTOne::BehaviourName;
	}

	void FormTOne::EditorUI(void) noexcept
	{
		
		
	}
	
	TypeErasure::TypeEraseMetaData FormTOne::GetMetaData()
	{
		static MetaData<Dystopia::FormTOne> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const FormTOne::GetMetaData() const
	{
		static MetaData<Dystopia::FormTOne> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}

	void FormTOne::SetDirection(int _directionToSet)
	{
		firingDirection = _directionToSet;
	}
}




