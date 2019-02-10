/* HEADER *********************************************************************************/
/*!
\file	GoblinAI.h
\author Aaron (100%)
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
#include "Behaviour/BehaviourMemberFunc.h"
#include "Reflection/Reflection.h"
#include "Reflection/ReflectionTypeErasure.h"

#include "System/AI/AISystem.h"
#include "System/Scene/SceneSystem.h"
#include "System/Graphics/GraphicsSystem.h"
#include "System/Graphics/Texture.h"
#include "System/File/FileSystem.h" 

#include "Object/GameObject.h"
#include "Component/SpriteRenderer.h"
#include "Component/RayCollider.h"
#include "Component/Transform.h"
#include "Component/RigidBody.h"
#include "Component/AiController.h"
#include "Component/AudioSource.h"

#include "Utility/GUID.h"

#define DllExport   __declspec( dllexport )

namespace Dystopia
{
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
		static uint64_t constexpr mGoblinAIID = 18446744071712376320;

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
		virtual void OnCollisionStay (const CollisionEvent&);
		// If Owner has a Collider, and exits another collider, this occurs
		virtual void OnCollisionExit (const CollisionEvent&);

		// If Owner has a Trigger, and enters another trigger, this occurs
		virtual void OnTriggerEnter(GameObject * const);
		// If Owner has a Trigger, and stays in another trigger, this occurs
		virtual void OnTriggerStay (GameObject * const);
		// If Owner has a Trigger, and exit another trigger, this occurs
		virtual void OnTriggerExit (GameObject * const);

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


		void SetPlayerDeath(bool _status);

		PP_MEMBERFUNC(Dystopia::GoblinAI, SetPlayerDeath)

		/*	
		*	Public Member Variables to Reflect
		*/
		float MaxTimer;
		float StateTimer;
		float AttackTimer;
		float AttackCooldown;
		int   RallyCount;
		bool  IsGrounded;
		bool  IsFacingRight;
		bool  HasFlock;
		bool  IsAlerted = false;
		bool  PlayerDied = false;
		Math::Vector3D mOrgScale;

		GameObject* AlertObj = nullptr;
		GameObject* Player = nullptr;
		AudioSource*    mpAudioSrc = nullptr;

		/*
		*  GOBLIN AI
		*/

		struct Idle : public NeuralTree::Task
		{
			HashString GetEditorName(void) const override { return "Idle"; }

			Idle(NeuralTree::Blackboard::Ptr _bb) 
				: Task(_bb)
			{
				Reset();
			}

			void Init() override;
			eStatus Update() override;
			void Exit(eStatus _status) override;

		private:
			GameObject* mpOwner = nullptr;
			float _stateTimer;
		};

		struct Patrol : public NeuralTree::Task
		{
			HashString GetEditorName(void) const override { return "Patrol"; }

			Patrol(NeuralTree::Blackboard::Ptr _bb) 
				: Task(_bb)
			{
				Reset();
			}
			
			void Init() override;
			eStatus Update() override;
			void Exit(eStatus _status) override;

		private:
			GameObject* mpOwner = nullptr;
			float _stateTimer;
			unsigned _direction = 0;

		};

		struct Alert : public NeuralTree::Task
		{
			HashString GetEditorName(void) const override { return "Alert"; }

			Alert(NeuralTree::Blackboard::Ptr _bb) 
				: Task(_bb)
			{
				Reset();
			}
			
		void Init() override;
		eStatus Update() override;
		void Exit(eStatus) override;
		private:
			GameObject* mpOwner = nullptr;
			GameObject* mpPlayer = nullptr;
		};

		struct Rally : public NeuralTree::Task
		{
			HashString GetEditorName(void) const override { return "Rally"; }

			Rally(NeuralTree::Blackboard::Ptr _bb) 
				: Task(_bb)
			{
				Reset();
			}
			
			void Init() override;
			eStatus Update() override;
			void Exit(eStatus) override;
		private:
			GameObject* mpOwner = nullptr;
		};

		struct Chase : public NeuralTree::Task
		{
			HashString GetEditorName(void) const override { return "Chase"; }

			Chase(NeuralTree::Blackboard::Ptr _bb) 
				: Task(_bb)
			{
				Reset();
			}
			
			void Init() override;
			eStatus Update() override;
			void Exit(eStatus) override;
		private:
			GameObject* mpOwner = nullptr;
			GameObject* mpPlayer = nullptr;
		};

		struct Melee_Slash : public NeuralTree::Task
		{
			HashString GetEditorName(void) const override { return "Melee_Slash"; }

			Melee_Slash(NeuralTree::Blackboard::Ptr _bb) 
				: Task(_bb)
			{
				Reset();
			}
			
			void Init() override;
			eStatus Update() override;
			void Exit(eStatus _status) override;
		private:
			GameObject* mpOwner = nullptr;
			GameObject* mpPlayer = nullptr;
		};

		struct Melee_Lunge : public NeuralTree::Task
		{
			HashString GetEditorName(void) const override { return "Melee_Lunge"; }

			Melee_Lunge(NeuralTree::Blackboard::Ptr _bb) 
				: Task(_bb)
			{
				Reset();
			}
			
			void Init() override;
			eStatus Update() override;
			void Exit(eStatus _status) override;
		private:
			GameObject* mpOwner = nullptr;
		};

		struct Death : public NeuralTree::Task
		{
			HashString GetEditorName(void) const override { return "Death"; }

			Death(NeuralTree::Blackboard::Ptr _bb) 
				: Task(_bb)
			{
				Reset();
			}
			
			void Init() override;
			eStatus Update() override;
			void Exit(eStatus _status) override;
		};

		private:
			// Don't touch
			friend MetaData<GoblinAI>;
			AiController* mpController;
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
//PP_REFLECT_EMPTY(Dystopia::GoblinAI)
/*
  Uncomment the line PP_REFLECT and add in the names of the variable you want to show
  Comment out PP_REFLECT_EMPTY.
*/
PP_REFLECT_STUFF(Dystopia::GoblinAI, MaxTimer
								   , StateTimer
								   , AttackTimer
								   , AttackCooldown
								   , RallyCount
								   , IsGrounded
								   , IsFacingRight
								   , HasFlock
								   , IsAlerted)

#endif //_GoblinAI_H_


