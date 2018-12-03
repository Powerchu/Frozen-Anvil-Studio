/* HEADER *********************************************************************************/
/*!
\file	ShiftChildrenZ.cpp
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "ShiftChildrenZ.h"
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

#include "Component/Transform.h"

namespace Dystopia
{
	namespace ShiftChildrenZ_MSG
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
	ShiftChildrenZ::ShiftChildrenZ()
		: EpsilonZ{0.001f}
	{
	}

	ShiftChildrenZ::~ShiftChildrenZ()
	{
	}

	void ShiftChildrenZ::Load()
	{
	}

	void ShiftChildrenZ::Awake()
	{
	}

	void ShiftChildrenZ::Init()
	{
		EpsilonZ = 0.001f;
	}

	void ShiftChildrenZ::Update(const float )
	{
		if (EngineCore::Get<InputManager>()->IsKeyTriggered(eButton::XBUTTON_LEFT_SHOULDER))
		{
			EpsilonZ = 0.001f;
			auto& children = GetOwner()->GetComponent<Transform>()->GetAllChild();
			for (auto& c : children)
			{
				auto pos = c->GetGlobalPosition();
				pos.z = pos.z + EpsilonZ;
				EpsilonZ += 0.001f;
				c->SetGlobalPosition(pos);
			}
		}
	}

	void ShiftChildrenZ::FixedUpdate(const float )
	{
	}

	void ShiftChildrenZ::PostUpdate(void)
	{
	}

	void ShiftChildrenZ::GameObjectDestroy(void)
	{
	}

	void ShiftChildrenZ::Unload(void)
	{
	}

	void Dystopia::ShiftChildrenZ::OnCollisionEnter(const CollisionEvent& )
	{

	}

	void Dystopia::ShiftChildrenZ::OnCollisionStay(const CollisionEvent& )
	{

	}

	void Dystopia::ShiftChildrenZ::OnCollisionExit(const CollisionEvent& )
	{

	}

	void Dystopia::ShiftChildrenZ::OnTriggerEnter(GameObject * const )
	{
	}

	void Dystopia::ShiftChildrenZ::OnTriggerStay(GameObject * const )
	{
	}

	void Dystopia::ShiftChildrenZ::OnTriggerExit(GameObject * const )
	{
	}

	ShiftChildrenZ * ShiftChildrenZ::Duplicate() const
	{
		return new ShiftChildrenZ{ *this };
	}

	void ShiftChildrenZ::Serialise(TextSerialiser& ) const
	{
	}

	void ShiftChildrenZ::Unserialise(TextSerialiser& )
	{
	}


	const char * const ShiftChildrenZ::GetBehaviourName() const
	{
		return ShiftChildrenZ::BehaviourName;
	}

	void ShiftChildrenZ::EditorUI(void) noexcept
	{


	}

	TypeErasure::TypeEraseMetaData ShiftChildrenZ::GetMetaData()
	{
		static MetaData<Dystopia::ShiftChildrenZ> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{ mMetaData };
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const ShiftChildrenZ::GetMetaData() const
	{
		static MetaData<Dystopia::ShiftChildrenZ> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{ mMetaData };
		return mReturn;
	}
}




