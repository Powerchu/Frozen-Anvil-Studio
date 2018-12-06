/* HEADER *********************************************************************************/
/*!
\file	HowToPlay.cpp
\author Dan (100%)
\par    email: dan.kang\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "HowToPlay.h"
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

#include "Component/Transform.h"


namespace Dystopia
{
	namespace HowToPlay_MSG
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
	HowToPlay::HowToPlay()
	:isPress{false}
	{
	}

	HowToPlay::~HowToPlay()
	{
	}

	void HowToPlay::Load()
	{
	}

	void HowToPlay::Awake()
	{
		if(EngineCore::Get<InputManager>()->IsController())
		{
			EngineCore::Get<InputManager>()->MapButton("Back", eButton::XBUTTON_A);
		}
		else
		{
			EngineCore::Get<InputManager>()->MapButton("Back", eButton::KEYBOARD_ESCAPE);
		}
	}

	void HowToPlay::Init()
	{
		if(EngineCore::Get<InputManager>()->IsController())
		{
			EngineCore::Get<InputManager>()->MapButton("Back", eButton::XBUTTON_A);
		}
		else
		{
			EngineCore::Get<InputManager>()->MapButton("Back", eButton::KEYBOARD_ESCAPE);
		}

	}

	void HowToPlay::Update(const float)
	{
		if(isPress)
		{
			if(auto ptr = EngineCore::Get<InputManager>()->IsKeyPressed("Back"))
			{
				auto pPausemenu = EngineCore::Get<SceneSystem>()->FindGameObject_cstr("PauseMenu");
				isPress = false;
				for(auto elemp : GetOwner()->GetComponent<Transform>()->GetAllChild())
				{
					elemp->GetOwner()->SetActive(false);
				}
				if(pPausemenu)
				{
					HowToPlay_MSG::SendExternalMessage(pPausemenu, "Show");
				}
			}
		}
	}

	void HowToPlay::FixedUpdate(const float)
	{
	}
	
	void HowToPlay::PostUpdate(void)
	{
	}

	void HowToPlay::GameObjectDestroy(void)
	{
	}

	void HowToPlay::Unload(void)
	{
	}

	void HowToPlay::ButtonPress(void)
	{
		for(auto ptr : GetOwner()->GetComponent<Transform>()->GetAllChild())
		{
			DEBUG_PRINT(eLog::MESSAGE, "Setting Child True");
			ptr->GetOwner()->SetActive(true);
		}
		isPress = true;
	}
	void Dystopia::HowToPlay::OnCollisionEnter(const CollisionEvent& )
	{

	}

	void Dystopia::HowToPlay::OnCollisionStay(const CollisionEvent& )
	{

	}

	void Dystopia::HowToPlay::OnCollisionExit(const CollisionEvent& )
	{

	}

	void Dystopia::HowToPlay::OnTriggerEnter(GameObject * const )
	{
	}

	void Dystopia::HowToPlay::OnTriggerStay(GameObject * const )
	{
	}

	void Dystopia::HowToPlay::OnTriggerExit(GameObject * const )
	{
	}

	HowToPlay * HowToPlay::Duplicate() const
	{
		return new HowToPlay{*this};
	}

	void HowToPlay::Serialise(TextSerialiser& ) const
	{
	}

	void HowToPlay::Unserialise(TextSerialiser& )
	{
	}


	const char * const HowToPlay::GetBehaviourName() const
	{
		return HowToPlay::BehaviourName;
	}

	void HowToPlay::EditorUI(void) noexcept
	{
		
		
	}
	
	TypeErasure::TypeEraseMetaData HowToPlay::GetMetaData()
	{
		static MetaData<Dystopia::HowToPlay> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const HowToPlay::GetMetaData() const
	{
		static MetaData<Dystopia::HowToPlay> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
}




