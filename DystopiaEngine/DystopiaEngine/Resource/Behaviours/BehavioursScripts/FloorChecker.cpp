/* HEADER *********************************************************************************/
/*!
\file	FloorChecker.cpp
\author Dan Kang (100%)
\par    email: dan.kang\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "FloorChecker.h"
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
	namespace FloorChecker_MSG
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
		void SendExternalMessage(GameObject const *  _ptr, const char * _FuncName, Ts ... _Params)
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
	FloorChecker::FloorChecker()
	{
	}

	FloorChecker::~FloorChecker()
	{
	}

	void FloorChecker::Load()
	{
	}

	void FloorChecker::Awake()
	{
	}

	void FloorChecker::Init()
	{
	}

	void FloorChecker::Update(const float _fDeltaTime)
	{
	}

	void FloorChecker::FixedUpdate(const float _fDeltaTime)
	{
	}
	
	void FloorChecker::PostUpdate(void)
	{
	}

	void FloorChecker::GameObjectDestroy(void)
	{
	}

	void FloorChecker::Unload(void)
	{
	}

	void Dystopia::FloorChecker::OnCollisionEnter(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::FloorChecker::OnCollisionStay(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::FloorChecker::OnCollisionExit(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::FloorChecker::OnTriggerEnter(GameObject * const _obj)
	{
	}

	void Dystopia::FloorChecker::OnTriggerStay(GameObject * const _obj)
	{
	}

	void Dystopia::FloorChecker::OnTriggerExit(GameObject * const _obj)
	{
	}

	FloorChecker * FloorChecker::Duplicate() const
	{
		return new FloorChecker{};
	}

	void FloorChecker::Serialise(TextSerialiser& _ser) const
	{
	}

	void FloorChecker::Unserialise(TextSerialiser& _ser)
	{
	}


	const char * const FloorChecker::GetBehaviourName() const
	{
		return FloorChecker::BehaviourName;
	}

	void FloorChecker::EditorUI(void) noexcept
	{
		
		
	}
	
	TypeErasure::TypeEraseMetaData FloorChecker::GetMetaData()
	{
		static MetaData<Dystopia::FloorChecker> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const FloorChecker::GetMetaData() const
	{
		static MetaData<Dystopia::FloorChecker> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
}




