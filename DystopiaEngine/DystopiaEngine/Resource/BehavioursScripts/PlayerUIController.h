/* HEADER *********************************************************************************/
/*!
\file	PlayerUIController.h
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _PlayerUIController_H_
#define _PlayerUIController_H_

#define str(s) #s

#include "Behaviour/Behaviour.h"
#include "Reflection/Reflection.h"
#include "Reflection/ReflectionTypeErasure.h"
#include "Behaviour/BehaviourMemberFunc.h"

#define DllExport   __declspec( dllexport )

namespace Dystopia
{
	class PlayerUIController : public Behaviour
	{
	public:
		static constexpr const char * BehaviourName = str(PlayerUIController);
#if !EDITOR
		
		using SYSTEM = BehaviourSystem;
		// unsigned GetBehaviourType(void) const
		// {
		// 	return Ut::MetaFind_t<Ut::Decay_t<decltype(*this)>, AllBehaviours>::value;
		// };

#endif
		virtual const std::string GetEditorName(void) const override { return "PlayerUIController"; }
		static uint64_t constexpr mPlayerUIControllerID = 37770752;

		// Default Constructor for this Behaviour - init your variables here
		PlayerUIController();
		// Default Destructor for this Behaviour - don't touch if you dk, else ask the tech team
		~PlayerUIController();

		// Load will never be called
		virtual void Load(void) override{}
		// Awake occurs when the editor is first started
		virtual void Awake(void) override; 
		// Init occurs when the scene is played
		virtual void Init(void) override; 

		// Update occurs every frame
		virtual void Update(const float ) override; 
		// FixedUpdate occurs every 0.02ms
		virtual void FixedUpdate(const float ) override{}
		// PostUpdate occurs after the above two updates
		virtual void PostUpdate(void) override{}

		// GameObjectDestroy occurs when the gameobject is destroyed
		virtual void GameObjectDestroy(void) override;
		// Unload will never occur
		virtual void Unload(void) override{}

		// If Owner has a Collider, and enters another collider, this occurs
		virtual void OnCollisionEnter(const CollisionEvent&){}
		// If Owner has a Collider, and stays in another collider, this occurs
		virtual void OnCollisionStay (const CollisionEvent&){}
		// If Owner has a Collider, and exits another collider, this occurs
		virtual void OnCollisionExit (const CollisionEvent&){}

		// If Owner has a Trigger, and enters another trigger, this occurs
		virtual void OnTriggerEnter(GameObject * const){}
		// If Owner has a Trigger, and stays in another trigger, this occurs
		virtual void OnTriggerStay (GameObject * const){}
		// If Owner has a Trigger, and exit another trigger, this occurs
		virtual void OnTriggerExit (GameObject * const){}

		virtual void Serialise(TextSerialiser&) const override{}
		virtual void Unserialise(TextSerialiser&) override{}

		virtual const char * const GetBehaviourName() const;

		// Don't touch
		virtual PlayerUIController * Duplicate() const;

		// Don't touch
		virtual void EditorUI(void) noexcept override{}
		
		// Reflection Stuff - Don't Touch
		virtual TypeErasure::TypeEraseMetaData       GetMetaData();
		virtual TypeErasure::TypeEraseMetaData const GetMetaData() const;

		float mfCurHealth;
		float mfCurDec;
		float mfSpeed;
		float mfAccuDt;
		bool mbInForce;
		float mfCurStamina;
		float mfStamSpeed;
		
		float mfTargetHealth;
		float mfTargetStamina;
		
		float mfCounterDuration;
		float mfCounterFadeDuration;
		
		float mfCounterDt;
        
		float mfImpactStay;
		float mfImpactFade;
		
        // instantly set to percentage 0 - 1
		void SetHealthPercentage(float);
        
        // instantly set to percentage 0 - 1
		void SetStaminaPercentage(float);
        
        // change between the two modes
		void SetForceMode(bool);
		
        // set linear go to percentage 0 - 1
		void GoToHealthPercentage(float);
        
        // set linear go to percentage 0 - 1
		void GoToStaminaPercentage(float);
        
        // set combo counter text renderer,
        // stay based on mfCounterDuration, fade based on mfCounterFadeDuration
		void SetCounter(unsigned);   
        
        // set impact stay and fade durations
        // repeated calls restarts duration
        void TakeImpact(void); 
		
		PP_MEMBERFUNC(Dystopia::PlayerUIController, SetHealthPercentage, SetStaminaPercentage, SetForceMode, GoToHealthPercentage, GoToStaminaPercentage, SetCounter, TakeImpact)

	private:
		// Don't touch
		friend MetaData<PlayerUIController>;
		
		GameObject* mpHealtbar;
		GameObject* mpMode;
		GameObject* mpStamina;
		GameObject* mpCombatCount;
		GameObject* mpImpactUI;
		GameObject* mpPlayer;
		
		unsigned mnCombatCounter;
		bool mbShowCounter;
		bool mbFadeCounter;
        bool mbImpactShow;
        bool mbImpactFadeOut;
        float mfImpactDt;
		
		void HealthbarShader(float);
		void StaminaBarShader(float);
		void CombatCounter(float);
		void ImpactUI(float);
	};

	extern "C"
	{
		inline DllExport PlayerUIController * PlayerUIControllerClone()
		{
			return new PlayerUIController;
		}
	}
}

/*Keep this if you do not want to show anything in Editor*/
//PP_REFLECT_EMPTY(Dystopia::PlayerUIController)
/*
  Uncomment the line PP_REFLECT and add in the names of the variable you want to show
  Comment out PP_REFLECT_EMPTY.
*/

PP_REFLECT_STUFF(Dystopia::PlayerUIController, mfSpeed, mfStamSpeed, mfCounterDuration, mfCounterFadeDuration, mfImpactStay, mfImpactFade);

#endif //_PlayerUIController_H_


