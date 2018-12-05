/* HEADER *********************************************************************************/
/*!
\file	RestartButton.cpp
\author Keith Goh (100%)
\par    email: keith.goh\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "RestartButton.h"
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
	namespace RestartButton_MSG
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
	RestartButton::RestartButton()
	{
	}

	RestartButton::~RestartButton()
	{
	}

	void RestartButton::Load()
	{
	}

	void RestartButton::Awake()
	{
	}

	void RestartButton::Init()
	{
	}

	void RestartButton::Update(const float )
	{
	}

	void RestartButton::FixedUpdate(const float )
	{
	}

	void RestartButton::PostUpdate(void)
	{
	}

	void RestartButton::GameObjectDestroy(void)
	{
	}

	void RestartButton::Unload(void)
	{
	}

	void Dystopia::RestartButton::OnCollisionEnter(const CollisionEvent& )
	{

	}

	void Dystopia::RestartButton::OnCollisionStay(const CollisionEvent& )
	{

	}

	void Dystopia::RestartButton::OnCollisionExit(const CollisionEvent& )
	{

	}

	void Dystopia::RestartButton::OnTriggerEnter(GameObject * const )
	{
	}

	void Dystopia::RestartButton::OnTriggerStay(GameObject * const )
	{
	}

	void Dystopia::RestartButton::OnTriggerExit(GameObject * const )
	{
	}

	RestartButton * RestartButton::Duplicate() const
	{
		return new RestartButton{ *this };
	}

	void RestartButton::Serialise(TextSerialiser& ) const
	{
	}

	void RestartButton::Unserialise(TextSerialiser& )
	{
	}

	void RestartButton::ButtonPress()
	{
		DEBUG_PRINT(eLog::MESSAGE, "Restarting Scene");
		EngineCore::Get<SceneSystem>()->RestartScene();
	}

	const char * const RestartButton::GetBehaviourName() const
	{
		return RestartButton::BehaviourName;
	}

	void RestartButton::EditorUI(void) noexcept
	{


	}

	TypeErasure::TypeEraseMetaData RestartButton::GetMetaData()
	{
		static MetaData<Dystopia::RestartButton> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{ mMetaData };
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const RestartButton::GetMetaData() const
	{
		static MetaData<Dystopia::RestartButton> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{ mMetaData };
		return mReturn;
	}
}




