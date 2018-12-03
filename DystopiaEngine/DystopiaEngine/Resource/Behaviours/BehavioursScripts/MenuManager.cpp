/* HEADER *********************************************************************************/
/*!
\file	MenuManager.cpp
\author Dan Kang (100%)
\par    email: dan.kang\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "MenuManager.h"
#include "System/Input/InputSystem.h"
#include "System/Input/InputMap.h"
#include "System/Driver/Driver.h"
#include "System/Behaviour/BehaviourSystem.h"

#include "System/Scene/SceneSystem.h"
#include "System/Collision/CollisionEvent.h"
#include "System/Collision/CollisionSystem.h"

#include "Object/ObjectFlags.h"
#include "Object/GameObject.h"
#include "Component/Transform.h"

#include "Editor/EGUI.h"
#include "Utility/DebugAssert.h"



namespace Dystopia
{
	namespace MenuManager_MSG
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
	MenuManager::MenuManager()
		: mpInputSys(nullptr)
		, yOffset(0.0f)
		, currentPos(0)
		, isSelected(false)
	{
	}

	MenuManager::~MenuManager()
	{
	}

	void MenuManager::Load()
	{
	}

	void MenuManager::Awake()
	{
		mpInputSys = EngineCore::GetInstance()->GetSystem<InputManager>();

		if (mpInputSys->IsController())
		{
			mpInputSys->MapButton("Move Down", eButton::XBUTTON_DPAD_DOWN);
			mpInputSys->MapButton("Move Up", eButton::XBUTTON_DPAD_UP);
			mpInputSys->MapButton("Select Button", eButton::XBUTTON_A);
			mpInputSys->MapButton("Back Button", eButton::XBUTTON_B);
		}

		else
		{
			mpInputSys->MapButton("Move Down", eButton::KEYBOARD_DOWN);
			mpInputSys->MapButton("Move Up", eButton::KEYBOARD_UP);
			mpInputSys->MapButton("Select Button", eButton::KEYBOARD_ENTER);
			mpInputSys->MapButton("Back Button", eButton::KEYBOARD_ESCAPE);
		}

		theIndicator = EngineCore::GetInstance()->Get<SceneSystem>()->FindGameObject_cstr("Indicator");
		controllerMenu = EngineCore::GetInstance()->Get<SceneSystem>()->FindGameObject_cstr("CPopup");
		settingMenu = EngineCore::GetInstance()->Get<SceneSystem>()->FindGameObject_cstr("SPopup");
		creditMenu = EngineCore::GetInstance()->Get<SceneSystem>()->FindGameObject_cstr("CRPopup");
	}

	void MenuManager::Init()
	{
		currentPos = 0;
		theIndicator = EngineCore::GetInstance()->Get<SceneSystem>()->FindGameObject_cstr("Indicator");
		controllerMenu = EngineCore::GetInstance()->Get<SceneSystem>()->FindGameObject_cstr("CPopup");
		settingMenu = EngineCore::GetInstance()->Get<SceneSystem>()->FindGameObject_cstr("SPopup");
		creditMenu = EngineCore::GetInstance()->Get<SceneSystem>()->FindGameObject_cstr("CRPopup");
	}

	void MenuManager::Update(const float _fDeltaTime)
	{
		UpdateSelection();
	}

	void MenuManager::FixedUpdate(const float _fDeltaTime)
	{
	}
	
	void MenuManager::PostUpdate(void)
	{
	}

	void MenuManager::Unload(void)
	{
	}

	void Dystopia::MenuManager::OnCollisionEnter(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::MenuManager::OnCollisionStay(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::MenuManager::OnCollisionExit(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::MenuManager::OnTriggerEnter(GameObject * const _obj)
	{
	}

	void Dystopia::MenuManager::OnTriggerStay(GameObject * const _obj)
	{
	}

	void Dystopia::MenuManager::OnTriggerExit(GameObject * const _obj)
	{
	}

	MenuManager * MenuManager::Duplicate() const
	{
		return new MenuManager{*this};
	}

	void MenuManager::Serialise(TextSerialiser& _ser) const
	{
	}

	void MenuManager::Unserialise(TextSerialiser& _ser)
	{
	}


	const char * const MenuManager::GetBehaviourName() const
	{
		return MenuManager::BehaviourName;
	}

	void MenuManager::EditorUI(void) noexcept
	{
		
		           
	}

	void MenuManager::UpdateSelection()
	{
		//move down
		if (mpInputSys->IsKeyTriggered("Move Down"))
		{
			if (currentPos < 4 && !isSelected)
			{
				currentPos++;
				theIndicator->GetComponent<Transform>()->SetGlobalPosition(theIndicator->GetComponent<Transform>()->GetGlobalPosition() - Math::Vec3D{ 0, yOffset, 0});
			}
		}

		//move up
		if (mpInputSys->IsKeyTriggered("Move Up"))
		{
			if (currentPos > 0 && !isSelected)
			{
				currentPos--;
				theIndicator->GetComponent<Transform>()->SetGlobalPosition(theIndicator->GetComponent<Transform>()->GetGlobalPosition() + Math::Vec3D{ 0, yOffset, 0});
			}
		}

		if (mpInputSys->IsKeyTriggered("Select Button"))
		{	
			if (!isSelected)
			{
				if (currentPos == 0)
				{
					//load scene into main game
					DEBUG_PRINT(eLog::MESSAGE, "Selected Start!");
					EngineCore::GetInstance()->GetSystem<SceneSystem>()->LoadScene("Forest.dscene");
				}


				else if (currentPos == 1)
				{
					//bring up settings menu
					DEBUG_PRINT(eLog::MESSAGE, "Selected Settings!");
					settingMenu->GetComponent<Transform>()->SetGlobalPosition(settingMenu->GetComponent<Transform>()->GetGlobalPosition() - Math::Vec3D{ 0, 130, 0});
				}
				
				else if (currentPos == 2)
				{	//bring up controller sheet
					DEBUG_PRINT(eLog::MESSAGE, "Selected Controls!");
					controllerMenu->GetComponent<Transform>()->SetGlobalPosition(controllerMenu->GetComponent<Transform>()->GetGlobalPosition() + Math::Vec3D{ 0, 130, 0});
				}

				else if (currentPos == 3)
				{
					//bring up credits board
					DEBUG_PRINT(eLog::MESSAGE, "Selected Credits!");
					creditMenu->GetComponent<Transform>()->SetGlobalPosition(creditMenu->GetComponent<Transform>()->GetGlobalPosition() - Math::Vec3D{ 250, 0, 0});
				}
					
				else if (currentPos == 4)
					//quit the game
					DEBUG_PRINT(eLog::MESSAGE, "Selected Quit!");

				isSelected = true;
			}
		}

		if (mpInputSys->IsKeyTriggered("Back Button"))
		{
			if (isSelected)
			{
				if (currentPos == 1)
				{
					//bring up settings menu
					DEBUG_PRINT(eLog::MESSAGE, "Returned from Settings!");
					settingMenu->GetComponent<Transform>()->SetGlobalPosition(settingMenu->GetComponent<Transform>()->GetGlobalPosition() + Math::Vec3D{ 0, 130, 0});
				}
			
				else if (currentPos == 2)
				{
					//bring up controller sheet
					DEBUG_PRINT(eLog::MESSAGE, "Returned from Controls!");
					controllerMenu->GetComponent<Transform>()->SetGlobalPosition(controllerMenu->GetComponent<Transform>()->GetGlobalPosition() - Math::Vec3D{ 0, 130, 0});
				}

				else if (currentPos == 3)
				{
					//bring up credits board
					DEBUG_PRINT(eLog::MESSAGE, "Returned from Credits!");
					creditMenu->GetComponent<Transform>()->SetGlobalPosition(creditMenu->GetComponent<Transform>()->GetGlobalPosition() + Math::Vec3D{ 250, 0, 0});
				}
				
				else if (currentPos == 4)
				//quit the game
				DEBUG_PRINT(eLog::MESSAGE, "Returned from Quit!");

				isSelected = false;
			}
		}
	}
	
	
	TypeErasure::TypeEraseMetaData MenuManager::GetMetaData()
	{
		static MetaData<Dystopia::MenuManager> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const MenuManager::GetMetaData() const
	{
		static MetaData<Dystopia::MenuManager> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
}




