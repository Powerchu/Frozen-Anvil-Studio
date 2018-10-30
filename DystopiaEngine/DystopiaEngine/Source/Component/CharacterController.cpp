#include "Component/CharacterController.h"
#include "Object/GameObject.h"
#include "Object/ObjectFlags.h"
#include "Component/RigidBody.h"
#include "Component/Collider.h"
#include "Component/SpriteRenderer.h"
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
		: mpBody(nullptr)
		  , mpCol(nullptr)
		  , mbIsFacingRight(true)
		  , mbIsGrounded(false)
		  , mbIsCeilinged(false)
		  , mbIsDodging(false)
		  , mfCharacterSpeed(30.0f)
		  , mfJumpForce(100.0F)
	{
	}


	CharacterController::~CharacterController()	= default;

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

			auto s_rend = GetOwner()->GetComponent<SpriteRenderer>();
			if (s_rend)
			{
				s_rend->SetPlay(true);
			}
			mpBody = GetOwner()->GetComponent<RigidBody>();
		}

		if (tInput->IsController())
		{
			tInput->MapButton("Run Left", eButton::XBUTTON_DPAD_LEFT);
			tInput->MapButton("Run Right", eButton::XBUTTON_DPAD_RIGHT);
			tInput->MapButton("Jump", eButton::XBUTTON_A);
			tInput->MapButton("Fly", eButton::XBUTTON_DPAD_UP);
			tInput->MapButton("Fireball", eButton::XBUTTON_B);
			tInput->MapButton("Missle", eButton::XBUTTON_Y);
		}
		else
		{
			tInput->MapButton("Run Left", eButton::KEYBOARD_LEFT);
			tInput->MapButton("Run Right", eButton::KEYBOARD_RIGHT);
			tInput->MapButton("Jump", eButton::KEYBOARD_SPACEBAR);
			tInput->MapButton("Fly", eButton::KEYBOARD_UP);
			tInput->MapButton("Fireball", eButton::KEYBOARD_C);
			tInput->MapButton("Missle", eButton::KEYBOARD_V);
		}

		
	}

	void CharacterController::Update(const float _dt)
	{
		MovePlayer(_dt);
		CheckGroundCeiling();
		CheckMoving();
	}

	void CharacterController::Unload()
	{
		auto s_rend = GetOwner()->GetComponent<SpriteRenderer>();
		if (s_rend)
		{
			s_rend->SetPlay(false);
		}
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
		const auto my_body = GetOwner()->GetComponent<RigidBody>();
		const auto my_col = GetOwner()->GetComponent<Collider>();

		if (nullptr != my_col)
		{
			if (!my_col->IsTrigger())
			{
				for (auto& manifold : my_col->GetCollisionEvents())
				{
					auto * CollidedWith = EngineCore::GetInstance()->Get<SceneSystem>()->GetActiveScene().FindGameObject(manifold.mOtherID);
					if (nullptr == CollidedWith) continue;
					if (CollidedWith->GetComponent<Collider>()->IsTrigger()) continue;

					const auto dotNormal = manifold.mEdgeNormal.Dot({ 1,0,0 });
					if (dotNormal < 0.98F && dotNormal > -0.98F)
					{
						if (my_body->GetLinearVelocity().y > FLT_EPSILON)
						{
							//mbIsCeilinged = true;
						}
						else
						{
							mbIsGrounded = true;
						}
					}
				}
			}
		}
	}

	/*
	 * Helper Functions
	 */
	void CharacterController::MovePlayer(float)
	{
		if (mpBody == nullptr) return;
		const auto tInput = EngineCore::GetInstance()->GetSystem<InputManager>();
		const auto tScale = GetOwner()->GetComponent<Transform>()->GetGlobalScale();

		if (tInput->IsController())
		{
			const auto leftThumb = tInput->GetAnalogX(0);
			const auto leftTriggerFloat = tInput->GetTriggers(0);

			if (leftThumb < -0.1F) // Moving Left
			{
				mpBody->AddLinearImpulse({ leftThumb * mfCharacterSpeed * mpBody->GetMass(),0,0 });
				if (mbIsFacingRight)
				{
					GetOwner()->GetComponent<Transform>()->SetScale(-tScale.x, tScale.y, tScale.z);
					mbIsFacingRight = false;
				}
			}
			else if (leftThumb > 0.1F)// Moving Right
			{
				mpBody->AddLinearImpulse({ leftThumb * mfCharacterSpeed * mpBody->GetMass(),0,0 });
				if (!mbIsFacingRight)
				{
					GetOwner()->GetComponent<Transform>()->SetScale(-tScale.x, tScale.y, tScale.z);
					mbIsFacingRight = true;
				}
			}

			const auto side = mbIsFacingRight ? 1.F : -1.F;

			if (leftTriggerFloat > 0.2F && mbIsGrounded)
			{
				if (!mbIsDodging)
				{
					mbIsDodging = true;
					mpBody->AddLinearImpulse({ side * mfCharacterSpeed * mpBody->GetMass() * 20 * leftTriggerFloat,0,0 });
				}
				else
				{
					if (Math::Abs(float(mpBody->GetLinearVelocity().x)) > 0.01F)
					{
						tInput->SetVibrate(static_cast<unsigned short>(Math::Min(leftTriggerFloat + 0.1F, 1.0F) * 32000) 
										  ,static_cast<unsigned short>(Math::Min(leftTriggerFloat + 0.1F, 1.0F) * 16000));
					}
					else
					{
						tInput->StopVibrate();
					}
				}
				
			}
			else
			{
				mbIsDodging = false;
				tInput->StopVibrate();
			}
			
		}

		if (tInput->IsKeyPressed("Run Left"))
		{

			mpBody->AddLinearImpulse({ -1 * mfCharacterSpeed * mpBody->GetMass(),0,0 });

			if (mbIsFacingRight)
			{
				GetOwner()->GetComponent<Transform>()->SetScale(-tScale.x, tScale.y, tScale.z);
				mbIsFacingRight = false;
			}
		}

		if (tInput->IsKeyPressed("Run Right"))
		{
			mpBody->AddLinearImpulse({mfCharacterSpeed * mpBody->GetMass(),0,0 });

			if (!mbIsFacingRight)
			{
				GetOwner()->GetComponent<Transform>()->SetScale(-tScale.x, tScale.y, tScale.z);
				mbIsFacingRight = true;
			}
		}

		if (tInput->IsKeyPressed("Fly"))
		{
			mpBody->AddForce({ 0,100 * mpBody->GetMass(),0 });
		}

		if (tInput->IsKeyPressed("Jump"))
		{
			if (mbIsGrounded)
			{
				mpBody->AddLinearImpulse({ 0,mfJumpForce * mpBody->GetMass() * 10,0 });
				mbIsGrounded = false;
			}
		}
		/*IM SORRY AARON - FROM (KEITH)*/
		if (tInput->IsKeyTriggered("Fireball"))
		{
			if (!mbIsFacingRight)
			{
				if (const auto ptr = EngineCore::GetInstance()->Get<SceneSystem>()->Instantiate("Fireball.dobj", GetOwner()->GetComponent<Transform>()->GetPosition() + Math::Vec3D{-40,-10,0}))
				{
					if (auto rigidptr = ptr->GetComponent<RigidBody>())
					{

						auto scale = ptr->GetComponent<Transform>()->GetGlobalScale();
						ptr->GetComponent<Transform>()->SetScale(-scale.x, scale.y, scale.z);
						rigidptr->AddLinearImpulse({ -300 * rigidptr->GetMass(),20*rigidptr->GetMass(),0 });
					}

				}
			}
			else
			{
				if (const auto ptr = EngineCore::GetInstance()->Get<SceneSystem>()->Instantiate("Fireball.dobj", GetOwner()->GetComponent<Transform>()->GetPosition() + Math::Vec3D{ 40,-10,0 }))
				{
					if (auto rigidptr = ptr->GetComponent<RigidBody>())
					{

						rigidptr->AddLinearImpulse({ 300 * rigidptr->GetMass(),20 * rigidptr->GetMass(),0 });
					}

				}
			}

		}
		if (tInput->IsKeyTriggered("Missle"))
		{
			if (!mbIsFacingRight)
			{

				if (auto ptr = EngineCore::GetInstance()->Get<SceneSystem>()->Instantiate("Missle.dobj", GetOwner()->GetComponent<Transform>()->GetPosition() + Math::Vec3D{ -10,0,0 }))
				{
					auto scale = ptr->GetComponent<Transform>()->GetGlobalScale();
					auto x = scale.x / 2;
					ptr->GetComponent<Transform>()->SetGlobalPosition(ptr->GetComponent<Transform>()->GetGlobalPosition() + Math::Vec3D{ -x,0,0 });
					ptr->GetComponent<Transform>()->SetScale(-scale.x, scale.y, scale.z);
				}
			}
			else
			{
				if (auto ptr = EngineCore::GetInstance()->Get<SceneSystem>()->Instantiate("Missle.dobj", GetOwner()->GetComponent<Transform>()->GetPosition() + Math::Vec3D{ 10,0,0 }))
				{
					auto scale = ptr->GetComponent<Transform>()->GetGlobalScale();
					auto x = scale.x / 2;
					ptr->GetComponent<Transform>()->SetGlobalPosition(ptr->GetComponent<Transform>()->GetGlobalPosition() + Math::Vec3D{ x,0,0 });
				

				}
			}

		}
	}

	void CharacterController::CheckMoving()
	{
		static bool isRunning = false;

		auto s_rend = GetOwner()->GetComponent<SpriteRenderer>();
		if( s_rend)
		{
			if (Math::Abs(float(mpBody->GetLinearVelocity().x)) > 0.5F) // moving
			{
				if (!isRunning)
				{
					s_rend->SetSpeed(0.080F);
					s_rend->SetAnimation("Running");
					isRunning = true;
				}
			}
			else // idling
			{
				if (isRunning)
				{
					s_rend->SetSpeed(0.16F);
					s_rend->SetAnimation("Idle");
					isRunning = false;
				}
			}
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
			EGUI::GetCommandHND()->StartRecording<CharacterController>(mnOwner, &CharacterController::mfCharacterSpeed);
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
			EGUI::GetCommandHND()->StartRecording<CharacterController>(mnOwner, &CharacterController::mfJumpForce);
			break;
		default:
			break;
		}
	}
}

