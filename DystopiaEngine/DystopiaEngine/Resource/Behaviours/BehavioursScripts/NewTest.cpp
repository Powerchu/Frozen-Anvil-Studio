/* HEADER *********************************************************************************/
/*!
\file	NewTest.cpp
\author d (100%)
\par    email: d\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "NewTest.h"
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
	namespace NewTest_MSG
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
	
	using namespace NewTest_MSG;
	
	NewTest::NewTest()
	{
	}

	NewTest::~NewTest()
	{
	}

	void NewTest::Load()
	{
	}

	void NewTest::Awake()
	{
	}

	void NewTest::Init()
	{
	}

	void NewTest::Update(const float _fDeltaTime)
	{
	}

	void NewTest::FixedUpdate(const float _fDeltaTime)
	{
	}
	
	void NewTest::PostUpdate(void)
	{
	}

	void NewTest::GameObjectDestroy(void)
	{
	}

	void NewTest::Unload(void)
	{
	}

	void Dystopia::NewTest::OnCollisionEnter(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::NewTest::OnCollisionStay(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::NewTest::OnCollisionExit(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::NewTest::OnTriggerEnter(GameObject * const _obj)
	{
	}

	void Dystopia::NewTest::OnTriggerStay(GameObject * const _obj)
	{
	}

	void Dystopia::NewTest::OnTriggerExit(GameObject * const _obj)
	{
	}

	NewTest * NewTest::Duplicate() const
	{
		return new NewTest{};
	}

	void NewTest::Serialise(TextSerialiser& _ser) const
	{
	}

	void NewTest::Unserialise(TextSerialiser& _ser)
	{
	}


	const char * const NewTest::GetBehaviourName() const
	{
		return NewTest::BehaviourName;
	}

	void NewTest::EditorUI(void) noexcept
	{
		
		
	}
	
	TypeErasure::TypeEraseMetaData NewTest::GetMetaData()
	{
		static MetaData<Dystopia::NewTest> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const NewTest::GetMetaData() const
	{
		static MetaData<Dystopia::NewTest> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
}




