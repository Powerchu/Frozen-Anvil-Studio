#include "Component/CharacterController.h"
#include "Object/GameObject.h"
#include "Component/RigidBody.h"
#include "System/Physics/PhysicsSystem.h"
#include "System/Input/InputSystem.h"
#include "System/Logger/LoggerSystem.h"
#include "System/Scene/SceneSystem.h"
#include "System/Scene/Scene.h"
#include "System/Base/ComponentDonor.h"
#if EDITOR
#include "IO/TextSerialiser.h"
#include "Editor/ProjectResource.h"
#include "Editor/EGUI.h"
#endif 
namespace Dystopia
{
	CharacterController::CharacterController()
	{
	}


	CharacterController::~CharacterController()
	{
	}

	void CharacterController::Load()
	{
	}

	void CharacterController::Init()
	{
		if (nullptr != GetOwner())
		{
			if (nullptr != GetOwner()->GetComponent<RigidBody>())
			{
				const auto rb = EngineCore::GetInstance()->GetSystem<PhysicsSystem>()->RequestComponent();
				GetOwner()->AddComponent(rb, RigidBody::TAG{});
			}

			mpBody = GetOwner()->GetComponent<RigidBody>();
		}
	}

	void CharacterController::Unload()
	{
	}

	Component* CharacterController::Duplicate() const
	{
		return nullptr;
	}

	void CharacterController::Serialise(TextSerialiser&) const
	{
	}

	void CharacterController::Unserialise(TextSerialiser&)
	{
	}

	void CharacterController::EditorUI() noexcept
	{
	}

	void CharacterController::MovePlayer(float)
	{
		if (mpBody == nullptr) return;
		if (EngineCore::GetInstance()->GetSystem<InputManager>()->IsKeyPressed(eUserButton::BUTTON_LEFT))
		{
			mpBody->AddForce({ -10,0,0 });
		}

		if (EngineCore::GetInstance()->GetSystem<InputManager>()->IsKeyPressed(eUserButton::BUTTON_RIGHT))
		{
			mpBody->AddForce({ 10,0,0 });

		}

		if (EngineCore::GetInstance()->GetSystem<InputManager>()->IsKeyPressed(eUserButton::BUTTON_UP))
		{
			mpBody->AddForce({ 0,20,0 });
		}

		if (EngineCore::GetInstance()->GetSystem<InputManager>()->IsKeyPressed(eUserButton::BUTTON_SPACEBAR))
		{
			mpBody->AddImpulse({ 0,100,0 });
		}
	}
}

