/* HEADER *********************************************************************************/
/*!
\file	CombatBox.cpp
\author Dan Kang (100%)
\par    email: dan.kang\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "CombatBox.h"
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
	namespace CombatBox_MSG
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
	CombatBox::CombatBox()
	{
	}

	CombatBox::~CombatBox()
	{
	}

	void CombatBox::Load()
	{
	}

	void CombatBox::Awake()
	{
	}

	void CombatBox::Init()
	{
	}

	void CombatBox::Update(const float _fDeltaTime)
	{
	}

	void CombatBox::FixedUpdate(const float _fDeltaTime)
	{
	}
	
	void CombatBox::PostUpdate(void)
	{
	}

	void CombatBox::GameObjectDestroy(void)
	{
	}

	void CombatBox::Unload(void)
	{
	}

	void Dystopia::CombatBox::OnCollisionEnter(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::CombatBox::OnCollisionStay(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::CombatBox::OnCollisionExit(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::CombatBox::OnTriggerEnter(const GameObject * _obj)
	{
		if (!_obj)
			return;

		auto * ptr = EngineCore::GetInstance()->Get<SceneSystem>()->FindGameObject(_obj->GetID());
		if (ptr)
		{
			const char * name = ptr->GetNamePtr();
			if (!strcmp(name, "Goblin"))
			{
				ptr->GetID();
				CombatBox_MSG::SendExternalMessage(ptr, "TEST", 10.f);
			}
		}
	}

	void Dystopia::CombatBox::OnTriggerStay(const GameObject * _obj)
	{
	}

	void Dystopia::CombatBox::OnTriggerExit(const GameObject * _obj)
	{
	}

	CombatBox * CombatBox::Duplicate() const
	{
		return new CombatBox{};
	}

	void CombatBox::Serialise(TextSerialiser& _ser) const
	{
	}

	void CombatBox::Unserialise(TextSerialiser& _ser)
	{
	}


	const char * const CombatBox::GetBehaviourName() const
	{
		return CombatBox::BehaviourName;
	}

	void CombatBox::EditorUI(void) noexcept
	{
		
		
	}
	
	TypeErasure::TypeEraseMetaData CombatBox::GetMetaData()
	{
		static MetaData<Dystopia::CombatBox> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const CombatBox::GetMetaData() const
	{
		static MetaData<Dystopia::CombatBox> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
}




