/* HEADER *********************************************************************************/
/*!
\file	CharacterController.h
\author Aaron Chu (100%)
\par    email: m.chu\@digipen.edu
\brief
Component to store the information about the object's transformations in space.

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef CHARACTER_CONTROLLER_H
#define CHARACTER_CONTROLLER_H
#include "Component.h"
#include "Component/ComponentList.h"	// CHARACTER CONTROLLER
#include "Behaviour/AI/TreeBuilder.h"
#include "Utility/DebugAssert.h"

namespace Dystopia
{
	class RigidBody;
	class Collider;
	class InputManager;

	class _DLL_EXPORT CharacterController : public Component
	{
	public:
		using SYSTEM = InputManager;
		unsigned GetComponentType(void) const
		{
			return Ut::MetaFind_t<Ut::Decay_t<decltype(*this)>, AllComponents>::value;
		}
		unsigned GetRealComponentType(void) const
		{
			return Ut::MetaFind_t<Ut::Decay_t<decltype(*this)>, UsableComponents>::value;
		};
		static const std::string GetCompileName(void) { return "Character Controller"; }
		const std::string GetEditorName(void) const { return GetCompileName(); }

	// ====================================== CONSTRUCTORS ======================================= // 
		CharacterController();
		~CharacterController();;

	// ==================================== PUBLIC FUNCTIONS =======================================//
		void Load(void);
		void Init(void);
		void Update(float _dt);
		void Unload(void);

		Component* Duplicate() const override;

		void Serialise(TextSerialiser&) const;
		void Unserialise(TextSerialiser&);
		void EditorUI(void) noexcept override;

		void CheckGroundCeiling(void);
		void MovePlayer(float _dt);
	private:
		RigidBody * mpBody;
		Collider  * mpCol;
		bool		mbIsFacingRight;
		bool		mbIsGrounded;
		bool		mbIsCeilinged;
		bool		mbIsDodging;
		float		mfCharacterSpeed;
		float		mfJumpForce;

		/*=================Editor Stuff=====================*/
#if EDITOR
		void UI_Debug();
		void UI_CharacterSpeed();
		void UI_CharacterJumpForce();

#endif // EDITOR
	};
}

#endif //CHARACTER_CONTROLLER_H

