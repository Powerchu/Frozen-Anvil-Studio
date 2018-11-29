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
#include "Behaviour/BehaviourMemberFunc.h"

#define DllExport   __declspec( dllexport )

namespace Dystopia
{
	class RigidBody;
	class InputManager;

	class CharacterController : public Behaviour
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

		virtual void OnTriggerEnter(GameObject * const);
		virtual void OnTriggerStay (GameObject * const);
		virtual void OnTriggerExit (GameObject * const);

		virtual void Serialise(TextSerialiser&) const override;
		virtual void Unserialise(TextSerialiser&) override;

		virtual const char * const GetBehaviourName() const;

		virtual CharacterController * Duplicate() const;
#if EDITOR
		virtual void EditorUI(void) noexcept override;
#endif
		
		// Reflection Stuff
		virtual TypeErasure::TypeEraseMetaData       GetMetaData();
		virtual TypeErasure::TypeEraseMetaData const GetMetaData() const;

		// Member Functions
		void CheckMoving();
		void CheckAttack();
		void MovePlayer(float);
		void CastLinked(int _skill, bool _isForm, float x, float y, float z);
		void TakeDamage(int _dmg);

		PP_MEMBERFUNC(Dystopia::CharacterController, TakeDamage)
	private:
		friend MetaData<CharacterController>;



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
		inline DllExport CharacterController * CharacterControllerClone()
		{
			return new CharacterController;
		}
	}
}

PP_REFLECT(Dystopia::CharacterController, playerHealth, IsDodging, CharacterSpeed, JumpForce, attackCount);

#endif //_CharacterController_H_


