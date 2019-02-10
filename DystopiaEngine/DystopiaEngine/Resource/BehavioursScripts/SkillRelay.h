/* HEADER *********************************************************************************/
/*!
\file	SkillRelay.h
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _SkillRelay_H_
#define _SkillRelay_H_

#define str(s) #s

#include "Behaviour/Behaviour.h"
#include "Reflection/Reflection.h"
#include "Reflection/ReflectionTypeErasure.h"
#include "Behaviour/BehaviourMemberFunc.h"

#define DllExport   __declspec( dllexport )

namespace Dystopia
{
	class SkillRelay : public Behaviour
	{
	public:
		static constexpr const char * BehaviourName = str(SkillRelay);
#if !EDITOR
		
		using SYSTEM = BehaviourSystem;
		// unsigned GetBehaviourType(void) const
		// {
		// 	return Ut::MetaFind_t<Ut::Decay_t<decltype(*this)>, AllBehaviours>::value;
		// };

#endif
		virtual const std::string GetEditorName(void) const override { return "SkillRelay"; }
		static uint64_t constexpr mSkillRelayID = 18446744073054238208;

		// Default Constructor for this Behaviour - init your variables here
		SkillRelay();
		// Default Destructor for this Behaviour - don't touch if you dk, else ask the tech team
		~SkillRelay();

		// Load will never be called
		virtual void Load(void) override {} 
		// Awake occurs when the editor is first started
		virtual void Awake(void) override; 
		// Init occurs when the scene is played
		virtual void Init(void) override; 

		// Update occurs every frame
		virtual void Update(const float ) override; 
		// FixedUpdate occurs every 0.02ms
		virtual void FixedUpdate(const float ) override {}
		// PostUpdate occurs after the above two updates
		virtual void PostUpdate(void) override {}

		// GameObjectDestroy occurs when the gameobject is destroyed
		virtual void GameObjectDestroy(void) override;
		// Unload will never occur
		virtual void Unload(void) override {}

		// If Owner has a Collider, and enters another collider, this occurs
		virtual void OnCollisionEnter(const CollisionEvent&) {}
		// If Owner has a Collider, and stays in another collider, this occurs
		virtual void OnCollisionStay (const CollisionEvent&) {}
		// If Owner has a Collider, and exits another collider, this occurs
		virtual void OnCollisionExit (const CollisionEvent&) {}

		// If Owner has a Trigger, and enters another trigger, this occurs
		virtual void OnTriggerEnter(GameObject * const) {}
		// If Owner has a Trigger, and stays in another trigger, this occurs
		virtual void OnTriggerStay (GameObject * const) {}
		// If Owner has a Trigger, and exit another trigger, this occurs
		virtual void OnTriggerExit (GameObject * const) {}

		virtual void Serialise(TextSerialiser&) const override {}
		virtual void Unserialise(TextSerialiser&) override {}

		virtual const char * const GetBehaviourName() const;

		// Don't touch
		virtual SkillRelay * Duplicate() const;

		// Don't touch
		virtual void EditorUI(void) noexcept override {}
		
		// Reflection Stuff - Don't Touch
		virtual TypeErasure::TypeEraseMetaData       GetMetaData();
		virtual TypeErasure::TypeEraseMetaData const GetMetaData() const;

		void SetKnockDirection(const Math::Vec2& _dir);
		void SetFacing(bool);
		void StartSkill(void);
		
		PP_MEMBERFUNC(Dystopia::SkillRelay, SetKnockDirection, SetFacing, StartSkill)
		
		float mfLifespan;
		float mfCountdown;
		
	private:
		// Don't touch
		friend MetaData<SkillRelay>;
		
		bool mbStartCountdown;
	};

	extern "C"
	{
		inline DllExport SkillRelay * SkillRelayClone()
		{
			return new SkillRelay;
		}
	}
}

/*Keep this if you do not want to show anything in Editor*/
//PP_REFLECT_EMPTY(Dystopia::SkillRelay)
/*
  Uncomment the line PP_REFLECT and add in the names of the variable you want to show
  Comment out PP_REFLECT_EMPTY.
*/
PP_REFLECT_STUFF(Dystopia::SkillRelay, mfLifespan, mfCountdown);

#endif //_SkillRelay_H_


