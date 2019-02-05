/* HEADER *********************************************************************************/
/*!
\file	PauseMenu.h
\author Keith Goh  (100%)
\par    email: keith.goh\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _PauseMenu_H_
#define _PauseMenu_H_

#define str(s) #s

#include "Behaviour/Behaviour.h"
#include "Reflection/Reflection.h"
#include "Reflection/ReflectionTypeErasure.h"
#include "Behaviour/BehaviourMemberFunc.h"


#define DllExport   __declspec( dllexport )

namespace Dystopia
{
	class GameObject;
	class InputManager;
	struct P_ResumeState
	{
		P_ResumeState()
		:fpState{&P_ResumeState::Init}
		{

		}
		virtual ~P_ResumeState(){}
		bool Init(float);
		bool Transition(float);
		bool Update(float);
		bool Exit(float);
		bool SetUp();
		bool Invoke(float);

		virtual bool Terminate() { return true; }
		virtual bool TogglePause(bool) {return true;}

		bool (P_ResumeState::*fpState)(float);
	};
	struct P_QuitState
	{
		enum QuitStateButtons : int
		{
			eYesButton = 0,
			eNoButton,

			eQuitTotal,
		};

		P_QuitState()
		:fpState{&P_QuitState::Init},Background{nullptr},pSelector{nullptr},
		 mQuitButtons
		 {
			 std::make_pair(eYesButton, nullptr),
			 std::make_pair(eNoButton,  nullptr)
		 },
		 pInputSys{nullptr}
		{
			
		}
		virtual ~P_QuitState(){}
		bool Init(float);
		bool Transition(float);
		bool Update(float);
		bool Exit(float);
		bool Invoke(float);
		bool SetUp();
		virtual bool Terminate() { return true; }
		virtual bool TogglePause(bool) {return true;}
		bool (P_QuitState::*fpState)(float);
		bool MoveSelector();

		InputManager* pInputSys;
		GameObject *  Background;
		GameObject *  pSelector;
		QuitStateButtons index;
		std::map<QuitStateButtons,GameObject*> mQuitButtons;
	};

	struct P_SettingsState
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

		P_SettingsState()
		:fpState{&P_SettingsState::Init}, pObject{nullptr}, alpha{0.f}, s_index{static_cast<SettingsButtons>(0)}, fpButton{nullptr},mBGM{1.f},mSFX{1.f},
		 pInputSys{nullptr}, mVibrate{true}, mVSync{true}, mGammaInterval{(5.f - 1.f) / 8.f}, mGammaSpriteInterval{0.f},mGamma{0.f},
		 mSettingsFunc
		 {
			std::make_pair(eSound,        &P_SettingsState::ApplySound),
			std::make_pair(eSoundFx,      &P_SettingsState::ApplySoundFx),
			std::make_pair(eVibrate,      &P_SettingsState::ApplyVibrate),
			std::make_pair(eVSync,        &P_SettingsState::ApplyVSync),
			std::make_pair(eGammaPointer, &P_SettingsState::ApplyGamma)
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
		virtual ~P_SettingsState(){};
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
		std::map<SettingsButtons, bool (P_SettingsState::*)(float)> mSettingsFunc;

		bool mBGM;
		bool mSFX;
		bool mVibrate;
		bool mVSync;

		float mGammaInterval;
		float mGammaSpriteInterval;
		float mGamma;

		bool (P_SettingsState::* fpState)(float);
		bool (P_SettingsState::* fpButton)(float);

	};
	struct P_HowtoplayState
	{
		P_HowtoplayState()
		:fpState{&P_HowtoplayState::Init}, alpha{0.f}, pObject{nullptr},
		 pInputSys{nullptr}
		{

		}
		virtual ~P_HowtoplayState(){}
		bool Init(float);
		bool Transition(float);
		bool Update(float);
		bool Exit(float);
		bool Invoke(float);
		bool SetUp();
		virtual bool Terminate() { return true; }
		bool (P_HowtoplayState::*fpState)(float);

		float alpha;
		GameObject* pObject;
		InputManager* pInputSys;
	};


	class PauseMenu : public Behaviour,
	                  public P_ResumeState,
					  public P_QuitState, 
					  public P_HowtoplayState,
					  public P_SettingsState
	{
	public:
		static constexpr const char * BehaviourName = str(PauseMenu);

		enum MenuStates : int
		{
			P_Play,
			P_Howtoplay,
			P_Settings,
			P_Quit,

			P_Total,

			P_PlayButton,
			P_HowtoplayButton,
			P_SettingsButton,
			P_QuitButton,

			P_PauseMenu,
		};

		template<typename T, PauseMenu::MenuStates _Index>
		struct MenuStateIndex
		{
			using type = T;
			static constexpr PauseMenu::MenuStates Index = _Index;
		};

		using PlayMenuType          = typename MenuStateIndex<P_ResumeState       ,PauseMenu::MenuStates::P_Play>::type;
		using HowtoplayMenuType     = typename MenuStateIndex<P_HowtoplayState    ,PauseMenu::MenuStates::P_Howtoplay>::type;
		using QuitMenuType          = typename MenuStateIndex<P_QuitState         ,PauseMenu::MenuStates::P_Quit>::type;
		using SettingsMenuType      = typename MenuStateIndex<P_SettingsState     ,PauseMenu::MenuStates::P_Settings>::type;
		using fp_type = bool (PauseMenu::*)(float);

#if !EDITOR
		
		using SYSTEM = BehaviourSystem;
		// unsigned GetBehaviourType(void) const
		// {
		// 	return Ut::MetaFind_t<Ut::Decay_t<decltype(*this)>, AllBehaviours>::value;
		// };

#endif
		virtual const std::string GetEditorName(void) const override { return "PauseMenu"; }
		static uint64_t constexpr mPauseMenuID = 18446744072329107968;

		// Default Constructor for this Behaviour - init your variables here
		PauseMenu();
		// Default Destructor for this Behaviour - don't touch if you dk, else ask the tech team
		~PauseMenu();

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
		virtual PauseMenu * Duplicate() const;

		// Don't touch
		virtual void EditorUI(void) noexcept override;
		
		// Reflection Stuff - Don't Touch
		virtual TypeErasure::TypeEraseMetaData       GetMetaData();
		virtual TypeErasure::TypeEraseMetaData const GetMetaData() const;

		bool Invoke(float _dt)
		{
			if(index == PauseMenu::MenuStates::P_Play)
				static_cast<PlayMenuType*>(this)->Invoke(_dt);
			if(index == PauseMenu::MenuStates::P_Quit)
				static_cast<QuitMenuType*>(this)->Invoke(_dt);
			if(index == PauseMenu::MenuStates::P_Howtoplay)
				static_cast<HowtoplayMenuType*>(this)->Invoke(_dt);
			if(index == PauseMenu::MenuStates::P_Settings)
				static_cast<SettingsMenuType*>(this)->Invoke(_dt);
			return true; 
		}
	private:

		virtual bool TogglePause(bool);
		virtual bool Terminate();
		bool MoveSelector();
		// Don't touch
		friend MetaData<PauseMenu>;
		bool(PauseMenu::* fpState)(float);
		PauseMenu::MenuStates index;
		std::map<PauseMenu::MenuStates, GameObject*> mMenuObjects;
		GameObject* pSelector;
		InputManager * pInputSys;
		bool isPause;
	};

	extern "C"
	{
		inline DllExport PauseMenu * PauseMenuClone()
		{
			return new PauseMenu;
		}
	}
}

/*Keep this if you do not want to show anything in Editor*/
PP_REFLECT_EMPTY(Dystopia::PauseMenu)
/*
  Uncomment the line PP_REFLECT and add in the names of the variable you want to show
  Comment out PP_REFLECT_EMPTY.
*/
//PP_REFLECT(Dystopia::PauseMenu)

#endif //_PauseMenu_H_


