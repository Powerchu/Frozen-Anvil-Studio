/* HEADER *********************************************************************************/
/*!
\file	ngng.cpp
\author ngng (100%)
\par    email: ngngngng\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "ngng.h"
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
	namespace ngng_MSG
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
	ngng::ngng()
	{
	}

	ngng::~ngng()
	{
	}

	void ngng::Load()
	{
	}

	void ngng::Awake()
	{
	}

	void ngng::Init()
	{
	}

	void ngng::Update(const float _fDeltaTime)
	{
	}

	void ngng::FixedUpdate(const float _fDeltaTime)
	{
	}
	
	void ngng::PostUpdate(void)
	{
	}

	void ngng::GameObjectDestroy(void)
	{
	}

	void ngng::Unload(void)
	{
	}

	void Dystopia::ngng::OnCollisionEnter(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::ngng::OnCollisionStay(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::ngng::OnCollisionExit(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::ngng::OnTriggerEnter(const GameObject * _obj)
	{
	}

	void Dystopia::ngng::OnTriggerStay(const GameObject * _obj)
	{
	}

	void Dystopia::ngng::OnTriggerExit(const GameObject * _obj)
	{
	}

	ngng * ngng::Duplicate() const
	{
		return new ngng{};
	}

	void ngng::Serialise(TextSerialiser& _ser) const
	{
	}

	void ngng::Unserialise(TextSerialiser& _ser)
	{
	}


	const char * const ngng::GetBehaviourName() const
	{
		return ngng::BehaviourName;
	}

	void ngng::EditorUI(void) noexcept
	{
		
		
	}
	
	TypeErasure::TypeEraseMetaData ngng::GetMetaData()
	{
		static MetaData<Dystopia::ngng> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const ngng::GetMetaData() const
	{
		static MetaData<Dystopia::ngng> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
}




