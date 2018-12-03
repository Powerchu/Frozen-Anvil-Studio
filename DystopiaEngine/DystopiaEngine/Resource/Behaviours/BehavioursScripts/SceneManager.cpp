/* HEADER *********************************************************************************/
/*!
\file	SceneManager.cpp
\author Dan Kang (100%)
\par    email: dan.kang\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "SceneManager.h"
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
	namespace SceneManager_MSG
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
	SceneManager::SceneManager()
	{
	}

	SceneManager::~SceneManager()
	{
	}

	void SceneManager::Load()
	{
	}

	void SceneManager::Awake()
	{
	}

	void SceneManager::Init()
	{
	}

	void SceneManager::Update(const float _fDeltaTime)
	{
	}

	void SceneManager::FixedUpdate(const float _fDeltaTime)
	{
	}
	
	void SceneManager::PostUpdate(void)
	{
	}

	void SceneManager::Unload(void)
	{
	}

	void Dystopia::SceneManager::OnCollisionEnter(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::SceneManager::OnCollisionStay(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::SceneManager::OnCollisionExit(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::SceneManager::OnTriggerEnter(GameObject * const _obj)
	{
	}

	void Dystopia::SceneManager::OnTriggerStay(GameObject * const _obj)
	{
	}

	void Dystopia::SceneManager::OnTriggerExit(GameObject * const _obj)
	{
	}

	SceneManager * SceneManager::Duplicate() const
	{
		return new SceneManager{*this};
	}

	void SceneManager::Serialise(TextSerialiser& _ser) const
	{
	}

	void SceneManager::Unserialise(TextSerialiser& _ser)
	{
	}


	const char * const SceneManager::GetBehaviourName() const
	{
		return SceneManager::BehaviourName;
	}

	void SceneManager::EditorUI(void) noexcept
	{
		
		 
	}
	
	TypeErasure::TypeEraseMetaData SceneManager::GetMetaData()
	{
		static MetaData<Dystopia::SceneManager> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const SceneManager::GetMetaData() const
	{
		static MetaData<Dystopia::SceneManager> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
}




