#include "Component/CharacterController.h"
#include "Object/GameObject.h"
#include "Object/ObjectFlags.h"
#include "Component/RigidBody.h"
#include "Component/Collider.h"
#include "System/Physics/PhysicsSystem.h"
#include "System/Input/InputSystem.h"
#include "System/Base/ComponentDonor.h"
#include "IO/TextSerialiser.h"
#include "System/Scene/SceneSystem.h"
#if EDITOR
#include "Editor/ProjectResource.h"
#include "Editor/EGUI.h"
#include "Editor/Editor.h"
#endif 

namespace Dystopia
{
	CharacterController::CharacterController()
		:mpBody(nullptr)
		,mpCol(nullptr)
		,mbIsFacingRight(true)
		,mbIsGrounded(false)
		,mbIsCeilinged(false)
		,mfCharacterSpeed(30.0f)
		,mfJumpForce(100.0F)
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
		const auto tInput = EngineCore::GetInstance()->GetSystem<InputManager>();

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

		if (tInput->IsController())
		{
			tInput->MapButton("Run Left", eButton::XBUTTON_DPAD_LEFT);
			tInput->MapButton("Run Right", eButton::XBUTTON_DPAD_RIGHT);
			tInput->MapButton("Jump", eButton::XBUTTON_A);
			tInput->MapButton("Fly", eButton::XBUTTON_B);
		}
		else
		{
			tInput->MapButton("Run Left", eButton::KEYBOARD_LEFT);
			tInput->MapButton("Run Right", eButton::KEYBOARD_RIGHT);
			tInput->MapButton("Jump", eButton::KEYBOARD_SPACEBAR);
			tInput->MapButton("Fly", eButton::KEYBOARD_UP);
		}
	}

	void CharacterController::Update(const float _dt)
	{
		MovePlayer(_dt);

		
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
		_out << mfCharacterSpeed;
		_out << mfJumpForce;
		_out.InsertEndBlock("Character_Controller");
	}

	void CharacterController::Unserialise(TextSerialiser& _in)
	{
		_in.ConsumeStartBlock();
		Component::Unserialise(_in);
		_in >> mfCharacterSpeed;
		_in >> mfJumpForce;
		_in.ConsumeEndBlock();

		Init();
	}

	void CharacterController::EditorUI() noexcept
	{
		UI_Debug();
		UI_CharacterSpeed();
		UI_CharacterJumpForce();
	}

	void CharacterController::CheckGroundCeiling()
	{

	}

	/*
	 * Helper Functions
	 */
	void CharacterController::MovePlayer(float)
	{
		if (mpBody == nullptr) return;
		if (EngineCore::GetInstance()->GetSystem<InputManager>()->IsKeyPressed("Run Left"))
		{
			const auto tScale = GetOwner()->GetComponent<Transform>()->GetGlobalScale();

			mpBody->AddLinearImpulse({ -1 * mfCharacterSpeed * mpBody->GetMass(),0,0 });

			if (mbIsFacingRight)
			{
				GetOwner()->GetComponent<Transform>()->SetScale(-tScale.x, tScale.y, tScale.z);
				mbIsFacingRight = false;
			}
		}

		if (EngineCore::GetInstance()->GetSystem<InputManager>()->IsKeyPressed("Run Right"))
		{
			const auto tScale = GetOwner()->GetComponent<Transform>()->GetGlobalScale();

			mpBody->AddLinearImpulse({mfCharacterSpeed * mpBody->GetMass(),0,0 });

			if (!mbIsFacingRight)
			{
				GetOwner()->GetComponent<Transform>()->SetScale(-tScale.x, tScale.y, tScale.z);
				mbIsFacingRight = true;
			}
		}

		if (EngineCore::GetInstance()->GetSystem<InputManager>()->IsKeyPressed("Fly"))
		{
			mpBody->AddForce({ 0,1000 * mpBody->GetMass(),0 });
		}

		if (EngineCore::GetInstance()->GetSystem<InputManager>()->IsKeyTriggered("Jump"))
		{
			mpBody->AddLinearImpulse({ 0,mfJumpForce * mpBody->GetMass() * 10,0 });
		}
	}

	/*
	 * EDITOR UI STUFF
	 */

	void CharacterController::UI_Debug()
	{
		EGUI::Display::CheckBox("Is Facing Right		  ", &mbIsFacingRight);
		EGUI::Display::CheckBox("Is Grounded			  ", &mbIsGrounded);
		EGUI::Display::CheckBox("Is Ceilinged			  ", &mbIsCeilinged);
	}

	void CharacterController::UI_CharacterSpeed()
	{
		switch (EGUI::Display::DragFloat("Move Speed			  ", &mfCharacterSpeed, 0.01f, FLT_EPSILON, FLT_MAX))
		{
		case EGUI::eDragStatus::eNO_CHANGE:
		case EGUI::eDragStatus::eDRAGGING:
			break;
		case EGUI::eDragStatus::eEND_DRAG:
		case EGUI::eDragStatus::eENTER:
		case EGUI::eDragStatus::eDEACTIVATED:
		case EGUI::eDragStatus::eTABBED:
			EGUI::GetCommandHND()->EndRecording();
			break;
		case EGUI::eDragStatus::eSTART_DRAG:
			EGUI::GetCommandHND()->StartRecording<RigidBody>(mnOwner, &mfCharacterSpeed);
			break;
		default:
			break;
		}
	}

	void CharacterController::UI_CharacterJumpForce()
	{
		switch (EGUI::Display::DragFloat("Jump Height			  ", &mfJumpForce, 0.01f, FLT_EPSILON, FLT_MAX))
		{
		case EGUI::eDragStatus::eNO_CHANGE:
		case EGUI::eDragStatus::eDRAGGING:
			break;
		case EGUI::eDragStatus::eEND_DRAG:
		case EGUI::eDragStatus::eENTER:
		case EGUI::eDragStatus::eDEACTIVATED:
		case EGUI::eDragStatus::eTABBED:
			EGUI::GetCommandHND()->EndRecording();
			break;
		case EGUI::eDragStatus::eSTART_DRAG:
			EGUI::GetCommandHND()->StartRecording<RigidBody>(mnOwner, &mfJumpForce);
			break;
		default:
			break;
		}
	}
}

