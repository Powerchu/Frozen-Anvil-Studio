/* HEADER *********************************************************************************/
/*!
\file	GoblinAI.cpp
\author AaronChu (100%)
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

#include "System/Collision/CollisionEvent.h"  
#include "System/Collision/CollisionSystem.h"

#include "Object/ObjectFlags.h"
#include "Object/GameObject.h"

#include "Editor/EGUI.h"  
#include "Utility/DebugAssert.h"  

#include "System/AI/AISystem.h"
#include "Component/AiController.h"
#include "Component/Transform.h"
#include "Component/RigidBody.h"
#include "Utility/GUID.h"
#include "System/AI/Blackboard.h"
#include "Component/SpriteRenderer.h"



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
		SetActive(true);

		const auto mController = GetOwner()->GetComponent<AiController>();
		if (mController == nullptr) return;  
 
		auto mContBlackboard = mController->GetBlackboard();		
		  
		const auto& root = NeuralTree::Builder() 
			.composite<NeuralTree::Selector>() 
				.task<IsAlive>(mContBlackboard)
				.composite<NeuralTree::Sequence>()  
					.task<Idle>(mContBlackboard)
					.task<Alert>(mContBlackboard)
					.task<Attack>(mContBlackboard) 
				.end() 
			.end() 
		.Build(mContBlackboard, "Goblin AI Tree"); 

		mController->SetTree(root);
	}
 
	void GoblinAI::Init()
	{   
	}

	void GoblinAI::Update(const float )
	{
		const auto mController = GetOwner()->GetComponent<AiController>();
		if (mController == nullptr) return;

		auto mContBlackboard = mController->GetBlackboard();
		auto Target = EngineCore::Get<SceneSystem>()->FindGameObject_cstr("Player");
		auto Self = GetOwner(); 

		if (Target == nullptr || Self == nullptr) return;

		const float _dist = (Self->GetComponent<Transform>()->GetGlobalPosition() - Target->GetComponent<Transform>()->GetGlobalPosition()).Magnitude();
		mContBlackboard->SetFloat("Dist Target", _dist);
		mContBlackboard->SetObject("Self",   Self->GetID()); 
		mContBlackboard->SetObject("Player", Target->GetID());
	} 

	void GoblinAI::FixedUpdate(const float )
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

	void Dystopia::GoblinAI::OnCollisionEnter(const CollisionEvent& )
	{

	}

	void Dystopia::GoblinAI::OnCollisionStay(const CollisionEvent& )
	{

	}

	void Dystopia::GoblinAI::OnCollisionExit(const CollisionEvent&)
	{

	}

	void Dystopia::GoblinAI::OnTriggerEnter(GameObject * const )
	{
	}

	void Dystopia::GoblinAI::OnTriggerStay(GameObject * const)
	{
	}

	void Dystopia::GoblinAI::OnTriggerExit(GameObject * const) 
	{
	}

	GoblinAI * GoblinAI::Duplicate() const
	{
		return new GoblinAI{ *this };
	}

	void GoblinAI::Serialise(TextSerialiser&) const
	{
	}

	void GoblinAI::Unserialise(TextSerialiser&)
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
		static auto mReturn = TypeErasure::TypeEraseMetaData{ mMetaData };
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const GoblinAI::GetMetaData() const
	{
		static MetaData<Dystopia::GoblinAI> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{ mMetaData };
		return mReturn;
	}
}




