/* HEADER *********************************************************************************/
/*!
\file	MainMenu.h
\author keith goh (100%)
\par    email: keith.goh\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _MainMenu_H_
#define _MainMenu_H_

#define str(s) #s

#include "Behaviour/Behaviour.h"
#include "Reflection/Reflection.h"
#include "Reflection/ReflectionTypeErasure.h"
#include "Behaviour/BehaviourMemberFunc.h"

#include <map>

#define DllExport   __declspec( dllexport )



namespace Dystopia
{

#ifndef _MAIN_MENU_SCRIPT_HELPER_
#define _MAIN_MENU_SCRIPT_HELPER_
 

#endif

	class GameObject;
	class InputManager;

	struct PlayState
	{
		PlayState()
		:fpState{&PlayState::Init}, pObject{nullptr}
		{

		}
		bool Init(float);
		bool Transition(float);
		bool Update(float);
		bool Quit(float);
		bool Invoke(float);
		bool SetUp();

		virtual bool Terminate(){return true;}
		virtual ~PlayState(){};
		virtual InputManager * GetInputSys() {return nullptr;};
		GameObject* pObject;
		bool (PlayState::* fpState)(float);
	};
	struct SettingsState
	{
		enum SettingsButtons
		{
			eSound,
			eSoundFx,
			eVibrate,
			eVSync,
			eGammaPointer,

			eTotal,
		};

		SettingsState()
		:fpState{&SettingsState::Init}, pObject{nullptr}, alpha{0.f}, s_index{static_cast<SettingsButtons>(0)}, fpButton{nullptr},mBGM{1.f},mSFX{1.f},
		 pInputSys{nullptr}, mVibrate{true}, mVSync{true}, mGammaInterval{(5.f - 1.f) / 8.f}, mGammaSpriteInterval{0.f},mGamma{0.f},
		 mSettingsFunc
		 {
			std::make_pair(eSound,        &SettingsState::ApplySound),
			std::make_pair(eSoundFx,      &SettingsState::ApplySoundFx),
			std::make_pair(eVibrate,      &SettingsState::ApplyVibrate),
			std::make_pair(eVSync,        &SettingsState::ApplyVSync),
			std::make_pair(eGammaPointer, &SettingsState::ApplyGamma)
		 }
		{
			
		}
		bool Init(float);
		bool Transition(float);
		bool Update(float);
		bool Quit(float);
		bool Invoke(float);
		bool SetUp();


		bool MoveSelector();
		virtual bool Terminate(){return true;}
		virtual ~SettingsState(){};
		virtual InputManager * GetInputSys() {return nullptr;};
		float alpha;

		bool ApplySound(float);
		bool ApplySoundFx(float);
		bool ApplyVibrate(float);
		bool ApplyVSync(float);
		bool ApplyGamma(float);

		GameObject* pObject;
		GameObject* pSelector;

		GameObject* SoundBtn;
		GameObject* SoundFx;
		GameObject* Vibrate;
		GameObject* VSync;
		GameObject* GammaSlider;
		GameObject* GammaPointer;
		GameObject* SliderBegin;
		GameObject* SliderEnd;

		InputManager* pInputSys;
		SettingsButtons s_index;
		std::map<SettingsButtons, GameObject*> mSettingsObjects;
		std::map<SettingsButtons, bool (SettingsState::*)(float)> mSettingsFunc;

		bool mBGM;
		bool mSFX;
		bool mVibrate;
		bool mVSync;
		bool isPress = false;
		bool isGammaPress = false;
		float mGammaInterval;
		float mGammaSpriteInterval;
		float mGamma;

		bool (SettingsState::* fpState)(float);
		bool (SettingsState::* fpButton)(float);

		
	};
	struct HowtoplayState
	{
		HowtoplayState()
		:fpState{&HowtoplayState::Init}, pObject{nullptr}, alpha{0.f}
		{

		} 
		bool Init(float);
		bool Transition(float);
		bool Update(float);
		bool Quit(float);
		bool Invoke(float);
		bool SetUp();

		virtual bool Terminate(){return true;}
		virtual ~HowtoplayState(){};
		virtual InputManager * GetInputSys() {return nullptr;};
		float alpha;
		GameObject* pObject;
		bool (HowtoplayState::* fpState)(float);
	};

	struct CreditsState
	{
		CreditsState()
		:fpState{&CreditsState::Init}, pObject{nullptr}, alpha{0.f}
		{

		} 
		bool Init(float);
		bool Transition(float);
		bool Update(float);
		bool Quit(float);
		bool Invoke(float);
		bool SetUp();

		virtual bool Terminate(){return true;}
		virtual ~CreditsState(){};
		virtual InputManager * GetInputSys() {return nullptr;};
		float alpha;
		GameObject* pObject;
		bool (CreditsState::* fpState)(float);
	};

	struct QuitState
	{
		QuitState()
		:fpState{&QuitState::Init}, pObject{nullptr}
		{

		}
		bool Init(float);
		bool Transition(float);
		bool Update(float);
		bool Quit(float);
		bool Invoke(float);
		bool SetUp();

		virtual bool Terminate(){return true;}
		virtual ~QuitState(){};
		virtual InputManager * GetInputSys() {return nullptr;};
		GameObject* pObject;
		bool (QuitState::* fpState)(float);
	};


	class MainMenu : public Behaviour, public PlayState, public SettingsState, public HowtoplayState, public QuitState, public CreditsState
	{
	public:
		enum class MenuStates : unsigned
		{
			Play = 0,
			Settings,
			Howtoplay,
			Credits,
			Quit,
			Total,
			
			Play_Button,
			Settings_Button,
			Howtoplay_Button,
			Credits_Button,
			Quit_Button,
			BlackScreen,

			
		};
		template<typename T, MainMenu::MenuStates _Index>
		struct MenuStateIndex
		{
			using type = T;
			static constexpr MainMenu::MenuStates Index = _Index;
		};

		using PlayMenuType           = typename MenuStateIndex<PlayState,MainMenu::MenuStates::Play>::type;
		using SettingsMenuType       = typename MenuStateIndex<SettingsState,MainMenu::MenuStates::Settings>::type;
		using HowtoplayMenuType      = typename MenuStateIndex<HowtoplayState,MainMenu::MenuStates::Howtoplay>::type;
		using QuitMenuType           = typename MenuStateIndex<QuitState,MainMenu::MenuStates::Quit>::type;
		using CreditsMenuType        = typename MenuStateIndex<CreditsState,MainMenu::MenuStates::Credits>::type;

		using fp_type      = bool (MainMenu::*)(float);
		using fp_transtype = bool (MainMenu::*)(float);
		
		union MenuFunc_Ptr
		{
			fp_transtype pTransition;
			fp_transtype pInit;
			fp_type      pMenuFunc;
		};
		
		
		static constexpr const char * BehaviourName = str(MainMenu);
#if !EDITOR
		
		using SYSTEM = BehaviourSystem;
		// unsigned GetBehaviourType(void) const
		// {
		// 	return Ut::MetaFind_t<Ut::Decay_t<decltype(*this)>, AllBehaviours>::value;
		// };

#endif
		virtual const std::string GetEditorName(void) const override { return "MainMenu"; }
		static uint64_t constexpr mMainMenuID = 697310208;

		// Default Constructor for this Behaviour - init your variables here
		MainMenu();
		// Default Destructor for this Behaviour - don't touch if you dk, else ask the tech team
		~MainMenu();

		// Load will never be called
		virtual void Load(void) override; 
		// Awake occurs when the editor is first started
		virtual void Awake(void) override; 
		// Init occurs when the scene is played
		virtual void Init(void) override; 

		// Update occurs every frame
		virtual void Update(const float _fDeltaTime) override; 
		// FixedUpdate occurs every 0.02ms
		virtual void FixedUpdate(const float _fDeltaTime) override; 
		// PostUpdate occurs after the above two updates
		virtual void PostUpdate(void) override;

		// GameObjectDestroy occurs when the gameobject is destroyed
		virtual void GameObjectDestroy(void) override;
		// Unload will never occur
		virtual void Unload(void) override;

		// If Owner has a Collider, and enters another collider, this occurs
		virtual void OnCollisionEnter(const CollisionEvent&);
		// If Owner has a Collider, and stays in another collider, this occurs
		virtual void OnCollisionStay (const CollisionEvent&);
		// If Owner has a Collider, and exits another collider, this occurs
		virtual void OnCollisionExit (const CollisionEvent&);

		// If Owner has a Trigger, and enters another trigger, this occurs
		virtual void OnTriggerEnter(GameObject * const);
		// If Owner has a Trigger, and stays in another trigger, this occurs
		virtual void OnTriggerStay (GameObject * const);
		// If Owner has a Trigger, and exit another trigger, this occurs
		virtual void OnTriggerExit (GameObject * const);

		virtual void Serialise(TextSerialiser&) const override;
		virtual void Unserialise(TextSerialiser&) override;

		virtual const char * const GetBehaviourName() const;

		// Don't touch
		virtual MainMenu * Duplicate() const;

		// Don't touch
		virtual void EditorUI(void) noexcept override;
		
		// Reflection Stuff - Don't Touch
		virtual TypeErasure::TypeEraseMetaData       GetMetaData();
		virtual TypeErasure::TypeEraseMetaData const GetMetaData() const;

		void TerminateMenuState();

		bool Invoke(float _dt)
		{ 
			if(index == MainMenu::MenuStates::Play)
				static_cast<PlayMenuType*>(this)->Invoke(_dt);
			if(index == MainMenu::MenuStates::Settings)
				static_cast<SettingsMenuType*>(this)->Invoke(_dt);
			if(index == MainMenu::MenuStates::Quit)
				static_cast<QuitMenuType*>(this)->Invoke(_dt);
			if(index == MainMenu::MenuStates::Howtoplay)
				static_cast<HowtoplayMenuType*>(this)->Invoke(_dt);
			if(index == MainMenu::MenuStates::Credits)
				static_cast<CreditsMenuType*>(this)->Invoke(_dt);
			return true; 
		};

		virtual bool Terminate()  
		{
			TerminateMenuState();
			return true;
		}

	private:
		// Don't touch
		friend MetaData<MainMenu>;

		MenuStates index;
		MenuFunc_Ptr mfp;

		std::map<MenuStates,fp_transtype>     mInitMap;
		std::map<MenuStates,GameObject*>      mMenuTabs;

		GameObject*                           pSelector;

		InputManager * pInputSys;
		bool isPress = false;

		InputManager * GetInputSys() {return pInputSys;};
		// float alpha;
		// /*Menu State Functions*/
		// bool Play(float);
		// bool Quit(float);
		// bool Settings(float);
		// bool Howtoplay(float);

		// /*Menu State Init Functions*/
		// bool HowtoplayInit(float);
		// bool PlayInit(float);
		// bool QuitInit(float);
		// bool SettingsInit(float);
		
		// /*Menu State Transition Functions*/
		// bool HowtoplayTransition(float);
		// bool PlayTransition(float);
		// bool QuitTransition(float);
		// bool SettingsTransition(float);

		// bool HowtoplayExit(float);
		// bool SettingsExit(float);
		// bool PlayExit(float);
		// bool QuitExit(float);
		
		
		bool MoveSelector();
	};


	extern "C"
	{
		inline DllExport MainMenu * MainMenuClone()
		{
			return new MainMenu;
		}
	}
}

/*Keep this if you do not want to show anything in Editor*/
PP_REFLECT_EMPTY(Dystopia::MainMenu)
/*
  Uncomment the line PP_REFLECT and add in the names of the variable you want to show
  Comment out PP_REFLECT_EMPTY.
*/
//PP_REFLECT_STUFF(Dystopia::MainMenu)

#endif //_MainMenu_H_


