#include "Component/CharacterController.h"
#include "Object/GameObject.h"
#include "Object/ObjectFlags.h"
#include "Component/RigidBody.h"
#include "System/Physics/PhysicsSystem.h"
#include "System/Input/InputSystem.h"
#include "System/Logger/LoggerSystem.h"
#include "System/Scene/SceneSystem.h"
#include "System/Scene/Scene.h"
#include "System/Base/ComponentDonor.h"
#include "IO/TextSerialiser.h"
#if EDITOR
#include "Editor/ProjectResource.h"
#include "Editor/EGUI.h"
#include "Editor/Editor.h"
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
			if (nullptr == GetOwner()->GetComponent<RigidBody>())
			{
				auto rb = EngineCore::GetInstance()->GetSystem<PhysicsSystem>()->RequestComponent();
				GetOwner()->AddComponent(rb, RigidBody::TAG{});
				rb->SetOwner(GetOwner());
				rb->Init();
			}

			mpBody = GetOwner()->GetComponent<RigidBody>();
		}
	}

	void CharacterController::Unload()
	{
	}

	Component* CharacterController::Duplicate() const
	{
		const auto cc = EngineCore::GetInstance()->GetSystem<InputManager>()->RequestComponent();
		cc->SetOwner(GetOwner());
		cc->Init();
		return cc;
	}

	void CharacterController::Serialise(TextSerialiser& _out) const
	{
		_out.InsertStartBlock("Character_Controller");
		Component::Serialise(_out);
		_out.InsertEndBlock("Character_Controller");
	}

	void CharacterController::Unserialise(TextSerialiser& _in)
	{
		_in.ConsumeStartBlock();
		Component::Unserialise(_in);
		_in.ConsumeEndBlock();
	}

	void CharacterController::EditorUI() noexcept
	{
	}

	void CharacterController::MovePlayer(float)
	{
		if (mpBody == nullptr) return;
		if (EngineCore::GetInstance()->GetSystem<InputManager>()->IsKeyPressed(eButton::KEYBOARD_LEFT))
		{
			mpBody->AddImpulse({ -20,0,0 });
		}

		if (EngineCore::GetInstance()->GetSystem<InputManager>()->IsKeyPressed(eUserButton::BUTTON_RIGHT))
		{
			mpBody->AddImpulse({ 20,0,0 });

		}

		if (EngineCore::GetInstance()->GetSystem<InputManager>()->IsKeyPressed(eUserButton::BUTTON_UP))
		{
			mpBody->AddImpulse({ 0,20,0 });
		}

		if (EngineCore::GetInstance()->GetSystem<InputManager>()->IsKeyTriggered(eUserButton::BUTTON_SPACEBAR))
		{
			mpBody->AddImpulse({ 0,300*mpBody->GetGravityScalar(),0 });
		}
	}
}

