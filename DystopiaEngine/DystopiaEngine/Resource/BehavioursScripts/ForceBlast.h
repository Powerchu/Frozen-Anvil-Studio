/* HEADER *********************************************************************************/
/*!
\file	ForceBlast.h
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _ForceBlast_H_
#define _ForceBlast_H_

#define str(s) #s

#include "Behaviour/Behaviour.h"
#include "Reflection/Reflection.h"
#include "Reflection/ReflectionTypeErasure.h"
#include "Behaviour/BehaviourMemberFunc.h"

#define DllExport   __declspec( dllexport )

namespace Dystopia
{
	class ForceBlast : public Behaviour
	{
	public:
		static constexpr const char * BehaviourName = str(ForceBlast);
#if !EDITOR
		
		using SYSTEM = BehaviourSystem;
		// unsigned GetBehaviourType(void) const
		// {
		// 	return Ut::MetaFind_t<Ut::Decay_t<decltype(*this)>, AllBehaviours>::value;
		// };

#endif
		virtual const std::string GetEditorName(void) const override { return "ForceBlast"; }
		static uint64_t constexpr mForceBlastID = 18446744072897470976;

		// Default Constructor for this Behaviour - init your variables here
		ForceBlast();
		// Default Destructor for this Behaviour - don't touch if you dk, else ask the tech team
		~ForceBlast();

		// Load will never be called
		virtual void Load(void) override{} 
		// Awake occurs when the editor is first started
		virtual void Awake(void) override; 
		// Init occurs when the scene is played
		virtual void Init(void) override; 

		// Update occurs every frame
		virtual void Update(const float _fDeltaTime) override; 
		// FixedUpdate occurs every 0.02ms
		virtual void FixedUpdate(const float _fDeltaTime) override{} 
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
		virtual void OnTriggerEnter(GameObject * const);
		// If Owner has a Trigger, and stays in another trigger, this occurs
		virtual void OnTriggerStay (GameObject * const);
		// If Owner has a Trigger, and exit another trigger, this occurs
		virtual void OnTriggerExit (GameObject * const);

		virtual void Serialise(TextSerialiser&) const override{}
		virtual void Unserialise(TextSerialiser&) override{}

		virtual const char * const GetBehaviourName() const;

		// Don't touch
		virtual ForceBlast * Duplicate() const;

		// Don't touch
		virtual void EditorUI(void) noexcept override{}
		
		// Reflection Stuff - Don't Touch
		virtual TypeErasure::TypeEraseMetaData       GetMetaData();
		virtual TypeErasure::TypeEraseMetaData const GetMetaData() const;

		void SetKnockDirection(Math::Vec2 _dir);
		void SetFacing(bool);
		void StartSkill(void);
		
		PP_MEMBERFUNC(Dystopia::ForceBlast, SetKnockDirection, SetFacing, StartSkill)
		
		bool mbActivate;
		
	private:
		// Don't touch
		friend MetaData<ForceBlast>;
		
		float mfCastDamage;
		float mfCastKnockback;
		float mfAnimSpeed;
		float mfApplyTiming;
		bool mbAppliedAtk;
		Math::Vec2 mForceDir;
		
		AutoArray<uint64_t> mArrFoundEnemies;
		AutoArray<uint64_t> mArrBlastInteractable;
		
		void SendDamage(void);
		
		SpriteRenderer* mpAnim;
	};

	extern "C"
	{
		inline DllExport ForceBlast * ForceBlastClone()
		{
			return new ForceBlast;
		}
	}
}

/*Keep this if you do not want to show anything in Editor*/
//PP_REFLECT_EMPTY(Dystopia::ForceBlast)
/*
  Uncomment the line PP_REFLECT and add in the names of the variable you want to show
  Comment out PP_REFLECT_EMPTY.
*/
PP_REFLECT_STUFF(Dystopia::ForceBlast, mbActivate);

#endif //_ForceBlast_H_


