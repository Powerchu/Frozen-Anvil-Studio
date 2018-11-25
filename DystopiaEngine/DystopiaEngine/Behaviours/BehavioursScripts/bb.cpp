/* HEADER *********************************************************************************/
/*!
\file	bb.cpp
\author bb (100%)
\par    email: bvbvb\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "bb.h"
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
	namespace bb_MSG
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
	bb::bb()
	{
	}

	bb::~bb()
	{
	}

	void bb::Load()
	{
	}

	void bb::Awake()
	{
	}

	void bb::Init()
	{
	}

	void bb::Update(const float _fDeltaTime)
	{
	}

	void bb::FixedUpdate(const float _fDeltaTime)
	{
	}
	
	void bb::PostUpdate(void)
	{
	}

	void bb::GameObjectDestroy(void)
	{
	}

	void bb::Unload(void)
	{
	}

	void Dystopia::bb::OnCollisionEnter(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::bb::OnCollisionStay(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::bb::OnCollisionExit(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::bb::OnTriggerEnter(const GameObject * _obj)
	{
	}

	void Dystopia::bb::OnTriggerStay(const GameObject * _obj)
	{
	}

	void Dystopia::bb::OnTriggerExit(const GameObject * _obj)
	{
	}

	bb * bb::Duplicate() const
	{
		return new bb{};
	}

	void bb::Serialise(TextSerialiser& _ser) const
	{
	}

	void bb::Unserialise(TextSerialiser& _ser)
	{
	}


	const char * const bb::GetBehaviourName() const
	{
		return bb::BehaviourName;
	}

	void bb::EditorUI(void) noexcept
	{
		
		
	}
	
	TypeErasure::TypeEraseMetaData bb::GetMetaData()
	{
		static MetaData<Dystopia::bb> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const bb::GetMetaData() const
	{
		static MetaData<Dystopia::bb> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
}




