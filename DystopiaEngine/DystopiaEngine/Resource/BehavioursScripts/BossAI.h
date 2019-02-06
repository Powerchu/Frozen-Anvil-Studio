/* HEADER *********************************************************************************/
/*!
\file	BossAI.h
\author aaron (100%)
\par    email: m.chu\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _BossAI_H_
#define _BossAI_H_

#define str(s) #s

#include "Behaviour/Behaviour.h"
#include "Reflection/Reflection.h"
#include "Reflection/ReflectionTypeErasure.h"
#include "Behaviour/BehaviourMemberFunc.h"
#include "System/AI/AISystem.h"
#include "Component/AiController.h"
#include "Component/Transform.h"
#include "Component/RigidBody.h"
#include "Utility/GUID.h"
#include "Object/GameObject.h"
#include "System/AI/Blackboard.h"
#include "System/Scene/SceneSystem.h"
#include "Component/SpriteRenderer.h"
#include "System/Graphics/GraphicsSystem.h"
#include "System/Graphics/Texture.h"
#include "System/File/FileSystem.h" 
#include "Component/Convex.h"


#define DllExport   __declspec( dllexport )

namespace Dystopia
{
	class CheckHealth : public NeuralTree::Task
	{
	public:
		HashString GetEditorName(void) const override { return "CheckHealth"; }

		CheckHealth(NeuralTree::Blackboard::Ptr _bb) 
			: Task(_bb)
		{

		}

		CheckHealth(NeuralTree::Blackboard::Ptr _bb, float healthCompare) 
			: Task(_bb), mfHealthCompare(healthCompare)
		{

		}
		
		void Init() override
		{
			mfHealth = mpBlackboard->GetFloat("Health");
		}

		eStatus Update() override
		{
			mfHealth = mpBlackboard->GetFloat("Health");

			if (mfHealth < mfHealthCompare)
			{
				return eStatus::SUCCESS;
			}
			else 
			{
				return eStatus::FAIL;
			}
		}
		float mfHealth, mfHealthCompare;
	};

	class Move : public NeuralTree::Task
	{
	public:
		HashString GetEditorName(void) const override { return "Move"; }

		Move(NeuralTree::Blackboard::Ptr _bb) 
			: Task(_bb)
		{

		}
		
		void Init() override
		{
			if (auto attk = EngineCore::Get<SceneSystem>()->FindGameObject_cstr("BossAttackArea"))
			{
				attk->SetActive(false);
			}

			if (const auto mpFire = EngineCore::Get<SceneSystem>()->FindGameObject_cstr("Flame"))
			{
				if ( const auto col = mpFire->GetComponent<Collider>())
				{
					col->SetActive(false);
				}
			}
			// change texture
			isFacingRight = mpBlackboard->GetBool("IsFacingRight");
			
			const auto gameObjID = mpBlackboard->GetObject("Player");
			const auto gameObjID2 = mpBlackboard->GetObject("Self");
			mpTarget = EngineCore::Get<SceneSystem>()->FindGameObject(gameObjID);
			mpOwner = EngineCore::Get<SceneSystem>()->FindGameObject(gameObjID2);

			mfDist = mpBlackboard->GetFloat("Dist Target");
			alertDist = mpBlackboard->GetFloat("Alert Range");
			mfSpeed = mpBlackboard->GetFloat("MoveSpeed");
			mfAttackDelay = mpBlackboard->GetFloat("AttackDelay");

			// change texture
			auto pTex = EngineCore::Get<GraphicsSystem>()->LoadTexture(EngineCore::Get<FileSystem>()->GetFromResource("Walk_Sheet.dds").c_str());
			auto mpRend = mpOwner->GetComponent<SpriteRenderer>();

			
			if (mpRend)
			{
				if (!mpRend->IsPlaying())
				{
					mpRend->Stop();
					mpRend->SetTexture(pTex);
					mpRend->SetAnimation("Walk");
					mpRend->Play();
				}
			}

		}

		eStatus Update() override
		{
			if (mpTarget == nullptr || mpOwner == nullptr) return eStatus::FAIL;


			mfAttackDelay -= 0.02f;

			mfDist = mpBlackboard->GetFloat("Dist Target");
			
			if (mfDist < alertDist)
			{
				// Melee Range
				if (mfDist < 1600.f && mfAttackDelay < 0.f) return eStatus::SUCCESS;
				
				const auto own_Scale = mpOwner->GetComponent<Transform>()->GetGlobalScale();
				// Player is Left of Self
				if (mpTarget->GetComponent<Transform>()->GetGlobalPosition().x - mpOwner->GetComponent<Transform>()->GetGlobalPosition().x < 0.f)
				{
					if (isFacingRight)
					{
						mpOwner->GetComponent<Transform>()->SetGlobalScale(-own_Scale.x, own_Scale.y, own_Scale.z);
						isFacingRight = false;
					}
					mpOwner->GetComponent<RigidBody>()->AddLinearImpulse({-1*200*mfSpeed, 0,0});
					
				}
				else // Player is Right of Self
				{
					if (!isFacingRight)
					{
						mpOwner->GetComponent<Transform>()->SetGlobalScale(-own_Scale.x, own_Scale.y, own_Scale.z);
						isFacingRight = true;
					}
					mpOwner->GetComponent<RigidBody>()->AddLinearImpulse({200*mfSpeed, 0,0});
				}

				mpBlackboard->SetBool("IsFacingRight", isFacingRight);
				

				if (mfDist > 1750.f) return eStatus::FAIL;

				return eStatus::RUNNING;

			}
			else
			{
				return eStatus::FAIL;	
			}
		}

		float mfDist, alertDist, mfSpeed;
		GameObject* mpTarget;
		GameObject* mpOwner;
		bool isFacingRight = true;
		float mfAttackDelay = 0.f;
	};

	class Melee : public NeuralTree::Task
	{
	public:
		HashString GetEditorName(void) const override { return "Melee"; }

		Melee(NeuralTree::Blackboard::Ptr _bb) 
			: Task(_bb)
		{

		}
		
	void Init() override
		{
			// change texture.
			isFacingRight = mpBlackboard->GetBool("IsFacingRight");
			const auto gameObjID = mpBlackboard->GetObject("Player");
			mpTarget = EngineCore::Get<SceneSystem>()->FindGameObject(gameObjID);
			const auto gameObjID2 = mpBlackboard->GetObject("Self");
			mpOwner = EngineCore::Get<SceneSystem>()->FindGameObject(gameObjID2);
			mpBlackboard->SetFloat("AttackDelay", 1.3f);

			const auto own_Scale = mpOwner->GetComponent<Transform>()->GetGlobalScale();

			// Player is Left of Self
			if (mpTarget->GetComponent<Transform>()->GetGlobalPosition().x - mpOwner->GetComponent<Transform>()->GetGlobalPosition().x < 0.f)
			{
				if (isFacingRight)
				{
					mpOwner->GetComponent<Transform>()->SetGlobalScale(-own_Scale.x, own_Scale.y, own_Scale.z);
					isFacingRight = false;
				}
				
			}
			else // Player is Right of Self
			{
				if (!isFacingRight)
				{
					mpOwner->GetComponent<Transform>()->SetGlobalScale(-own_Scale.x, own_Scale.y, own_Scale.z);
					isFacingRight = true;
				}
			}

			mpBlackboard->SetBool("IsFacingRight", isFacingRight);
			
			// change texture
			auto pTex = EngineCore::Get<GraphicsSystem>()->LoadTexture(EngineCore::Get<FileSystem>()->GetFromResource("Slash_Sheet.dds").c_str());
			auto mpRend = mpOwner->GetComponent<SpriteRenderer>();
			
			if (mpRend)
			{
				mpRend->Stop();
				mpRend->SetTexture(pTex);
				mpRend->SetAnimation("Attack");
				mpRend->Play();
			}

		}

		eStatus Update() override
		{
			auto mpRend = mpOwner->GetComponent<SpriteRenderer>();
			
			if (mpRend)
			{
				if (!mpRend->IsPlaying())
				{
					if (auto attk = EngineCore::Get<SceneSystem>()->FindGameObject_cstr("BossAttackArea"))
					{
						attk->SetActive(true);
					}
					return eStatus::SUCCESS;
				}
			}

			return eStatus::RUNNING;
		}
		GameObject* mpOwner = nullptr;
		GameObject* mpTarget = nullptr;
		bool isFacingRight = false;
	};

	class Breathe : public NeuralTree::Task
	{
	public:
		HashString GetEditorName(void) const override { return "Breathe"; }

		Breathe(NeuralTree::Blackboard::Ptr _bb) 
			: Task(_bb)
		{

		}
		
		void Init() override
		{
			// change texture.
			isFacingRight = mpBlackboard->GetBool("IsFacingRight");
			const auto gameObjID = mpBlackboard->GetObject("Player");
			mpTarget = EngineCore::Get<SceneSystem>()->FindGameObject(gameObjID);
			const auto gameObjID2 = mpBlackboard->GetObject("Self");
			mpOwner = EngineCore::Get<SceneSystem>()->FindGameObject(gameObjID2);
			const auto own_Scale = mpOwner->GetComponent<Transform>()->GetGlobalScale();
			
			// Player is Left of Self
			if (mpTarget->GetComponent<Transform>()->GetGlobalPosition().x - mpOwner->GetComponent<Transform>()->GetGlobalPosition().x < 0.f)
			{
				if (isFacingRight)
				{
					mpOwner->GetComponent<Transform>()->SetGlobalScale(-own_Scale.x, own_Scale.y, own_Scale.z);
					isFacingRight = false;
				}
				
			}
			else // Player is Right of Self
			{
				if (!isFacingRight)
				{
					mpOwner->GetComponent<Transform>()->SetGlobalScale(-own_Scale.x, own_Scale.y, own_Scale.z);
					isFacingRight = true;
				}
			}

			mpBlackboard->SetBool("IsFacingRight", isFacingRight);

			// change texture
			auto pTex = EngineCore::Get<GraphicsSystem>()->LoadTexture(EngineCore::Get<FileSystem>()->GetFromResource("FireBreath_Sheet.dds").c_str());
			auto mpRend = mpOwner->GetComponent<SpriteRenderer>();
			
			// play breathe in
			if (mpRend)
			{
				mpRend->SetTexture(pTex);
				mpRend->SetAnimation("Breathing");
				mpRend->Play();
				
			}
		}

		eStatus Update() override
		{
			auto mpRend = mpOwner->GetComponent<SpriteRenderer>();
			
			// play breathe in
			if (mpRend)
			{
				if (!mpRend->IsPlaying())
				{
					mpRend->Stop();
					mpRend->SetAnimation("Fire");
					mpRend->Play();
					return eStatus::SUCCESS;	
				}
			}
			return eStatus::RUNNING;
		}

		GameObject* mpOwner = nullptr;
		GameObject* mpTarget = nullptr;
		bool isFacingRight = false;
	};

	class Fire : public NeuralTree::Task
	{
	public:
		HashString GetEditorName(void) const override { return "Fire"; }

		Fire(NeuralTree::Blackboard::Ptr _bb) 
			: Task(_bb)
		{

		}
		
		void Init() override
		{
			// play fire out animation
			const auto gameObjID2 = mpBlackboard->GetObject("Self");
			//const auto gameObjID1 = mpBlackboard->GetObject("Flame");
			
			mpOwner = EngineCore::Get<SceneSystem>()->FindGameObject(gameObjID2);
			//mpFire = EngineCore::Get<SceneSystem>()->FindGameObject(gameObjID1);

			auto mpRend = mpOwner->GetComponent<SpriteRenderer>();
			if (mpRend)
			{
				mpRend->Stop();
				mpRend->SetAnimation("Fire");
				mpRend->Play();
				
			}
			
		}

		eStatus Update() override
		{
			//!isPlaying then success
			if (mpOwner == nullptr) return eStatus::FAIL;

			auto mpRend = mpOwner->GetComponent<SpriteRenderer>();
			if (mpRend)
			{
				auto mpFireRend = mpFire->GetComponent<SpriteRenderer>();
				// play breathe in
				// if !isPlaying then SUCCESS
				if (!mpRend->IsPlaying())
				{
					if (mpFireRend)
					{
						mpFireRend->Stop();
						mpFireRend->SetAnimation("Flame");
						mpFireRend->Play();
						if (auto attk = mpFire->GetComponent<Collider>())
						{
							attk->SetActive(true);
						}
					}
					return eStatus::SUCCESS;
				}
			}
			return eStatus::RUNNING;
		}
		GameObject* mpOwner = nullptr;
		GameObject* mpFire = nullptr;
	};


	class BossAI : public Behaviour
	{
	public:
		static constexpr const char * BehaviourName = str(BossAI);
#if !EDITOR

		using SYSTEM = BehaviourSystem;
		// unsigned GetBehaviourType(void) const
		// {
		// 	return Ut::MetaFind_t<Ut::Decay_t<decltype(*this)>, AllBehaviours>::value;
		// };

#endif
		virtual const std::string GetEditorName(void) const override { return "BossAI"; }
		static uint64_t constexpr mBossAIID = 796908032;

		// Default Constructor for this Behaviour - init your variables here
		BossAI();
		// Default Destructor for this Behaviour - don't touch if you dk, else ask the tech team
		~BossAI();

		// Load will never be called
		virtual void Load(void) override;
		// Awake occurs when the editor is first started
		virtual void Awake(void) override;
		// Init occurs when the scene is played
		virtual void Init(void) override;

		// Update occurs every frame
		virtual void Update(const float _fDeltaTime) override;
		// FixedUpdate occurs every 0.02ms
		virtual void FixedUpdate(const float _fDeltaTime) override;
		// PostUpdate occurs after the above two updates
		virtual void PostUpdate(void) override;

		// GameObjectDestroy occurs when the gameobject is destroyed
		virtual void GameObjectDestroy(void) override;
		// Unload will never occur
		virtual void Unload(void) override;

		// If Owner has a Collider, and enters another collider, this occurs
		virtual void OnCollisionEnter(const CollisionEvent&);
		// If Owner has a Collider, and stays in another collider, this occurs
		virtual void OnCollisionStay(const CollisionEvent&);
		// If Owner has a Collider, and exits another collider, this occurs
		virtual void OnCollisionExit(const CollisionEvent&);

		// If Owner has a Trigger, and enters another trigger, this occurs
		virtual void OnTriggerEnter(GameObject * const);
		// If Owner has a Trigger, and stays in another trigger, this occurs
		virtual void OnTriggerStay(GameObject * const);
		// If Owner has a Trigger, and exit another trigger, this occurs
		virtual void OnTriggerExit(GameObject * const);

		virtual void Serialise(TextSerialiser&) const override;
		virtual void Unserialise(TextSerialiser&) override;

		virtual const char * const GetBehaviourName() const;

		// Don't touch
		virtual BossAI * Duplicate() const;

		// Don't touch
		virtual void EditorUI(void) noexcept override;

		// Reflection Stuff - Don't Touch
		virtual TypeErasure::TypeEraseMetaData       GetMetaData();
		virtual TypeErasure::TypeEraseMetaData const GetMetaData() const;

	private:
		// Don't touch
		friend MetaData<BossAI>;
	};

	extern "C"
	{
		inline DllExport BossAI * BossAIClone()
		{
			return new BossAI;
		}
	}
}

/*Keep this if you do not want to show anything in Editor*/
PP_REFLECT_EMPTY(Dystopia::BossAI)
/*
Uncomment the line PP_REFLECT and add in the names of the variable you want to show
Comment out PP_REFLECT_EMPTY.
*/
//PP_REFLECT(Dystopia::BossAI)

#endif //_BossAI_H_


