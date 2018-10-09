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

namespace Dystopia
{
	class RigidBody;
	class InputManager;

	class _DLL_EXPORT CharacterController : public Component
	{
	public:
		using SYSTEM = InputManager;
		unsigned GetComponentType(void) const
		{
			return Ut::MetaFind_t<Ut::Decay_t<decltype(*this)>, AllComponents>::value;
		}

		static const std::string GetCompileName(void) { return "Character Controller"; }
		const std::string GetEditorName(void) const { return GetCompileName(); }

	// ====================================== CONSTRUCTORS ======================================= // 
		CharacterController();
		~CharacterController();;

	// ==================================== PUBLIC FUNCTIONS =======================================//
		void Load(void) override;
		void Init(void) override;

		void Unload(void) override;

		Component* Duplicate() const override;

		void Serialise(TextSerialiser&) const override;
		void Unserialise(TextSerialiser&) override;
		void EditorUI(void) noexcept override;

		void MovePlayer(float _dt);
	private:
		RigidBody * mpBody;
		bool		mbIsFacingRight;
		bool		mbIsGrounded;
		float		mfCharacterSpeed;
	};
}

#endif //CHARACTER_CONTROLLER_H

