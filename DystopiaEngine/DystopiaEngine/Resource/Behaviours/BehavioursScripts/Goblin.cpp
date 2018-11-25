/* HEADER *********************************************************************************/
/*!
\file	Goblin.cpp
\author Dan (100%)
\par    email: Dan.Kang\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "Goblin.h"
#include "System/Input/InputSystem.h"
#include "System/Input/InputMap.h"
#include "System/Driver/Driver.h"
#include "Editor/EGUI.h"

#include "System/Scene/SceneSystem.h"
#include "Object/GameObject.h"
#include "System/Collision/CollisionEvent.h" 
#include "Component/SpriteRenderer.h"

namespace Dystopia
{
	
	namespace Goblin_MSG
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
	
	Goblin::Goblin()
	{
	}

	Goblin::~Goblin()
	{
	}

	void Goblin::Load()
	{
	}
    void Goblin::Awake(void)
	{
		rBody = GetOwner()->GetComponent<RigidBody>();
		mass = rBody->GetMass();
		SetFlags(FLAG_ACTIVE);
	}
	void Goblin::Init()
	{
		const auto mpTarget = EngineCore::GetInstance()->Get<SceneSystem>()->FindGameObject_cstr("Player");
		auto blackboard = bTree.GetBlackboard();
		Math::Vector4 vectorT;
		if (mpTarget)
		{
			vectorT = mpTarget->GetComponent<Transform>()->GetGlobalPosition();
		}
		blackboard->setVector("Target", vectorT);
		blackboard->setVector("Owner", GetOwner()->GetComponent<Transform>()->GetGlobalPosition());
		blackboard->setGameObj("Owner", GetOwner());
		blackboard->setInt("Health", mHealth);
		NeuralTree::Builder()
			.composite<NeuralTree::Sequence>()
				.leaf<CheckDistNode>(blackboard)
				.decorator<NeuralTree::Inverter>()
					.composite<NeuralTree::Sequence>()
						.leaf<CheckHealth>(blackboard)
						.leaf<ChaseEnemy>(blackboard)
					.end()
				.end()
				.leaf<RunAway>(blackboard)
			.end()
		.Build(bTree);
		
		rBody = GetOwner()->GetComponent<RigidBody>();
		mass = rBody->GetMass();
		SetFlags(FLAG_ACTIVE);
	} 

	void Goblin::Update(const float _fDeltaTime)
	{
		return;
		const auto mpTarget = EngineCore::GetInstance()->Get<SceneSystem>()->FindGameObject_cstr("Player");
		auto blackboard = bTree.GetBlackboard();
		Math::Vector4 vectorT;
		if (mpTarget)
		{
			vectorT = mpTarget->GetComponent<Transform>()->GetGlobalPosition();
		}
		blackboard->setVector("Target", vectorT);
		blackboard->setVector("Owner", GetOwner()->GetComponent<Transform>()->GetPosition());
		blackboard->setInt("Health", mHealth);
		
		bTree.Update();
		
		if(EngineCore::GetInstance()->Get<InputManager>()->IsKeyTriggered(eButton::KEYBOARD_X) && isColliding)
		{
			mHealth -= 5;
		}

        if(mHealth <= 0)
        {
            GetOwner()->Destroy();
        }

	}

	void Goblin::FixedUpdate(const float _fDeltaTime)
	{
	}
	
	void Goblin::PostUpdate(void)
	{
		
	}

	void Goblin::GameObjectDestroy(void)
	{
	}

	void Goblin::Unload(void)
	{
	}

	void Dystopia::Goblin::OnCollisionEnter(const CollisionEvent& _colEvent)
	{
		auto * ptr = EngineCore::GetInstance()->Get<SceneSystem>()->FindGameObject(_colEvent.mOtherID);
		//if(EngineCore::GetInstance()->Get<InputManager>()->IsKeyPressed(eButton::KEYBOARD_RIGHT))
			
		if(ptr)
		{
			//const char * name = ptr->Get
			const char * name = ptr->GetNamePtr();
			if(!strcmp(name,"PlayerAttackTrig"))
			{
				isColliding = true;
			}
			else if(!strcmp(name,"Fireball"))
			{
				mHealth -= 5;
			}
			else if(!strcmp(name,"Missle"))
			{
				mHealth -= 1;
				isColliding = true;
			}
		}
	}

	void Dystopia::Goblin::OnCollisionStay(const CollisionEvent& _colEvent)
	{
		auto * ptr = EngineCore::GetInstance()->Get<SceneSystem>()->FindGameObject(_colEvent.mOtherID);
		if(ptr)
		{
			//const char * name = ptr->Get
			const char * name = ptr->GetNamePtr();
			if(!strcmp(name,"PlayerAttackTrig"))
			{
				isColliding = true;
			}
			else if(!strcmp(name,"Missle"))
			{
				mHealth -= 1;
				isColliding = true;
			}
		}
	}

	void Dystopia::Goblin::OnCollisionExit(const CollisionEvent& _colEvent)
	{
		isColliding = false;
	}

	void Dystopia::Goblin::OnTriggerEnter(GameObject * const _obj)
	{
		if(_obj)
		{
			//const char * name = ptr->Get
			const char * name = _obj->GetNamePtr();
			if(!strcmp(name,"Missle"))
			{
				mHealth -= 1;
				isColliding = true;
			}	
		}
	}

	void Dystopia::Goblin::OnTriggerStay(GameObject * const _obj)
	{
		if(_obj)
		{
			//const char * name = ptr->Get
			const char * name = _obj->GetNamePtr();
			if(!strcmp(name,"Missle"))
			{
				//if(_obj->GetComponent<SpriteRenderer>()->AnimationFinished())
				isColliding = true;
			}
		}
	}

	void Dystopia::Goblin::OnTriggerExit(GameObject * const _obj)
	{
		isColliding = false;
	}

	Goblin * Goblin::Duplicate() const
	{
		return new Goblin{}; 
	}

	void Goblin::Serialise(TextSerialiser& _ser) const
	{
	}

	void Goblin::Unserialise(TextSerialiser& _ser)
	{
	}


	const char * const Goblin::GetBehaviourName() const
	{
		return Goblin::BehaviourName;
	}

	void Goblin::EditorUI(void) noexcept
	{
		
		
	}
	
	void Goblin::TakeDamage(int _dmg, bool _isFacingRight)
	{
		mHealth -= _dmg;

		if (mHealth < 0)
		GetOwner()->Destroy();

		if (_isFacingRight)
			rBody->AddLinearImpulse({30 * mass,0,0});
		else
			rBody->AddLinearImpulse({-30 * mass,0,0});
	}

	void Goblin::Knock(int _amt, int _direction)
	{
		DEBUG_PRINT(eLog::MESSAGE, "received Knock");
		
		if (_direction == 0) //knock up
			rBody->AddLinearImpulse({0, _amt * mass, 0});

		if (_direction == 1) //knock left
			rBody->AddLinearImpulse({ -(_amt * mass), 0, 0 });
		
		if (_direction == 2) //knock right
			rBody->AddLinearImpulse({ _amt * mass, 0, 0 });
	}

	TypeErasure::TypeEraseMetaData Goblin::GetMetaData()
	{
		/*TO DO*/
		/*REMEMBER TO RETURN YOUR REFLECTED DATA HERE*/
		static MetaData<Dystopia::Goblin> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const Goblin::GetMetaData() const
	{
		/*TO DO*/
		/*REMEMBER TO RETURN YOUR REFLECTED DATA HERE*/
		return TypeErasure::TypeEraseMetaData{};
	}
	
	// void Goblin::ReceiveMessage(const char * const _FuncName, BehaviourMessage _msg)
    // {
		// for(auto & elem : mMemberFunc)
		// {
			// if(!std::strcmp(elem.first, _FuncName))
			// { 
				// elem.second.Invoke(this, _msg); 
			// }
		// }
    // }
}




