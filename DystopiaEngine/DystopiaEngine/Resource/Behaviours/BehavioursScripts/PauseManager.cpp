/* HEADER *********************************************************************************/
/*!
\file	PauseManager.cpp
\author Keith Goh (100%)
\par    email: keith.goh\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "PauseManager.h"
#include "System/Input/InputSystem.h"
#include "System/Input/InputMap.h"
#include "System/Driver/Driver.h"
#include "System/Behaviour/BehaviourSystem.h"

#include "System/Scene/SceneSystem.h"
#include "System/Time/TimeSystem.h"
#include "System/Collision/CollisionEvent.h"
#include "System/Collision/CollisionSystem.h"

#include "Object/ObjectFlags.h"
#include "Object/GameObject.h"

#include "Editor/EGUI.h"
#include "Utility/DebugAssert.h"



namespace Dystopia
{
	namespace PauseManager_MSG
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
	PauseManager::PauseManager()
	:mPauseState{false}
	{
	}  

	PauseManager::~PauseManager()
	{
	}

	void PauseManager::Load()
	{
		SetFlags(FLAG_ACTIVE);
		mPauseState = false;
		EngineCore::GetInstance()->Get<TimeSystem>()->SetTimeScale(1.f); 
	}

	void PauseManager::Awake()
	{
		DEBUG_PRINT(eLog::MESSAGE, "Awake");
		SetFlags(FLAG_ACTIVE);
		mPauseState = false;
		EngineCore::GetInstance()->Get<TimeSystem>()->SetTimeScale(1.f); 
	}

	void PauseManager::Init()
	{
		DEBUG_PRINT(eLog::MESSAGE, "Init");
		SetFlags(FLAG_ACTIVE);
		mPauseState = false;
		EngineCore::GetInstance()->Get<TimeSystem>()->SetTimeScale(1.f); 
	}

	void PauseManager::Update(const float)
	{

		bool isPaused = EngineCore::GetInstance()->GetSystem<InputManager>()->IsKeyTriggered(XBUTTON_START) || EngineCore::GetInstance()->GetSystem<InputManager>()->IsKeyTriggered(KEYBOARD_ESCAPE);
		if(isPaused) 
		{
			if(mPauseState)
			{
				DEBUG_PRINT(eLog::MESSAGE, "paused!");
				mPauseState = false;
				EngineCore::GetInstance()->Get<TimeSystem>()->SetTimeScale(1.f); 
			} 
			else
			{
				DEBUG_PRINT(eLog::MESSAGE, "unpaused!"); 
				EngineCore::GetInstance()->Get<TimeSystem>()->SetTimeScale(0.f); 
				mPauseState = true;   
			} 

		}
		
	}

	void PauseManager::FixedUpdate(const float)
	{
	}

	void PauseManager::PostUpdate(void)
	{
	}

	void PauseManager::GameObjectDestroy(void)
	{
	}

	void PauseManager::Unload(void)
	{
	}

	void Dystopia::PauseManager::OnCollisionEnter(const CollisionEvent&)
	{

	}

	void Dystopia::PauseManager::OnCollisionStay(const CollisionEvent&)
	{

	}

	void Dystopia::PauseManager::OnCollisionExit(const CollisionEvent&)
	{

	}

	void Dystopia::PauseManager::OnTriggerEnter(GameObject * const)
	{
	}

	void Dystopia::PauseManager::OnTriggerStay(GameObject * const)
	{
	}

	void Dystopia::PauseManager::OnTriggerExit(GameObject * const)
	{
	}

	PauseManager * PauseManager::Duplicate() const
	{
		return new PauseManager{ *this };
	}

	void PauseManager::Serialise(TextSerialiser&) const
	{
	}

	void PauseManager::Unserialise(TextSerialiser&)
	{
	}


	const char * const PauseManager::GetBehaviourName() const
	{
		return PauseManager::BehaviourName;
	}

	void PauseManager::EditorUI(void) noexcept
	{


	}

	TypeErasure::TypeEraseMetaData PauseManager::GetMetaData()
	{
		static MetaData<Dystopia::PauseManager> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{ mMetaData };
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const PauseManager::GetMetaData() const
	{
		static MetaData<Dystopia::PauseManager> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{ mMetaData };
		return mReturn;
	}
}




