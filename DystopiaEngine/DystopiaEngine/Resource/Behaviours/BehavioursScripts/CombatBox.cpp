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
		void SendExternalMessage(GameObject * const _ptr, const char * _FuncName, Ts ... _Params)
		{
			EngineCore::GetInstance()->Get<BehaviourSystem>()->SendExternalMessage(_ptr, _FuncName, _Params...);
		}
		
		template<typename ... Ts>
		void SendExternalMessage(GameObject const * _ptr, const char * _FuncName, Ts ... _Params)
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
		: targetViable(false)
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
		SetFlags(FLAG_ACTIVE);
	}

	void CombatBox::Init()
	{
		SetFlags(FLAG_ACTIVE);
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

	void Dystopia::CombatBox::OnTriggerEnter(GameObject * const _obj)
	{
		auto * ptr = EngineCore::GetInstance()->Get<SceneSystem>()->FindGameObject(_obj->GetID());

		if (ptr)
		{
			name = ptr->GetNamePtr();

			if (!strcmp(name, "Goblin"))
			{
				targetViable = true;
			}
		}
	}

	void Dystopia::CombatBox::OnTriggerStay(GameObject * const _obj)
	{
	}

	void Dystopia::CombatBox::OnTriggerExit(GameObject * const _obj)
	{
		if (_obj != nullptr && name != nullptr)
		{
			if (!strcmp(name, _obj->GetNamePtr()))
			{
				targetViable = false;
				name = nullptr;
			}
		}
	}

	CombatBox * CombatBox::Duplicate() const
	{
		return new CombatBox{*this};
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
	
	void CombatBox::DealDamage(int _damage, bool _isFacingRight)
	{
		if (targetViable)
		{
			if (auto target = EngineCore::GetInstance()->Get<SceneSystem>()->FindGameObject_cstr(name))
			{
				DEBUG_PRINT(eLog::MESSAGE, "Doing Damange to Goblin");	
				CombatBox_MSG::SendExternalMessage(target, "TakeDamage", 10);
			}

		}
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




