/* HEADER *********************************************************************************/
/*!
\file	BossAI.cpp
\author aaron (100%)
\par    email: m.chu\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "BossAI.h"
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


#include "Math/MathUtility.h"

namespace Dystopia
{
	namespace BossAI_MSG
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
	BossAI::BossAI()  
	{ 
	}

	BossAI::~BossAI()
	{
	}

	void BossAI::Load() 
	{ 
	}  
  
	void BossAI::Awake()
	{
		const auto mController = GetOwner()->GetComponent<AiController>();
		if (mController == nullptr) return;

		auto mContBlackboard = mController->GetBlackboard();		 
		const auto& root = NeuralTree::Builder()     
			.composite<NeuralTree::Selector>() // root   
				.composite<NeuralTree::Selector>()
					.task<CheckHealth>(mContBlackboard,200.f) 
					.decorator<NeuralTree::Inverter>()
						.composite<NeuralTree::Sequence>()
							.task<Move>(mContBlackboard)
							.task<Melee>(mContBlackboard)
						.end()
					.end()
				.end()  
				.composite<NeuralTree::Selector>() 
					.task<CheckHealth>(mContBlackboard, 50.f)
					.decorator<NeuralTree::Inverter>() 
						.composite<NeuralTree::Selector>()
							.composite<NeuralTree::Sequence>()
								.task<Move>(mContBlackboard)
								.task<Melee>(mContBlackboard) 
							.end()
							.composite<NeuralTree::Sequence>()
								.task<Breathe>(mContBlackboard)
								.task<Fire>(mContBlackboard) 
							.end()
						.end()
					.end()
				.end()
			.end() 
		.Build(mContBlackboard, "Boss AI Tree");

		mController->SetTree(root); 
	}  

	void BossAI::Init() 
	{
	}
 
	void BossAI::Update(const float ) 
	{
		const auto mController = GetOwner()->GetComponent<AiController>();
		if (mController == nullptr) return;
 
		auto mContBlackboard = mController->GetBlackboard(); 
		auto Target = EngineCore::Get<SceneSystem>()->FindGameObject_cstr("CharacterController");
		//auto Flame = EngineCore::Get<SceneSystem>()->FindGameObject_cstr("Flame");
		auto Self = GetOwner();

		if (Target == nullptr || Self == nullptr) return;
		
		const float _dist = (Self->GetComponent<Transform>()->GetGlobalPosition() - Target->GetComponent<Transform>()->GetGlobalPosition()).Magnitude();
		mContBlackboard->SetFloat("Dist Target", _dist);
		mContBlackboard->SetObject("Self", GetOwner()->GetID());  
		mContBlackboard->SetObject("CharacterController", Target->GetID());
		//mContBlackboard->SetObject("Flame", Flame->GetID());
	    if (Math::Abs(_dist) <= mContBlackboard->GetFloat("Alert Range"))
			if (auto o = EngineCore::Get<SceneSystem>()->FindGameObject_cstr("CharacterController"))
				BossAI_MSG::SendExternalMessage(o, "FadeHUD", true);
	}

	void BossAI::FixedUpdate(const float )
	{
	}

	void BossAI::PostUpdate(void)
	{
	}

	void BossAI::GameObjectDestroy(void)
	{
	}
 
	void BossAI::Unload(void) 
	{
	}
 
	void Dystopia::BossAI::OnCollisionEnter(const CollisionEvent& )
	{

	}

	void Dystopia::BossAI::OnCollisionStay(const CollisionEvent& )
	{

	}

	void Dystopia::BossAI::OnCollisionExit(const CollisionEvent& )
	{

	}

	void Dystopia::BossAI::OnTriggerEnter(GameObject * const )
	{
	}

	void Dystopia::BossAI::OnTriggerStay(GameObject * const )
	{
	}

	void Dystopia::BossAI::OnTriggerExit(GameObject * const )
	{
	}

	BossAI * BossAI::Duplicate() const
	{
		return new BossAI{ *this };
	}

	void BossAI::Serialise(TextSerialiser& ) const
	{
	}
 
	void BossAI::Unserialise(TextSerialiser& )
	{
	}
 

	const char * const BossAI::GetBehaviourName() const
	{
		return BossAI::BehaviourName;
	}

	void BossAI::EditorUI(void) noexcept
	{


	}

	TypeErasure::TypeEraseMetaData BossAI::GetMetaData()
	{
		static MetaData<Dystopia::BossAI> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{ mMetaData };
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const BossAI::GetMetaData() const
	{
		static MetaData<Dystopia::BossAI> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{ mMetaData };
		return mReturn;
	}
}




