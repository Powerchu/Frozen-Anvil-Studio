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

	class HungerCheck : public NeuralTree::Leaf
	{
	public:
		//HungerCheck(NeuralTree::Blackboard::Ptr _ptr) : Leaf(_ptr) {}
		eStatus Update() override
		{
			const auto a = mpBlackboard->getFloat("Hunger");
			if (a < 10.0F)
			{
				DEBUG_PRINT(eLog::MESSAGE, "I am hungry. Hunger: %f", a);
				return eStatus::SUCCESS;
			}	
			else
			{ 
				DEBUG_PRINT(eLog::MESSAGE, "I am not hungry. Hunger: %f", a);
				return eStatus::FAIL;
			}
		}
	};

	class HaveFood : public NeuralTree::Leaf
	{
	public:
		//HaveFood(NeuralTree::Blackboard::Ptr _ptr) : Leaf(_ptr) {}
		eStatus Update() override
		{
			if (mpBlackboard->getInt("Food") == 0)
			{
				DEBUG_PRINT(eLog::MESSAGE, "I have no food.");
				return eStatus::FAIL;
			}
			else
			{
				DEBUG_PRINT(eLog::MESSAGE, "I will eat now.");
				return eStatus::SUCCESS;
			}
		}
	};

	class FindFood : public NeuralTree::Leaf
	{
	public:
		//FindFood(NeuralTree::Blackboard::Ptr _ptr) : Leaf(_ptr) {}
		eStatus Update() override
		{
			DEBUG_PRINT(eLog::MESSAGE, "Finding Food");
			const auto a = mpBlackboard->getInt("Food");
			mpBlackboard->setInt("Food", a + 1);
			DEBUG_PRINT(eLog::MESSAGE, "I have %d food now.", a);
			return eStatus::SUCCESS;
		}
	};

	class EatFood : public NeuralTree::Leaf
	{
	public:
		//EatFood(NeuralTree::Blackboard::Ptr _ptr) : Leaf(_ptr) {}
		eStatus Update() override
		{
			DEBUG_PRINT(eLog::MESSAGE, "Eating food");
			const auto a = mpBlackboard->getFloat("Hunger");
			const auto b = mpBlackboard->getInt("Food");
			mpBlackboard->setInt("Food", b - 1);
			mpBlackboard->setFloat("Hunger", a + 30.0F);
			DEBUG_PRINT(eLog::MESSAGE, "Hunger: ", a);
			return eStatus::SUCCESS;
		}
	};

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
		void Load(void) override;
		void Init(void) override;
		void Update(float _dt);
		void Unload(void) override;

		Component* Duplicate() const override;

		void Serialise(TextSerialiser&) const override;
		void Unserialise(TextSerialiser&) override;
		void EditorUI(void) noexcept override;

		void CheckGroundCeiling(void);
		void MovePlayer(float _dt);
	private:
		RigidBody * mpBody;
		Collider  * mpCol;
		bool		mbIsFacingRight;
		bool		mbIsGrounded;
		bool		mbIsCeilinged;
		float		mfCharacterSpeed;
		float		mfJumpForce;

		NeuralTree::BehaviourTree btree;

		/*=================Editor Stuff=====================*/
#if EDITOR
		void UI_Debug();
		void UI_CharacterSpeed();
		void UI_CharacterJumpForce();

#endif // EDITOR
	};
}

#endif //CHARACTER_CONTROLLER_H

