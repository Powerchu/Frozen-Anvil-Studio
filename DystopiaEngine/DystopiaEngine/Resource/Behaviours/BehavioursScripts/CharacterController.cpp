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
		, CurrentSpeed(CharacterSpeed)
		, disableControls{false}
		, isInvulnerable(false)
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
			mpInputSys->MapButton("Run Left", eButton::KEYBOARD_LEFT);
			mpInputSys->MapButton("Run Right", eButton::KEYBOARD_RIGHT);
			mpInputSys->MapButton("Jump", eButton::KEYBOARD_SPACEBAR);
			mpInputSys->MapButton("Skill B", eButton::KEYBOARD_C);
			mpInputSys->MapButton("Skill Y", eButton::KEYBOARD_V);
			mpInputSys->MapButton("Attack", eButton::KEYBOARD_X); 
			mpInputSys->MapButton("Cheat One", eButton::KEYBOARD_1);
			mpInputSys->MapButton("Cheat Two", eButton::KEYBOARD_2);
		}

		combatName = EngineCore::GetInstance()->Get<SceneSystem>()->FindGameObject_cstr("Combat Box");
		sManagerName = EngineCore::GetInstance()->Get<SceneSystem>()->FindGameObject_cstr("SkillManager");
		spriteName = EngineCore::GetInstance()->Get<SceneSystem>()->FindGameObject_cstr("Player Sprite");
		hudName = EngineCore::GetInstance()->Get<SceneSystem>()->FindGameObject_cstr("HUD");
		//playerHealth = 100.0f;
		currentHealth = playerHealth;
		playerEnergy = 70.0f;
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
			mpInputSys->MapButton("Skill B", eButton::KEYBOARD_V);
			mpInputSys->MapButton("Skill Y", eButton::KEYBOARD_C);
			mpInputSys->MapButton("Attack", eButton::KEYBOARD_X);
			mpInputSys->MapButton("SetForm", eButton::KEYBOARD_D);
			mpInputSys->MapButton("SetForce", eButton::KEYBOARD_F);
			mpInputSys->MapButton("Roll", eButton::KEYBOARD_LCTRL);
			mpInputSys->MapButton("Cheat One", eButton::KEYBOARD_1);
			mpInputSys->MapButton("Cheat Two", eButton::KEYBOARD_2);
		}
		CurrentSpeed = CharacterSpeed;
		mpBody = GetOwner()->GetComponent<RigidBody>();
		playerEnergy = 70.0f;
		currentEnergy = playerEnergy; 
	}

	void CharacterController::Update(const float _fDeltaTime)
	{
		if (disableControls)
		{
			if(auto r = GetOwner()->GetComponent<Transform>()->GetAllChild()[0]->GetOwner()->GetComponent<SpriteRenderer>())
			{
				if (r->GetCurrentAnimation() != HashString{"Idle"})
				{
					r->Stop();
					r->SetAnimation("Idle");
					r->SetSpeed(0.066f);
					r->Play();
				}
			}
			return;
		}
		
		if(auto r = GetOwner()->GetComponent<Transform>()->GetAllChild()[0]->GetOwner()->GetComponent<SpriteRenderer>())
		{
			if (r->GetCurrentAnimation() == HashString{"CastForce"} || 
				r->GetCurrentAnimation() == HashString{"CastForm"} )
				isCasting = true;
			else 
				isCasting = false;
		}

		CurrentSpeed = isAttacking ? CurrentSpeed : CharacterSpeed;
 
		if (_fDeltaTime == 0)
			return;

		MovePlayer(_fDeltaTime);
		//CheckMoving();

		attackDelay = attackDelay + _fDeltaTime;
		if (attackDelay > 1.2f)
		{
			attackCount = 0;
			attackDelay = 0.f;
		}

		if (isJumping && mpBody->GetLinearVelocity().y < 0)
		{
			isFalling = true;
			isJumping = false;
		}

		if (currentEnergy < playerEnergy && !isCasting)
		{
			currentEnergy = currentEnergy + (_fDeltaTime * 15.f);
			SpendEnergy(0.0f);
		}
	}

	void CharacterController::FixedUpdate(const float)
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

	void Dystopia::CharacterController::OnCollisionStay(const CollisionEvent& )
	{

	}

	void Dystopia::CharacterController::OnCollisionExit(const CollisionEvent& )
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

	void CharacterController::Unserialise(TextSerialiser& )
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
	void CharacterController::DisableControls(bool _b)
	{
		disableControls = _b;
	}
	
	void CharacterController::MovePlayer(float)
	{
		if (mpBody == nullptr) return;
		const auto tScale = GetOwner()->GetComponent<Transform>()->GetGlobalScale();

		if (mpInputSys->IsController())
		{
			const auto leftThumb = mpInputSys->GetAnalogX(0); 
			const auto leftTriggerFloat = mpInputSys->GetTriggers(0);
			if (!IsDodging && !isCasting)
			{
				if (leftThumb < -0.1F) // Moving Left
				{
					mpBody->AddLinearImpulse({ 2 * leftThumb * CurrentSpeed * mpBody->GetMass(),0,0 });
					if (mbIsFacingRight)
					{
						GetOwner()->GetComponent<Transform>()->SetScale(-1.f * Math::Abs<float>(tScale.x), tScale.y, tScale.z);
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
					mpBody->AddLinearImpulse({ 2 * leftThumb * CurrentSpeed * mpBody->GetMass(),0,0 });
					if (!mbIsFacingRight)
					{
						GetOwner()->GetComponent<Transform>()->SetScale(Math::Abs<float>(tScale.x), tScale.y, tScale.z);
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
			}
			else
			{
				if (auto r = GetOwner()->GetComponent<Transform>()->GetAllChild()[0]->GetOwner()->GetComponent<SpriteRenderer>())
				{
					if (!r->IsPlaying())
					{				
						IsDodging = false;
						mpInputSys->StopVibrate();
					}
					else
						mpInputSys->SetVibrate(static_cast<unsigned short>(Math::Min(leftTriggerFloat + 0.1F, 1.0F) * 32000) 
										  ,static_cast<unsigned short>(Math::Min(leftTriggerFloat + 0.1F, 1.0F) * 16000));
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
				// else
				// {
				// 	if (Math::Abs(float(mpBody->GetLinearVelocity().x)) > 0.01F)
				// 	{
				// 		mpInputSys->SetVibrate(static_cast<unsigned short>(Math::Min(leftTriggerFloat + 0.1F, 1.0F) * 32000) 
				// 						  ,static_cast<unsigned short>(Math::Min(leftTriggerFloat + 0.1F, 1.0F) * 16000));
				// 	}
				// 	else
				// 	{
				// 		mpInputSys->StopVibrate();
				// 	}
				// }	
			}
			// else
			// {
			// 	IsDodging = false;
			// 	mpInputSys->StopVibrate();
			// }
		} // Controller Scheme end
		else
		{
			if (!IsDodging && !isCasting)
			{
				
				if (mpInputSys->IsKeyPressed("Run Left"))
				{		
					mpBody->AddLinearImpulse({ -2 * CurrentSpeed * mpBody->GetMass(),0,0 });
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

				if (mpInputSys->IsKeyPressed("Run Right"))
				{
					DEBUG_PRINT(eLog::MESSAGE, "Running");
					mpBody->AddLinearImpulse({ 2 * CurrentSpeed * mpBody->GetMass(),0,0 });
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

				if (mpInputSys->IsKeyReleased("Run Right") || mpInputSys->IsKeyReleased("Run Left"))
				{
					if (auto r = GetOwner()->GetComponent<Transform>()->GetAllChild()[0]->GetOwner()->GetComponent<SpriteRenderer>())
					{
						if (r->IsPlaying() && r->GetCurrentAnimation() == HashString{"Run"})
						{
							CharacterController_MSG::SendExternalMessage(spriteName, "PlayAnimation", 0, mbIsFacingRight);
						}
					}
				}

				if (mpInputSys->IsKeyTriggered("Roll") && mbIsGrounded)
				{
					if (!IsDodging)
					{
						IsDodging = true;
						auto roll = CharacterSpeed * mpBody->GetMass() * 50;
						roll = mbIsFacingRight ? roll : roll * -1.f;
						mpBody->AddLinearImpulse(Math::Vector3D{roll,0,0 });
						CharacterController_MSG::SendExternalMessage(spriteName, "PlayAnimation", 10, mbIsFacingRight);
					}	
				}
			}
			else
			{
				if (auto r = GetOwner()->GetComponent<Transform>()->GetAllChild()[0]->GetOwner()->GetComponent<SpriteRenderer>())
					if (!r->IsPlaying() || r->GetCurrentAnimation() == HashString{"Idle"})
						IsDodging = false;
			}
		}

		if (mpInputSys->IsKeyTriggered("Jump"))
		{
			if (mbIsGrounded)
			{
				mpBody->AddLinearImpulse(Math::Vector3D{ 0,JumpForce * mpBody->GetMass() * 10 + mpBody->GetLinearVelocity().y, 0 });
				mbIsGrounded = false;

				CharacterController_MSG::SendExternalMessage(spriteName, "PlayAnimation", 7, mbIsFacingRight);
				isJumping = true;
			}
		}

		if (mpInputSys->IsKeyTriggered("Cheat One"))
		{
			if (!isInvulnerable)
				isInvulnerable = true;
			
			else
				isInvulnerable = false;
		}
		
		if (mpInputSys->IsKeyTriggered("Cheat Two"))
		{
			EngineCore::GetInstance()->GetSystem<SceneSystem>()->LoadScene("Town.dscene");
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

		    if (attackDelay >= 0.35f)
			{
				if (combatName)
				{
					CharacterController_MSG::SendExternalMessage(combatName, "DealDamage", 10, mbIsFacingRight);
				}
				else
					DEBUG_PRINT(eLog::MESSAGE, "Combat Box not found");
				

				if (attackCount < 3)
				{
					CurrentSpeed = CharacterSpeed * 0.25f;
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
		if (currentEnergy < 20.0f)
			return;


		isCasting = true;
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

		SpendEnergy(20.0f);
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
		{
			CurrentSpeed = CharacterSpeed;
			isAttacking = false;
		}

		// if (isCasting)
		// 	isCasting = false;
	}

	void CharacterController::CheckMoving()
	{
		if (mpInputSys->IsKeyTriggered("Run Left") || mpInputSys->IsKeyTriggered("Run Right"))
			CharacterController_MSG::SendExternalMessage(spriteName, "PlayAnimation", 4, mbIsFacingRight);

		if (mpInputSys->IsKeyReleased("Run Left") || mpInputSys->IsKeyReleased("Run Right"))
			CharacterController_MSG::SendExternalMessage(spriteName, "PlayAnimation", 0, mbIsFacingRight); 
	}

	void CharacterController::FadeHUD(bool _b)
	{
		if(_b && currentHealth > 0.f)
		{
			CharacterController_MSG::SendExternalMessage(hudName, "Fade", true);
		}
		else
			CharacterController_MSG::SendExternalMessage(hudName, "Fade", false);
	}
	
	void CharacterController::TakeDamage(float _dmg)
	{
		if (isInvulnerable)
			return;
		
		currentHealth = currentHealth - _dmg; 

		if (currentHealth > 0.0f)
		{
			CharacterController_MSG::SendExternalMessage(hudName, "UpdateHealth", currentHealth / playerHealth);
		}

		else
		{
			//EngineCore::Get<SceneSystem>()->RestartScene();
			if (auto o = EngineCore::Get<SceneSystem>()->FindGameObject_cstr("GameOver"))
				CharacterController_MSG::SendExternalMessage(o, "ActivateGameOver");
			if (auto o = EngineCore::Get<SceneSystem>()->FindGameObject_cstr("HUD"))
				CharacterController_MSG::SendExternalMessage(o, "Fade", false);
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




