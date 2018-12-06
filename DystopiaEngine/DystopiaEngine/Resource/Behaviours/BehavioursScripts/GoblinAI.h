/* HEADER *********************************************************************************/
/*!
\file	GoblinAI.h
\author AaronChu (100%)
\par    email: m.chu\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _GoblinAI_H_
#define _GoblinAI_H_

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

#define DllExport   __declspec( dllexport )

namespace Dystopia
{
	class IsAlive : public NeuralTree::Task
	{
	public:
		HashString GetEditorName(void) const override { return "IsAlive"; }

		IsAlive(NeuralTree::Blackboard::Ptr _bb) 
			: Task(_bb)
		{

		}
		
		void Init() override
		{
			mfHealth = mfMaxHealth = mpBlackboard->GetFloat("Health");

			const auto gameObjID2 = mpBlackboard->GetObject("Self");
			mpOwner = EngineCore::Get<SceneSystem>()->FindGameObject(gameObjID2);
			if (mpOwner == nullptr) return;
			auto mpRend = mpOwner->GetComponent<SpriteRenderer>();
			if (mpRend)
			{
				mpRend->Stop();
				mpRend->SetAnimation("Idle");
				mpRend->Play();
				
			}
		}

		eStatus Update() override
		{
			mfHealth = mpBlackboard->GetFloat("Health");
			if (mfHealth < 0.f)
			{
				return eStatus::SUCCESS;
			}
			else
			{
			 	return eStatus::FAIL;
			}
		}

		float mfHealth = 10.f, mfMaxHealth = 20.f;
		GameObject* mpOwner = nullptr;
	};

	class Idle : public NeuralTree::Task
	{
	public:
		HashString GetEditorName(void) const override { return "Idle"; }

		Idle(NeuralTree::Blackboard::Ptr _bb) 
			: Task(_bb)
		{

		}
		
		void Init() override
		{
			mfDist = mpBlackboard->GetFloat("Dist Target");
			alertDist = mpBlackboard->GetFloat("Alert Range");

			const auto gameObjID2 = mpBlackboard->GetObject("Self");
			mpOwner = EngineCore::Get<SceneSystem>()->FindGameObject(gameObjID2);

			if (mpOwner == nullptr) return;
			auto mpRend = mpOwner->GetComponent<SpriteRenderer>();
			if (mpRend)
			{
				mpRend->Stop();
				mpRend->SetAnimation("Idle");
				mpRend->Play();
			}

		}

		eStatus Update() override
		{
			mfDist = mpBlackboard->GetFloat("Dist Target");
			if (mfDist < alertDist)
			{
			 	return eStatus::SUCCESS;
			}
			else
			{
				return eStatus::RUNNING;
			}

		}
		float mfDist, alertDist;
		GameObject* mpOwner = nullptr;

	};

	class Alert : public NeuralTree::Task
	{
	public:
		HashString GetEditorName(void) const override { return "Alert"; }
		Alert(NeuralTree::Blackboard::Ptr _bb): Task(_bb)
		{
			
		}
		
		void Init() override 
		{
			const auto gameObjID = mpBlackboard->GetObject("Player");
			const auto gameObjID2 = mpBlackboard->GetObject("Self");
			mpTarget = EngineCore::Get<SceneSystem>()->FindGameObject(gameObjID);
			mpOwner = EngineCore::Get<SceneSystem>()->FindGameObject(gameObjID2);

			mfDist = mpBlackboard->GetFloat("Dist Target");
			alertDist = mpBlackboard->GetFloat("Alert Range");
			mfSpeed = mpBlackboard->GetFloat("MoveSpeed");

			if (mpOwner == nullptr) return;
			auto mpRend = mpOwner->GetComponent<SpriteRenderer>();
			if (mpRend)
			{
				if (!mpRend->IsPlaying())
				{
					mpRend->Stop();
					mpRend->SetAnimation("Chase");
					mpRend->Play();
				}
			}
		}

		eStatus Update() override
		{
			if (mpTarget == nullptr || mpOwner == nullptr) return eStatus::FAIL;
			
			mfDist = mpBlackboard->GetFloat("Dist Target");

			if (mfDist < alertDist)
			{
				if (mfDist < 35.f) return eStatus::SUCCESS;
				
				const auto own_Scale = mpOwner->GetComponent<Transform>()->GetGlobalScale();
				// Player is Left of Self
				if (mpTarget->GetComponent<Transform>()->GetGlobalPosition().x - mpOwner->GetComponent<Transform>()->GetGlobalPosition().x < 0.f)
				{
					if (isFacingRight)
					{
						mpOwner->GetComponent<Transform>()->SetGlobalScale(-own_Scale.x, own_Scale.y, own_Scale.z);
						isFacingRight = false;
						DEBUG_PRINT(eLog::MESSAGE, "Left");
					}
					mpOwner->GetComponent<RigidBody>()->AddLinearImpulse({-1*10*mfSpeed, 0,0});
					
				}
				else // Player is Right of Self
				{
					if (!isFacingRight)
					{
						mpOwner->GetComponent<Transform>()->SetGlobalScale(-own_Scale.x, own_Scale.y, own_Scale.z);
						isFacingRight = true;
						DEBUG_PRINT(eLog::MESSAGE, "RIGHT");
					}
					mpOwner->GetComponent<RigidBody>()->AddLinearImpulse({10*mfSpeed, 0,0});
				}

				return eStatus::RUNNING;

			}
			else
			{
				return eStatus::FAIL;	
			}
		}

		float mfDist, alertDist, mfSpeed;
		GameObject* mpTarget = nullptr;
		GameObject* mpOwner = nullptr;
		bool isFacingRight = true;
	};

	class Attack : public NeuralTree::Task
	{
	public:
		HashString GetEditorName(void) const override { return "Attack"; }

		Attack(NeuralTree::Blackboard::Ptr _bb)	: Task(_bb)
		{

		}
		
		void Init() override
		{
			const auto gameObjID2 = mpBlackboard->GetObject("Self");
			mpOwner = EngineCore::Get<SceneSystem>()->FindGameObject(gameObjID2);
			if (mpOwner) return;
			auto mpRend = mpOwner->GetComponent<SpriteRenderer>();
			if (mpRend)
			{
				mpRend->Stop();
				mpRend->SetAnimation("Attack");
				mpRend->Play();
			}

		}

		eStatus Update() override
		{
			if (mpOwner != nullptr)
			{
				auto mpRend = mpOwner->GetComponent<SpriteRenderer>();
				if (mpRend)
				{
					return eStatus::SUCCESS;
				}
			}
		
			return eStatus::RUNNING;
		}
		GameObject* mpOwner = nullptr;
	};

	class GoblinAI : public Behaviour
	{
	public:
		static constexpr const char * BehaviourName = str(GoblinAI);
#if !EDITOR

		using SYSTEM = BehaviourSystem;
		// unsigned GetBehaviourType(void) const
		// {
		// 	return Ut::MetaFind_t<Ut::Decay_t<decltype(*this)>, AllBehaviours>::value;
		// };

#endif
		virtual const std::string GetEditorName(void) const override { return "GoblinAI"; }
		static uint64_t constexpr mGoblinAIID = 50860544;

		// Default Constructor for this Behaviour - init your variables here
		GoblinAI();
		// Default Destructor for this Behaviour - don't touch if you dk, else ask the tech team
		~GoblinAI();

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
		virtual GoblinAI * Duplicate() const;

		// Don't touch
		virtual void EditorUI(void) noexcept override;

		// Reflection Stuff - Don't Touch
		virtual TypeErasure::TypeEraseMetaData       GetMetaData();
		virtual TypeErasure::TypeEraseMetaData const GetMetaData() const;

	private:
		// Don't touch
		friend MetaData<GoblinAI>;
		AiController* mpAiComp;
	};

	extern "C"
	{
		inline DllExport GoblinAI * GoblinAIClone()
		{
			return new GoblinAI;
		}
	}
}

/*Keep this if you do not want to show anything in Editor*/
PP_REFLECT_EMPTY(Dystopia::GoblinAI)
/*
Uncomment the line PP_REFLECT and add in the names of the variable you want to show
Comment out PP_REFLECT_EMPTY.
*/
//PP_REFLECT(Dystopia::GoblinAI)

#endif //_GoblinAI_H_


