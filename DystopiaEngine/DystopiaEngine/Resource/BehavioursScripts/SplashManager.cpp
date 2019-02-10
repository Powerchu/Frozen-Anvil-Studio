/* HEADER *********************************************************************************/
/*!
\file	SplashManager.cpp
\author Keith Goh (100%)
\par    email: keith.goh\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "SplashManager.h"
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
	namespace SplashManager_MSG
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
	SplashManager::SplashManager()
	{
	}

	SplashManager::~SplashManager()
	{
	}

	void SplashManager::Load()
	{
	}

	void SplashManager::Awake()
	{
	}

	void SplashManager::Init()
	{
	}

	void SplashManager::Update(const float _fDeltaTime)
	{
		Time -= _fDeltaTime;
		if(Time <= 0)
			EngineCore::Get<SceneSystem>()->LoadScene("MainMenuFinal");
	}

	void SplashManager::FixedUpdate(const float )
	{
	}
	
	void SplashManager::PostUpdate(void)
	{
	}

	void SplashManager::GameObjectDestroy(void)
	{
	}

	void SplashManager::Unload(void)
	{
	}

	void Dystopia::SplashManager::OnCollisionEnter(const CollisionEvent& )
	{

	}

	void Dystopia::SplashManager::OnCollisionStay(const CollisionEvent& )
	{

	}

	void Dystopia::SplashManager::OnCollisionExit(const CollisionEvent& )
	{

	}

	void Dystopia::SplashManager::OnTriggerEnter(GameObject * const )
	{
	}

	void Dystopia::SplashManager::OnTriggerStay(GameObject * const )
	{
	}

	void Dystopia::SplashManager::OnTriggerExit(GameObject * const )
	{
	}

	SplashManager * SplashManager::Duplicate() const
	{
		return new SplashManager{*this};
	}

	void SplashManager::Serialise(TextSerialiser& ) const
	{
	}

	void SplashManager::Unserialise(TextSerialiser& )
	{
	}


	const char * const SplashManager::GetBehaviourName() const
	{
		return SplashManager::BehaviourName;
	}

	void SplashManager::EditorUI(void) noexcept
	{
		
		
	}
	
	TypeErasure::TypeEraseMetaData SplashManager::GetMetaData()
	{
		static MetaData<Dystopia::SplashManager> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const SplashManager::GetMetaData() const
	{
		static MetaData<Dystopia::SplashManager> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
}




