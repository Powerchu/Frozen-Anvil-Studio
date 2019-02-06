/* HEADER *********************************************************************************/
/*!
\file	GoblinAI.cpp
\author Aaron (100%)
\par    email: m.chu\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved. 
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "GoblinAI.h"
#include "System/Input/InputSystem.h"
#include "System/Input/InputMap.h"
#include "System/Driver/Driver.h"
#include "System/Behaviour/BehaviourSystem.h"

#include "System/Scene/SceneSystem.h"
#include "System/Collision/CollisionEvent.h" 
#include "System/Collision/CollisionSystem.h"

#include "Object/ObjectFlags.h"
#include "Object/GameObject.h"
#include "Component/Collider.h"

#include "Editor/EGUI.h"
#include "Utility/DebugAssert.h"
#include "Component/AudioSource.h"

#define A_GOBLIN_RALLY "goblin_rally.wav"
#define A_GOBLIN_ATTACK "goblin_attack.wav"
#define A_GOBLIN_FLINCH "goblin_flinch.wav"
#define A_GOBLIN_HIT "EnemyHit.wav"


namespace Dystopia
{
	namespace GoblinAI_MSG
	{
		template<typename ... Ts>
		void SendInternalMessage(Behaviour * _ptr, const char * _FuncName, Ts ... _Params)
		{
			EngineCore::GetInstance()->Get<BehaviourSystem>()->SendInternalMessage(_ptr, _FuncName, _Params...); 
		}
		
		template<typename ... Ts>
		void SendExternalMessage(uint64_t _ObjectID, const char * _FuncName, Ts ... _Params)
		{
			EngineCore::GetInstance()->Get<BehaviourSystem>()->SendExternalMessage(_ObjectID, _FuncName, _Params...);
		}
		template<typename ... Ts>
		void SendExternalMessage(const GameObject * _ptr, const char * _FuncName, Ts ... _Params)
		{
			EngineCore::GetInstance()->Get<BehaviourSystem>()->SendExternalMessage(_ptr, _FuncName, _Params...);
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
	GoblinAI::GoblinAI() 
		:	MaxTimer(3.00f),
		 	StateTimer(MaxTimer),
			AttackTimer(MaxTimer),
			AttackCooldown(0.1f),
			RallyCount(3),
			IsGrounded(false), 
			IsFacingRight(true),
			HasFlock(false)
	{

	}

	GoblinAI::~GoblinAI()
	{
	}

	void GoblinAI::Load()
	{
	}

	void GoblinAI::Awake()
	{
		mpController = GetOwner()->GetComponent<AiController>();
		IsFacingRight = true;
		if (mpController == nullptr) return;
 
		auto mContBlackboard = mpController->GetBlackboard();	
 
		if (mContBlackboard != nullptr)  
		{
			const auto& root = NeuralTree::Builder()     
				.composite<NeuralTree::Sequence>() // root   
					.decorator<NeuralTree::UntilFailure>() 
						.composite<NeuralTree::Sequence>() 
							.task<Idle>(mContBlackboard)
							.task<Patrol>(mContBlackboard)
						.end()
					.end()
					.task<Alert>(mContBlackboard)
					.composite<NeuralTree::Sequence>()
						.task<Chase>(mContBlackboard)
						.composite<NeuralTree::Selector>() 
							.task<Melee_Slash>(mContBlackboard) 
							.task<Melee_Lunge>(mContBlackboard)
							.task<Rally>(mContBlackboard) 
						.end()
					.end()
				.end() 
			.Build(mContBlackboard, "Goblin AI Tree");

			// const auto& root = NeuralTree::Builder()      
			// 	.composite<NeuralTree::Sequence>() // root   
			// 		.decorator<NeuralTree::UntilFailure>() 
			// 			.composite<NeuralTree::RandomSequence>()
			// 				.task<Idle>(mContBlackboard) 
			// 				.task<Patrol>(mContBlackboard)
			// 			.end()
			// 		.end() 
			// 	.end() 
			// .Build(mContBlackboard, "Goblin AI Tree");

			mpController->SetTree(root);  
		}	 
	}

	void GoblinAI::Init()
	{
		mpController = GetOwner()->GetComponent<AiController>(); 

		if (nullptr == mpController) return;
		auto mContBlackboard = mpController->GetBlackboard();  

		/*Set Blackboard/Update Blackboard*/
		mContBlackboard->SetFloat("State Timer", StateTimer);
		mContBlackboard->SetFloat("Attack Timer", AttackTimer);
		mContBlackboard->SetInt("Rally Count", RallyCount);
		mContBlackboard->SetBool("Is Grounded", IsGrounded);
		mContBlackboard->SetBool("Is Facing Right", IsFacingRight);
		mContBlackboard->SetBool("Has Flock", HasFlock);
		mContBlackboard->SetBool("Is Alert", IsAlerted);
		mContBlackboard->SetObject("Self", GetOwner()->GetID());

		auto& allchild = GetOwner()->GetComponent<Transform>()->GetAllChild();
        for (auto& c : allchild)
        {
            if (c->GetOwner()->GetName() == HashString{"AlertRadius"})
                AlertObj = c->GetOwner();
        } 

		Player   = EngineCore::Get<SceneSystem>()->FindGameObject("CharacterController"); 
		mContBlackboard->SetObject("Player", Player->GetID());
		mpAudioSrc = GetOwner()->GetComponent<AudioSource>();
	}

	

	void GoblinAI::Update(const float _fDeltaTime)
	{
		if (nullptr == mpController) return; 
		auto mContBlackboard = mpController->GetBlackboard(); 

		if (StateTimer > 0.0f) 
			StateTimer -= _fDeltaTime;
		else
			StateTimer = mContBlackboard->GetFloat("State Timer");
		
		
		if (AttackTimer > 0.0f)
			AttackTimer -= _fDeltaTime;
		else
			AttackTimer = mContBlackboard->GetFloat("Attack Timer");

		if (AlertObj)
		{
			IsAlerted = false;
			if (AlertObj->GetComponent<Collider>())
				if(AlertObj->GetComponent<Collider>()->HasCollisionWith(Player))
					IsAlerted = true;
		}
		

		/*Set Blackboard/Update Blackboard*/
		mContBlackboard->SetFloat("State Timer", StateTimer);
		mContBlackboard->SetFloat("Attack Timer", AttackTimer);
		mContBlackboard->SetInt("Rally Count", RallyCount);
		mContBlackboard->SetBool("Is Grounded", IsGrounded);
		//mContBlackboard->SetBool("Is Facing Right", IsFacingRight);
		mContBlackboard->SetBool("Has Flock", HasFlock);
		mContBlackboard->SetBool("Is Alert", IsAlerted);
		mContBlackboard->SetObject("Self", GetOwner()->GetID());
	}

	void GoblinAI::FixedUpdate(const float _fDeltaTime)
	{
	}
	
	void GoblinAI::PostUpdate(void)
	{
	}

	void GoblinAI::GameObjectDestroy(void) 
	{
	}
 
	void GoblinAI::Unload(void)
	{
	}

	void Dystopia::GoblinAI::OnCollisionEnter(const CollisionEvent& _colEvent)
	{

	} 

	void Dystopia::GoblinAI::OnCollisionStay(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::GoblinAI::OnCollisionExit(const CollisionEvent& _colEvent)
	{
 
	}

	void Dystopia::GoblinAI::OnTriggerEnter(GameObject * const _obj)
	{
	}

	void Dystopia::GoblinAI::OnTriggerStay(GameObject * const _obj)
	{
	}

	void Dystopia::GoblinAI::OnTriggerExit(GameObject * const _obj)
	{
	}

	GoblinAI * GoblinAI::Duplicate() const
	{
		return new GoblinAI{*this};
	}

	void GoblinAI::Serialise(TextSerialiser& _ser) const
	{
	}

	void GoblinAI::Unserialise(TextSerialiser& _ser)
	{
	}


	const char * const GoblinAI::GetBehaviourName() const
	{
		return GoblinAI::BehaviourName;
	}

	void GoblinAI::EditorUI(void) noexcept
	{
		
		
	}
	
	TypeErasure::TypeEraseMetaData GoblinAI::GetMetaData()
	{
		static MetaData<Dystopia::GoblinAI> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const GoblinAI::GetMetaData() const
	{
		static MetaData<Dystopia::GoblinAI> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
			
	/***************************************************************************
	*  GOBLIN TASK: Idle
	***************************************************************************/
	void GoblinAI::Idle::Init() 
	{
		// Get Stuff From Blackboard
		const auto gameObj_Self = mpBlackboard->GetObject("Self");
		
		// If pointer to self is null, set him.
		if (nullptr == mpOwner)
			mpOwner = CORE::Get<SceneSystem>()->FindGameObject(gameObj_Self);

		else
		{
			// Set Animation To Idle
			auto mpRend = mpOwner->GetComponent<SpriteRenderer>();
			
			if (mpRend)
			{
				mpRend->Stop();
				mpRend->SetAnimation("Idle");
				mpRend->Play();
			}

			//Set Time
			mpBlackboard->SetFloat("State Timer", 2.0f);
		}
	}

	NeuralTree::Node::eStatus GoblinAI::Idle::Update() 
	{
		if (mpOwner == nullptr) return eStatus::FAIL;

		_stateTimer = mpBlackboard->GetFloat("State Timer");

		if (mpBlackboard->GetBool("Is Alert")) return eStatus::FAIL;

		// Idle has elasped 3.0f
		if (_stateTimer <= 0.0f)
			return eStatus::SUCCESS;

		return eStatus::RUNNING;
	}

	void GoblinAI::Idle::Exit(eStatus _status) 
	{
		if (IsRunning())
		{
		}
		else if (IsSuccess())
		{
		}
		else if (IsFailure())
		{
		}
	}

	/***************************************************************************
	*  GOBLIN TASK: Patrol
	***************************************************************************/
	void GoblinAI::Patrol::Init() 
	{
		// Get Stuff From Blackboard
		const auto gameObj_Self = mpBlackboard->GetObject("Self");
		
		// If pointer to self is null, set him.
		if (nullptr == mpOwner)
			mpOwner = CORE::Get<SceneSystem>()->FindGameObject(gameObj_Self);

		else
		{
			// Set Animation To Idle
			auto mpRend = mpOwner->GetComponent<SpriteRenderer>();
			
			if (mpRend)
			{
				mpRend->Stop();
				mpRend->SetAnimation("Run");
				mpRend->Play();
			}

			//Set Time
			mpBlackboard->SetFloat("State Timer", 5.0f);
			_direction = _direction ? 0 : 1;
		}
	}

	NeuralTree::Node::eStatus GoblinAI::Patrol::Update() 
	{
		if (mpOwner == nullptr) return eStatus::FAIL;
		const auto rigidBod = mpOwner->GetComponent<RigidBody>();
		auto mpTransform = mpOwner->GetComponent<Transform>();
		auto mScale = mpTransform->GetGlobalScale();
		
		_stateTimer = mpBlackboard->GetFloat("State Timer");

		if (mpBlackboard->GetBool("Is Alert")) return eStatus::FAIL;
		
		// Patrol has elasped 5.0f
		if (_stateTimer <= 0.0f)
			return eStatus::SUCCESS;

		if (_direction) // left
		{
			if (mpBlackboard->GetBool("Is Facing Right"))
			{
				mpTransform->SetGlobalScale(-1*mScale.x, mScale.y, mScale.z);
				mpBlackboard->SetBool("Is Facing Right", false);
			}

			rigidBod->AddLinearImpulse({-1*rigidBod->GetMass()*25.0f,0,0});
		}
		else // right
		{
			if (!mpBlackboard->GetBool("Is Facing Right"))
			{
				mpTransform->SetGlobalScale(-1*mScale.x, mScale.y, mScale.z);
				mpBlackboard->SetBool("Is Facing Right", true);
			}
			rigidBod->AddLinearImpulse({1*rigidBod->GetMass()*25.0f,0,0});
		}
		

		return eStatus::RUNNING;
	}

	void GoblinAI::Patrol::Exit(eStatus _status) 
	{
	
	}

	/***************************************************************************
	*  GOBLIN TASK: Alert
	***************************************************************************/
	void GoblinAI::Alert::Init() 
	{
		// Get Stuff From Blackboard
		const auto gameObj_Self = mpBlackboard->GetObject("Self");
	
		// If pointer to self is null, set him.
		if (nullptr == mpOwner)
			mpOwner = CORE::Get<SceneSystem>()->FindGameObject(gameObj_Self);
		
		const auto gameObj_Player = mpBlackboard->GetObject("Player");
		
		if (nullptr == mpPlayer)
			mpPlayer = CORE::Get<SceneSystem>()->FindGameObject(gameObj_Player);

		// Set Animation To Flinch
		if (nullptr != mpOwner)
		{
			auto mpRend = mpOwner->GetComponent<SpriteRenderer>();
			
			if (mpRend)
			{
				mpRend->Stop();
				mpRend->SetAnimation("Flinch");
				mpRend->Play();
			}
		}
	}

	NeuralTree::Node::eStatus GoblinAI::Alert::Update() 
	{
		// Set Animation To Flinch
		if (nullptr != mpOwner)
		{
			auto mpRend = mpOwner->GetComponent<SpriteRenderer>();
			
			if (mpRend)
			{
				if (!mpRend->IsPlaying())
				{
					return eStatus::SUCCESS;
				}
			}
			return eStatus::RUNNING;
		}
		return eStatus::FAIL;
	}

	void GoblinAI::Alert::Exit(eStatus _status) 
	{
	
	}

	/***************************************************************************
	*  GOBLIN TASK: Rally
	***************************************************************************/
	void GoblinAI::Rally::Init() 
	{
		// Get Stuff From Blackboard
		const auto gameObj_Self = mpBlackboard->GetObject("Self");
		
		// If pointer to self is null, set him.
		if (nullptr == mpOwner)
			mpOwner = CORE::Get<SceneSystem>()->FindGameObject(gameObj_Self);


		// Set Animation To Idle
		auto mpRend = mpOwner->GetComponent<SpriteRenderer>();
		
		if (mpRend)
		{
			if (!mpRend->IsPlaying())
			{
				mpRend->Stop();
				mpRend->SetAnimation("Rally");
				mpRend->Play();
			}
		}
	}

	NeuralTree::Node::eStatus GoblinAI::Rally::Update() 
	{
		return eStatus::SUCCESS;
	}

	void GoblinAI::Rally::Exit(eStatus _status) 
	{
	
	}

	/***************************************************************************
	*  GOBLIN TASK: Chase
	***************************************************************************/
	void GoblinAI::Chase::Init() 
	{
		// Get Stuff From Blackboard
		const auto gameObj_Self = mpBlackboard->GetObject("Self");
		const auto gameObj_Player = mpBlackboard->GetObject("Player");
		
		if (nullptr == mpPlayer)
			mpPlayer = CORE::Get<SceneSystem>()->FindGameObject(gameObj_Player);

		// If pointer to self is null, set him.
		if (nullptr == mpOwner)
			mpOwner = CORE::Get<SceneSystem>()->FindGameObject(gameObj_Self);

		// Set Animation To Idle
		auto mpRend = mpOwner->GetComponent<SpriteRenderer>();
		
		if (mpRend)
		{
			mpRend->Stop();
			mpRend->SetAnimation("Run");
			mpRend->Play();
		}
	}

	NeuralTree::Node::eStatus GoblinAI::Chase::Update() 
	{
		const auto myTrans = mpOwner->GetComponent<Transform>();
		const auto playerTrans = mpPlayer->GetComponent<Transform>();
		const auto rigidBod = mpOwner->GetComponent<RigidBody>();

		const auto dist = (myTrans->GetGlobalPosition().x-playerTrans->GetGlobalPosition().x);
		
		auto mScale = myTrans->GetGlobalScale();
		
		// right
		if (dist > 0.f)
		{
			rigidBod->AddLinearImpulse({-1*rigidBod->GetMass()*35.0f,0,0});

			if (mpBlackboard->GetBool("Is Facing Right")) 
			{
				myTrans->SetGlobalScale(-1*mScale.x, mScale.y, mScale.z);
				mpBlackboard->SetBool("Is Facing Right", false);
			}
		}
		// left
		else if (dist < -0.f)
		{
			rigidBod->AddLinearImpulse({1*rigidBod->GetMass()*35.0f,0,0});

			if (!mpBlackboard->GetBool("Is Facing Right")) 
			{
				myTrans->SetGlobalScale(-1*mScale.x, mScale.y, mScale.z);
				mpBlackboard->SetBool("Is Facing Right", true);
			}
		}

		if (!mpBlackboard->GetBool("Is Alert")) return eStatus::FAIL;

		if (dist > -160.0f && dist < 160.f)
		{
			return eStatus::SUCCESS;
		}

		return eStatus::RUNNING;
	}

	void GoblinAI::Chase::Exit(eStatus _status) 
	{
	
	}

	/***************************************************************************
	*  GOBLIN TASK: Rally
	***************************************************************************/
	void GoblinAI::Melee_Slash::Init() 
	{
		// Get Stuff From Blackboard
		const auto gameObj_Self = mpBlackboard->GetObject("Self");
		const auto gameObj_Player = mpBlackboard->GetObject("Player");

		// If pointer to self is null, set him.
		if (nullptr == mpOwner)
			mpOwner = CORE::Get<SceneSystem>()->FindGameObject(gameObj_Self);

		if (nullptr == mpPlayer)
			mpPlayer = CORE::Get<SceneSystem>()->FindGameObject(gameObj_Player);

		// Set Animation To Idle
		auto mpRend = mpOwner->GetComponent<SpriteRenderer>();
		if (mpRend)
		{
			mpRend->Stop();
			mpRend->SetAnimation("Attack");
			mpRend->Play();
		}
	}

	NeuralTree::Node::eStatus GoblinAI::Melee_Slash::Update() 
	{
		const auto myTrans = mpOwner->GetComponent<Transform>();
		const auto playerTrans = mpPlayer->GetComponent<Transform>();
		auto mpRend = mpOwner->GetComponent<SpriteRenderer>();

		const auto dist = (myTrans->GetGlobalPosition().x-playerTrans->GetGlobalPosition().x);
		
		if (dist < -160.0f || dist > 160.f)
		{
			if (!mpRend->IsPlaying())
			{
				return eStatus::FAIL;
			}
		}

		// Set Animation To Idle
		if (mpRend)
		{
			if (!mpRend->IsPlaying())
			{
				return eStatus::SUCCESS;
			}
		}
		return eStatus::RUNNING;
	}

	void GoblinAI::Melee_Slash::Exit(eStatus _status)
	{
		const auto isRight = mpBlackboard->GetBool("Is Facing Right");
		if (IsSuccess())
		{
			auto mpAudio = mpOwner->GetComponent<AudioSource>();
			if (mpAudio)
			{
				mpAudio->Stop();
				mpAudio->SetVolume(1.3f);
				mpAudio->SetPitch(1.0f);
				mpAudio->ChangeAudio("goblin_attack.wav");
				mpAudio->Play(); 
				mpAudio->SetPitch(1.1f);
			}
			GoblinAI_MSG::SendExternalMessage(mpPlayer, "TakeDamage", 10.0f, 1.f);
			
			if (isRight)
				GoblinAI_MSG::SendExternalMessage(mpPlayer, "TakeForce", 400.0f,Math::Vec2{1.0f,0.5f});
			else
			{
				GoblinAI_MSG::SendExternalMessage(mpPlayer, "TakeForce", 400.0f, Math::Vec2{-1.0f,0.5f});
			}
		}
	}

	/***************************************************************************
	*  GOBLIN TASK: Melee_Lunge
	***************************************************************************/
	void GoblinAI::Melee_Lunge::Init() 
	{
		// Get Stuff From Blackboard
		const auto gameObj_Self = mpBlackboard->GetObject("Self");
		
		// If pointer to self is null, set him.
		if (nullptr == mpOwner)
			mpOwner = CORE::Get<SceneSystem>()->FindGameObject(gameObj_Self);


		// Set Animation To Idle
		auto mpRend = mpOwner->GetComponent<SpriteRenderer>();
		
		if (mpRend)
		{
			if (!mpRend->IsPlaying())
			{
				mpRend->Stop();
				mpRend->SetAnimation("Attack");
				mpRend->Play();
			}
		}
	}

	NeuralTree::Node::eStatus GoblinAI::Melee_Lunge::Update() 
	{
		return eStatus::SUCCESS;
	}

	void GoblinAI::Melee_Lunge::Exit(eStatus _status) 
	{
	
	}

	/***************************************************************************
	*  GOBLIN TASK: Death
	***************************************************************************/
	void GoblinAI::Death::Init() 
	{
		
	}

	NeuralTree::Node::eStatus GoblinAI::Death::Update() 
	{
		return eStatus::SUCCESS;
	}

	void GoblinAI::Death::Exit(eStatus _status) 
	{
	
	}

	
}




