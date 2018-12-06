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
#include "System/Graphics/VertexDefs.h"

#include "Object/ObjectFlags.h"
#include "Object/GameObject.h"
#include "Component/Transform.h"

#include "Math/MathUtility.h"

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
		currentPos = 0;
		
		mpInputSys = EngineCore::GetInstance()->GetSystem<InputManager>();

		if (mpInputSys->IsController())
		{
			mpInputSys->MapButton("Move Down", eButton::XBUTTON_DPAD_DOWN);
			mpInputSys->MapButton("Move Up", eButton::XBUTTON_DPAD_UP);
			mpInputSys->MapButton("Move Left", eButton::XBUTTON_DPAD_LEFT);
			mpInputSys->MapButton("Move Right", eButton::XBUTTON_DPAD_RIGHT);
			mpInputSys->MapButton("Select Button", eButton::XBUTTON_A);
			mpInputSys->MapButton("Back Button", eButton::XBUTTON_B);
		}

		else
		{
			mpInputSys->MapButton("Move Down", eButton::KEYBOARD_DOWN);
			mpInputSys->MapButton("Move Up", eButton::KEYBOARD_UP);
			mpInputSys->MapButton("Move Left", eButton::KEYBOARD_LEFT);
			mpInputSys->MapButton("Move Right", eButton::KEYBOARD_RIGHT);
			mpInputSys->MapButton("Select Button", eButton::KEYBOARD_ENTER);
			mpInputSys->MapButton("Back Button", eButton::KEYBOARD_ESCAPE);
		}

		theIndicator = EngineCore::GetInstance()->Get<SceneSystem>()->FindGameObject_cstr("Indicator");
		controllerMenu = EngineCore::GetInstance()->Get<SceneSystem>()->FindGameObject_cstr("CPopup");
		settingMenu = EngineCore::GetInstance()->Get<SceneSystem>()->FindGameObject_cstr("SPopup");
		creditMenu = EngineCore::GetInstance()->Get<SceneSystem>()->FindGameObject_cstr("CRPopup");
		splashMenu = EngineCore::GetInstance()->Get<SceneSystem>()->FindGameObject_cstr("SplashPanel");
		mainMenu = EngineCore::GetInstance()->Get<SceneSystem>()->FindGameObject_cstr("MenuGroup");

		GameObject *obj = nullptr;
		obj = EngineCore::Get<SceneSystem>()->FindGameObject_cstr("StartButton");
		if (obj)
			PlayButton = obj->GetComponent<TextRenderer>();
		obj = EngineCore::Get<SceneSystem>()->FindGameObject_cstr("SettingsButton");
		if (obj)
			SettingsBtn = obj->GetComponent<TextRenderer>();
		obj = EngineCore::Get<SceneSystem>()->FindGameObject_cstr("ControlsButton");
		if (obj)
			ControlsBtn = obj->GetComponent<TextRenderer>();
		obj = EngineCore::Get<SceneSystem>()->FindGameObject_cstr("CreditsButton");
		if (obj)
			CreditsBtn = obj->GetComponent<TextRenderer>();
		obj = EngineCore::Get<SceneSystem>()->FindGameObject_cstr("QuitButton");
		if (obj)
			QuitBtn = obj->GetComponent<TextRenderer>();
	}

	void MenuManager::Init()
	{
		mpInputSys = EngineCore::GetInstance()->GetSystem<InputManager>();

		if (mpInputSys->IsController())
		{
			mpInputSys->MapButton("Move Down", eButton::XBUTTON_DPAD_DOWN);
			mpInputSys->MapButton("Move Up", eButton::XBUTTON_DPAD_UP);
			mpInputSys->MapButton("Move Left", eButton::XBUTTON_DPAD_LEFT);
			mpInputSys->MapButton("Move Right", eButton::XBUTTON_DPAD_RIGHT);
			mpInputSys->MapButton("Select Button", eButton::XBUTTON_A);
			mpInputSys->MapButton("Back Button", eButton::XBUTTON_B);
		}

		else
		{
			mpInputSys->MapButton("Move Down", eButton::KEYBOARD_DOWN);
			mpInputSys->MapButton("Move Up", eButton::KEYBOARD_UP);
			mpInputSys->MapButton("Move Left", eButton::KEYBOARD_LEFT);
			mpInputSys->MapButton("Move Right", eButton::KEYBOARD_RIGHT);
			mpInputSys->MapButton("Select Button", eButton::KEYBOARD_ENTER);
			mpInputSys->MapButton("Back Button", eButton::KEYBOARD_ESCAPE);
		}
		
		theIndicator = EngineCore::GetInstance()->Get<SceneSystem>()->FindGameObject_cstr("Indicator");
		controllerMenu = EngineCore::GetInstance()->Get<SceneSystem>()->FindGameObject_cstr("CPopup");
		settingMenu = EngineCore::GetInstance()->Get<SceneSystem>()->FindGameObject_cstr("SPopup");
		creditMenu = EngineCore::GetInstance()->Get<SceneSystem>()->FindGameObject_cstr("CRPopup");
		splashMenu = EngineCore::GetInstance()->Get<SceneSystem>()->FindGameObject_cstr("SplashPanel");
		mainMenu = EngineCore::GetInstance()->Get<SceneSystem>()->FindGameObject_cstr("MenuGroup");

		GameObject *obj = nullptr;
		obj = EngineCore::Get<SceneSystem>()->FindGameObject_cstr("StartButton");
		if (obj)
			PlayButton = obj->GetComponent<TextRenderer>();
		obj = EngineCore::Get<SceneSystem>()->FindGameObject_cstr("SettingsButton");
		if (obj)
			SettingsBtn = obj->GetComponent<TextRenderer>();
		obj = EngineCore::Get<SceneSystem>()->FindGameObject_cstr("ControlsButton");
		if (obj)
			ControlsBtn = obj->GetComponent<TextRenderer>();
		obj = EngineCore::Get<SceneSystem>()->FindGameObject_cstr("CreditsButton");
		if (obj)
			CreditsBtn = obj->GetComponent<TextRenderer>();
		obj = EngineCore::Get<SceneSystem>()->FindGameObject_cstr("QuitButton");
		if (obj)
			QuitBtn = obj->GetComponent<TextRenderer>();

		obj = EngineCore::Get<SceneSystem>()->FindGameObject_cstr("fullscreenText");
		if (obj)
			fullBtn   = obj->GetComponent<TextRenderer>();

		obj = EngineCore::Get<SceneSystem>()->FindGameObject_cstr("vsyncText");
		if (obj)
			vsyncBtn  = obj->GetComponent<TextRenderer>();

		obj = EngineCore::Get<SceneSystem>()->FindGameObject_cstr("bgmText");
		if (obj)
			bgmBtn  = obj->GetComponent<TextRenderer>();

		obj = EngineCore::Get<SceneSystem>()->FindGameObject_cstr("sfxText");
		if (obj)
			sfxBtn  = obj->GetComponent<TextRenderer>();

		obj = EngineCore::Get<SceneSystem>()->FindGameObject_cstr("gammaText");
		if (obj)
			gammaBtn  = obj->GetComponent<TextRenderer>();


		fullTick = EngineCore::GetInstance()->Get<SceneSystem>()->FindGameObject_cstr("fullscreenTick");
		vsyncTick = EngineCore::GetInstance()->Get<SceneSystem>()->FindGameObject_cstr("vsyncTick");
		bgmTick = EngineCore::GetInstance()->Get<SceneSystem>()->FindGameObject_cstr("bgmTick");
		sfxTick = EngineCore::GetInstance()->Get<SceneSystem>()->FindGameObject_cstr("sfxTick");
			
	}

	void MenuManager::Update(const float _dt)
	{
		UpdatePanel(_dt);
		UpdateSelection();
	}

	void MenuManager::FixedUpdate(const float )
	{
	}
	
	void MenuManager::PostUpdate(void)
	{
	}

	void MenuManager::Unload(void)
	{
	}

	void Dystopia::MenuManager::OnCollisionEnter(const CollisionEvent& )
	{

	}

	void Dystopia::MenuManager::OnCollisionStay(const CollisionEvent& )
	{

	}

	void Dystopia::MenuManager::OnCollisionExit(const CollisionEvent& )
	{

	}

	void Dystopia::MenuManager::OnTriggerEnter(GameObject * const )
	{
	}

	void Dystopia::MenuManager::OnTriggerStay(GameObject * const )
	{
	}

	void Dystopia::MenuManager::OnTriggerExit(GameObject * const )
	{
	}

	MenuManager * MenuManager::Duplicate() const
	{
		return new MenuManager{*this};
	}

	void MenuManager::Serialise(TextSerialiser& ) const
	{
	}

	void MenuManager::Unserialise(TextSerialiser& )
	{
	}


	const char * const MenuManager::GetBehaviourName() const
	{
		return MenuManager::BehaviourName;
	}

	void MenuManager::EditorUI(void) noexcept
	{
		
		           
	}

	void MenuManager::UpdatePanel(float _dt) 
	{
		DEBUG_PRINT(eLog::MESSAGE, "Current Menu: %d", currentMenu);
		DEBUG_PRINT(eLog::MESSAGE, "Prev Menu: %d", prevMenu);
		DEBUG_PRINT(eLog::MESSAGE, "Next Menu: %d", nextMenu);
		
		if (currentMenu != nextMenu)
		{
			currentPos = 0;
			isSelected = false;
			prevMenu = currentMenu;

			switch (currentMenu)
			{
				case -1: // splash
				{
					if (nextMenu == 0)
					{					
						isAnimating = true;

						float alpha = splashMenu->GetComponent<Transform>()->GetOpacity();

						if (alpha > 0.f)
							splashMenu->GetComponent<Transform>()->SetOpacity(alpha - animateSpeed * _dt);
						else splashMenu->GetComponent<Transform>()->SetOpacity(0.f);

						float alpha2 = mainMenu->GetComponent<Transform>()->GetOpacity();

						if (alpha2 < 1.0)
							mainMenu->GetComponent<Transform>()->SetOpacity(alpha2 + animateSpeed * _dt);
						else 
						{
							isAnimating = false;
							mainMenu->GetComponent<Transform>()->SetOpacity(1.f);		
							splashMenu->SetActive(false);
						}
					}
				}
				break;
				case 0: /// main 
				{
					switch (nextMenu)
					{
						case -1:
						{
							splashMenu->SetActive(true);
							isAnimating = true;
							float alpha = mainMenu->GetComponent<Transform>()->GetOpacity();

							if (alpha > 0.f)
								mainMenu->GetComponent<Transform>()->SetOpacity(alpha - animateSpeed * _dt);
							else mainMenu->GetComponent<Transform>()->SetOpacity(0.f);

							float alpha2 = splashMenu->GetComponent<Transform>()->GetOpacity();

							if (alpha2 < 1.0)
								splashMenu->GetComponent<Transform>()->SetOpacity(alpha2 + animateSpeed * _dt);
							else
							{
								isAnimating = false;
								splashMenu->GetComponent<Transform>()->SetOpacity(1.f);		
							} 	
						}
						break;
						case 1: // settings
						{
							isAnimating = true;

							if (settingMenu->GetComponent<Transform>()->GetGlobalPosition().x < 0.0f )
							{
								Math::Vec4 currPos = settingMenu->GetComponent<Transform>()->GetGlobalPosition();
								Math::Vec4 newPos{2,currPos.x,currPos.z,currPos.w};
								
								currPos.x = Math::Lerp<float>(currPos.x, newPos.x, animateSpeed * _dt);
								
								settingMenu->GetComponent<Transform>()->SetGlobalPosition(currPos);
							}

							if (mainMenu->GetComponent<Transform>()->GetGlobalPosition().x < 138.f)
							{
								Math::Vec4 currPos = mainMenu->GetComponent<Transform>()->GetGlobalPosition();
								Math::Vec4 newPos{140.f,currPos.x,currPos.z,currPos.w};
								
								currPos.x = Math::Lerp<float>(currPos.x, newPos.x, animateSpeed * _dt);
								
								mainMenu->GetComponent<Transform>()->SetGlobalPosition(currPos);
							}
							else
							{
								isAnimating = false;
							}

						}
						break;
						case 2: // controls
						{
							isAnimating = true;

							if (controllerMenu->GetComponent<Transform>()->GetGlobalPosition().x < 0.0f )
							{
								Math::Vec4 currPos = controllerMenu->GetComponent<Transform>()->GetGlobalPosition();
								Math::Vec4 newPos{2,currPos.x,currPos.z,currPos.w};
								
								currPos.x = Math::Lerp<float>(currPos.x, newPos.x, animateSpeed * _dt);
								
								
								controllerMenu->GetComponent<Transform>()->SetGlobalPosition(currPos);
							}

							if (mainMenu->GetComponent<Transform>()->GetGlobalPosition().x < 138.f)
							{
								Math::Vec4 currPos = mainMenu->GetComponent<Transform>()->GetGlobalPosition();
								Math::Vec4 newPos{140.f,currPos.x,currPos.z,currPos.w};
								
								currPos.x = Math::Lerp<float>(currPos.x, newPos.x, animateSpeed * _dt);
								
								
								mainMenu->GetComponent<Transform>()->SetGlobalPosition(currPos);
							}
							else
							{
								isAnimating = false;
							}
						}
						break;
						case 3: // credits
						{
							isAnimating = true;

							if (creditMenu->GetComponent<Transform>()->GetGlobalPosition().x < 0.0f )
							{
								Math::Vec4 currPos = creditMenu->GetComponent<Transform>()->GetGlobalPosition();
								Math::Vec4 newPos{2,currPos.x,currPos.z,currPos.w};
								
								currPos.x = Math::Lerp<float>(currPos.x, newPos.x, animateSpeed * _dt);
								
								
								creditMenu->GetComponent<Transform>()->SetGlobalPosition(currPos);
							}

							if (mainMenu->GetComponent<Transform>()->GetGlobalPosition().x < 138.f)
							{
								Math::Vec4 currPos = mainMenu->GetComponent<Transform>()->GetGlobalPosition();
								Math::Vec4 newPos{140.f,currPos.x,currPos.z,currPos.w};
								
								currPos.x = Math::Lerp<float>(currPos.x, newPos.x, animateSpeed * _dt);
								
								
								mainMenu->GetComponent<Transform>()->SetGlobalPosition(currPos);
							}
							else
							{
								isAnimating = false;
							}
						}
						break;
					}
				}
				break;
				case 1: // settings
				{
					if (nextMenu == 0)
					{
						isAnimating = true;

						if (settingMenu->GetComponent<Transform>()->GetGlobalPosition().x > -190.0f )
						{
							Math::Vec4 currPos = settingMenu->GetComponent<Transform>()->GetGlobalPosition();
							Math::Vec4 newPos{-200.f,currPos.x,currPos.z,currPos.w};
							
							currPos.x = Math::Lerp<float>(currPos.x, newPos.x, animateSpeed * _dt);
							
							
							settingMenu->GetComponent<Transform>()->SetGlobalPosition(currPos);
						}

						if (mainMenu->GetComponent<Transform>()->GetGlobalPosition().x > -50.f)
						{
							Math::Vec4 currPos = mainMenu->GetComponent<Transform>()->GetGlobalPosition();
							Math::Vec4 newPos{-55.f,currPos.x,currPos.z,currPos.w};
							
							currPos.x = Math::Lerp<float>(currPos.x, newPos.x, animateSpeed * _dt);
							
							
							mainMenu->GetComponent<Transform>()->SetGlobalPosition(currPos);
						}
						else
						{
							isAnimating = false;
						}

					}
				}
				break;
				case 2: // controls
				{
					if (nextMenu == 0)
					{
						isAnimating = true;

						if (controllerMenu->GetComponent<Transform>()->GetGlobalPosition().x > -190.0f )
						{
							Math::Vec4 currPos = controllerMenu->GetComponent<Transform>()->GetGlobalPosition();
							Math::Vec4 newPos{-200.f,currPos.x,currPos.z,currPos.w};
							
							currPos.x = Math::Lerp<float>(currPos.x, newPos.x, animateSpeed * _dt);
							
							
							controllerMenu->GetComponent<Transform>()->SetGlobalPosition(currPos);
						}
						else
						{
							isAnimating = false;
						}

						if (mainMenu->GetComponent<Transform>()->GetGlobalPosition().x > -50.f)
						{
							Math::Vec4 currPos = mainMenu->GetComponent<Transform>()->GetGlobalPosition();
							Math::Vec4 newPos{-55.f,currPos.x,currPos.z,currPos.w};
							
							currPos.x = Math::Lerp<float>(currPos.x, newPos.x, animateSpeed * _dt);
							
							
							mainMenu->GetComponent<Transform>()->SetGlobalPosition(currPos);
						}
						else
						{
							isAnimating = false;
						}
					}
					
				}
				break;
				case 3: // credits
				{
					if (nextMenu == 0)
					{
						isAnimating = true;

						if (creditMenu->GetComponent<Transform>()->GetGlobalPosition().x > -190.0f )
						{
							Math::Vec4 currPos = creditMenu->GetComponent<Transform>()->GetGlobalPosition();
							Math::Vec4 newPos{-200.f,currPos.x,currPos.z,currPos.w};
							
							currPos.x = Math::Lerp<float>(currPos.x, newPos.x, animateSpeed * _dt);
							
							
							creditMenu->GetComponent<Transform>()->SetGlobalPosition(currPos);
						}
						else
						{
							isAnimating = false;
						}

						if (mainMenu->GetComponent<Transform>()->GetGlobalPosition().x > -50.f)
						{
							Math::Vec4 currPos = mainMenu->GetComponent<Transform>()->GetGlobalPosition();
							Math::Vec4 newPos{-55.f,currPos.x,currPos.z,currPos.w};
							
							currPos.x = Math::Lerp<float>(currPos.x, newPos.x, animateSpeed * _dt);
							
							
							mainMenu->GetComponent<Transform>()->SetGlobalPosition(currPos);
						}
						else
						{
							isAnimating = false;
						}
					}
				}
				break;
			}

			if (!isAnimating)
				currentMenu = nextMenu;
		}
	}

	void MenuManager::UpdateSelection()
	{
		

		if (currentMenu == -1) //splash
		{
			if (mpInputSys->IsKeyTriggered("Select Button"))
			{
				nextMenu = 0;
			}	
		}

		else if (currentMenu == 0)
		{
			if (PlayButton)
			PlayButton->ChangeState(TextEditor::eTextState::DEFAULT);
			if (SettingsBtn)
				SettingsBtn->ChangeState(TextEditor::eTextState::DEFAULT);
			if (ControlsBtn)
				ControlsBtn->ChangeState(TextEditor::eTextState::DEFAULT);
			if (CreditsBtn)
				CreditsBtn->ChangeState(TextEditor::eTextState::DEFAULT);
			if (QuitBtn)
				QuitBtn->ChangeState(TextEditor::eTextState::DEFAULT);


			//move down
			if (mpInputSys->IsKeyTriggered("Move Down"))
			{
				if (currentPos < 4 && !isSelected)
				{
					++currentPos;
					theIndicator->GetComponent<Transform>()->SetGlobalPosition(theIndicator->GetComponent<Transform>()->GetGlobalPosition() - Math::Vec3D{ 0, yOffset, 0});
				}
			}

			//move up
			if (mpInputSys->IsKeyTriggered("Move Up"))
			{
				if (currentPos > 0 && !isSelected)
				{
					--currentPos;
					theIndicator->GetComponent<Transform>()->SetGlobalPosition(theIndicator->GetComponent<Transform>()->GetGlobalPosition() + Math::Vec3D{ 0, yOffset, 0});
				}
			}

			switch (currentPos)
			{
				case 0:
				if (PlayButton)
					PlayButton->ChangeState(TextEditor::eTextState::ON_HOVER);
				break;
				case 1:
				if (SettingsBtn)
					SettingsBtn->ChangeState(TextEditor::eTextState::ON_HOVER);
				break;
				case 2:
				if (ControlsBtn)
					ControlsBtn->ChangeState(TextEditor::eTextState::ON_HOVER);
				break;
				case 3:
				if (CreditsBtn)
					CreditsBtn->ChangeState(TextEditor::eTextState::ON_HOVER);
				break;
				case 4:
				if (QuitBtn)
					QuitBtn->ChangeState(TextEditor::eTextState::ON_HOVER);
				break;
				default:
				break;
			}

			if (mpInputSys->IsKeyTriggered("Select Button"))
			{
				if (!isSelected)
				{
					if (currentPos == 0)
					{
						//load scene into main game
						DEBUG_PRINT(eLog::MESSAGE, "Selected Start!");
						if (auto o = EngineCore::Get<SceneSystem>()->FindGameObject_cstr("ChangeToForest"))
						{
							MenuManager_MSG::SendExternalMessage(o, "TransitToLevel"); 
						}
					}

					else if (currentPos == 1)
					{
						//bring up settings menu
						DEBUG_PRINT(eLog::MESSAGE, "Selected Settings!");
						//settingMenu->GetComponent<Transform>()->SetGlobalPosition(settingMenu->GetComponent<Transform>()->GetGlobalPosition() - Math::Vec3D{ 0, 130, 0});
						nextMenu = 1;
					}
					
					else if (currentPos == 2)
					{	//bring up controller sheet
						DEBUG_PRINT(eLog::MESSAGE, "Selected Controls!");
						//controllerMenu->GetComponent<Transform>()->SetGlobalPosition(controllerMenu->GetComponent<Transform>()->GetGlobalPosition() + Math::Vec3D{ 0, 130, 0});
						nextMenu = 2;
					}

					else if (currentPos == 3)
					{
						//bring up credits board
						DEBUG_PRINT(eLog::MESSAGE, "Selected Credits!");
						nextMenu = 3;
						//creditMenu->GetComponent<Transform>()->SetGlobalPosition(creditMenu->GetComponent<Transform>()->GetGlobalPosition() - Math::Vec3D{ 250, 0, 0});
					}
						
					else if (currentPos == 4)
					{
						//quit the game
						DEBUG_PRINT(eLog::MESSAGE, "Selected Quit!");
						EngineCore::GetInstance()->Quit();
					}
					isSelected = true;
				}
			}	

			

			if (mpInputSys->IsKeyTriggered("Back Button") && currentMenu == 0 && nextMenu != 1 && nextMenu != 2 && nextMenu != 3)
			{
				isSelected = false;		
				nextMenu = -1;	
			}
		}

		// Settings
		else if (currentMenu == 1)
		{
			if (fullBtn)
				fullBtn->ChangeState(TextEditor::eTextState::DEFAULT);
			if (vsyncBtn)
				vsyncBtn->ChangeState(TextEditor::eTextState::DEFAULT);
			if (bgmBtn)
				bgmBtn->ChangeState(TextEditor::eTextState::DEFAULT);
			if (sfxBtn)
				sfxBtn->ChangeState(TextEditor::eTextState::DEFAULT);
			if (gammaBtn)
				gammaBtn->ChangeState(TextEditor::eTextState::DEFAULT);


			//move down
			if (mpInputSys->IsKeyTriggered("Move Down"))
			{
				if (currentPos < 4 && !isSelected)
				{
					++currentPos;
				}
			}

			//move up
			if (mpInputSys->IsKeyTriggered("Move Up"))
			{
				if (currentPos > 0 && !isSelected)
				{
					--currentPos;
				}
			}

			switch (currentPos)
			{
				case 0:
				if (fullBtn)
					fullBtn->ChangeState(TextEditor::eTextState::ON_HOVER);
				break;
				case 1:
				if (vsyncBtn)
					vsyncBtn->ChangeState(TextEditor::eTextState::ON_HOVER);
				break;
				case 2:
				if (bgmBtn)
					bgmBtn->ChangeState(TextEditor::eTextState::ON_HOVER);
				break;
				case 3:
				if (sfxBtn)
					sfxBtn->ChangeState(TextEditor::eTextState::ON_HOVER);
				break;
				case 4:
				if (gammaBtn)
					gammaBtn->ChangeState(TextEditor::eTextState::ON_HOVER);
				break;
				default:
				break;
			}

			if (mpInputSys->IsKeyTriggered("Select Button"))
			{

				if (currentPos == 0)
				{
					auto window = EngineCore::GetInstance()->GetSystem<WindowManager>();
					//load scene into main game
					DEBUG_PRINT(eLog::MESSAGE, "Selected FullScreen!");
					if (isFullScreen)
					{ 
						isFullScreen = false;
					}
					else
					{
						isFullScreen = true;
					}
					window->ToggleFullscreen(isFullScreen);
					fullTick->SetActive(isFullScreen);
				}

				else if (currentPos == 1)
				{
					//bring up settings menu
					DEBUG_PRINT(eLog::MESSAGE, "Selected Vsync");
					if (isVsync)
					{
						isVsync = false;
					}
					else
					{
						isVsync = true;
					}
					EngineCore::GetInstance()->GetSystem<GraphicsSystem>()->ToggleVsync(isVsync);
					vsyncTick->SetActive(isVsync);
				}
				
				else if (currentPos == 2)
				{	//bring up controller sheet
					DEBUG_PRINT(eLog::MESSAGE, "Selected Bgm");
					if (hasBgm)
					{
						hasBgm = false;
					}
					else
					{
						hasBgm = true;
					}
					EngineCore::GetInstance()->GetSystem<SoundSystem>()->SetBGM(static_cast<float>(hasBgm));
					bgmTick->SetActive(hasBgm);
				}

				else if (currentPos == 3)
				{
					//bring up credits board
					DEBUG_PRINT(eLog::MESSAGE, "Selected Sfx");
					if (hasSfx)
					{
						hasSfx = false;
					}
					else
					{
						hasSfx = true;
					}
					EngineCore::GetInstance()->GetSystem<SoundSystem>()->SetFX(static_cast<float>(hasSfx));
					sfxTick->SetActive(hasSfx);
				}
			}	

			float _gamma = 	EngineCore::GetInstance()->GetSystem<GraphicsSystem>()->GetGamma();

			if (mpInputSys->IsKeyPressed("Move Left"))
			{
				if (currentPos == 4 && _gamma > 0)
				{
					_gamma -= 0.05f;
					if (_gamma < 0) _gamma = 0;
				}
				EngineCore::GetInstance()->GetSystem<GraphicsSystem>()->SetGamma(_gamma);
			}

			if (mpInputSys->IsKeyPressed("Move Right")) 
			{
				if (currentPos == 4)
				{
					_gamma += 0.05f;
				}
				EngineCore::GetInstance()->GetSystem<GraphicsSystem>()->SetGamma(_gamma);
			}

			if (mpInputSys->IsKeyTriggered("Back Button"))
			{
				isSelected = false;		
				nextMenu = 0;	
			}
		}
		else
		{
			if (mpInputSys->IsKeyTriggered("Back Button"))
			{
				isSelected = false;		
				nextMenu = 0;	
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




