/* HEADER *********************************************************************************/
/*!
\file	SpriteManager.cpp
\author Dan Kang (100%)
\par    email: dan.kang\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "SpriteManager.h"
#include "System/Input/InputSystem.h"
#include "System/Input/InputMap.h"
#include "System/Driver/Driver.h"
#include "System/Behaviour/BehaviourSystem.h"

#include "System/Scene/SceneSystem.h"
#include "System/Collision/CollisionEvent.h"
#include "System/Collision/CollisionSystem.h"

#include "Object/ObjectFlags.h"
#include "Object/GameObject.h"
#include "Component/SpriteRenderer.h"

#include "Editor/EGUI.h"
#include "Utility/DebugAssert.h"



namespace Dystopia
{
	namespace SpriteManager_MSG
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
	SpriteManager::SpriteManager()
		: messageSent(false)
		, isJumping(false)
		, facingRight(true)
	{
	}

	SpriteManager::~SpriteManager()
	{
	}

	void SpriteManager::Load()
	{
	}

	void SpriteManager::Awake()
	{
		myRenderer = GetOwner()->GetComponent<SpriteRenderer>();
		SetFlags(FLAG_ACTIVE);
	}

	void SpriteManager::Init()
	{
		myRenderer = GetOwner()->GetComponent<SpriteRenderer>();
		SetFlags(FLAG_ACTIVE);  
	}

	void SpriteManager::Update(const float )
	{
		if(!myRenderer->IsPlaying())
			IsPlaying();
	}

	void SpriteManager::FixedUpdate(const float )
	{
	} 

	void SpriteManager::PostUpdate(void)
	{
	}

	void SpriteManager::GameObjectDestroy(void)
	{
	}

	void SpriteManager::Unload(void)
	{
	}

	void Dystopia::SpriteManager::OnCollisionEnter(const CollisionEvent& )
	{

	}

	void Dystopia::SpriteManager::OnCollisionStay(const CollisionEvent& )
	{

	}

	void Dystopia::SpriteManager::OnCollisionExit(const CollisionEvent& )
	{

	}

	void Dystopia::SpriteManager::OnTriggerEnter(GameObject * const )
	{
	}

	void Dystopia::SpriteManager::OnTriggerStay(GameObject * const )
	{
	}

	void Dystopia::SpriteManager::OnTriggerExit(GameObject * const )
	{
	}

	SpriteManager * SpriteManager::Duplicate() const
	{
		return new SpriteManager{ *this };
	}

	void SpriteManager::Serialise(TextSerialiser& ) const
	{
	}

	void SpriteManager::Unserialise(TextSerialiser& )
	{
	}


	const char * const SpriteManager::GetBehaviourName() const
	{
		return SpriteManager::BehaviourName;
	}

	void SpriteManager::EditorUI(void) noexcept
	{


	}

	void SpriteManager::PlayAnimation(int animNum, bool isFacingRight)
	{
		DEBUG_PRINT(eLog::MESSAGE, "animNum %d!", animNum);
		if (auto ptr = GetOwner()->GetComponent<SpriteRenderer>())
		{
			facingRight = isFacingRight;
			messageSent = true;

			if (!messageSent)
				return; 
			
			messageSent = false;

			if (animNum == 1)
			{
				myRenderer->Stop();
				myRenderer->SetAnimation("AttackOne");
				myRenderer->ResizeToFit( isFacingRight ? 0.1f : -0.1f );
				myRenderer->SetSpeed(0.066f);
				myRenderer->Play();
			}

			else if (animNum == 2)
			{
				myRenderer->Stop();
				myRenderer->SetAnimation("AttackTwo");
				myRenderer->ResizeToFit( isFacingRight ? 0.1f : -0.1f );
				myRenderer->SetSpeed(0.086f);
				myRenderer->Play();
			}

			else if (animNum == 3)
			{
				myRenderer->Stop();
				myRenderer->SetAnimation("AttackThree");
				myRenderer->ResizeToFit( isFacingRight ? 0.1f : -0.1f );
				myRenderer->SetSpeed(0.066f);
				myRenderer->Play();
			}

			else if (animNum == 4)
			{
				myRenderer->Stop();
				myRenderer->SetAnimation("Run");
				myRenderer->ResizeToFit( isFacingRight ? 0.1f : -0.1f );
				myRenderer->SetSpeed(0.066f);
				myRenderer->Play();
			}
			
			else if (animNum == 0)
			{
				myRenderer->Stop();
				myRenderer->SetAnimation("Idle");
				myRenderer->ResizeToFit( isFacingRight ? 0.1f : -0.1f );
				myRenderer->SetSpeed(0.066f);
				myRenderer->Play();
			}

			else if (animNum == 5)
			{
				myRenderer->Stop();
				myRenderer->SetAnimation("CastForce");
				myRenderer->ResizeToFit( isFacingRight ? 0.1f : -0.1f );
				myRenderer->SetSpeed(0.066f);
				myRenderer->Play();
			}

			else if (animNum == 6)
			{
				myRenderer->Stop();
				myRenderer->SetAnimation("CastForm");
				myRenderer->ResizeToFit( isFacingRight ? 0.1f : -0.1f );
				myRenderer->SetSpeed(0.066f);
				myRenderer->Play();
			}

			else if (animNum == 7)
			{
				myRenderer->Stop();
				myRenderer->SetAnimation("Jump");
				myRenderer->ResizeToFit( isFacingRight ? 0.1f : -0.1f );
				myRenderer->SetSpeed(0.066f);
				myRenderer->Play();
				isJumping = true; 
			}

			else if (animNum == 8)
			{
				myRenderer->Stop();
				myRenderer->SetAnimation("Fall");
				myRenderer->ResizeToFit( isFacingRight ? 0.1f : -0.1f );
				myRenderer->SetSpeed(0.066f);
				myRenderer->Play();
			}

			else if (animNum == 9)
			{
				myRenderer->Stop();
				myRenderer->SetAnimation("Land");
				myRenderer->ResizeToFit( isFacingRight ? 0.1f : -0.1f );
				myRenderer->SetSpeed(0.066f);
				myRenderer->Play();  
			}

		}
	}

	void SpriteManager::IsPlaying()
	{
		if (!messageSent)
		{
			if (!isJumping)
			{
				SpriteManager_MSG::SendExternalMessage(GetOwner()->GetComponent<Transform>()->GetParent()->GetOwner(), "AnimationComplete");
				myRenderer->Stop();
				myRenderer->SetAnimation("Idle");
				myRenderer->ResizeToFit( facingRight ? 0.1f : -0.1f );
				myRenderer->Play();
			}

			else
			{
				SpriteManager_MSG::SendExternalMessage(GetOwner()->GetComponent<Transform>()->GetParent()->GetOwner(), "AnimationComplete");
				myRenderer->Stop();
				myRenderer->SetAnimation("Fall");
				myRenderer->ResizeToFit( facingRight ? 0.1f : -0.1f );
				myRenderer->Play();
				isJumping = false;
			}
			messageSent = true;  
		}
	}

	TypeErasure::TypeEraseMetaData SpriteManager::GetMetaData()
	{
		static MetaData<Dystopia::SpriteManager> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{ mMetaData };
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const SpriteManager::GetMetaData() const
	{
		static MetaData<Dystopia::SpriteManager> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{ mMetaData };
		return mReturn;
	}
}




