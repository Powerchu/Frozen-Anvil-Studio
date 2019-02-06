/* HEADER *********************************************************************************/
/*!
\file	RuneAnim.cpp
\author Dan (100%)
\par    email: dan.kang\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "RuneAnim.h"
#include "System/Input/InputSystem.h"
#include "System/Input/InputMap.h"
#include "System/Driver/Driver.h"
#include "System/Behaviour/BehaviourSystem.h"

#include "System/Scene/SceneSystem.h"
#include "System/Collision/CollisionEvent.h"
#include "System/Collision/CollisionSystem.h"

#include "Object/ObjectFlags.h"
#include "Object/GameObject.h"

#include "Editor/EGUI.h"
#include "Utility/DebugAssert.h"
#include "Component/SpriteRenderer.h"



namespace Dystopia
{
	namespace RuneAnim_MSG
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
	RuneAnim::RuneAnim()
		:mbTriggered{false}
	{
	}

	RuneAnim::~RuneAnim()
	{
	}

	void RuneAnim::Load()
	{
	}

	void RuneAnim::Awake()
	{
	}

	void RuneAnim::Init()
	{
	}

	void RuneAnim::Update(const float _fDeltaTime)
	{
	}

	void RuneAnim::FixedUpdate(const float _fDeltaTime)
	{
	}
	
	void RuneAnim::PostUpdate(void)
	{
	}

	void RuneAnim::GameObjectDestroy(void)
	{
	}

	void RuneAnim::Unload(void)
	{
	}

	void Dystopia::RuneAnim::OnCollisionEnter(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::RuneAnim::OnCollisionStay(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::RuneAnim::OnCollisionExit(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::RuneAnim::OnTriggerEnter(GameObject * const _obj)
	{
	}

	void Dystopia::RuneAnim::OnTriggerStay(GameObject * const _obj)
	{
	}

	void Dystopia::RuneAnim::OnTriggerExit(GameObject * const _obj)
	{
	}

	RuneAnim * RuneAnim::Duplicate() const
	{
		return new RuneAnim{*this};
	}

	void RuneAnim::Serialise(TextSerialiser& _ser) const
	{
	}

	void RuneAnim::Unserialise(TextSerialiser& _ser)
	{
	}

	void RuneAnim::Activate()
	{
		auto mpRend = GetOwner()->GetComponent<SpriteRenderer>();

		if(mpRend && !mbTriggered)
		{
			mpRend->Stop();
			mpRend->SetAnimation("Activated");
			mpRend->Play();  
			mbTriggered = true;
		}
	}

	const char * const RuneAnim::GetBehaviourName() const
	{
		return RuneAnim::BehaviourName;
	}

	void RuneAnim::EditorUI(void) noexcept
	{
		
		
	}
	
	TypeErasure::TypeEraseMetaData RuneAnim::GetMetaData()
	{
		static MetaData<Dystopia::RuneAnim> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const RuneAnim::GetMetaData() const
	{
		static MetaData<Dystopia::RuneAnim> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
}




