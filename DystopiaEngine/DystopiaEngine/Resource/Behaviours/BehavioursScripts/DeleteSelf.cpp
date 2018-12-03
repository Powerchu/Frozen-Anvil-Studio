/* HEADER *********************************************************************************/
/*!
\file	DeleteSelf.cpp
\author Dan Kang (100%)
\par    email: dan.kang\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "DeleteSelf.h"
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
	namespace DeleteSelf_MSG
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
	DeleteSelf::DeleteSelf()
		: destroyDelay(0.0f)
		, destroyCount(0.0f)
	{
	}

	DeleteSelf::~DeleteSelf()
	{
		destroyCount = 0.f;
	}

	void DeleteSelf::Load()
	{
	}

	void DeleteSelf::Awake()
	{
		SetFlags(FLAG_ACTIVE);
		destroyCount = 0.f;
	}

	void DeleteSelf::Init()
	{
		SetFlags(FLAG_ACTIVE);
		destroyCount = 0.f;
	}

	void DeleteSelf::Update(const float _fDeltaTime)
	{
		if (nullptr == GetOwner()) return;
		if (GetOwner()->GetFlag() & eObjFlag::FLAG_EDITOR_OBJ) return;
		
		

		destroyCount = destroyCount + _fDeltaTime;

		if (destroyCount >= destroyDelay)
		{
			/*DEBUG_PRINT(eLog::MESSAGE, "%p", this);
			DEBUG_PRINT(eLog::MESSAGE, "%u", GetOwnerID());
			DEBUG_PRINT(eLog::MESSAGE, "%f", destroyCount);
			DEBUG_PRINT(eLog::MESSAGE, "DEATHHHH");*/
			GetOwner()->Destroy();
		}
	}

	void DeleteSelf::FixedUpdate(const float )
	{
	}
	
	void DeleteSelf::PostUpdate(void)
	{
	}

	void DeleteSelf::GameObjectDestroy(void)
	{
	}

	void DeleteSelf::Unload(void)
	{
	}

	void Dystopia::DeleteSelf::OnCollisionEnter(const CollisionEvent& )
	{

	}

	void Dystopia::DeleteSelf::OnCollisionStay(const CollisionEvent& )
	{

	}

	void Dystopia::DeleteSelf::OnCollisionExit(const CollisionEvent& )
	{

	}

	void Dystopia::DeleteSelf::OnTriggerEnter(GameObject * const )
	{
	}

	void Dystopia::DeleteSelf::OnTriggerStay(GameObject * const )
	{
	}

	void Dystopia::DeleteSelf::OnTriggerExit(GameObject * const )
	{
	}

	DeleteSelf * DeleteSelf::Duplicate() const
	{
		return new DeleteSelf{*this};
	}

	void DeleteSelf::Serialise(TextSerialiser& ) const
	{
	}

	void DeleteSelf::Unserialise(TextSerialiser& )
	{
	}


	const char * const DeleteSelf::GetBehaviourName() const
	{
		return DeleteSelf::BehaviourName;
	}

	void DeleteSelf::EditorUI(void) noexcept
	{
		
		
	}
	
	TypeErasure::TypeEraseMetaData DeleteSelf::GetMetaData()
	{
		static MetaData<Dystopia::DeleteSelf> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const DeleteSelf::GetMetaData() const
	{
		static MetaData<Dystopia::DeleteSelf> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
}




