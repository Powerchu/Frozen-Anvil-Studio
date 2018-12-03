/* HEADER *********************************************************************************/
/*!
\file	QuitButton.cpp
\author Keith Goh (100%)
\par    email: keith.goh\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "QuitButton.h"
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
	namespace QuitButton_MSG
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
	QuitButton::QuitButton()
	{
	}

	QuitButton::~QuitButton()
	{
	}

	void QuitButton::Load()
	{
	}

	void QuitButton::Awake()
	{
	}

	void QuitButton::Init()
	{
		Component::SetFlags(FLAG_ACTIVE);  
	}

	void QuitButton::Update(const float _fDeltaTime)
	{
		
	}

	void QuitButton::FixedUpdate(const float _fDeltaTime)
	{
	}

	void QuitButton::PostUpdate(void)
	{
	}

	void QuitButton::GameObjectDestroy(void)
	{
	}

	void QuitButton::Unload(void)
	{
	}

	void QuitButton::ButtonPress()
	{
		/*Quit the level*/
		DEBUG_PRINT(eLog::MESSAGE, "Quiting Level");
		EngineCore::Get<SceneSystem>()->LoadScene("MainMenu.dscene");
	}
	void Dystopia::QuitButton::OnCollisionEnter(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::QuitButton::OnCollisionStay(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::QuitButton::OnCollisionExit(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::QuitButton::OnTriggerEnter(GameObject * const _obj)
	{
	}

	void Dystopia::QuitButton::OnTriggerStay(GameObject * const _obj)
	{
	}

	void Dystopia::QuitButton::OnTriggerExit(GameObject * const _obj)
	{
	}

	QuitButton * QuitButton::Duplicate() const
	{
		return new QuitButton{ *this };
	}

	void QuitButton::Serialise(TextSerialiser& _ser) const
	{
	}

	void QuitButton::Unserialise(TextSerialiser& _ser)
	{
	}


	const char * const QuitButton::GetBehaviourName() const
	{
		return QuitButton::BehaviourName;
	}

	void QuitButton::EditorUI(void) noexcept
	{


	}

	TypeErasure::TypeEraseMetaData QuitButton::GetMetaData()
	{
		static MetaData<Dystopia::QuitButton> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{ mMetaData };
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const QuitButton::GetMetaData() const
	{
		static MetaData<Dystopia::QuitButton> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{ mMetaData };
		return mReturn;
	}
}




