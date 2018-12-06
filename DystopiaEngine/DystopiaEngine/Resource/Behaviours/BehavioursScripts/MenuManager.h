/* HEADER *********************************************************************************/
/*!
\file	MenuManager.h
\author Dan Kang (100%)
\par    email: dan.kang\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _MenuManager_H_
#define _MenuManager_H_

#define str(s) #s

#include "Behaviour/Behaviour.h"
#include "Reflection/Reflection.h"
#include "Reflection/ReflectionTypeErasure.h"
#include "Behaviour/BehaviourMemberFunc.h"
#include "Component/TextRenderer.h"
#include "System/Sound/SoundSystem.h"
#include "System/Graphics/GraphicsSystem.h"	// File header
#include "System/Window/WindowManager.h"	// File Header
#include "System/Window/Window.h"

#define DllExport   __declspec( dllexport )

namespace Dystopia
{
	class MenuManager : public Behaviour
	{
	public:
		static constexpr const char * BehaviourName = str(MenuManager);
#if !EDITOR
		
		using SYSTEM = BehaviourSystem;
		// unsigned GetBehaviourType(void) const
		// {
		// 	return Ut::MetaFind_t<Ut::Decay_t<decltype(*this)>, AllBehaviours>::value;
		// };

#endif
		virtual const std::string GetEditorName(void) const override { return "MenuManager"; }
		static uint64_t constexpr mMenuManagerID = 18446744073492534784;

		// Default Constructor for this Behaviour - init your variables here
		MenuManager();
		// Default Destructor for this Behaviour - don't touch if you dk, else ask the tech team
		~MenuManager();

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
		virtual MenuManager * Duplicate() const;

		// Don't touch
		virtual void EditorUI(void) noexcept override;
		
		// Reflection Stuff - Don't Touch
		virtual TypeErasure::TypeEraseMetaData       GetMetaData();
		virtual TypeErasure::TypeEraseMetaData const GetMetaData() const;

		void UpdateSelection();
		void UpdatePanel(float _dt);

	private:
		// Don't touch
		friend MetaData<MenuManager>;

		InputManager * mpInputSys;
		TextRenderer * PlayButton   = nullptr;
		TextRenderer * SettingsBtn  = nullptr;
		TextRenderer * ControlsBtn  = nullptr;
		TextRenderer * CreditsBtn  = nullptr;
		TextRenderer * QuitBtn  = nullptr;

		TextRenderer * fullBtn   = nullptr;
		TextRenderer * vsyncBtn  = nullptr;
		TextRenderer * bgmBtn  = nullptr;
		TextRenderer * sfxBtn  = nullptr;
		TextRenderer * gammaBtn  = nullptr;

		bool isFullScreen = true;
		bool isVsync = true;
		bool hasBgm = true;
		bool hasSfx = true;

	public:
		GameObject * theIndicator  = nullptr;
		GameObject * controllerMenu  = nullptr;
		GameObject * settingMenu  = nullptr;
		GameObject * creditMenu  = nullptr;
		GameObject * splashMenu  = nullptr;
		GameObject * mainMenu = nullptr;

		GameObject * fullTick = nullptr;
		GameObject * vsyncTick = nullptr;
		GameObject * bgmTick = nullptr;
		GameObject * sfxTick = nullptr;

		float yOffset;
		int currentPos;
		bool isSelected;
		float animateSpeed = 3;
		bool isAnimating = false;

		// -1 = Splash, 0 = Main, 1 = Settings, 2 = How To Play, 3 = Credits, 4 = Quit
		int currentMenu = -1; 
		int prevMenu = -1;
		int nextMenu = -1;
	};

	extern "C"
	{
		inline DllExport MenuManager * MenuManagerClone()
		{
			return new MenuManager;
		}
	}
}

/*Keep this if you do not want to show anything in Editor*/
//PP_REFLECT_EMPTY(Dystopia::MenuManager)
/*
  Uncomment the line PP_REFLECT and add in the names of the variable you want to show
  Comment out PP_REFLECT_EMPTY.
*/
PP_REFLECT(Dystopia::MenuManager, yOffset, currentPos, isSelected, animateSpeed);

#endif //_MenuManager_H_


