/* HEADER *********************************************************************************/
/*!
\file	keith.cpp
\author TestB (100%)
\par    email: TestB\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "keith.h"
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
	namespace keith_MSG
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
	keith::keith()
	{
	}

	keith::~keith()
	{
	}

	void keith::Load()
	{
	}

	void keith::Awake()
	{
	}

	void keith::Init()
	{
	}

	void keith::Update(const float _fDeltaTime)
	{
	}

	void keith::FixedUpdate(const float _fDeltaTime)
	{
	}
	
	void keith::PostUpdate(void)
	{
	}

	void keith::GameObjectDestroy(void)
	{
	}

	void keith::Unload(void)
	{
	}

	void Dystopia::keith::OnCollisionEnter(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::keith::OnCollisionStay(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::keith::OnCollisionExit(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::keith::OnTriggerEnter(const GameObject * _obj)
	{
	}

	void Dystopia::keith::OnTriggerStay(const GameObject * _obj)
	{
	}

	void Dystopia::keith::OnTriggerExit(const GameObject * _obj)
	{
	}

	keith * keith::Duplicate() const
	{
		return new keith{};
	}

	void keith::Serialise(TextSerialiser& _ser) const
	{
	}

	void keith::Unserialise(TextSerialiser& _ser)
	{
	}


	const char * const keith::GetBehaviourName() const
	{
		return keith::BehaviourName;
	}

	void keith::EditorUI(void) noexcept
	{
		
		
	}
	
	TypeErasure::TypeEraseMetaData keith::GetMetaData()
	{
		static MetaData<Dystopia::keith> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const keith::GetMetaData() const
	{
		static MetaData<Dystopia::keith> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
}




