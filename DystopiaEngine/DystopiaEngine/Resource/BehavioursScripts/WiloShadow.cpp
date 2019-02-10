/* HEADER *********************************************************************************/
/*!
\file	WiloShadow.cpp
\author Aaron (100%)
\par    email: m.chu\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "WiloShadow.h"
#include "System/Input/InputSystem.h"
#include "System/Input/InputMap.h"
#include "System/Driver/Driver.h"
#include "System/Behaviour/BehaviourSystem.h"
#include "Component/SpriteRenderer.h"

#include "System/Scene/SceneSystem.h"
#include "System/Collision/CollisionEvent.h"
#include "System/Collision/CollisionSystem.h"

#include "Object/ObjectFlags.h"
#include "Object/GameObject.h"

#include "Editor/EGUI.h"
#include "Utility/DebugAssert.h"



namespace Dystopia
{
	namespace WiloShadow_MSG
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
	WiloShadow::WiloShadow()
	{
	}

	WiloShadow::~WiloShadow()
	{
	}

	void WiloShadow::Load()
	{
	}

	void WiloShadow::Awake()
	{
		heroObj = EngineCore::Get<SceneSystem>()->FindGameObject("CharacterController");
		originalScale = GetOwner()->GetComponent<Transform>()->GetGlobalScale();
	}
 
	void WiloShadow::Init()
	{
	}

	void WiloShadow::Update(const float )
	{
		auto myTransform = GetOwner()->GetComponent<Transform>();
		auto myPosition = myTransform->GetGlobalPosition();
		auto myAlpha = GetOwner()->GetComponent<SpriteRenderer>()->GetTint().w;
		
		float heightToPlayer = 0.f;

		if (nullptr != heroObj)
			heightToPlayer = Math::Abs(heroObj->GetComponent<Transform>()->GetGlobalPosition().y - myPosition.y);


		if (nullptr != heroObj)
		{
			auto heightPerc = 145.0f / heightToPlayer;

			myTransform->SetGlobalPosition(heroObj->GetComponent<Transform>()->GetGlobalPosition().x,myPosition.y,myPosition.z);
			myTransform->SetGlobalScale(heightPerc * originalScale.x, originalScale.y, originalScale.z);
			GetOwner()->GetComponent<SpriteRenderer>()->SetAlphaPerc(myAlpha * heightPerc);
		}
	}

	void WiloShadow::FixedUpdate(const float )
	{
	}
	
	void WiloShadow::PostUpdate(void)
	{
	}

	void WiloShadow::GameObjectDestroy(void)
	{
	}

	void WiloShadow::Unload(void)
	{
	}

	void Dystopia::WiloShadow::OnCollisionEnter(const CollisionEvent& )
	{

	}

	void Dystopia::WiloShadow::OnCollisionStay(const CollisionEvent& )
	{

	}

	void Dystopia::WiloShadow::OnCollisionExit(const CollisionEvent& )
	{

	}

	void Dystopia::WiloShadow::OnTriggerEnter(GameObject * const )
	{
	}

	void Dystopia::WiloShadow::OnTriggerStay(GameObject * const )
	{
	}

	void Dystopia::WiloShadow::OnTriggerExit(GameObject * const )
	{
	}

	WiloShadow * WiloShadow::Duplicate() const
	{
		return new WiloShadow{*this};
	}

	void WiloShadow::Serialise(TextSerialiser& ) const
	{
	}

	void WiloShadow::Unserialise(TextSerialiser& )
	{
	}


	const char * const WiloShadow::GetBehaviourName() const
	{
		return WiloShadow::BehaviourName;
	}

	void WiloShadow::EditorUI(void) noexcept
	{
		
		
	}
	
	TypeErasure::TypeEraseMetaData WiloShadow::GetMetaData()
	{
		static MetaData<Dystopia::WiloShadow> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const WiloShadow::GetMetaData() const
	{
		static MetaData<Dystopia::WiloShadow> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
}




