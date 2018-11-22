/* HEADER *********************************************************************************/
/*!
\file	CharacterController.h
\author Aaron Chu (100%)
\par    email: m.chu\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _CharacterController_H_
#define _CharacterController_H_

#define str(s) #s

#include "Behaviour/Behaviour.h"
#include "Reflection/Reflection.h"
#include "Reflection/ReflectionTypeErasure.h"

#define DllExport   __declspec( dllexport )

namespace Dystopia
{
	class RigidBody;
	class InputManager;

	class CharacterController : Behaviour
	{
	public:

		static constexpr const char * BehaviourName = str(CharacterController);
#if !EDITOR
		
		using SYSTEM = BehaviourSystem;
		// unsigned GetBehaviourType(void) const
		// {
		// 	return Ut::MetaFind_t<Ut::Decay_t<decltype(*this)>, AllBehaviours>::value;
		// };

#endif
		virtual const std::string GetEditorName(void) const override { return "CharacterController"; }
		static uint64_t constexpr mCharacterControllerID = 1496323584;

		CharacterController();
		~CharacterController();
		
		virtual void Load(void) override;
		virtual void Awake(void) override;
		virtual void Init(void) override;

		virtual void Update(const float _fDeltaTime) override;
		virtual void FixedUpdate(const float _fDeltaTime) override;
		virtual void PostUpdate(void) override;

		virtual void GameObjectDestroy(void) override;
		virtual void Unload(void) override;

		virtual void OnCollisionEnter(const CollisionEvent&);
		virtual void OnCollisionStay (const CollisionEvent&);
		virtual void OnCollisionExit (const CollisionEvent&);

		virtual void OnTriggerEnter(const GameObject *);
		virtual void OnTriggerStay (const GameObject *);
		virtual void OnTriggerExit (const GameObject *);

		virtual void Serialise(TextSerialiser&) const override;
		virtual void Unserialise(TextSerialiser&) override;

		virtual const char * const GetBehaviourName() const;

		virtual CharacterController * Duplicate() const;
		
		virtual void EditorUI(void) noexcept override;
		
		// Reflection Stuff
		virtual TypeErasure::TypeEraseMetaData       GetMetaData();
		virtual TypeErasure::TypeEraseMetaData const GetMetaData() const;

		// Member Functions
		void CheckMoving();
		void CheckAttack();
		void MovePlayer(float);
		void CastLinked(int, bool);
		void TakeDamage(int _dmg);

	private:
		friend MetaData<CharacterController>;

		PP_MEMBERFUNC(Dystopia::CharacterController, TakeDamage)

	// Member Variables
	public: 
		int playerHealth;
		bool IsDodging;
		float CharacterSpeed;
		float JumpForce;
		int attackCount;
		float attackDelay;
		GameObject * combatName;
		GameObject * sManagerName;
	private:
		bool mbIsGrounded;
		bool mbIsCeilinged;
		bool mbIsFacingRight;
		bool isAttacking;
		bool currentType;

		RigidBody * mpBody;
		InputManager * mpInputSys;
	};

	extern "C"
	{
		DllExport CharacterController * CharacterControllerClone()
		{
			return new CharacterController;
		}
	}
}

PP_REFLECT(Dystopia::playerHealth, CharacterController, IsDodging, CharacterSpeed, JumpForce, attackCount, attackDelay);

#endif //_CharacterController_H_


