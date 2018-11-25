/* HEADER *********************************************************************************/
/*!
\file	SkillManager.cpp
\author Dan Kang (100%)
\par    email: dan.kang\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "SkillManager.h"
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
	namespace SkillManager_MSG
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
	SkillManager::SkillManager()
	{
	}

	SkillManager::~SkillManager()
	{
	}

	void SkillManager::Load()
	{
	}

	void SkillManager::Awake()
	{
		SetFlags(FLAG_ACTIVE);
	}

	void SkillManager::Init()
	{
		SetFlags(FLAG_ACTIVE);
	}

	void SkillManager::Update(const float _fDeltaTime)
	{
	}

	void SkillManager::FixedUpdate(const float _fDeltaTime)
	{
	}
	
	void SkillManager::PostUpdate(void)
	{
	}

	void SkillManager::GameObjectDestroy(void)
	{
	}

	void SkillManager::Unload(void)
	{
	}

	void Dystopia::SkillManager::OnCollisionEnter(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::SkillManager::OnCollisionStay(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::SkillManager::OnCollisionExit(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::SkillManager::OnTriggerEnter(GameObject * const _obj)
	{
	}

	void Dystopia::SkillManager::OnTriggerStay(GameObject * const _obj)
	{
	}

	void Dystopia::SkillManager::OnTriggerExit(GameObject * const _obj)
	{
	}

	SkillManager * SkillManager::Duplicate() const
	{
		return new SkillManager{};
	}

	void SkillManager::Serialise(TextSerialiser& _ser) const
	{
	}

	void SkillManager::Unserialise(TextSerialiser& _ser)
	{
	}


	const char * const SkillManager::GetBehaviourName() const
	{
		return SkillManager::BehaviourName;
	}

	void SkillManager::EditorUI(void) noexcept
	{
		
		
	}
	
	TypeErasure::TypeEraseMetaData SkillManager::GetMetaData()
	{
		static MetaData<Dystopia::SkillManager> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const SkillManager::GetMetaData() const
	{
		static MetaData<Dystopia::SkillManager> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	void SkillManager::CastForm(int _skillNum, bool isFacingRight)
	{
		if (_skillNum == 0)
		{
			DEBUG_PRINT(eLog::MESSAGE, "CAST SPIKES");
		}
	}
	void SkillManager::CastForce(int _skillNum, bool isFacingRight)
	{
	}
}




