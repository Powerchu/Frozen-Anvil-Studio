/* HEADER *********************************************************************************/
/*!
\file	SettingsButton.cpp
\author Keith Goh (100%)
\par    email: keith.goh\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "SettingsButton.h"
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
	namespace SettingsButton_MSG
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
	SettingsButton::SettingsButton()
	{
	}

	SettingsButton::~SettingsButton()
	{
	}

	void SettingsButton::Load()
	{
	}

	void SettingsButton::Awake()
	{
	}

	void SettingsButton::Init()
	{
	}

	void SettingsButton::Update(const float )
	{
	}

	void SettingsButton::FixedUpdate(const float )
	{
	}

	void SettingsButton::PostUpdate(void)
	{
	}

	void SettingsButton::GameObjectDestroy(void)
	{
	}

	void SettingsButton::Unload(void)
	{
	}

	void Dystopia::SettingsButton::OnCollisionEnter(const CollisionEvent& )
	{

	}

	void Dystopia::SettingsButton::OnCollisionStay(const CollisionEvent& )
	{

	}

	void Dystopia::SettingsButton::OnCollisionExit(const CollisionEvent& )
	{

	}

	void Dystopia::SettingsButton::OnTriggerEnter(GameObject * const )
	{
	}

	void Dystopia::SettingsButton::OnTriggerStay(GameObject * const )
	{
	}

	void Dystopia::SettingsButton::OnTriggerExit(GameObject * const )
	{
	}

	SettingsButton * SettingsButton::Duplicate() const
	{
		return new SettingsButton{ *this };
	}

	void SettingsButton::Serialise(TextSerialiser& ) const
	{
	}

	void SettingsButton::Unserialise(TextSerialiser& )
	{
	}
	void SettingsButton::ButtonPress()
	{ 
		/*Open Settings Menu*/
		DEBUG_PRINT(eLog::MESSAGE, "Settings Screen Open");
	}

	const char * const SettingsButton::GetBehaviourName() const
	{
		return SettingsButton::BehaviourName;
	}

	void SettingsButton::EditorUI(void) noexcept
	{


	}

	TypeErasure::TypeEraseMetaData SettingsButton::GetMetaData()
	{
		static MetaData<Dystopia::SettingsButton> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{ mMetaData };
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const SettingsButton::GetMetaData() const
	{
		static MetaData<Dystopia::SettingsButton> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{ mMetaData };
		return mReturn;
	}
}




