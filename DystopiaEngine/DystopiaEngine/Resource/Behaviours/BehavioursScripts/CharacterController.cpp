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
#include "System/Behaviour/BehaviourSystem.h"
#include "System/Scene/SceneSystem.h"
#include "System/Driver/Driver.h"
#if EDITOR
#include "Editor/EGUI.h"
#endif
#include "Utility/DebugAssert.h"
#include "Math/MathLib.h"

#include "Object/GameObject.h"

#include "Component/RigidBody.h"
#include "Component/SpriteRenderer.h"
#include "Component/Transform.h"
#include "Component/Collider.h"

namespace Dystopia
{
	namespace CharacterController_MSG
	{
		template<typename ... Ts>
		void SendInternalMessage(Behaviour * ptr, const char * _FuncName, Ts ... _Params)
		{
			EngineCore::GetInstance()->Get<BehaviourSystem>()->SendInternalMessage(ptr, _FuncName, _Params...);
		}
		template<typename ... Ts>
		void SendExternalMessage(uint64_t _ObjectID, const char * _FuncName, Ts ... _Params)
		{
			EngineCore::GetInstance()->Get<BehaviourSystem>()->SendExternalMessage(_ObjectID, _FuncName, _Params...);
		}

		template<typename ... Ts>
		void SendExternalMessage(GameObject * _ptr, const char * _FuncName, Ts ... _Params)
		{
			EngineCore::GetInstance()->Get<BehaviourSystem>()->SendExternalMessage(_ptr, _FuncName, _Params...);
		}

		template<typename ... Ts>
		void SendAllMessage(const char * _FuncName, Ts ... _Params)
		{
			EngineCore::GetInstance()->Get<BehaviourSystem>()->SendAllMessage(_FuncName, _Params...);
		}
	}

	CharacterController::CharacterController()
		: mbIsFacingRight(true)
		, mbIsGrounded(false)
		, mbIsCeilinged(false)
		, IsDodging(false)
		, CharacterSpeed(25.0f)
		, JumpForce(60.0F)
		, mpBody(nullptr)
		, mpInputSys(nullptr)
		, attackCount(0)
		, attackDelay(0.0f)
		, isAttacking(false)
		, currentType(true)
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
				//s_rend->SetPlay(true);
			}

			mpBody = GetOwner()->GetComponent<RigidBody>();
		}

		if (mpInputSys->IsController())
		{
			mpInputSys->MapButton("Run Left", eButton::XBUTTON_DPAD_LEFT);
			mpInputSys->MapButton("Run Right", eButton::XBUTTON_DPAD_RIGHT);
			mpInputSys->MapButton("Jump", eButton::XBUTTON_A);
			mpInputSys->MapButton("Fly", eButton::XBUTTON_DPAD_UP);
			mpInputSys->MapButton("Skill B", eButton::XBUTTON_B);
			mpInputSys->MapButton("Skill Y", eButton::XBUTTON_Y);
			mpInputSys->MapButton("Attack", eButton::XBUTTON_X);
			mpInputSys->MapButton("SetForm", eButton::XBUTTON_LEFT_SHOULDER);
			mpInputSys->MapButton("SetForce", eButton::XBUTTON_RIGHT_SHOULDER);
		}
		else
		{
			mpInputSys->MapButton("Run Left", eButton::KEYBOARD_LEFT);
			mpInputSys->MapButton("Run Right", eButton::KEYBOARD_RIGHT);
			mpInputSys->MapButton("Jump", eButton::KEYBOARD_SPACEBAR);
			mpInputSys->MapButton("Fly", eButton::KEYBOARD_UP);
			mpInputSys->MapButton("Skill B", eButton::KEYBOARD_C);
			mpInputSys->MapButton("Skill Y", eButton::KEYBOARD_V);
			mpInputSys->MapButton("Attack", eButton::KEYBOARD_X);
		}

		combatName = EngineCore::GetInstance()->Get<SceneSystem>()->FindGameObject_cstr("Combat Box");
		sManagerName = EngineCore::GetInstance()->Get<SceneSystem>()->FindGameObject_cstr("SkillManager");

		playerHealth = 100;
		SetFlags(FLAG_ACTIVE);
	}

	void CharacterController::Init()
	{ 
		combatName = EngineCore::GetInstance()->Get<SceneSystem>()->FindGameObject_cstr("Combat Box");
		sManagerName = EngineCore::GetInstance()->Get<SceneSystem>()->FindGameObject_cstr("SkillManager");
		SetFlags(FLAG_ACTIVE);  
	}

	void CharacterController::Update(const float _fDeltaTime)
	{
		MovePlayer(_fDeltaTime);
		CheckMoving();
		CheckAttack();

		attackDelay = attackDelay + _fDeltaTime;
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
		const auto colBTrigger = _colEvent.mCollidedWith->GetComponent<Collider>()->IsTrigger();
		const float dotNormal = _colEvent.mEdgeNormal.Dot({ 0.0F,-1.0F,0.0F });
	//	DEBUG_PRINT(eLog::MESSAGE, "DotNormal: %f && Trigger?: %d", dotNormal, colBTrigger);
		if (dotNormal > 0.65F && dotNormal < 1.1F && !colBTrigger)
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

	void Dystopia::CharacterController::OnTriggerEnter(GameObject * const _obj)
	{
	}

	void Dystopia::CharacterController::OnTriggerStay(GameObject * const _obj)
	{
	}

	void Dystopia::CharacterController::OnTriggerExit(GameObject * const _obj)
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
#if EDITOR
	void CharacterController::EditorUI(void) noexcept
	{
		
		
	}
#endif
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
			//mpBody->AddTorque({0, 0,1 * CharacterSpeed});
			if (mbIsFacingRight && Math::Abs(static_cast<float>(mpBody->GetLinearVelocity().x)) > 5.0F)
			{
				mbIsFacingRight = false;
				GetOwner()->GetComponent<Transform>()->SetScale(-tScale.x, tScale.y, tScale.z);
			}
		}

		if (mpInputSys->IsKeyPressed("Run Right") && !mpInputSys->IsKeyPressed("Run Left"))
		{
			mpBody->AddLinearImpulse({CharacterSpeed * mpBody->GetMass(),0,0 });
			//mpBody->AddTorque({0, 0,-1 * CharacterSpeed}); 
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

		if (mpInputSys->IsKeyTriggered("Skill Y") || mpInputSys->IsKeyTriggered("Skill B"))
		{
			const Math::Vec3D spawnLocation = GetOwner()->GetComponent<Transform>()->GetPosition();

			CharacterController::CastLinked(1, currentType, float(spawnLocation.x), float(spawnLocation.y), float(spawnLocation.z));
		}

		if (mpInputSys->IsKeyTriggered("SetForm"))
		{
			currentType = true;
			DEBUG_PRINT(eLog::MESSAGE, "Form!");
		}

		if (mpInputSys->IsKeyTriggered("SetForce"))
		{
			currentType = false;
			DEBUG_PRINT(eLog::MESSAGE, "Force!");
		}

		if (mpInputSys->IsKeyTriggered("Attack"))
		{
			const Math::Vec3D spawnLocation = GetOwner()->GetComponent<Transform>()->GetPosition();

		    if (attackDelay > 0.5f)
			{
				if (combatName)
				{
					if (!mbIsFacingRight)
					{
						CharacterController_MSG::SendExternalMessage(combatName, "DealDamage", 10, mbIsFacingRight);
					}

					else
					{
						CharacterController_MSG::SendExternalMessage(combatName, "DealDamage", 10, mbIsFacingRight);
					}
				}
				else
					DEBUG_PRINT(eLog::MESSAGE, "Combat Box not found");
				

				if (attackCount < 3)
				{
					attackCount++;
					DEBUG_PRINT(eLog::MESSAGE, "Attacking");
				}

				else
				{
					attackCount = 0;
				}

				//use linked skill
				if (attackCount == 0)
				{
					CharacterController::CastLinked(0, currentType, float(spawnLocation.x), float(spawnLocation.y), float(spawnLocation.z));
				}

				attackDelay = 0.0f;
			}
		}
	}

	void Dystopia::CharacterController::CastLinked(int _skill, bool _isForm, float x, float y, float z)
	{
		if (_isForm)
		{
			if (_skill == 0)
			{
				DEBUG_PRINT(eLog::MESSAGE, "Sending to SManager FORM");
				CharacterController_MSG::SendExternalMessage(sManagerName, "CastForm", 0, mbIsFacingRight, x, y, z);
			}

			else if (_skill == 1)
				CharacterController_MSG::SendExternalMessage(sManagerName, "CastForm", 1, mbIsFacingRight, x, y, z);

			/*else if (_skill == 2)
				CharacterController_MSG::SendExternalMessage(sManagerName, "CastForm", 2);*/
		}

		else
		{
			DEBUG_PRINT(eLog::MESSAGE, "Sending to SManager FORCE");
			if (_skill == 0)
				CharacterController_MSG::SendExternalMessage(sManagerName, "CastForce", 0, mbIsFacingRight, x, y, z);

			else if (_skill == 1)
				CharacterController_MSG::SendExternalMessage(sManagerName, "CastForce", 1, mbIsFacingRight, x, y, z);

			/*else if (_skill == 2)
				CharacterController_MSG::SendExternalMessage(sManagerName, "CastForce", 2);*/
		}
			
	}

	void CharacterController::CheckAttack()
	{
		auto s_rend = GetOwner()->GetComponent<SpriteRenderer>();

		if (attackDelay > 1.5f)
		{
			attackCount = 0;
			attackDelay = 0.5f;
		}

		if (s_rend && attackDelay < 1.0f)
		{
			/*if (attackCount == 1)
			{
				s_rend->SetAnimation("AttackOne");
			}

			if (attackCount == 2)
			{
				s_rend->SetAnimation("AttackTwo");
			}

			if (attackCount == 3)
			{
				s_rend->SetAnimation("AttackThree");
			}*/
		}
	}

	void CharacterController::CheckMoving()
	{
		auto s_rend = GetOwner()->GetComponent<SpriteRenderer>();
		if( s_rend)
		{
			if (mpInputSys->IsKeyTriggered("Run Left") || mpInputSys->IsKeyTriggered("Run Right"))
			{
				//s_rend->SetSpeed(0.080F);
				s_rend->SetAnimation("Running");
			}

			if (mpInputSys->IsKeyReleased("Run Left") || mpInputSys->IsKeyReleased("Run Right"))
			{
				//s_rend->SetSpeed(0.16F);
				s_rend->SetAnimation("Idle");
			}			
		}
	}

	void CharacterController::TakeDamage(int _dmg)
	{
		playerHealth -= _dmg;

		if (playerHealth < 0)
		{
			DEBUG_PRINT(eLog::MESSAGE, "YOU DIED!");
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




