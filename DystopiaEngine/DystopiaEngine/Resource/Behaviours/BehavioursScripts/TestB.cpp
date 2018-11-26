/* HEADER *********************************************************************************/
/*!
\file	TestB.cpp
\author TestB (100%)
\par    email: Tesbt\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "TestB.h"
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
	namespace TestB_MSG
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
	TestB::TestB()
	{
	}

	TestB::~TestB()
	{
	}

	void TestB::Load()
	{
	}

	void TestB::Awake()
	{
	}

	void TestB::Init()
	{
	}

	void TestB::Update(const float _fDeltaTime)
	{
	}

	void TestB::FixedUpdate(const float _fDeltaTime)
	{
	}
	
	void TestB::PostUpdate(void)
	{
	}

	void TestB::GameObjectDestroy(void)
	{
	}

	void TestB::Unload(void)
	{
	}

	void Dystopia::TestB::OnCollisionEnter(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::TestB::OnCollisionStay(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::TestB::OnCollisionExit(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::TestB::OnTriggerEnter(const GameObject * _obj)
	{
	}

	void Dystopia::TestB::OnTriggerStay(const GameObject * _obj)
	{
	}

	void Dystopia::TestB::OnTriggerExit(const GameObject * _obj)
	{
	}

	TestB * TestB::Duplicate() const
	{
		return new TestB{};
	}

	void TestB::Serialise(TextSerialiser& _ser) const
	{
	}

	void TestB::Unserialise(TextSerialiser& _ser)
	{
	}


	const char * const TestB::GetBehaviourName() const
	{
		return TestB::BehaviourName;
	}

	void TestB::EditorUI(void) noexcept
	{
		
		
	}
	
	TypeErasure::TypeEraseMetaData TestB::GetMetaData()
	{
		static MetaData<Dystopia::TestB> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const TestB::GetMetaData() const
	{
		static MetaData<Dystopia::TestB> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
}




