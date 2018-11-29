/* HEADER *********************************************************************************/
/*!
\file	Goblin.h
\author Dan (100%)
\par    email: Dan.Kang\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _Goblin_H_
#define _Goblin_H_

#define str(s) #s

#include "Behaviour/Behaviour.h"
#include "Reflection/Reflection.h"
#include "Reflection/ReflectionTypeErasure.h"
#include "System/AI/AISystem.h"
#include "Math/Vector4.h"
#include "Object/GameObject.h"
#include "Component/RigidBody.h"
#include "Utility/DebugAssert.h"
#include <map>
#include "Behaviour/BehaviourMemberFunc.h"

#define DllExport   __declspec( dllexport )

namespace Dystopia
{
	class CheckDistNode : public NeuralTree::Task
	{
	public:
		CheckDistNode(NeuralTree::Blackboard::Ptr _bb)
			: Task(_bb)
		{
			
		}

		void Init() override
		{
			mTarget = mpBlackboard->GetVector("Target");
			mOwner = mpBlackboard->GetVector("Owner");
			mpOwnerObj = mpBlackboard->GetObject("Owner");
		}

		eStatus Update() override
		{
			Math::Vec4 own_Scale;
			if (mpOwnerObj)
			{
				own_Scale = mpOwnerObj->GetComponent<Transform>()->GetScale();
			}

			const float dist = (mTarget - mOwner).Magnitude();

			if (dist < 220.0F)
			{
				if (mpOwnerObj)
				{
					if ((mOwner.x-mTarget.x) > 0.0F) // player is left
					{
						if (isFacingRight)
						{
							mpOwnerObj->GetComponent<Transform>()->SetScale(-own_Scale.x, own_Scale.y, own_Scale.z);
							isFacingRight = false;
						}
					}
					else // player is right
					{
						if (!isFacingRight)
						{
							mpOwnerObj->GetComponent<Transform>()->SetScale(-own_Scale.x, own_Scale.y, own_Scale.z);
							isFacingRight = true;
						}
					}
				}
				mpBlackboard->SetBool("IsFacingRight", isFacingRight);
				return eStatus::SUCCESS;
			}
			mpBlackboard->SetBool("IsFacingRight", isFacingRight);
			return eStatus::FAIL;
		}

		Math::Vec3D mTarget;
		Math::Vec3D mOwner;
		GameObject* mpOwnerObj = nullptr;
		bool isFacingRight = false;
	};

	class CheckHealth : public NeuralTree::Task
	{
	public:
		CheckHealth(NeuralTree::Blackboard::Ptr _bb)
			: Task(_bb)
		{

		}

		void Init() override
		{
			mfHealth = mpBlackboard->GetInt("Health");
			//DEBUG_PRINT(eLog::MESSAGE, "CheckHealth");
		}

		eStatus Update() override
		{
			if (mfHealth > 25.0F)
			{
				return eStatus::SUCCESS;
			}
			return eStatus::FAIL;
		}

		int mfHealth;
	};

	class ChaseEnemy : public NeuralTree::Task
	{
	public:
		ChaseEnemy(NeuralTree::Blackboard::Ptr _bb)
			: Task(_bb)
		{

		}

		void Init() override
		{
			mbIsFacingRight = mpBlackboard->GetBool("IsFacingRight");
			myRigid = mpBlackboard->GetObject("Owner")->GetComponent<RigidBody>();
			//DEBUG_PRINT(eLog::MESSAGE, "ChaseEnemy");
		}

		eStatus Update() override
		{
			if (mbIsFacingRight)
			{
				myRigid->AddLinearImpulse({ 15 * myRigid->GetMass(),0,0 });
			}
			else
			{
				myRigid->AddLinearImpulse({ -15 * myRigid->GetMass(),0,0 });
			}

			return eStatus::SUCCESS;
		}

		bool mbIsFacingRight = false;
		RigidBody* myRigid = nullptr;
	};

	class RunAway : public NeuralTree::Task
	{
	public:
		RunAway(NeuralTree::Blackboard::Ptr _bb)
			: Task(_bb)
		{

		}

		void Init() override
		{
			mTarget = mpBlackboard->GetVector("Target");
			mOwner = mpBlackboard->GetVector("Owner");
			my_rigid = mpBlackboard->GetObject("Owner")->GetComponent<RigidBody>();
			//DEBUG_PRINT(eLog::MESSAGE, "RunAway");
		}

		eStatus Update() override
		{
			if ((mOwner.x - mTarget.x) > 0.0F) // player is left
			{
				my_rigid->AddLinearImpulse({ 15 * my_rigid->GetMass(),0,0 });
			}
			else // player is right
			{
				my_rigid->AddLinearImpulse({ -15 * my_rigid->GetMass(),0,0 });
			}
			return eStatus::SUCCESS;

		}

		Math::Vec3D mTarget;
		Math::Vec3D mOwner;
		RigidBody* my_rigid = nullptr;
	};
	
	class Goblin : public Behaviour
	{
	public:

		static constexpr const char * BehaviourName = str(Goblin);
#if !EDITOR
		
		using SYSTEM = BehaviourSystem;
		// unsigned GetBehaviourType(void) const
		// {
		// 	return Ut::MetaFind_t<Ut::Decay_t<decltype(*this)>, AllBehaviours>::value;
		// };

#endif
		virtual const std::string GetEditorName(void) const override { return "Goblin"; }
		static uint64_t constexpr mGoblinID = 708352512;

		Goblin();
		~Goblin();
		
		virtual void Load(void) override;
		virtual void Init(void) override;
		virtual void Awake(void) override;
		
		virtual void Update(const float _fDeltaTime) override;
		virtual void FixedUpdate(const float _fDeltaTime) override;
		virtual void PostUpdate(void) override;

		virtual void GameObjectDestroy(void) override;
		virtual void Unload(void) override;

		virtual void OnCollisionEnter(const CollisionEvent&) override;
		virtual void OnCollisionStay (const CollisionEvent&) override;
		virtual void OnCollisionExit (const CollisionEvent&) override;

		virtual void OnTriggerEnter(GameObject * const) override;
		virtual void OnTriggerStay (GameObject * const) override;
		virtual void OnTriggerExit (GameObject * const) override;

		virtual void Serialise(TextSerialiser&) const override;
		virtual void Unserialise(TextSerialiser&) override;

		virtual const char * const GetBehaviourName() const;

		virtual Goblin * Duplicate() const;
		
		virtual void EditorUI(void) noexcept override;
		
		void TakeDamage(int _dmg, bool _isFacingRight);
		void Knock(int _amt, int _direction);
		
		// Reflection Stuff
		virtual TypeErasure::TypeEraseMetaData       GetMetaData();
		virtual TypeErasure::TypeEraseMetaData const GetMetaData() const;

		int mHealth = 50;
		RigidBody * rBody;
		float mass;
		
		bool isColliding = false;
		NeuralTree::BehaviourTree bTree;
		
		PP_MEMBERFUNC(Dystopia::Goblin, TakeDamage, Knock)
		
	private:
		friend MetaData<Goblin>;
	};

	extern "C"
	{
		inline DllExport Goblin * GoblinClone()
		{
			return new Goblin;
		}
	}
}

PP_REFLECT(Dystopia::Goblin, mHealth)

#endif //_Goblin_H_


