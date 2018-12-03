/* HEADER *********************************************************************************/
/*!
\file	LoadNext.cpp
\author ShannonTan (100%)
\par    email: t.shannon\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "LoadNext.h"
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
	namespace LoadNext_MSG
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
	LoadNext::LoadNext()
	{
	}

	LoadNext::~LoadNext()
	{
	}

	void LoadNext::Load()
	{
	}

	void LoadNext::Awake()
	{
	}

	void LoadNext::Init()
	{
		Component::SetActive(true);
	}

	void LoadNext::Update(const float _fDeltaTime)
	{
	}

	void LoadNext::FixedUpdate(const float _fDeltaTime)
	{
	}

	void LoadNext::PostUpdate(void)
	{
	}

	void LoadNext::GameObjectDestroy(void)
	{
	}

	void LoadNext::Unload(void)
	{
	}

	void Dystopia::LoadNext::OnCollisionEnter(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::LoadNext::OnCollisionStay(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::LoadNext::OnCollisionExit(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::LoadNext::OnTriggerEnter(GameObject * const _obj)
	{
		if (_obj->GetName() == HashString{"Player"})
		{
			EngineCore::Get<SceneSystem>()->LoadScene("Town.dscene");
			DEBUG_PRINT(eLog::MESSAGE, "LOAD");
		}
	}

	void Dystopia::LoadNext::OnTriggerStay(GameObject * const _obj)
	{
	}

	void Dystopia::LoadNext::OnTriggerExit(GameObject * const _obj)
	{
	}

	LoadNext * LoadNext::Duplicate() const
	{
		return new LoadNext{ *this };
	}

	void LoadNext::Serialise(TextSerialiser& _ser) const
	{
	}

	void LoadNext::Unserialise(TextSerialiser& _ser)
	{
	}


	const char * const LoadNext::GetBehaviourName() const
	{
		return LoadNext::BehaviourName;
	}

	void LoadNext::EditorUI(void) noexcept
	{


	}

	TypeErasure::TypeEraseMetaData LoadNext::GetMetaData()
	{
		static MetaData<Dystopia::LoadNext> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{ mMetaData };
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const LoadNext::GetMetaData() const
	{
		static MetaData<Dystopia::LoadNext> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{ mMetaData };
		return mReturn;
	}
}




