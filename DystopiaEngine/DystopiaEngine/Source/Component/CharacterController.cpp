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
		_out << mnOwner;					// gObjID
		_out.InsertEndBlock("Character_Controller");
	}

	void CharacterController::Unserialise(TextSerialiser& _in)
	{
		_in.ConsumeStartBlock();
		_in >> mnOwner;						// gObjID
		_in.ConsumeEndBlock();

		auto sceneSys = EngineCore::GetInstance()->GetSystem<SceneSystem>();

		// technically should unserialize into next scene 
		GameObject* owner = sceneSys->GetNextScene().FindGameObject(mnOwner);
		if (owner)
		{
			// dont need init cuz next scene will get init-ed when the scene inits
			owner->AddComponent(this, CharacterController::TAG{});
		}
		else
		{
			owner = sceneSys->GetCurrentScene().FindGameObject(mnOwner);
			if (owner)
			{
				// in case of reloading current scene, then need re-init
				owner->AddComponent(this, CharacterController::TAG{});
				owner->Init();
			}
		}
	}

	void CharacterController::EditorUI() noexcept
	{
	}

	void CharacterController::MovePlayer(float)
	{
		if (mpBody == nullptr) return;
		if (EngineCore::GetInstance()->GetSystem<InputManager>()->IsKeyPressed(eUserButton::BUTTON_LEFT))
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

