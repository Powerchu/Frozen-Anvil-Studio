/* HEADER *********************************************************************************/
/*!
\file	AiDirector.cpp
\author Aaron (100%)
\par    email: m.chu\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "AiDirector.h"
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
	namespace AiDirector_MSG
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
	AiDirector::AiDirector()
	{
	}

	AiDirector::~AiDirector()
	{
	}

	void AiDirector::Load()
	{
	}

	void AiDirector::Awake()
	{
	}

	void AiDirector::Init()
	{
	}

	void AiDirector::Update(const float _fDeltaTime)
	{
	}

	void AiDirector::FixedUpdate(const float _fDeltaTime)
	{
	}
	
	void AiDirector::PostUpdate(void)
	{
	}

	void AiDirector::GameObjectDestroy(void)
	{
	}

	void AiDirector::Unload(void)
	{
	}

	void Dystopia::AiDirector::OnCollisionEnter(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::AiDirector::OnCollisionStay(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::AiDirector::OnCollisionExit(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::AiDirector::OnTriggerEnter(GameObject * const _obj)
	{
	}

	void Dystopia::AiDirector::OnTriggerStay(GameObject * const _obj)
	{
	}

	void Dystopia::AiDirector::OnTriggerExit(GameObject * const _obj)
	{
	}

	AiDirector * AiDirector::Duplicate() const
	{
		return new AiDirector{*this};
	}

	void AiDirector::Serialise(TextSerialiser& _ser) const
	{
	}

	void AiDirector::Unserialise(TextSerialiser& _ser)
	{
	}


	const char * const AiDirector::GetBehaviourName() const
	{
		return AiDirector::BehaviourName;
	}

	void AiDirector::EditorUI(void) noexcept
	{
		
		
	}
	
	TypeErasure::TypeEraseMetaData AiDirector::GetMetaData()
	{
		static MetaData<Dystopia::AiDirector> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const AiDirector::GetMetaData() const
	{
		static MetaData<Dystopia::AiDirector> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
}




