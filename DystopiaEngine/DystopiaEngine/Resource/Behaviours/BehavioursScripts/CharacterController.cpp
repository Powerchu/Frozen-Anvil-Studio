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
#include "Editor/EGUI.h"
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
		template<typename ... Ts>
		void SendExternalMessage(const GameObject * _ptr, const char * _FuncName, Ts ... _Params)
		{
			EngineCore::GetInstance()->Get<BehaviourSystem>()->SendExternalMessage(_ptr, _FuncName, _Params...);
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
		, isCasting(false)
		, currentType(true)
		, isJumping(false)
		, isFalling(false)
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
			mpBody = GetOwner()->GetComponent<RigidBody>();
		}

		if (mpInputSys->IsController())
		{
			mpInputSys->MapButton("Run Left", eButton::XBUTTON_DPAD_LEFT);
			mpInputSys->MapButton("Run Right", eButton::XBUTTON_DPAD_RIGHT);
			mpInputSys->MapButton("Jump", eButton::XBUTTON_A);
			mpInputSys->MapButton("Skill B", eButton::XBUTTON_B);
			mpInputSys->MapButton("Skill Y", eButton::XBUTTON_Y);
			mpInputSys->MapButton("Attack", eButton::XBUTTON_X);
			mpInputSys->MapButton("SetForm", eButton::XBUTTON_LEFT_SHOULDER);
			mpInputSys->MapButton("SetForce", eButton::XBUTTON_RIGHT_SHOULDER);
		}
		else
		{
			mpInputSys->MapButton("Run Left",  eButton::KEYBOARD_LEFT);
			mpInputSys->MapButton("Run Right", eButton::KEYBOARD_RIGHT);
			mpInputSys->MapButton("Jump",      eButton::KEYBOARD_SPACEBAR);
			mpInputSys->MapButton("Skill B",   eButton::KEYBOARD_C);
			mpInputSys->MapButton("Skill Y",   eButton::KEYBOARD_V);
			mpInputSys->MapButton("Attack",    eButton::KEYBOARD_X); 
		}

		combatName = EngineCore::GetInstance()->Get<SceneSystem>()->FindGameObject_cstr("Combat Box");
		sManagerName = EngineCore::GetInstance()->Get<SceneSystem>()->FindGameObject_cstr("SkillManager");
		spriteName = EngineCore::GetInstance()->Get<SceneSystem>()->FindGameObject_cstr("Player Sprite");
		hudName = EngineCore::GetInstance()->Get<SceneSystem>()->FindGameObject_cstr("HUD");
		playerHealth = 100.0f;
		currentHealth = playerHealth;
		playerEnergy = 100.0f;
		currentEnergy = playerEnergy; 
		SetFlags(FLAG_ACTIVE);
	}

	void CharacterController::Init()
	{ 
		combatName = EngineCore::GetInstance()->Get<SceneSystem>()->FindGameObject_cstr("Combat Box");
		sManagerName = EngineCore::GetInstance()->Get<SceneSystem>()->FindGameObject_cstr("SkillManager");
		spriteName = EngineCore::GetInstance()->Get<SceneSystem>()->FindGameObject_cstr("Player Sprite");
		hudName = EngineCore::GetInstance()->Get<SceneSystem>()->FindGameObject_cstr("HUD");
		SetFlags(FLAG_ACTIVE);  

		if (mpInputSys->IsController())
		{
			mpInputSys->MapButton("Run Left", eButton::XBUTTON_DPAD_LEFT);
			mpInputSys->MapButton("Run Right", eButton::XBUTTON_DPAD_RIGHT);
			mpInputSys->MapButton("Jump", eButton::XBUTTON_A);
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
			mpInputSys->MapButton("Skill B", eButton::KEYBOARD_C);
			mpInputSys->MapButton("Skill Y", eButton::KEYBOARD_V);
			mpInputSys->MapButton("Attack", eButton::KEYBOARD_X);
		}
	}

	void CharacterController::Update(const float _fDeltaTime)
	{
		if (_fDeltaTime == 0)
			return;

		MovePlayer(_fDeltaTime);
		CheckMoving();

		attackDelay = attackDelay + _fDeltaTime;
		if (attackDelay > 1.5f)
		{
			attackCount = 0;
			attackDelay = 0.5f;
		}

		if (isJumping && mpBody->GetLinearVelocity().y < 0)
		{
			isFalling = true;
			isJumping = false;
		}

		if (currentEnergy < playerEnergy)
		{
			currentEnergy = currentEnergy + (_fDeltaTime * 5.f);
			SpendEnergy(0.0f);
		}
	}

	void CharacterController::FixedUpdate(const float )
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
		//DEBUG_PRINT(eLog::MESSAGE, "DotNormal: %f && Trigger?: %d", dotNormal, colBTrigger);
		if (dotNormal > 0.65F && dotNormal < 1.1F && !colBTrigger)
		{
			mbIsGrounded = true;	
			
			if (isFalling)
			{
				isFalling = false;
				CharacterController_MSG::SendExternalMessage(spriteName, "PlayAnimation", 9, mbIsFacingRight);
			}
		}
	}

	void Dystopia::CharacterController::OnCollisionStay(const CollisionEvent&)
	{

	}

	void Dystopia::CharacterController::OnCollisionExit(const CollisionEvent&)
	{
		
		
		
	}

	void Dystopia::CharacterController::OnTriggerEnter(GameObject * const )
	{
	}

	void Dystopia::CharacterController::OnTriggerStay(GameObject * const )
	{
	}

	void Dystopia::CharacterController::OnTriggerExit(GameObject * const )
	{
	}

	CharacterController * CharacterController::Duplicate() const
	{
		return new CharacterController{*this};
	}

	void CharacterController::Serialise(TextSerialiser& ) const
	{
	}

	void CharacterController::Unserialise(TextSerialiser&)
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
					CharacterController_MSG::SendExternalMessage(spriteName, "PlayAnimation", 4, mbIsFacingRight);
				}
				else if(auto r = GetOwner()->GetComponent<Transform>()->GetAllChild()[0]->GetOwner()->GetComponent<SpriteRenderer>())
				{
					if (r->GetCurrentAnimation() == HashString{"Idle"})
					{
						CharacterController_MSG::SendExternalMessage(spriteName, "PlayAnimation", 4, mbIsFacingRight);
					}
				}
			}
			else if (leftThumb > 0.1F)// Moving Right
			{
				DEBUG_PRINT(eLog::MESSAGE,"RIGHT");
				mpBody->AddLinearImpulse({ leftThumb * CharacterSpeed * mpBody->GetMass(),0,0 });
				if (!mbIsFacingRight)
				{
					GetOwner()->GetComponent<Transform>()->SetScale(-tScale.x, tScale.y, tScale.z);
					mbIsFacingRight = true;
					CharacterController_MSG::SendExternalMessage(spriteName, "PlayAnimation", 4, mbIsFacingRight);
				}
				else if(auto r = GetOwner()->GetComponent<Transform>()->GetAllChild()[0]->GetOwner()->GetComponent<SpriteRenderer>())
				{
					if (r->GetCurrentAnimation() == HashString{"Idle"})
					{
						CharacterController_MSG::SendExternalMessage(spriteName, "PlayAnimation", 4, mbIsFacingRight);
					}
				}
			}
			else
			{
				if (auto r = GetOwner()->GetComponent<Transform>()->GetAllChild()[0]->GetOwner()->GetComponent<SpriteRenderer>())
				{
					if (r->IsPlaying() && r->GetCurrentAnimation() == HashString{"Run"})
					{
						CharacterController_MSG::SendExternalMessage(spriteName, "PlayAnimation", 0, mbIsFacingRight);
					}
				}
			}

			const auto side = mbIsFacingRight ? 1.F : -1.F;

			if (leftTriggerFloat > 0.2F && mbIsGrounded)
			{
				if (!IsDodging)
				{
					IsDodging = true;
					mpBody->AddLinearImpulse({ side * CharacterSpeed * mpBody->GetMass() * 20 * leftTriggerFloat,0,0 });
					CharacterController_MSG::SendExternalMessage(spriteName, "PlayAnimation", 10, mbIsFacingRight);
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

		if (mpInputSys->IsKeyPressed("Run Left") && !mpInputSys->IsKeyPressed("Run Left"))
		{			
			mpBody->AddLinearImpulse({ -1 * CharacterSpeed * mpBody->GetMass(),0,0 });
			//mpBody->AddTorque({0, 0,1 * CharacterSpeed});
			if (mbIsFacingRight && Math::Abs(static_cast<float>(mpBody->GetLinearVelocity().x)) > 5.0F)
			{
				mbIsFacingRight = false;
				GetOwner()->GetComponent<Transform>()->SetScale(-tScale.x, tScale.y, tScale.z);
			}
		}

		if (mpInputSys->IsKeyPressed("Run Left") && !mpInputSys->IsKeyPressed("Run Left"))
		{
			mpBody->AddLinearImpulse({CharacterSpeed * mpBody->GetMass(),0,0 });
			//mpBody->AddTorque({0, 0,-1 * CharacterSpeed}); 
			if (!mbIsFacingRight && Math::Abs(static_cast<float>(mpBody->GetLinearVelocity().x)) > 5.0F)
			{
				mbIsFacingRight = true;
				GetOwner()->GetComponent<Transform>()->SetScale(-tScale.x, tScale.y, tScale.z);
			}
		}

		if (mpInputSys->IsKeyPressed("Jump"))
		{
			if (mbIsGrounded)
			{
				mpBody->AddLinearImpulse({ 0,JumpForce * mpBody->GetMass() * 10 + mpBody->GetLinearVelocity().y, 0 });
				mbIsGrounded = false;

				CharacterController_MSG::SendExternalMessage(spriteName, "PlayAnimation", 7, mbIsFacingRight);
				isJumping = true;
			}
		}

		if (mpInputSys->IsKeyTriggered("Skill Y"))
		{
			const Math::Vec3D spawnLocation = GetOwner()->GetComponent<Transform>()->GetPosition();

			CharacterController::CastLinked(1, currentType, float(spawnLocation.x), float(spawnLocation.y), float(spawnLocation.z));
		}

		if (mpInputSys->IsKeyTriggered("Skill B"))
		{
			const Math::Vec3D spawnLocation = GetOwner()->GetComponent<Transform>()->GetPosition();

			CharacterController::CastLinked(2, currentType, float(spawnLocation.x), float(spawnLocation.y), float(spawnLocation.z));
		}

		if (mpInputSys->IsKeyTriggered("SetForm"))
		{
			currentType = true;
			CharacterController_MSG::SendExternalMessage(hudName, "ChangeFF", 1);
		}

		if (mpInputSys->IsKeyTriggered("SetForce"))
		{
			currentType = false;
			CharacterController_MSG::SendExternalMessage(hudName, "ChangeFF", 2);
		}

		if (mpInputSys->IsKeyTriggered("Attack"))
		{
			const Math::Vec3D spawnLocation = GetOwner()->GetComponent<Transform>()->GetPosition();

		    if (attackDelay > 0.f)
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
				CheckAttack();
			}
		}
	}

	void Dystopia::CharacterController::CastLinked(int _skill, bool _isForm, float x, float y, float z)
	{
		if (currentEnergy < 10.0f)
			return;


		DEBUG_PRINT(eLog::MESSAGE, "WTF %d", _isForm);

		if (_isForm)
		{
			CharacterController_MSG::SendExternalMessage(spriteName, "PlayAnimation", 6, mbIsFacingRight);

			if (_skill == 0)
			{
				CharacterController_MSG::SendExternalMessage(sManagerName, "CastForm", 0, mbIsFacingRight, x, y, z);
			}

			else if (_skill == 1)
				CharacterController_MSG::SendExternalMessage(sManagerName, "CastForm", 1, mbIsFacingRight, x, y, z);

			else if (_skill == 2)
				CharacterController_MSG::SendExternalMessage(sManagerName, "CastForm", 2, mbIsFacingRight, x, y, z);
		}

		else
		{	
			CharacterController_MSG::SendExternalMessage(spriteName, "PlayAnimation", 5, mbIsFacingRight);

			if (_skill == 0)
				CharacterController_MSG::SendExternalMessage(sManagerName, "CastForce", 0, mbIsFacingRight, x, y, z);

			else if (_skill == 1)
				CharacterController_MSG::SendExternalMessage(sManagerName, "CastForce", 1, mbIsFacingRight, x, y, z);

			else if (_skill == 2)
				CharacterController_MSG::SendExternalMessage(sManagerName, "CastForce", 2, mbIsFacingRight, x, y, z);
		}

		SpendEnergy(10.0f);
	}

	void CharacterController::CheckAttack()
	{
		if (!isAttacking)    
		{	
			if (attackCount == 1)
			{
				DEBUG_PRINT(eLog::MESSAGE, "Test is ondsffsdfsd");
				CharacterController_MSG::SendExternalMessage(spriteName, "PlayAnimation", int(1), mbIsFacingRight);
				isAttacking = true; 
			}

			if (attackCount == 2)
			{
				CharacterController_MSG::SendExternalMessage(spriteName, "PlayAnimation", 2, mbIsFacingRight);
				isAttacking = true; 
			}

			if (attackCount == 3)
			{
				CharacterController_MSG::SendExternalMessage(spriteName, "PlayAnimation", 3, mbIsFacingRight);
				isAttacking = true; 
			}

			if (attackCount == 0)
			{
				if (currentType)
				{
					CharacterController_MSG::SendExternalMessage(spriteName, "PlayAnimation", 6, mbIsFacingRight);
					isCasting = true;
				}

				else
				{
					CharacterController_MSG::SendExternalMessage(spriteName, "PlayAnimation", 5, mbIsFacingRight);
					isCasting = true;
				}
			}
		}
	}         

	void CharacterController::AnimationComplete()
	{
		if (isAttacking)
			isAttacking = false;

		if (isCasting)
			isCasting = false;
	}

	void CharacterController::CheckMoving()
	{
		if (mpInputSys->IsKeyTriggered(eButton::XBUTTON_DPAD_LEFT) || mpInputSys->IsKeyTriggered(eButton::XBUTTON_DPAD_RIGHT))
			CharacterController_MSG::SendExternalMessage(spriteName, "PlayAnimation", 4, mbIsFacingRight);

		if (mpInputSys->IsKeyReleased(eButton::XBUTTON_DPAD_LEFT) || mpInputSys->IsKeyReleased(eButton::XBUTTON_DPAD_RIGHT))
			CharacterController_MSG::SendExternalMessage(spriteName, "PlayAnimation", 0, mbIsFacingRight); 
	}

	void CharacterController::TakeDamage(float _dmg)
	{
		currentHealth = currentHealth - _dmg; 

		if (currentHealth > 0.0f)
		{
			CharacterController_MSG::SendExternalMessage(hudName, "UpdateHealth", currentHealth / playerHealth);
		}

		else
		{
			EngineCore::Get<SceneSystem>()->RestartScene();
		}
	}

	void CharacterController::SpendEnergy(float _amt)
	{
		currentEnergy = currentEnergy - _amt; 

		CharacterController_MSG::SendExternalMessage(hudName, "UpdateEnergy", currentEnergy / playerEnergy);
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




