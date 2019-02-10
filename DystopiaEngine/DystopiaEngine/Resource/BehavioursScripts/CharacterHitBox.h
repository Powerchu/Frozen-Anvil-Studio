/* HEADER *********************************************************************************/
/*!
\file	CharacterHitBox.h
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _CharacterHitBox_H_
#define _CharacterHitBox_H_

#define str(s) #s

#include "Behaviour/Behaviour.h"
#include "Reflection/Reflection.h"
#include "Reflection/ReflectionTypeErasure.h"
#include "Behaviour/BehaviourMemberFunc.h"
#include "DataStructure/AutoArray.h"

#include "Math/Vectors.h"

#define DllExport   __declspec( dllexport )

namespace Dystopia
{
	class Convex;
	class AudioSource;
	class CharacterHitBox : public Behaviour
	{
	public:
		static constexpr const char * BehaviourName = str(CharacterHitBox);
#if !EDITOR
		
		using SYSTEM = BehaviourSystem;
		// unsigned GetBehaviourType(void) const
		// {
		// 	return Ut::MetaFind_t<Ut::Decay_t<decltype(*this)>, AllBehaviours>::value;
		// };

#endif
		virtual const std::string GetEditorName(void) const override { return "CharacterHitBox"; }
		static uint64_t constexpr mCharacterHitBoxID = 1929197056;

		// Default Constructor for this Behaviour - init your variables here
		CharacterHitBox();
		// Default Destructor for this Behaviour - don't touch if you dk, else ask the tech team
		~CharacterHitBox();

		// Load will never be called
		virtual void Load(void) override{}
		// Awake occurs when the editor is first started
		virtual void Awake(void) override; 
		// Init occurs when the scene is played
		virtual void Init(void) override; 

		// Update occurs every frame
		virtual void Update(const float _fDeltaTime) override; 
		// FixedUpdate occurs every 0.02ms
		virtual void FixedUpdate(const float _fDeltaTime) override; 
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
		virtual CharacterHitBox * Duplicate() const;

		// Don't touch
		virtual void EditorUI(void) noexcept override{}
		
		// Reflection Stuff - Don't Touch
		virtual TypeErasure::TypeEraseMetaData       GetMetaData();
		virtual TypeErasure::TypeEraseMetaData const GetMetaData() const;
		
		int mnEnemyCount;
		int mnInteractCount;		
		void SendDamage(float _amount);
		void SendForce(float _force, Math::Vec2 _dir);
        void InvokeAudio(const char* _audioName);
		
		PP_MEMBERFUNC(Dystopia::CharacterHitBox, SendDamage, SendForce, InvokeAudio)
		
	private:
		// Don't touch
		friend MetaData<CharacterHitBox>;
		
		void EnemyEnteredHitbox(const uint64_t& _id);
		void EnemyExitHitbox(const uint64_t& _id);
		void InteractablesEnteredHitbox(const uint64_t& _id);		void InteractablesExitHitbox(const uint64_t& _id);		
		AutoArray<uint64_t> mArrFoundEnemies;
		AutoArray<uint64_t> mArrInteractable;
		AudioSource* mpAudioSrc = nullptr;
	};

	extern "C"
	{ 
		inline DllExport CharacterHitBox * CharacterHitBoxClone()
		{
			return new CharacterHitBox;
		}
	}
}

/*Keep this if you do not want to show anything in Editor*/
//PP_REFLECT_EMPTY(Dystopia::CharacterHitBox)
/*
  Uncomment the line PP_REFLECT and add in the names of the variable you want to show
  Comment out PP_REFLECT_EMPTY.
*/
PP_REFLECT_STUFF(Dystopia::CharacterHitBox, mnEnemyCount, mnInteractCount);

#endif //_CharacterHitBox_H_


