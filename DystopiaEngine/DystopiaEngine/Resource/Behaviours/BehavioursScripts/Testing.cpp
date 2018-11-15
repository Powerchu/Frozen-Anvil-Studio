/* HEADER *********************************************************************************/
/*!
\file	Testing.cpp
\author a (100%)
\par    email: a\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "Testing.h"
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
	namespace Testing_MSG
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
		void SendAllMessage(const char * _FuncName, Ts ... _Params)
		{
			EngineCore::GetInstance()->Get<BehaviourSystem>()->SendAllMessage(_FuncName, _Params...);
		}
	}
	Testing::Testing()
	{
	}

	Testing::~Testing()
	{
	}

	void Testing::Load()
	{
	}

	void Testing::Awake()
	{
	}

	void Testing::Init()
	{
	}

	void Testing::Update(const float _fDeltaTime)
	{
	}

	void Testing::FixedUpdate(const float _fDeltaTime)
	{
	}
	
	void Testing::PostUpdate(void)
	{
	}

	void Testing::GameObjectDestroy(void)
	{
	}

	void Testing::Unload(void)
	{
	}

	void Dystopia::Testing::OnCollisionEnter(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::Testing::OnCollisionStay(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::Testing::OnCollisionExit(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::Testing::OnTriggerEnter(const GameObject * _obj)
	{
	}

	void Dystopia::Testing::OnTriggerStay(const GameObject * _obj)
	{
	}

	void Dystopia::Testing::OnTriggerExit(const GameObject * _obj)
	{
	}

	Testing * Testing::Duplicate() const
	{
		return new Testing{};
	}

	void Testing::Serialise(TextSerialiser& _ser) const
	{
	}

	void Testing::Unserialise(TextSerialiser& _ser)
	{
	}


	const char * const Testing::GetBehaviourName() const
	{
		return Testing::BehaviourName;
	}

	void Testing::EditorUI(void) noexcept
	{
		
		
	}
	
	TypeErasure::TypeEraseMetaData Testing::GetMetaData()
	{
		static MetaData<Dystopia::Testing> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const Testing::GetMetaData() const
	{
		static MetaData<Dystopia::Testing> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
}




