/* HEADER *********************************************************************************/
/*!
\file	RuneWall.cpp
\author Dan (100%)
\par    email: dan.kang\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "RuneWall.h"
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
	namespace RuneWall_MSG
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
	RuneWall::RuneWall()
		:mfTravelSpeed{0.0f},
		mbActivated{false},
		mfDelay{3.0f},
		mfCount{0.0f}
	{
	}

	RuneWall::~RuneWall()
	{
	}

	void RuneWall::Load()
	{
	}

	void RuneWall::Awake()
	{
	}

	void RuneWall::Init()
	{
	}

	void RuneWall::Update(const float _fDeltaTime)
	{
		if (mbActivated) 
		{
			auto player = EngineCore::Get<SceneSystem>()->FindGameObject("CharacterController");
        	auto CamAi = EngineCore::Get<SceneSystem>()->FindGameObject("CameraAI");
            RuneWall_MSG::SendExternalMessage(player, "DisableControls", true);
            RuneWall_MSG::SendExternalMessage(CamAi, "SetNewTarget", Math::Vector3D{15130,1200,0});

			mfCount = mfCount + _fDeltaTime;
		}
		
		if (mfCount >= mfDelay)
		{
			auto t = GetOwner()->GetComponent<Transform>()->GetGlobalPosition();
			
			if (t.y >= -500)
				t.y = t.y - 10;
			else
			{
				auto player = EngineCore::Get<SceneSystem>()->FindGameObject("CharacterController");
				auto CamAi = EngineCore::Get<SceneSystem>()->FindGameObject("CameraAI");
            	RuneWall_MSG::SendExternalMessage(player, "DisableControls", false);
            	RuneWall_MSG::SendExternalMessage(CamAi, "ResetTarget");
			}
			
			GetOwner()->GetComponent<Transform>()->SetGlobalPosition(t);
		}
	}

	void RuneWall::FixedUpdate(const float )
	{
	}
	
	void RuneWall::PostUpdate(void)
	{
	}

	void RuneWall::GameObjectDestroy(void)
	{
	}

	void RuneWall::Unload(void)
	{
	}

	void Dystopia::RuneWall::OnCollisionEnter(const CollisionEvent& )
	{

	}

	void Dystopia::RuneWall::OnCollisionStay(const CollisionEvent& )
	{

	}

	void Dystopia::RuneWall::OnCollisionExit(const CollisionEvent& )
	{

	}

	void Dystopia::RuneWall::OnTriggerEnter(GameObject * const )
	{
	}

	void Dystopia::RuneWall::OnTriggerStay(GameObject * const )
	{
	}

	void Dystopia::RuneWall::OnTriggerExit(GameObject * const )
	{
	}

	RuneWall * RuneWall::Duplicate() const
	{
		return new RuneWall{*this};
	}

	void RuneWall::Serialise(TextSerialiser& ) const
	{
	}

	void RuneWall::Unserialise(TextSerialiser& )
	{
	}

	void RuneWall::Activate()
	{
		mbActivated = true;
	}


	const char * const RuneWall::GetBehaviourName() const
	{
		return RuneWall::BehaviourName;
	}

	void RuneWall::EditorUI(void) noexcept
	{
		
		
	}
	
	TypeErasure::TypeEraseMetaData RuneWall::GetMetaData()
	{
		static MetaData<Dystopia::RuneWall> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const RuneWall::GetMetaData() const
	{
		static MetaData<Dystopia::RuneWall> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
}




