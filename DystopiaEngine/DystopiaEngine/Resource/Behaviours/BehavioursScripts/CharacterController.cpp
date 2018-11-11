/* HEADER *********************************************************************************/
/*!
\file	CharacterController.cpp
\author Aaron Chu (100%)
\par    email: m.chu\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "CharacterController.h"
#include "System/Input/InputSystem.h"
#include "System/Input/InputMap.h"
#include "System/Physics/PhysicsSystem.h"
#include "System/Collision/CollisionEvent.h"
#include "System/Collision/CollisionSystem.h"
#include "System/Scene/SceneSystem.h"
#include "System/Driver/Driver.h"
#include "Editor/EGUI.h"
#include "Utility/DebugAssert.h"
#include "Math/MathLib.h"

#include "Object/GameObject.h"

#include "Component/RigidBody.h"
#include "Component/SpriteRenderer.h"
#include "Component/Transform.h"

namespace Dystopia
{
	CharacterController::CharacterController()
		: mbIsFacingRight(true)
		, mbIsGrounded(false)
		, mbIsCeilinged(false)
		, IsDodging(false)
		, CharacterSpeed(25.0f)
		, JumpForce(60.0F)
		, mpBody(nullptr)
		, mpInputSys(nullptr)
	{
	}

	CharacterController::~CharacterController()
	{
	}

	void CharacterController::Load()
	{
	}

	void CharacterController::Awake()
	{
		mpInputSys = EngineCore::GetInstance()->GetSystem<InputManager>();

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

		if (mpInputSys->IsController())
		{
			mpInputSys->MapButton("Run Left", eButton::XBUTTON_DPAD_LEFT);
			mpInputSys->MapButton("Run Right", eButton::XBUTTON_DPAD_RIGHT);
			mpInputSys->MapButton("Jump", eButton::XBUTTON_A);
			mpInputSys->MapButton("Fly", eButton::XBUTTON_DPAD_UP);
			mpInputSys->MapButton("Fireball", eButton::XBUTTON_B);
			mpInputSys->MapButton("Missile", eButton::XBUTTON_Y);
		}
		else
		{
			mpInputSys->MapButton("Run Left", eButton::KEYBOARD_LEFT);
			mpInputSys->MapButton("Run Right", eButton::KEYBOARD_RIGHT);
			mpInputSys->MapButton("Jump", eButton::KEYBOARD_SPACEBAR);
			mpInputSys->MapButton("Fly", eButton::KEYBOARD_UP);
			mpInputSys->MapButton("Fireball", eButton::KEYBOARD_C);
			mpInputSys->MapButton("Missile", eButton::KEYBOARD_V);
		}
	}

	void CharacterController::Init()
	{ 
		
	}

	void CharacterController::Update(const float _fDeltaTime)
	{
		MovePlayer(_fDeltaTime);
		CheckMoving();
	}

	void CharacterController::FixedUpdate(const float _fDeltaTime)
	{
	}
	
	void CharacterController::PostUpdate(void)
	{
	}

	void CharacterController::GameObjectDestroy(void)
	{
	}

	void CharacterController::Unload(void)
	{
	}

	void Dystopia::CharacterController::OnCollisionEnter(const CollisionEvent& _colEvent)
	{
		const float dotNormal = _colEvent.mEdgeNormal.Dot({ 0.0F,-1.0F,0.0F });
		//DEBUG_PRINT(eLog::MESSAGE, "DotNormal: %f", dotNormal);	
		if (dotNormal > 0.65F && dotNormal < 1.1F)
		{
			mbIsGrounded = true;	
		}
	}

	void Dystopia::CharacterController::OnCollisionStay(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::CharacterController::OnCollisionExit(const CollisionEvent& _colEvent)
	{
		
		
		
	}

	void Dystopia::CharacterController::OnTriggerEnter(const GameObject * _obj)
	{
	}

	void Dystopia::CharacterController::OnTriggerStay(const GameObject * _obj)
	{
	}

	void Dystopia::CharacterController::OnTriggerExit(const GameObject * _obj)
	{
	}

	CharacterController * CharacterController::Duplicate() const
	{
		return new CharacterController{};
	}

	void CharacterController::Serialise(TextSerialiser& _ser) const
	{
	}

	void CharacterController::Unserialise(TextSerialiser& _ser)
	{
	}


	const char * const CharacterController::GetBehaviourName() const
	{
		return CharacterController::BehaviourName;
	}

	void CharacterController::EditorUI(void) noexcept
	{
		
		
	}

	/*
	 * Helper Functions
	 */
	void CharacterController::MovePlayer(float)
	{
		if (mpBody == nullptr) return;
		const auto tScale = GetOwner()->GetComponent<Transform>()->GetGlobalScale();

		if (mpInputSys->IsController())
		{
			const auto leftThumb = mpInputSys->GetAnalogX(0);
			const auto leftTriggerFloat = mpInputSys->GetTriggers(0);

			if (leftThumb < -0.1F) // Moving Left
			{
				mpBody->AddLinearImpulse({ leftThumb * CharacterSpeed * mpBody->GetMass(),0,0 });
				if (mbIsFacingRight)
				{
					GetOwner()->GetComponent<Transform>()->SetScale(-tScale.x, tScale.y, tScale.z);
					mbIsFacingRight = false;
				}
			}
			else if (leftThumb > 0.1F)// Moving Right
			{
				mpBody->AddLinearImpulse({ leftThumb * CharacterSpeed * mpBody->GetMass(),0,0 });
				if (!mbIsFacingRight)
				{
					GetOwner()->GetComponent<Transform>()->SetScale(-tScale.x, tScale.y, tScale.z);
					mbIsFacingRight = true;
				}
			}

			const auto side = mbIsFacingRight ? 1.F : -1.F;

			if (leftTriggerFloat > 0.2F && mbIsGrounded)
			{
				if (!IsDodging)
				{
					IsDodging = true;
					mpBody->AddLinearImpulse({ side * CharacterSpeed * mpBody->GetMass() * 20 * leftTriggerFloat,0,0 });
				}
				else
				{
					if (Math::Abs(float(mpBody->GetLinearVelocity().x)) > 0.01F)
					{
						mpInputSys->SetVibrate(static_cast<unsigned short>(Math::Min(leftTriggerFloat + 0.1F, 1.0F) * 32000) 
										  ,static_cast<unsigned short>(Math::Min(leftTriggerFloat + 0.1F, 1.0F) * 16000));
					}
					else
					{
						mpInputSys->StopVibrate();
					}
				}	
			}
			else
			{
				IsDodging = false;
				mpInputSys->StopVibrate();
			}
		} // Controller Scheme end

		if (mpInputSys->IsKeyPressed("Run Left") && !mpInputSys->IsKeyPressed("Run Right"))
		{			
			mpBody->AddLinearImpulse({ -1 * CharacterSpeed * mpBody->GetMass(),0,0 });

			if (mbIsFacingRight && Math::Abs(static_cast<float>(mpBody->GetLinearVelocity().x)) > 5.0F)
			{
				mbIsFacingRight = false;
				GetOwner()->GetComponent<Transform>()->SetScale(-tScale.x, tScale.y, tScale.z);
			}
		}

		if (mpInputSys->IsKeyPressed("Run Right") && !mpInputSys->IsKeyPressed("Run Left"))
		{
			mpBody->AddLinearImpulse({CharacterSpeed * mpBody->GetMass(),0,0 });

			if (!mbIsFacingRight && Math::Abs(static_cast<float>(mpBody->GetLinearVelocity().x)) > 5.0F)
			{
				mbIsFacingRight = true;
				GetOwner()->GetComponent<Transform>()->SetScale(-tScale.x, tScale.y, tScale.z);
			}
		}

		if (mpInputSys->IsKeyPressed("Fly"))
		{
			mpBody->AddForce({ 0,100 * mpBody->GetMass(),0 });
		}

		if (mpInputSys->IsKeyPressed("Jump"))
		{
			if (mbIsGrounded)
			{
				mpBody->AddLinearImpulse({ 0,JumpForce * mpBody->GetMass() * 10 + mpBody->GetLinearVelocity().y, 0 });
				mbIsGrounded = false;
			}
		}
		
		if (mpInputSys->IsKeyTriggered("Fireball"))
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

		if (mpInputSys->IsKeyTriggered("Missile"))
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
		auto s_rend = GetOwner()->GetComponent<SpriteRenderer>();
		if( s_rend)
		{
			if (mpInputSys->IsKeyTriggered("Run Left") || mpInputSys->IsKeyTriggered("Run Right"))
			{
				s_rend->SetSpeed(0.080F);
				s_rend->SetAnimation("Running");
			}

			if (mpInputSys->IsKeyReleased("Run Left") || mpInputSys->IsKeyReleased("Run Right"))
			{
				s_rend->SetSpeed(0.16F);
				s_rend->SetAnimation("Idle");		
			}			
		}
	}
	
	TypeErasure::TypeEraseMetaData CharacterController::GetMetaData()
	{
		static MetaData<Dystopia::CharacterController> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const CharacterController::GetMetaData() const
	{
		static MetaData<Dystopia::CharacterController> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
}




