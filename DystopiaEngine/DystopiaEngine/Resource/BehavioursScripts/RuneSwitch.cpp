/* HEADER *********************************************************************************/
/*!
\file	RuneSwitch.cpp
\author Dan (100%)
\par    email: dan.kang\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "RuneSwitch.h"
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



namespace Dystopia
{
	namespace RuneSwitch_MSG
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
	RuneSwitch::RuneSwitch()
		:mbIsActivated{false},
		mpTargetWall{nullptr},
		mpTargetAnim{nullptr},
		mbIsDown{false}
	{
	}

	RuneSwitch::~RuneSwitch()
	{
	}

	void RuneSwitch::Load()
	{
	}

	void RuneSwitch::Awake()
	{
	}

	void RuneSwitch::Init()
	{
		mpTargetWall = EngineCore::Get<SceneSystem>()->FindGameObject("SwitchWall");
		mpTargetAnim = EngineCore::Get<SceneSystem>()->FindGameObject("Switch1");
	}

	void RuneSwitch::Update(const float _fDeltaTime)
	{
	}

	void RuneSwitch::FixedUpdate(const float _fDeltaTime)
	{
	}
	
	void RuneSwitch::PostUpdate(void)
	{
	}

	void RuneSwitch::GameObjectDestroy(void)
	{
	}

	void RuneSwitch::Unload(void)
	{
	}

	void Dystopia::RuneSwitch::OnCollisionEnter(const CollisionEvent& _colEvent)
	{
		if (_colEvent.mCollidedWith && _colEvent.mCollidedWith->HasTag("Form_Slam_Rock"))
		{
			mbIsActivated = true;
			RuneSwitch_MSG::SendExternalMessage(mpTargetWall, "Activate");
			RuneSwitch_MSG::SendExternalMessage(mpTargetAnim, "Activate");
			
			if (!mbIsDown)
			{
				auto t = GetOwner()->GetComponent<Transform>()->GetGlobalPosition();
				t.y = t.y - 0.05;
				
				GetOwner()->GetComponent<Transform>()->SetGlobalPosition(t);
				mbIsDown = true;
			}
		}
	}

	void Dystopia::RuneSwitch::OnCollisionStay(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::RuneSwitch::OnCollisionExit(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::RuneSwitch::OnTriggerEnter(GameObject * const _obj)
	{
	}

	void Dystopia::RuneSwitch::OnTriggerStay(GameObject * const _obj)
	{
	}

	void Dystopia::RuneSwitch::OnTriggerExit(GameObject * const _obj)
	{
	}

	RuneSwitch * RuneSwitch::Duplicate() const
	{
		return new RuneSwitch{*this};
	}

	void RuneSwitch::Serialise(TextSerialiser& _ser) const
	{
	}

	void RuneSwitch::Unserialise(TextSerialiser& _ser)
	{
	}


	const char * const RuneSwitch::GetBehaviourName() const
	{
		return RuneSwitch::BehaviourName;
	}

	void RuneSwitch::EditorUI(void) noexcept
	{
		
		
	}
	
	TypeErasure::TypeEraseMetaData RuneSwitch::GetMetaData()
	{
		static MetaData<Dystopia::RuneSwitch> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const RuneSwitch::GetMetaData() const
	{
		static MetaData<Dystopia::RuneSwitch> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
}