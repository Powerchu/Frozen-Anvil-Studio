/* HEADER *********************************************************************************/
/*!
\file	PauseMenu.cpp
\author Keith Goh  (100%)
\par    email: keith.goh\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "PauseMenu.h"
#include "System/Input/InputSystem.h"
#include "System/Input/InputMap.h"
#include "System/Driver/Driver.h"
#include "System/Behaviour/BehaviourSystem.h"

#include "System/Scene/SceneSystem.h"
#include "System/Collision/CollisionEvent.h"
#include "System/Collision/CollisionSystem.h"
#include "System/Time/TimeSystem.h"
#include "System/Sound/SoundSystem.h"
#include "System/Graphics/GraphicsSystem.h"

#include "Component/SpriteRenderer.h"

#include "Object/ObjectFlags.h"
#include "Object/GameObject.h"

#include "Editor/EGUI.h" 
#include "Utility/DebugAssert.h"

#define FIND_GAME_OBJECT(_NAME_) EngineCore::Get<SceneSystem>()->FindGameObject_cstr(_NAME_);
#define GET_POSITION(_PTR_) _PTR_ ## ->GetComponent<Transform>()->GetGlobalPosition();
namespace Dystopia
{
	namespace PauseMenu_MSG
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
	PauseMenu::PauseMenu()
	:mMenuObjects
	{
		std::make_pair(P_Play,nullptr),
		std::make_pair(P_Howtoplay,nullptr),
		std::make_pair(P_Quit,nullptr),
		std::make_pair(P_PlayButton,nullptr),
		std::make_pair(P_HowtoplayButton,nullptr),
		std::make_pair(P_QuitButton,nullptr),

	}, fpState{nullptr} , pSelector{nullptr}, index{0}, isPause{false},
	pInputSys{EngineCore::Get<InputManager>()}
	{ 
	}

	PauseMenu::~PauseMenu()
	{
	}

	void PauseMenu::Load()
	{ 
	}

	void PauseMenu::Awake()  
	{
		isPause = false;
		fpState = nullptr;
		/*Find my menu objects*/
		mMenuObjects[P_Play]            = FIND_GAME_OBJECT("ResumeButton");
		mMenuObjects[P_Howtoplay]       = FIND_GAME_OBJECT("Howtoplay");
		mMenuObjects[P_Quit]            = FIND_GAME_OBJECT("QuitBG");
		mMenuObjects[P_Settings]        = FIND_GAME_OBJECT("Settings");
		mMenuObjects[P_PlayButton]      = FIND_GAME_OBJECT("ResumeButton");
		mMenuObjects[P_HowtoplayButton] = FIND_GAME_OBJECT("HowtoplayButton");
		mMenuObjects[P_SettingsButton]  = FIND_GAME_OBJECT("SettingsButton");
		mMenuObjects[P_QuitButton]      = FIND_GAME_OBJECT("QuitButton");
		mMenuObjects[P_PauseMenu]       = FIND_GAME_OBJECT("Menu");
		pSelector                       = FIND_GAME_OBJECT("Selector");
		index = P_Play;
		pSelector && MoveSelector();
		pInputSys = EngineCore::GetInstance()->GetSystem<InputManager>();
	}

	void PauseMenu::Init()
	{
		isPause = false;
		fpState = nullptr;
		/*Find my menu objects*/
		mMenuObjects[P_Play]            = FIND_GAME_OBJECT("ResumeButton");
		mMenuObjects[P_Howtoplay]       = FIND_GAME_OBJECT("Howtoplay");
		mMenuObjects[P_Quit]            = FIND_GAME_OBJECT("QuitBG");
		mMenuObjects[P_Settings]        = FIND_GAME_OBJECT("Settings");
		mMenuObjects[P_PlayButton]      = FIND_GAME_OBJECT("ResumeButton");
		mMenuObjects[P_HowtoplayButton] = FIND_GAME_OBJECT("HowtoplayButton");
		mMenuObjects[P_SettingsButton]  = FIND_GAME_OBJECT("SettingsButton");
		mMenuObjects[P_QuitButton]      = FIND_GAME_OBJECT("QuitButton");
		mMenuObjects[P_PauseMenu]       = FIND_GAME_OBJECT("Menu");
		pSelector                       = FIND_GAME_OBJECT("Selector");
		index = P_Play;
		pSelector && MoveSelector();
		pInputSys = EngineCore::GetInstance()->GetSystem<InputManager>();
	}

	void PauseMenu::Update(const float _dt)
	{
		if(!isPause)
		{
			if(pInputSys->GetButtonDown("Pause"))
			{
				index = P_Play;
				isPause = true;
				EngineCore::Get<TimeSystem>()->SetTimeScale(0.f);
				mMenuObjects[P_PauseMenu]->SetActive(true);
			}
			return;
		}


		/*Some Menu State is in control*/
		if(fpState) 
		{
			/*This will execute the relevant function pointer*/
			(this->*(fpState))(_dt);
		}
		/*Main Menu is in Control*/ 
		else
		{
			static float Axis = 0.f;
			Axis = pInputSys->GetAxis("L Stick Vertical");
			/*Check Key Down*/
			if(pInputSys->GetButtonDown("Vertical", true) || Axis < -0.8f)
			{
				//DEBUG_PRINT(eLog::MESSAGE, "Keypress Down");
				index = static_cast<MenuStates>((static_cast<int>(index) + 1) % static_cast<int>(MenuStates::P_Total));
				(pSelector && mMenuObjects[static_cast<MenuStates>(static_cast<int>(index) + static_cast<int>(MenuStates::P_Total) + 1)]) &&  MoveSelector();
			}
			/*Check Key Up*/
			else if(pInputSys->GetButtonDown("Vertical", false) || Axis < -0.8f)
			{ 
				index = (static_cast<int>(index) - 1) < 0 ? static_cast<MenuStates>(static_cast<int>(MenuStates::P_Total)-1) : static_cast<MenuStates>( static_cast<int>(index) - 1);
				(pSelector && mMenuObjects[static_cast<MenuStates>(static_cast<int>(index) + static_cast<int>(MenuStates::P_Total) + 1)]) && MoveSelector();
			}
			/*Check Key Accept*/
			else if(pInputSys->GetButtonDown("Select")) 
			{
				/*Launch the relevent Init function*/
				fpState    = &PauseMenu::Invoke;
				if(mMenuObjects[index])
					mMenuObjects[index]->SetActive(true);
			}
			else if(pInputSys->GetButtonDown("Back"))
			{      
				fpState = nullptr;
				TogglePause(false);
				isPause = false;
				auto && children = GetOwner()->GetComponent<Transform>()->GetAllChild();
				for(auto & child : children)
					child->GetOwner()->SetActive(false);
				
			}
		}  
	}

	void PauseMenu::FixedUpdate(const float )
	{
	} 
	
	void PauseMenu::PostUpdate(void)
	{
	}

	void PauseMenu::GameObjectDestroy(void)
	{
		TogglePause(false);
	} 

	void PauseMenu::Unload(void)
	{
		TogglePause(false);
	}

	void Dystopia::PauseMenu::OnCollisionEnter(const CollisionEvent& )
	{

	}

	void Dystopia::PauseMenu::OnCollisionStay(const CollisionEvent& )
	{

	}

	void Dystopia::PauseMenu::OnCollisionExit(const CollisionEvent& )
	{

	}

	void Dystopia::PauseMenu::OnTriggerEnter(GameObject * const )
	{
	}

	void Dystopia::PauseMenu::OnTriggerStay(GameObject * const )
	{
	}

	void Dystopia::PauseMenu::OnTriggerExit(GameObject * const )
	{
	}

	PauseMenu * PauseMenu::Duplicate() const
	{
		return new PauseMenu{*this};
	}

	void PauseMenu::Serialise(TextSerialiser& ) const
	{
	}

	void PauseMenu::Unserialise(TextSerialiser& )
	{
	}


	const char * const PauseMenu::GetBehaviourName() const
	{
		return PauseMenu::BehaviourName;
	}

	bool PauseMenu::Terminate()
	{
		index = P_Play;
		pSelector && MoveSelector();
		fpState = nullptr;
		return true;
	}

	void PauseMenu::EditorUI(void) noexcept
	{ 
		
		
	}
	
	TypeErasure::TypeEraseMetaData PauseMenu::GetMetaData()
	{
		static MetaData<Dystopia::PauseMenu> mMetaData; 
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const PauseMenu::GetMetaData() const
	{
		static MetaData<Dystopia::PauseMenu> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}

	bool PauseMenu::MoveSelector()
	{
		auto && tempVec = GET_POSITION(mMenuObjects[static_cast<MenuStates>(static_cast<int>(index) + static_cast<int>(MenuStates::P_Total) + 1)]);
		auto && tempMy  = GET_POSITION(pSelector);
		
		pSelector->GetComponent<Transform>()->SetGlobalPosition(tempMy.x,tempVec.y,tempMy.z);
		return true; 
	} 
    bool PauseMenu::TogglePause(bool _b)
	{
		isPause = _b;
		auto && Children = GetOwner()->GetComponent<Transform>()->GetAllChild();
		for(auto & child : Children)
		{
			child->GetOwner()->SetActive(false);
		}
		EngineCore::Get<TimeSystem>()->SetTimeScale(1.f * !_b);
		return isPause;
	} 

	bool P_ResumeState::Init(float) 
	{
		fpState = &P_ResumeState::Transition;
		return true; 
	}
	bool P_ResumeState::Transition(float)
	{
		fpState = &P_ResumeState::Update;
		return true;
	}
	bool P_ResumeState::Update(float)
	{
		fpState = &P_ResumeState::Exit;
		return true; 
	}
	bool P_ResumeState::Exit(float)
	{ 
		fpState = &P_ResumeState::Init;
		TogglePause(false);
		Terminate();
		EngineCore::Get<TimeSystem>()->SetTimeScale(1.f);
		return true;
	}
	bool P_ResumeState::Invoke(float _dt)
	{
		fpState && (this->*fpState)(_dt);
		return true; 
	}
	bool P_ResumeState::SetUp()
	{
		return true;
	}


	bool P_QuitState::Init(float)
	{
		fpState   = &P_QuitState::Transition;
		pInputSys = EngineCore::GetInstance()->GetSystem<InputManager>();
		pSelector                = FIND_GAME_OBJECT("QuitSelector");
		Background               = FIND_GAME_OBJECT("QuitBG");
		mQuitButtons[eYesButton] = FIND_GAME_OBJECT("QuitYesBtn");
		mQuitButtons[eNoButton]  = FIND_GAME_OBJECT("QuitNoBtn");
		
		if (!pSelector) DEBUG_PRINT(eLog::MESSAGE, "pSelector not found");
		if (!mQuitButtons[eYesButton]) DEBUG_PRINT(eLog::MESSAGE, "Yes Button not found");
		if (!mQuitButtons[eNoButton]) DEBUG_PRINT(eLog::MESSAGE, "No Button not found");
		if (!Background) DEBUG_PRINT(eLog::MESSAGE, "Background not found");
		index = eNoButton;;
		pSelector && MoveSelector();  

		return true;
	}
	bool P_QuitState::Transition(float)
	{
		fpState = &P_QuitState::Update;
		Background->SetActive(true);
		for(auto & elem : mQuitButtons)
		{
			if(elem.second)
				elem.second->SetActive(true);
		}
		return true;
	}
	bool P_QuitState::Update(float)
	{
		static float Axis = 0.f;
		Axis = pInputSys->GetAxis("L Stick Horizontal");
		/*Check Key Down*/
		if(pInputSys->GetButtonDown("Horizontal", false) || Axis >= 0.8f)
		{
			index = static_cast<QuitStateButtons>((static_cast<int>(index) + 1) % static_cast<int>(QuitStateButtons::eQuitTotal));
			(pSelector && mQuitButtons[static_cast<QuitStateButtons>(static_cast<int>(index))]) &&  MoveSelector();
		}
		/*Check Key Up*/
		else if(pInputSys->GetButtonDown("Horizontal", true) || Axis < -0.8f)
		{ 
			index = (static_cast<int>(index) - 1) < 0 ? static_cast<QuitStateButtons>(static_cast<int>(QuitStateButtons::eQuitTotal)-1) : static_cast<QuitStateButtons>( static_cast<int>(index) - 1);
			(pSelector && mQuitButtons[static_cast<QuitStateButtons>(static_cast<int>(index))]) && MoveSelector();
		}
		/*Check Key Accept*/
		else if(pInputSys->GetButtonDown("Select")) 
		{
			if(index == eYesButton)
			{
				fpState = &P_QuitState::Exit; 
			}
			else if(index == eNoButton)
			{
				Background->SetActive(false);
				for(auto & elem : mQuitButtons)
				{
					if(elem.second)
						elem.second->SetActive(false);
				}
				fpState = &P_QuitState::Init; 
				Terminate();
			}
		} 
		else if(pInputSys->GetButtonDown("Back"))
		{    
			
			Background->SetActive(false);
			for(auto & elem : mQuitButtons)
			{
				if(elem.second)
					elem.second->SetActive(false);
			}
			fpState = &P_QuitState::Init; 
			Terminate();
		}
		return true;  
	}
	bool P_QuitState::Exit(float)
	{
		fpState = &P_QuitState::Init;
		TogglePause(false);
		Terminate();
		/*Exit to Main Menu here*/
		EngineCore::Get<SceneSystem>()->LoadScene("MainMenuFinal");
		return true;
	}
	bool P_QuitState::Invoke(float _dt)
	{
		fpState && (this->*fpState)(_dt);
		return true;
	}
	bool P_QuitState::SetUp()
	{
		return true;
	}
	bool P_QuitState::MoveSelector()
	{
		if (!mQuitButtons[index])
			return false;
		auto && tempVec = GET_POSITION(mQuitButtons[index]);
		pSelector->GetComponent<Transform>()->SetGlobalPosition(tempVec.x,tempVec.y,tempVec.z);
		return true; 
	}

	bool P_HowtoplayState::Init(float)
	{
		pInputSys = EngineCore::GetInstance()->GetSystem<InputManager>();
		pObject = FIND_GAME_OBJECT("Howtoplay");
		alpha   = 0.f;
		fpState = &P_HowtoplayState::Transition; 
		return true;
	}
	bool P_HowtoplayState::Transition(float)
	{
		/*This will execute once, you should do the set up for Settings here*/
		/*Fade In*/
		alpha += 0.5f + alpha * 0.5f;
		if(pObject)
		{
			pObject->SetActive(true);
			if(auto && p = pObject->GetComponent<SpriteRenderer>())
			{
				p->SetAlpha(alpha);
			} 
			auto && Childs = pObject->GetComponent<Transform>()->GetAllChild();
			for(auto & child : Childs)
			{
				if(auto && p = child->GetOwner()->GetComponent<SpriteRenderer>())
				{
					p->SetAlpha(alpha);
				} 
			}
		}
		if(alpha >= 1.f)
		{
			/*Set to transition stage*/
			fpState = &P_HowtoplayState::Update;
		}
		return true;
	}
	bool P_HowtoplayState::Update(float)
	{
		if(pInputSys->GetButtonDown("Back"))
		{
			fpState = &P_HowtoplayState::Exit;
		}
		return true;
	}
	bool P_HowtoplayState::Exit(float)
	{
		/*Call TerminateMenuState() when the current menu state is done,
		This will relinquish control back to the main menu*/
		alpha -= 0.5f * 5.f;
		//DEBUG_PRINT(eLog::MESSAGE, "Howtoplay Exit \n");
		if(pObject)
		{
			if(auto && p = pObject->GetComponent<SpriteRenderer>())
			{
				p->SetAlpha(alpha);
			}
			auto && Childs = pObject->GetComponent<Transform>()->GetAllChild();
			for(auto & child : Childs)
			{
				if(auto && p = child->GetOwner()->GetComponent<SpriteRenderer>())
				{
					p->SetAlpha(alpha);
				} 
			}
		}
		if(alpha < 0.1f)
		{
			if(auto && p = pObject->GetComponent<SpriteRenderer>())
			{
				p->SetAlpha(0.f);
			}
			auto && Childs = pObject->GetComponent<Transform>()->GetAllChild();
			for(auto & child : Childs)
			{
				if(auto && p = child->GetOwner()->GetComponent<SpriteRenderer>())
				{
					p->SetAlpha(0.f);
				} 
			}

			/*Terminate Howtoplay Menu*/
			if(pObject)
				pObject->SetActive(false);
			fpState = &P_HowtoplayState::Init;
			Terminate();
			alpha = 0.f;
		}
		return true;
	}
	bool P_HowtoplayState::Invoke(float _dt)
	{
		fpState && (this->*fpState)(_dt);
		return true;
	}

	bool P_HowtoplayState::SetUp()
	{
		return true;
	}
	
	bool P_SettingsState::Init(float)
	{   
		pInputSys  = EngineCore::GetInstance()->GetSystem<InputManager>();

		mSettingsObjects[SettingsButtons::eSound  ]      = FIND_GAME_OBJECT("Sound_Btn");
		mSettingsObjects[SettingsButtons::eSoundFx]      = FIND_GAME_OBJECT("SoundFx_Btn");
		mSettingsObjects[SettingsButtons::eVibrate]      = FIND_GAME_OBJECT("Vibrate_Btn");
		mSettingsObjects[SettingsButtons::eVSync]        = FIND_GAME_OBJECT("VSync_Btn");
		mSettingsObjects[SettingsButtons::eGammaPointer] = FIND_GAME_OBJECT("GammaPointer");

		pObject       = FIND_GAME_OBJECT("Settings");
		pSelector     = FIND_GAME_OBJECT("Setting_select");
		GammaSlider   = FIND_GAME_OBJECT("GammaSlider");
		SliderBegin   = FIND_GAME_OBJECT("Slider_Begin");
		SliderEnd     = FIND_GAME_OBJECT("Slider_End");
		mVibrate      = pInputSys->IsVibrateOn(); 
		mVSync        = EngineCore::Get<GraphicsSystem>()->GetVsync();

		mSFX = EngineCore::Get<SoundSystem>()->GetFX()  != 0;
		mBGM = EngineCore::Get<SoundSystem>()->GetBGM() != 0;

		if(mSettingsObjects[SettingsButtons::eSoundFx])
			mSettingsObjects[SettingsButtons::eSoundFx]->SetActive(mSFX);
		if(mSettingsObjects[SettingsButtons::eSound])
			mSettingsObjects[SettingsButtons::eSound]->SetActive(mBGM);
		if(mSettingsObjects[SettingsButtons::eVibrate])
			mSettingsObjects[SettingsButtons::eVibrate]->SetActive(mVibrate);
		if(mSettingsObjects[SettingsButtons::eVSync])
			mSettingsObjects[SettingsButtons::eVSync]->SetActive(mVSync);

		s_index = SettingsButtons::eSound;
		alpha   = 0.f;
		auto && temp  = mSettingsObjects[SettingsButtons::eGammaPointer]->GetComponent<Transform>()->GetGlobalPosition();
		auto && Begin = SliderBegin->GetComponent<Transform>()->GetGlobalPosition();
		auto && End   = SliderEnd->GetComponent<Transform>()->GetGlobalPosition();
		mGammaSpriteInterval = (End.x - Begin.x)/8.f;
		
		/*Check if the gamma set is way off the max value*/
		mGamma               = EngineCore::Get<GraphicsSystem>()->GetGamma() >= 5.f?5.f:EngineCore::Get<GraphicsSystem>()->GetGamma();
		mGamma               = mGamma <= 1.3f?1.f:mGamma;
		EngineCore::Get<GraphicsSystem>()->SetGamma(mGamma);
		mSettingsObjects[SettingsButtons::eGammaPointer]->GetComponent<Transform>()->SetGlobalPosition(Begin.x + static_cast<int>((mGamma - 1.f)/mGammaInterval) * mGammaSpriteInterval, temp.y,temp.z);

		fpButton = mSettingsFunc[s_index];          
		fpState = &P_SettingsState::Transition;
		pSelector && MoveSelector();  
		return true;
	}
	bool P_SettingsState::SetUp()
	{
		pInputSys = EngineCore::Get<InputManager>();

		mSettingsObjects[SettingsButtons::eSound  ]      = FIND_GAME_OBJECT("Sound_Btn");
		mSettingsObjects[SettingsButtons::eSoundFx]      = FIND_GAME_OBJECT("SoundFx_Btn");
		mSettingsObjects[SettingsButtons::eVibrate]      = FIND_GAME_OBJECT("Vibrate_Btn");
		mSettingsObjects[SettingsButtons::eVSync]        = FIND_GAME_OBJECT("VSync_Btn");
		mSettingsObjects[SettingsButtons::eGammaPointer] = FIND_GAME_OBJECT("GammaPointer");

		pObject      = FIND_GAME_OBJECT("Settings");
		pSelector    = FIND_GAME_OBJECT("Setting_select");
		GammaSlider  = FIND_GAME_OBJECT("GammaSlider");
		SliderBegin  = FIND_GAME_OBJECT("Slider_Begin");
		SliderEnd    = FIND_GAME_OBJECT("Slider_End");

		mSFX      = EngineCore::Get<SoundSystem>()->GetFX()  != 0.f;
		mBGM      = EngineCore::Get<SoundSystem>()->GetBGM() != 0.f;
		mVibrate  = pInputSys->IsVibrateOn(); 
		mVSync    = EngineCore::Get<GraphicsSystem>()->GetVsync();
 
		auto && temp  = mSettingsObjects[SettingsButtons::eGammaPointer]->GetComponent<Transform>()->GetGlobalPosition();
		auto && Begin = SliderBegin->GetComponent<Transform>()->GetGlobalPosition();
		auto && End   = SliderEnd->GetComponent<Transform>()->GetGlobalPosition();
		mGammaSpriteInterval = (End.x - Begin.x)/8.f;
		
		/*Check if the gamma set is way off the max value*/
		mGamma               = EngineCore::Get<GraphicsSystem>()->GetGamma() >= 5.f?5.f:EngineCore::Get<GraphicsSystem>()->GetGamma();
		mGamma               = mGamma <= 1.3f?1.f:mGamma;
		EngineCore::Get<GraphicsSystem>()->SetGamma(mGamma);
		mSettingsObjects[SettingsButtons::eGammaPointer]->GetComponent<Transform>()->SetGlobalPosition(Begin.x + static_cast<int>((mGamma - 1.f)/mGammaInterval) * mGammaSpriteInterval, temp.y,temp.z);

		fpButton = mSettingsFunc[s_index];
		pSelector && MoveSelector();        
		return true;
	}
	bool P_SettingsState::Transition(float )
	{
		/*This will execute once, you should do the set up for Settings here*/
		/*Fade In*/
		alpha += 0.5f + alpha * 0.5f;
		/*When whatever animation is done, set the pointer to point to the Howtoplay function*/
		if(pObject)
		{
			pObject->SetActive(true);
			if(auto && p = pObject->GetComponent<SpriteRenderer>())
			{
				p->SetAlpha(alpha);
			}  
		}
		if(alpha >= 1.f)
		{
			/*Set to transition stage*/
			fpState = &P_SettingsState::Update;
		}
		return true; 
	}  
	bool P_SettingsState::Update(float _dt)
	{
		DEBUG_PRINT(eLog::MESSAGE, "Settings Update");
		static float Axis = 0.f;
		Axis = pInputSys->GetAxis("L Stick Vertical");

		if( pInputSys->GetButtonDown("Vertical", true) || Axis < -0.8f)
		{
			s_index = static_cast<SettingsButtons>((static_cast<int>(s_index) + 1) % static_cast<int>(SettingsButtons::eTotal));
			(pSelector && mSettingsObjects[s_index]) &&  MoveSelector();
			fpButton = mSettingsFunc[s_index];
		}
		else if(pInputSys->GetButtonDown("Vertical", false)|| Axis > 0.8f)
		{
			s_index = (static_cast<int>(s_index) - 1) < 0 ? static_cast<SettingsButtons>(static_cast<int>(SettingsButtons::eTotal)-1) : static_cast<SettingsButtons>( static_cast<int>(s_index) - 1);
			(pSelector && mSettingsObjects[s_index]) && MoveSelector();
			fpButton = mSettingsFunc[s_index];
		}
		else if(pInputSys->GetButtonDown("Back"))
		{
			fpState = &P_SettingsState::Quit;
			return true;
		}
		fpButton && (this->*fpButton)(_dt);
		return true;  
	} 
	bool P_SettingsState::Quit(float )
	{
		//return true;
		/*Call TerminateMenuState() when the current menu state is done,
		  This will relinquish control back to the main menu*/
		alpha -= 0.5f * 5.f;
		//DEBUG_PRINT(eLog::MESSAGE, "Howtoplay Exit \n");
		if(pObject)
		{
			if(auto && p = pObject->GetComponent<SpriteRenderer>())
			{
				p->SetAlpha(alpha);  
			}
		}
		if(alpha < 0.1f && pObject)
		{
			DEBUG_PRINT(eLog::MESSAGE, "Howtoplay Exit \n");
			if(auto && p = pObject->GetComponent<SpriteRenderer>())
			{
				p->SetAlpha(0.f);
			} 
			/*Terminate Howtoplay Menu*/
			if(pObject)
				pObject->SetActive(false);
			/*Remember to set back*/
			fpState = &P_SettingsState::Init;
			Terminate();
			alpha = 0.f;
		}
		return true;
	}  
    bool P_SettingsState::Invoke(float _dt)
	{
		fpState && (this->*fpState)(_dt);
		return true;
	}
	bool P_SettingsState::MoveSelector()
	{
		auto && tempVec = GET_POSITION(mSettingsObjects[static_cast<SettingsButtons>(static_cast<int>(s_index))]);
		auto && tempMy  = GET_POSITION(pSelector);
		
		pSelector->GetComponent<Transform>()->SetGlobalPosition(tempMy.x,tempVec.y,tempMy.z);
		return true; 
	}
	bool P_SettingsState::ApplySound(float)
	{
		if(pInputSys && pInputSys->GetButtonDown("Select", false))
		{
			mBGM = !mBGM; 
			EngineCore::Get<SoundSystem>()->SetBGM(static_cast<float>(mBGM));
			mSettingsObjects[SettingsButtons::eSound]->SetActive(mBGM);
		}

		return true;
	}
	bool P_SettingsState::ApplySoundFx(float)
	{
		if(pInputSys && pInputSys->GetButtonDown("Select", false))
		{
			mSFX = !mSFX;
			EngineCore::Get<SoundSystem>()->SetFX(static_cast<float>(mSFX));
			mSettingsObjects[SettingsButtons::eSoundFx]->SetActive(mSFX);
		}
		return true;
	}
	bool P_SettingsState::ApplyVibrate(float)
	{
		if(pInputSys && pInputSys->GetButtonDown("Select", false))
		{
			mVibrate = !mVibrate;
			pInputSys->ToggleVibrate(mVibrate);
			mSettingsObjects[SettingsButtons::eVibrate]->SetActive(mVibrate);
		}
		return true;
	}
	bool P_SettingsState::ApplyVSync(float)
	{
		if(pInputSys && pInputSys->GetButtonDown("Select", false))
		{
			mVSync = !mVSync;
			EngineCore::Get<GraphicsSystem>()->ToggleVsync(mVSync);
			mSettingsObjects[SettingsButtons::eVSync]->SetActive(mVSync);
		}
		return true;
	}
	bool P_SettingsState::ApplyGamma(float)
	{
		/*Right*/
		if(pInputSys && pInputSys->GetButtonDown("Horizontal", false))
		{
			auto && temp  = mSettingsObjects[SettingsButtons::eGammaPointer]->GetComponent<Transform>()->GetGlobalPosition();
			mGamma = mGamma >=4.6f? 5.0f: mGamma + mGammaInterval;
			EngineCore::Get<GraphicsSystem>()->SetGamma(mGamma);
			auto && Begin = SliderBegin->GetComponent<Transform>()->GetGlobalPosition();
			mSettingsObjects[SettingsButtons::eGammaPointer]->GetComponent<Transform>()->SetGlobalPosition(Begin.x + static_cast<int>((mGamma-1.f)/mGammaInterval) * mGammaSpriteInterval, temp.y,temp.z);
		}
		// /*Left*/
		if(pInputSys && pInputSys->GetButtonDown("Horizontal", true))
		{
			auto && temp  = mSettingsObjects[SettingsButtons::eGammaPointer]->GetComponent<Transform>()->GetGlobalPosition();
			mGamma = mGamma <= 1.3f? 1.0f: mGamma - mGammaInterval; 
			EngineCore::Get<GraphicsSystem>()->SetGamma(mGamma);
			auto && Begin = SliderBegin->GetComponent<Transform>()->GetGlobalPosition();
			mSettingsObjects[SettingsButtons::eGammaPointer]->GetComponent<Transform>()->SetGlobalPosition(Begin.x + static_cast<int>((mGamma-1.f)/mGammaInterval) * mGammaSpriteInterval, temp.y,temp.z);
		}
		
		return true;
	}
	

}

