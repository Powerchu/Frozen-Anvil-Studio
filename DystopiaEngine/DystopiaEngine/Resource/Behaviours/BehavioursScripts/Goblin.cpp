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
#include "System/Behaviour/BehaviourSystem.h"
#include "Editor/EGUI.h"

#include "System/Scene/SceneSystem.h"
#include "Object/GameObject.h"
#include "System/Collision/CollisionEvent.h" 
#include "Component/SpriteRenderer.h"
#include "Component/Transform.h"


namespace Dystopia
{
	
	namespace Goblin_MSG
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
	
	Goblin::Goblin()
		: theMass(0.f)
		, takenDamage(false)
		, damageCount(0.0f)
		, damageDelay(1.0f)
		, rBody(nullptr) 
		, attackCount(0.0f)
		, attackDelay(0.95f)
		, isWinding(true)
		, playerTarget{nullptr}
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
		playerTarget = EngineCore::GetInstance()->Get<SceneSystem>()->FindGameObject_cstr("Player");

		if (auto s = GetOwner()->GetComponent<SpriteRenderer>())
		{
			s->Stop();
			s->SetAnimation("Run");
			s->Play();
		}

		rBody = GetOwner()->GetComponent<RigidBody>();
		if(rBody)
			theMass = rBody->GetMass();
		SetFlags(FLAG_ACTIVE);
	}
	void Goblin::Init()
	{
		playerTarget = EngineCore::GetInstance()->Get<SceneSystem>()->FindGameObject_cstr("Player");

		rBody = GetOwner()->GetComponent<RigidBody>();
		if(rBody)
			theMass = rBody->GetMass();
		SetFlags(FLAG_ACTIVE);
	} 

	void Goblin::Update(const float _fDeltaTime)
	{
		auto Position   = playerTarget->GetComponent<Transform>()->GetGlobalPosition();
		auto myPosition = GetOwner()->GetComponent<Transform>()->GetGlobalPosition();
		auto v          = (Position - myPosition);
		//rBody->AddLinearImpulse(Math::MakeVector3D(-2000000.f,0,0));
		if (isWinding)
		{
			attackCount = attackCount + _fDeltaTime;
			if (auto s = GetOwner()->GetComponent<SpriteRenderer>())
			{
				if (s->IsPlaying() && s->GetCurrentAnimation() == HashString{"Run"})
				{
					s->Stop();
					s->SetAnimation("Attack"); 
					s->Play();	
				}
			}
		}
		else if (v.Magnitude() < 100.f)
		{
			if (auto s = GetOwner()->GetComponent<SpriteRenderer>())
			{
				if (!s->IsPlaying())
				{
					s->Stop();
					s->SetAnimation("Run");
					s->Play();	
				}
			}

			if(rBody)
			{
				auto scale = GetOwner()->GetComponent<Transform>()->GetScale();
				if(v.x < 0 && scale.x > 0)
				{
					GetOwner()->GetComponent<Transform>()->SetScale(-scale.x, scale.y, scale.z  );


				}
				else if(v.x > 0 && scale.x < 0)
				{
					GetOwner()->GetComponent<Transform>()->SetScale(-scale.x, scale.y, scale.z  );

				}
				rBody->AddLinearImpulse(Math::MakeVector3D(70.f * scale.x/abs(scale.x), 0,0));
			}
		}
						
		if (damageCount <= damageDelay && takenDamage)
			damageCount = damageCount + _fDeltaTime;

		if (damageCount >= damageDelay)
		{
			takenDamage = false;
			damageCount = 0.0f;
		}

		if (attackCount >= attackDelay)
		{
			isWinding = false;
			attackCount = 0.0f;
			Goblin_MSG::SendExternalMessage(GetOwner()->GetComponent<Transform>()->GetAllChild()[0]->GetOwner(), "DealDamage", 10.0f);
		}
	}

	void Goblin::FixedUpdate(const float )
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

	void Dystopia::Goblin::OnCollisionEnter(const CollisionEvent& )
	{
		// auto * ptr = EngineCore::GetInstance()->Get<SceneSystem>()->FindGameObject(_colEvent.mOtherID);
			
		// if(ptr)
		// {
		// 	const char * name = ptr->GetNamePtr();
		// 	if(!strcmp(name,"PlayerAttackTrig"))
		// 	{
		// 		isColliding = true; 
		// 	}
		// 	else if(!strcmp(name,"Fireball"))
		// 	{
		// 		mHealth -= 5;
		// 	}
		// 	else if(!strcmp(name,"Missle"))
		// 	{
		// 		mHealth -= 1;
		// 		isColliding = true;
		// 	}
		// }
	}

	void Dystopia::Goblin::OnCollisionStay(const CollisionEvent& )
	{
		// auto * ptr = EngineCore::GetInstance()->Get<SceneSystem>()->FindGameObject(_colEvent.mOtherID);
		// if(ptr)
		// {
		// 	//const char * name = ptr->Get
		// 	const char * name = ptr->GetNamePtr();
		// 	if(!strcmp(name,"PlayerAttackTrig"))
		// 	{
		// 		isColliding = true;
		// 	}
		// 	else if(!strcmp(name,"Missle"))
		// 	{
		// 		mHealth -= 1;
		// 		isColliding = true;
		// 	}
		// }
	}

	void Dystopia::Goblin::OnCollisionExit(const CollisionEvent& )
	{
		isColliding = false;
	}

	void Dystopia::Goblin::OnTriggerEnter(GameObject * const )
	{
		// if(_obj)
		// {
		// 	//const char * name = ptr->Get
		// 	const char * name = _obj->GetNamePtr();
		// 	if(!strcmp(name,"Missle"))
		// 	{
		// 		mHealth -= 1;
		// 		isColliding = true;
		// 	}	
		// }
	}

	void Dystopia::Goblin::OnTriggerStay(GameObject * const )
	{
		// if(_obj)
		// {
		// 	//const char * name = ptr->Get
		// 	const char * name = _obj->GetNamePtr();
		// 	if(!strcmp(name,"Missle"))
		// 	{
		// 		//if(_obj->GetComponent<SpriteRenderer>()->AnimationFinished())
		// 		isColliding = true;
		// 	}
		// }
	}

	void Dystopia::Goblin::OnTriggerExit(GameObject * const _obj)
	{
		isColliding = false;
	}

	Goblin * Goblin::Duplicate() const
	{
		return new Goblin{*this}; 
	}

	void Goblin::Serialise(TextSerialiser& ) const
	{
	}

	void Goblin::Unserialise(TextSerialiser& )
	{
	}


	const char * const Goblin::GetBehaviourName() const
	{
		return Goblin::BehaviourName;
	}

	void Goblin::EditorUI(void) noexcept 
	{
		
		
	}
	
	void Goblin::TakeDamage(int _dmg)
	{
		if (!takenDamage)
		{
			DEBUG_PRINT(eLog::MESSAGE, "TOOK DMG");
			mHealth -= _dmg;
			
			if (auto s = GetOwner()->GetComponent<SpriteRenderer>())
			{
				if (!s->IsPlaying())
				{
					s->Stop();
					s->SetAnimation("Flinch");
					s->Play();	
				}
			}

			if (mHealth <= 0)
				GetOwner()->Destroy();
			
			takenDamage = true;
		}
	}

	void Goblin::Knock(int _amt, int _direction)
	{
		DEBUG_PRINT(eLog::MESSAGE, "received Knock");
		
		if (_direction == 0) //knock up
			rBody->AddLinearImpulse({0, _amt * theMass, 0});

		if (_direction == 1) //knock left
			rBody->AddLinearImpulse({ -(_amt * theMass), 0, 0 });
		
		if (_direction == 2) //knock right
			rBody->AddLinearImpulse({ _amt * theMass, 0, 0 });
	}

	void Goblin::Attacking()
	{
		isWinding = true;
		if (auto s = GetOwner()->GetComponent<SpriteRenderer>())
		{
			s->Stop();
			s->SetAnimation("Run");
			s->Play();	
		}
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




