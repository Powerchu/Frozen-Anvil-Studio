/* HEADER *********************************************************************************/
/*!
\file	SplashScreen.cpp
\author Dan Kang (100%)
\par    email: dan.kang\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "SplashScreen.h"
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
	namespace SplashScreen_MSG
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
	SplashScreen::SplashScreen()
		: delay(0.0f)
		, count(0.0f)
	{
	}

	SplashScreen::~SplashScreen()
	{
	}

	void SplashScreen::Load()
	{
	}

	void SplashScreen::Awake()
	{
		count = 0.0f;
	}

	void SplashScreen::Init()
	{
		count = 0.0f;
	}

	void SplashScreen::Update(const float _fDeltaTime)
	{
		count = count + _fDeltaTime;

		if (count >= delay)
		{
			EngineCore::GetInstance()->GetSystem<SceneSystem>()->LoadScene("MainMenu.dscene");
		}
	}

	void SplashScreen::FixedUpdate(const float )
	{
	}
	
	void SplashScreen::PostUpdate(void)
	{
	}

	void SplashScreen::Unload(void)
	{
	}

	void Dystopia::SplashScreen::OnCollisionEnter(const CollisionEvent& )
	{

	}

	void Dystopia::SplashScreen::OnCollisionStay(const CollisionEvent& )
	{

	}

	void Dystopia::SplashScreen::OnCollisionExit(const CollisionEvent& )
	{

	}

	void Dystopia::SplashScreen::OnTriggerEnter(GameObject * const )
	{
	}

	void Dystopia::SplashScreen::OnTriggerStay(GameObject * const )
	{
	}

	void Dystopia::SplashScreen::OnTriggerExit(GameObject * const )
	{
	}

	SplashScreen * SplashScreen::Duplicate() const
	{
		return new SplashScreen{};
	}

	void SplashScreen::Serialise(TextSerialiser& ) const
	{
	}

	void SplashScreen::Unserialise(TextSerialiser& )
	{
	}


	const char * const SplashScreen::GetBehaviourName() const
	{
		return SplashScreen::BehaviourName;
	}

	void SplashScreen::EditorUI(void) noexcept
	{
		
		
	}
	
	TypeErasure::TypeEraseMetaData SplashScreen::GetMetaData()
	{
		static MetaData<Dystopia::SplashScreen> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const SplashScreen::GetMetaData() const
	{
		static MetaData<Dystopia::SplashScreen> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
}




