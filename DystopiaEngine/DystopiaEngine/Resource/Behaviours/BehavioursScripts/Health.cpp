/* HEADER *********************************************************************************/
/*!
\file	Health.cpp
\author Dan Kang (100%)
\par    email: dan.kang\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "Health.h"
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
	namespace Health_MSG
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
	Health::Health()
	{
	}

	Health::~Health()
	{
	}

	void Health::Load()
	{
	}

	void Health::Awake()
	{
	}

	void Health::Init()
	{
	}

	void Health::Update(const float )
	{
	}

	void Health::FixedUpdate(const float )
	{
	}

	void Health::PostUpdate(void)
	{
	}

	void Health::GameObjectDestroy(void)
	{
	}

	void Health::Unload(void)
	{
	}

	void Dystopia::Health::OnCollisionEnter(const CollisionEvent& )
	{

	}

	void Dystopia::Health::OnCollisionStay(const CollisionEvent& )
	{

	}

	void Dystopia::Health::OnCollisionExit(const CollisionEvent& )
	{

	}

	void Dystopia::Health::OnTriggerEnter(GameObject * const )
	{
	}

	void Dystopia::Health::OnTriggerStay(GameObject * const )
	{
	}

	void Dystopia::Health::OnTriggerExit(GameObject * const )
	{
	}

	Health * Health::Duplicate() const
	{
		return new Health{ *this };
	}

	void Health::Serialise(TextSerialiser& ) const
	{
	}

	void Health::Unserialise(TextSerialiser& )
	{
	}


	const char * const Health::GetBehaviourName() const
	{
		return Health::BehaviourName;
	}

	void Health::EditorUI(void) noexcept
	{


	}

	TypeErasure::TypeEraseMetaData Health::GetMetaData()
	{
		static MetaData<Dystopia::Health> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{ mMetaData };
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const Health::GetMetaData() const
	{
		static MetaData<Dystopia::Health> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{ mMetaData };
		return mReturn;
	}
}




