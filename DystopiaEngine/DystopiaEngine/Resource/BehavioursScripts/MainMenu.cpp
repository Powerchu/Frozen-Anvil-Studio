/* HEADER *********************************************************************************/
/*!
\file	MainMenu.cpp
\author keith goh (100%)
\par    email: keith.goh\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "MainMenu.h"
#include "System/Input/InputSystem.h"
#include "System/Input/InputMap.h"
#include "System/Driver/Driver.h"
#include "System/Behaviour/BehaviourSystem.h"
#include "System/Graphics/GraphicsSystem.h"
#include "System/Scene/SceneSystem.h"
#include "System/Collision/CollisionEvent.h"
#include "System/Collision/CollisionSystem.h"
#include "Component/SpriteRenderer.h"
#include "System/Sound/SoundSystem.h"

#include "Object/ObjectFlags.h"
#include "Object/GameObject.h"

#include "Editor/EGUI.h"
#include "Utility/DebugAssert.h"
 

#define STATE_PAIR_GEN(_NAME_) std::make_pair(MainMenu::MenuStates:: ##_NAME_, &MainMenu:: ## _NAME_ ## Init)

#define TAB_PAIR_GEN(_NAME_)   std::make_pair(MainMenu::MenuStates:: ##_NAME_, nullptr)

#define MENUSTATE_PAIR_GEN(_NAME_)   std::make_pair(MainMenu::MenuStates:: ##_NAME_, new _NAME_ ## State{this})

#define TAB_PAIR_INIT(_NAME_) mMenuTabs[MainMenu::MenuStates:: ##_NAME_] = EngineCore::Get<SceneSystem>()->FindGameObject_cstr(#_NAME_);

#define BUTTON_PAIR_INIT(_NAME_) mMenuTabs[MainMenu::MenuStates:: ##_NAME_ ##_Button] = EngineCore::Get<SceneSystem>()->FindGameObject_cstr("## #_NAME_ ##_Button");

#define ACTIVATE_MENU(_NAME_) if(mMenuTabs[MainMenu::MenuStates:: ##_NAME_]) MainMenu_MSG::SendExternalMessage(mMenuTabs[MainMenu::MenuStates:: ##_NAME_], "Activate");

#define FIND_GAME_OBJECT(_NAME_) EngineCore::Get<SceneSystem>()->FindGameObject_cstr(_NAME_);

#define GET_POSITION(_PTR_) _PTR_ ## ->GetComponent<Transform>()->GetGlobalPosition();

#define GET_TRANSFORM(_PTR_) _PTR_ ## ->GetComponent<Transform>();

#define SET_GLOBAL_POSITION(_PTR_,_VEC3_) _PTR_ ## ->GetComponent<Transform>()->SetGlobalPosition(_VEC3_);

namespace Dystopia 
{ 
	namespace MainMenu_MSG 
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
	MainMenu::MainMenu()
	:
	 mMenuTabs{TAB_PAIR_GEN(Play) ,TAB_PAIR_GEN(Quit) ,TAB_PAIR_GEN(Howtoplay) ,TAB_PAIR_GEN(Settings), TAB_PAIR_GEN(Credits),
	           TAB_PAIR_GEN(Play_Button),TAB_PAIR_GEN(Quit_Button),TAB_PAIR_GEN(Settings_Button),TAB_PAIR_GEN(Howtoplay_Button),TAB_PAIR_GEN(Credits_Button)},
	 index {MainMenu::MenuStates::Play}, mfp{}, pInputSys(EngineCore::Get<InputManager>()), pSelector{nullptr}
	{ 
		mfp.pMenuFunc = nullptr;   
	}
    
	MainMenu::~MainMenu()   
	{   
	}

	void MainMenu::Load()   
	{
	}   

	void MainMenu::Awake()
	{
		TAB_PAIR_INIT(Howtoplay);
		TAB_PAIR_INIT(Settings);
		TAB_PAIR_INIT(Credits);

		mMenuTabs[MainMenu::MenuStates::Play]             = EngineCore::Get<SceneSystem>()->FindGameObject_cstr("Play_Button");
		mMenuTabs[MainMenu::MenuStates::Quit]             = EngineCore::Get<SceneSystem>()->FindGameObject_cstr("Quit_Button");
		mMenuTabs[MainMenu::MenuStates::Play_Button]      = EngineCore::Get<SceneSystem>()->FindGameObject_cstr("Play_Button");
		mMenuTabs[MainMenu::MenuStates::Quit_Button]      = EngineCore::Get<SceneSystem>()->FindGameObject_cstr("Quit_Button");
		mMenuTabs[MainMenu::MenuStates::Settings_Button]  = EngineCore::Get<SceneSystem>()->FindGameObject_cstr("Settings_Button");
		mMenuTabs[MainMenu::MenuStates::Howtoplay_Button] = EngineCore::Get<SceneSystem>()->FindGameObject_cstr("Howtoplay_Button");
		mMenuTabs[MainMenu::MenuStates::Credits_Button]   = EngineCore::Get<SceneSystem>()->FindGameObject_cstr("Credits_Button");
        mMenuTabs[MainMenu::MenuStates::Credits]          = EngineCore::Get<SceneSystem>()->FindGameObject_cstr("Credits");
		mMenuTabs[MainMenu::MenuStates::BlackScreen]      = EngineCore::Get<SceneSystem>()->FindGameObject_cstr("BlackScreen");
		static_cast<PlayMenuType*>(this)->pObject              = mMenuTabs[MainMenu::MenuStates::Play];
		static_cast<SettingsMenuType*>(this)->pObject          = mMenuTabs[MainMenu::MenuStates::Settings];
		static_cast<QuitMenuType*>(this)->pObject              = mMenuTabs[MainMenu::MenuStates::Quit_Button];
		static_cast<HowtoplayMenuType*>(this)->pObject         = mMenuTabs[MainMenu::MenuStates::Howtoplay];
		static_cast<CreditsMenuType*>(this)->pObject           = mMenuTabs[MainMenu::MenuStates::Credits];

		static_cast<PlayMenuType*>(this)->SetUp();
		static_cast<SettingsMenuType*>(this)->SetUp();
		static_cast<QuitMenuType*>(this)->SetUp();
		static_cast<HowtoplayMenuType*>(this)->SetUp();
		static_cast<CreditsMenuType*>(this)->SetUp();

		pSelector = FIND_GAME_OBJECT("Selector"); 

	}

	void MainMenu::Init()
	{
		TAB_PAIR_INIT(Howtoplay);
		TAB_PAIR_INIT(Settings);
		TAB_PAIR_INIT(Credits);

		mMenuTabs[MainMenu::MenuStates::Play]             = EngineCore::Get<SceneSystem>()->FindGameObject_cstr("Play_Button");
		mMenuTabs[MainMenu::MenuStates::Quit]             = EngineCore::Get<SceneSystem>()->FindGameObject_cstr("Quit_Button");
		mMenuTabs[MainMenu::MenuStates::Play_Button]      = EngineCore::Get<SceneSystem>()->FindGameObject_cstr("Play_Button");
		mMenuTabs[MainMenu::MenuStates::Quit_Button]      = EngineCore::Get<SceneSystem>()->FindGameObject_cstr("Quit_Button");
		mMenuTabs[MainMenu::MenuStates::Settings_Button]  = EngineCore::Get<SceneSystem>()->FindGameObject_cstr("Settings_Button");
		mMenuTabs[MainMenu::MenuStates::Howtoplay_Button] = EngineCore::Get<SceneSystem>()->FindGameObject_cstr("Howtoplay_Button"); 
		mMenuTabs[MainMenu::MenuStates::Credits_Button]   = EngineCore::Get<SceneSystem>()->FindGameObject_cstr("Credits_Button");
        mMenuTabs[MainMenu::MenuStates::Credits]          = EngineCore::Get<SceneSystem>()->FindGameObject_cstr("Credits");
        mMenuTabs[MainMenu::MenuStates::BlackScreen]      = EngineCore::Get<SceneSystem>()->FindGameObject_cstr("BlackScreen");

		static_cast<PlayMenuType*>(this)->pObject              = mMenuTabs[MainMenu::MenuStates::Play];
		static_cast<SettingsMenuType*>(this)->pObject          = mMenuTabs[MainMenu::MenuStates::Settings];
		static_cast<QuitMenuType*>(this)->pObject              = mMenuTabs[MainMenu::MenuStates::Quit];
		static_cast<HowtoplayMenuType*>(this)->pObject         = mMenuTabs[MainMenu::MenuStates::Howtoplay];
		static_cast<CreditsMenuType*>(this)->pObject           = mMenuTabs[MainMenu::MenuStates::Credits];

		static_cast<PlayMenuType*>(this)->SetUp();
		static_cast<SettingsMenuType*>(this)->SetUp();
		static_cast<QuitMenuType*>(this)->SetUp();
		static_cast<HowtoplayMenuType*>(this)->SetUp();
		static_cast<CreditsMenuType*>(this)->SetUp();

		pSelector = FIND_GAME_OBJECT("Selector");  
		isPress = false;
	}   

	void MainMenu::Update(const float _dt)
	{
		/*Some Menu State is in control*/
		if(mfp.pMenuFunc) 
		{
			/*This will execute the relevant function pointer*/
			(this->*(mfp.pMenuFunc))(_dt);
		}
		/*Main Menu is in Control*/ 
		else
		{
			static float Axis = 0.f;
			Axis = pInputSys->GetAxis("L Stick Vertical");

			/*Check Key Down*/
			if(!isPress && (pInputSys->GetButtonDown("Vertical", true) || Axis < -0.9f))
			{
				//DEBUG_PRINT(eLog::MESSAGE, "Keypress Down");
				index = static_cast<MenuStates>((static_cast<int>(index) + 1) % static_cast<int>(MenuStates::Total));
				(pSelector && mMenuTabs[static_cast<MenuStates>(static_cast<int>(index) + static_cast<int>(MenuStates::Total) + 1)]) &&  MoveSelector();
				isPress = true;
			}
			/*Check Key Up*/
			else if(!isPress && (pInputSys->GetButtonDown("Vertical", false) || Axis > 0.9f))
			{
				index = (static_cast<int>(index) - 1) < 0 ? static_cast<MenuStates>(static_cast<int>(MenuStates::Total)-1) : static_cast<MenuStates>( static_cast<int>(index) - 1);
				(pSelector && mMenuTabs[static_cast<MenuStates>(static_cast<int>(index) + static_cast<int>(MenuStates::Total) + 1)]) && MoveSelector();
				isPress = true;
			}
			/*Check Key Accept*/
			else if(pInputSys->GetButtonDown("Select"))
			{
				/*Launch the relevent Init function*/
				mfp.pMenuFunc    = &MainMenu::Invoke;
				if(mMenuTabs[MainMenu::MenuStates::BlackScreen])
					mMenuTabs[MainMenu::MenuStates::BlackScreen]->SetActive(true);
			}
			else if(pInputSys->GetButtonDown("Back"))
			{      
				
			}
			isPress = Axis < -0.9f || Axis > 0.9f;
		}  
	}

	void MainMenu::FixedUpdate(const float )
	{
	}
	
	void MainMenu::PostUpdate(void)
	{
	}

	void MainMenu::GameObjectDestroy(void)
	{
	}

	void MainMenu::Unload(void)
	{
	}

	void Dystopia::MainMenu::OnCollisionEnter(const CollisionEvent& )
	{

	}

	void Dystopia::MainMenu::OnCollisionStay(const CollisionEvent& )
	{

	}

	void Dystopia::MainMenu::OnCollisionExit(const CollisionEvent& )
	{

	}

	void Dystopia::MainMenu::OnTriggerEnter(GameObject * const )
	{
	}

	void Dystopia::MainMenu::OnTriggerStay(GameObject * const )
	{
	}

	void Dystopia::MainMenu::OnTriggerExit(GameObject * const )
	{
	}

	MainMenu * MainMenu::Duplicate() const
	{
		return new MainMenu{*this};
	}

	void MainMenu::Serialise(TextSerialiser& ) const
	{
	}

	void MainMenu::Unserialise(TextSerialiser& )
	{
	}


	const char * const MainMenu::GetBehaviourName() const
	{
		return MainMenu::BehaviourName;
	}

	void MainMenu::EditorUI(void) noexcept
	{
		
		
	}   
	
	TypeErasure::TypeEraseMetaData MainMenu::GetMetaData()
	{
		static MetaData<Dystopia::MainMenu> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const MainMenu::GetMetaData() const
	{    
		static MetaData<Dystopia::MainMenu> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}  


	void MainMenu::TerminateMenuState()
	{
		mfp.pMenuFunc = nullptr;
		if(mMenuTabs[MainMenu::MenuStates::BlackScreen])
			mMenuTabs[MainMenu::MenuStates::BlackScreen]->SetActive(false);
	} 
	
	bool MainMenu::MoveSelector()
	{ 
		auto && tempVec = GET_POSITION(mMenuTabs[static_cast<MenuStates>(static_cast<int>(index) + static_cast<int>(MenuStates::Total) + 1)]);
		auto && tempMy  = GET_POSITION(pSelector);
		
		pSelector->GetComponent<Transform>()->SetGlobalPosition(tempMy.x,tempVec.y,tempMy.z);
		DEBUG_PRINT(eLog::MESSAGE, "Selecter translating to current selected button %d", static_cast<int>(static_cast<int>(index) + static_cast<int>(MenuStates::Total) + 1));
		return true; 
	}
 
  
	bool PlayState::Init(float)
	{
		fpState = &PlayState::Transition;
		return true;
	} 
	bool PlayState::Transition(float)
	{
		fpState = &PlayState::Update;
		return true;
	}
	bool PlayState::Update(float)
	{
		fpState = &PlayState::Quit;
		/*Insert Code to Change Scene here*/
		EngineCore::Get<SceneSystem>()->LoadScene("IntroCutScene.dscene");
		return true;
	}
	bool PlayState::Quit(float)
	{
		/*Reset base to init*/
		fpState = &PlayState::Init;
		Terminate();
		return true;
	}
    bool PlayState::Invoke(float _dt) 
	{
		fpState && (this->*fpState)(_dt);
		return true;
	}
	bool PlayState::SetUp()
	{
		return true;
	}




	bool SettingsState::Init(float)
	{   
		pInputSys  = EngineCore::GetInstance()->GetSystem<InputManager>();

		mSettingsObjects[SettingsButtons::eSound  ]      = FIND_GAME_OBJECT("Sound_Btn");
		mSettingsObjects[SettingsButtons::eSoundFx]      = FIND_GAME_OBJECT("SoundFx_Btn");
		mSettingsObjects[SettingsButtons::eVibrate]      = FIND_GAME_OBJECT("Vibrate_Btn");
		mSettingsObjects[SettingsButtons::eVSync]        = FIND_GAME_OBJECT("VSync_Btn");
		mSettingsObjects[SettingsButtons::eGammaPointer] = FIND_GAME_OBJECT("GammaPointer");

		pSelector     = FIND_GAME_OBJECT("Setting_select");
		GammaSlider   = FIND_GAME_OBJECT("GammaSlider");
		mSFX = EngineCore::Get<SoundSystem>()->GetFX()  != 0;
		mBGM = EngineCore::Get<SoundSystem>()->GetBGM() != 0;
		mVibrate  = pInputSys->IsVibrateOn(); 
		mVSync    = EngineCore::Get<GraphicsSystem>()->GetVsync();

		if(pObject)
		{
			pObject->SetActive(true);
		}
		if(mSettingsObjects[SettingsButtons::eSoundFx])
			mSettingsObjects[SettingsButtons::eSoundFx]->SetActive(mSFX);
		if(mSettingsObjects[SettingsButtons::eSound])
			mSettingsObjects[SettingsButtons::eSound]->SetActive(mBGM);
		if(mSettingsObjects[SettingsButtons::eVibrate])
			mSettingsObjects[SettingsButtons::eVibrate]->SetActive(mVibrate);
		if(mSettingsObjects[SettingsButtons::eVSync])
			mSettingsObjects[SettingsButtons::eVSync]->SetActive(mVSync);

		alpha   = 0.f;
		fpState = &SettingsState::Transition;
		isPress = false;
		return true;
	}
	bool SettingsState::SetUp()
	{
		pInputSys = EngineCore::GetInstance()->GetSystem<InputManager>();
		mSettingsObjects[SettingsButtons::eSound  ]      = FIND_GAME_OBJECT("Sound_Btn");
		mSettingsObjects[SettingsButtons::eSoundFx]      = FIND_GAME_OBJECT("SoundFx_Btn");
		mSettingsObjects[SettingsButtons::eVibrate]      = FIND_GAME_OBJECT("Vibrate_Btn");
		mSettingsObjects[SettingsButtons::eVSync]        = FIND_GAME_OBJECT("VSync_Btn");
		mSettingsObjects[SettingsButtons::eGammaPointer] = FIND_GAME_OBJECT("GammaPointer");

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
		return true;
	}
	bool SettingsState::Transition(float _dt)
	{
		/*This will execute once, you should do the set up for Settings here*/
		/*Fade In*/
		alpha += _dt + 5.f * _dt;
		/*When whatever animation is done, set the pointer to point to the Howtoplay function*/
		DEBUG_PRINT(eLog::MESSAGE, "Settings Transition \n");
		
		if(pObject)
		{
			if(auto && p = pObject->GetComponent<SpriteRenderer>())
			{
				p->SetAlpha(alpha);
			}  
		}
		if(alpha >= 1.f)
		{
			/*Set to transition stage*/
			fpState = &SettingsState::Update;
		}
		return true; 
	}  
	bool SettingsState::Update(float _dt)
	{
		static float Axis = 0.f;
		Axis = pInputSys->GetAxis("L Stick Vertical");
		if(!isPress && (pInputSys->GetButtonDown("Vertical", true) || Axis < -0.9f))
		{
			s_index = static_cast<SettingsButtons>((static_cast<int>(s_index) + 1) % static_cast<int>(SettingsButtons::eTotal));
			(pSelector && mSettingsObjects[s_index]) &&  MoveSelector();
			fpButton = mSettingsFunc[s_index];
			isGammaPress = false;
		}
		else if(!isPress && (pInputSys->GetButtonDown("Vertical", false)|| Axis > 0.9f))
		{
			s_index = (static_cast<int>(s_index) - 1) < 0 ? static_cast<SettingsButtons>(static_cast<int>(SettingsButtons::eTotal)-1) : static_cast<SettingsButtons>( static_cast<int>(s_index) - 1);
			(pSelector && mSettingsObjects[s_index]) && MoveSelector();
			fpButton = mSettingsFunc[s_index];
			isGammaPress = false;
		}
		else if(pInputSys->GetButtonDown("Back"))
		{
			fpState = &SettingsState::Quit;
			return true;
		}
		isPress = Axis < -0.9f || Axis > 0.9f;
		fpButton && (this->*fpButton)(_dt);
		return true; 
	} 
	bool SettingsState::Quit(float _dt)
	{
		/*Call TerminateMenuState() when the current menu state is done,
		  This will relinquish control back to the main menu*/
		alpha -= _dt * 5.f;
		//DEBUG_PRINT(eLog::MESSAGE, "Howtoplay Exit \n");
		if(pObject)
		{
			if(auto && p = pObject->GetComponent<SpriteRenderer>())
			{
				p->SetAlpha(alpha);  
			}
		}
		if(alpha < 0.1f)
		{
			if(auto && p = pObject->GetComponent<SpriteRenderer>())
			{
				p->SetAlpha(0.f);
			} 

			/*Terminate Howtoplay Menu*/
			DEBUG_PRINT(eLog::MESSAGE, "HowtoplayExit Fade out completed \n");
			pObject->SetActive(false);
			/*Remember to set back*/
			fpState = &SettingsState::Init;
			Terminate();
			alpha = 0.f;
		}
		return true;
	}  
    bool SettingsState::Invoke(float _dt)
	{
		fpState && (this->*fpState)(_dt);
		return true;
	}
	bool SettingsState::MoveSelector()
	{
		auto && tempVec = GET_POSITION(mSettingsObjects[static_cast<SettingsButtons>(static_cast<int>(s_index))]);
		auto && tempMy  = GET_POSITION(pSelector);
		
		pSelector->GetComponent<Transform>()->SetGlobalPosition(tempMy.x,tempVec.y,tempMy.z);
		return true; 
	}
	bool SettingsState::ApplySound(float)
	{
		if(pInputSys && pInputSys->GetButtonDown("Select", false))
		{
			mBGM = !mBGM; 
			EngineCore::Get<SoundSystem>()->SetBGM(static_cast<float>(mBGM));
			mSettingsObjects[SettingsButtons::eSound]->SetActive(mBGM);
		}

		return true;
	}
	bool SettingsState::ApplySoundFx(float)
	{
		if(pInputSys && pInputSys->GetButtonDown("Select", false))
		{
			mSFX = !mSFX;
			EngineCore::Get<SoundSystem>()->SetFX(static_cast<float>(mSFX));
			mSettingsObjects[SettingsButtons::eSoundFx]->SetActive(mSFX);
		}
		return true;
	}
	bool SettingsState::ApplyVibrate(float)
	{
		if(pInputSys && pInputSys->GetButtonDown("Select", false))
		{
			mVibrate = !mVibrate;
			pInputSys->ToggleVibrate(mVibrate);
			mSettingsObjects[SettingsButtons::eVibrate]->SetActive(mVibrate);
		}
		return true;
	}
	bool SettingsState::ApplyVSync(float)
	{
		if(pInputSys && pInputSys->GetButtonDown("Select", false))
		{
			mVSync = !mVSync;
			EngineCore::Get<GraphicsSystem>()->ToggleVsync(mVSync);
			mSettingsObjects[SettingsButtons::eVSync]->SetActive(mVSync);
		}
		return true;
	}
	bool SettingsState::ApplyGamma(float)
	{
		static float Axis = 0.f;
		Axis = pInputSys->GetAxis("L Stick Horizontal");
		/*Right*/
		if(!isGammaPress && (pInputSys && (pInputSys->GetButtonDown("Horizontal", false) || Axis > 0.9f)))
		{
			auto && temp  = mSettingsObjects[SettingsButtons::eGammaPointer]->GetComponent<Transform>()->GetGlobalPosition();
			mGamma = mGamma >=4.6f? 5.0f: mGamma + mGammaInterval;
			EngineCore::Get<GraphicsSystem>()->SetGamma(mGamma);
			auto && Begin = SliderBegin->GetComponent<Transform>()->GetGlobalPosition();
			mSettingsObjects[SettingsButtons::eGammaPointer]->GetComponent<Transform>()->SetGlobalPosition(Begin.x + static_cast<int>((mGamma-1.f)/mGammaInterval) * mGammaSpriteInterval, temp.y,temp.z);
		}
		// /*Left*/
		if(!isGammaPress && (pInputSys && (pInputSys->GetButtonDown("Horizontal", true) || Axis < -0.9f)))
		{
			auto && temp  = mSettingsObjects[SettingsButtons::eGammaPointer]->GetComponent<Transform>()->GetGlobalPosition();
			mGamma = mGamma <= 1.3f? 1.0f: mGamma - mGammaInterval; 
			EngineCore::Get<GraphicsSystem>()->SetGamma(mGamma);
			auto && Begin = SliderBegin->GetComponent<Transform>()->GetGlobalPosition();
			mSettingsObjects[SettingsButtons::eGammaPointer]->GetComponent<Transform>()->SetGlobalPosition(Begin.x + static_cast<int>((mGamma-1.f)/mGammaInterval) * mGammaSpriteInterval, temp.y,temp.z);
		}
		isGammaPress = Axis < -0.9f || Axis > 0.9f;
		return true;
	}











	bool HowtoplayState::Init(float)
	{
	    alpha   = 0.f;
		fpState = &HowtoplayState::Transition; 
		return true;
	}
	bool HowtoplayState::Transition(float _dt)
	{
		/*This will execute once, you should do the set up for Settings here*/
		/*Fade In*/
		alpha += _dt + 5.f * _dt;
		/*When whatever animation is done, set the pointer to point to the Howtoplay function*/
		DEBUG_PRINT(eLog::MESSAGE, "Howtoplay Transition \n");
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
			fpState = &HowtoplayState::Update;
		}
		return true;
	}
	bool HowtoplayState::Update(float)
	{
		/*This will run until TerminateMenuState() is called*/
    	/*Find Howtoplay Game Object*/
		InputManager * pInputSys = GetInputSys();
		if(pInputSys->GetKey(XBUTTON_DPAD_DOWN))
		{

		}
		else if(pInputSys->GetKey(XBUTTON_DPAD_UP))
		{

		}
		else if((pInputSys->GetKey(XBUTTON_A)))
		{
			
		}
		else if(pInputSys->GetButtonDown("Back"))
		{
			fpState = &HowtoplayState::Quit;
		}

		return true;
	}
	bool HowtoplayState::Quit(float _dt)
	{
		/*Call TerminateMenuState() when the current menu state is done,
		  This will relinquish control back to the main menu*/
		alpha -= _dt * 5.f;
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
			pObject->SetActive(false);
			fpState = &HowtoplayState::Init;
			Terminate();
			alpha = 0.f;
		}
		return true;
	}
    bool HowtoplayState::Invoke(float _dt) 
	{
		fpState && (this->*fpState)(_dt);
		return true;
	}
	bool HowtoplayState::SetUp()
	{
		return true;
	}



	bool QuitState::Init(float)
	{
		fpState = &QuitState::Transition;
		return true;
	}
	bool QuitState::Transition(float)
	{
		/*When whatever animation is done, set the pointer to point to the Howtoplay function*/
		fpState = &QuitState::Update;
		DEBUG_PRINT(eLog::MESSAGE, "Quit Transition");
		return true;
	}
	bool QuitState::Update(float)
	{
		fpState = &QuitState::Quit;
		EngineCore::GetInstance()->Quit();
		return true;
	}
	bool QuitState::Quit(float) 
	{
		fpState = &QuitState::Init; 
		Terminate();
		return true;
	}
	bool QuitState::Invoke(float _dt)
	{
		fpState && (this->*fpState)(_dt);
		return true;
	}
	bool QuitState::SetUp()
	{
		return true; 
	}




	bool CreditsState::Init(float)
	{
		alpha   = 0.f;
		fpState = &CreditsState::Transition;
		DEBUG_PRINT(eLog::MESSAGE, "CreditState ");
		return true;
	}
	bool CreditsState::Transition(float _dt)
	{
		/*This will execute once, you should do the set up for Settings here*/
		/*Fade In*/
		alpha += _dt + 5.f * _dt;
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
			fpState = &CreditsState::Update;
		}
		return true;
	}
	bool CreditsState::Update(float)
	{
		if(GetInputSys()->GetButtonDown("Back"))
		{
			fpState = &CreditsState::Quit;
		}
		return true;
	}
	bool CreditsState::Quit(float _dt)
	{
		/*Call TerminateMenuState() when the current menu state is done,
		  This will relinquish control back to the main menu*/
		alpha -= _dt * 5.f;
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
			DEBUG_PRINT(eLog::MESSAGE, "HowtoplayExit Fade out completed \n");
			pObject->SetActive(false);
			fpState = &CreditsState::Init;
			Terminate();
			alpha = 0.f;
		}
		return true;
	}
	bool CreditsState::Invoke(float _dt)
	{
		fpState && (this->*fpState)(_dt);
		return true;
	}
	bool CreditsState::SetUp()
	{
		pObject = FIND_GAME_OBJECT("Credits");
		return true;
	}

}




