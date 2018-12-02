/* HEADER *********************************************************************************/
/*!
\file	ForceTThree.cpp
\author Dan Kang (100%)
\par    email: dan.kang\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "ForceTThree.h"
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
	namespace ForceTThree_MSG
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
	ForceTThree::ForceTThree()
	{
	}

	ForceTThree::~ForceTThree()
	{
	}

	void ForceTThree::Load()
	{
	}

	void ForceTThree::Awake()
	{
	}

	void ForceTThree::Init()
	{
	}

	void ForceTThree::Update(const float _fDeltaTime)
	{
	}

	void ForceTThree::FixedUpdate(const float _fDeltaTime)
	{
	}
	
	void ForceTThree::PostUpdate(void)
	{
	}

	void ForceTThree::Unload(void)
	{
	}

	void Dystopia::ForceTThree::OnCollisionEnter(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::ForceTThree::OnCollisionStay(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::ForceTThree::OnCollisionExit(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::ForceTThree::OnTriggerEnter(GameObject * const _obj)
	{
	}

	void Dystopia::ForceTThree::OnTriggerStay(GameObject * const _obj)
	{
	}

	void Dystopia::ForceTThree::OnTriggerExit(GameObject * const _obj)
	{
	}

	ForceTThree * ForceTThree::Duplicate() const
	{
		return new ForceTThree{*this};
	}

	void ForceTThree::Serialise(TextSerialiser& _ser) const
	{
	}

	void ForceTThree::Unserialise(TextSerialiser& _ser)
	{
	}


	const char * const ForceTThree::GetBehaviourName() const
	{
		return ForceTThree::BehaviourName;
	}

	void ForceTThree::EditorUI(void) noexcept
	{
		
		
	}
	
	TypeErasure::TypeEraseMetaData ForceTThree::GetMetaData()
	{
		static MetaData<Dystopia::ForceTThree> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const ForceTThree::GetMetaData() const
	{
		static MetaData<Dystopia::ForceTThree> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
}




