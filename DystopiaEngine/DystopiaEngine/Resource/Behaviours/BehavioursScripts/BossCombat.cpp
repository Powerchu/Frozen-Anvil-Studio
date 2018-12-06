/* HEADER *********************************************************************************/
/*!
\file	BossCombat.cpp
\author Dan (100%)
\par    email: dan\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "BossCombat.h"
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
	namespace BossCombat_MSG
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
	BossCombat::BossCombat()
	{
	}

	BossCombat::~BossCombat()
	{
	}

	void BossCombat::Load()
	{
	}

	void BossCombat::Awake()
	{
	}

	void BossCombat::Init()
	{
	}

	void BossCombat::Update(const float )
	{ 
		if (mfHealth <= 1.f)
		{
			GetOwner()->Destroy();
			if ( const auto Boss = EngineCore::Get<SceneSystem>()->FindGameObject_cstr("Boss"))
			{
				Boss->Destroy();
				EngineCore::Get<SceneSystem>()->LoadScene("MainMenu.dscene");
			}
		}
	}

	void BossCombat::FixedUpdate(const float )
	{
	}
	
	void BossCombat::PostUpdate(void)
	{
	}

	void BossCombat::GameObjectDestroy(void)
	{
	}

	void BossCombat::Unload(void)
	{
	}

	void Dystopia::BossCombat::TakeDamage(int _dmg)
	{
		mfHealth -= static_cast<float>(_dmg);
		
	}


	void Dystopia::BossCombat::OnCollisionEnter(const CollisionEvent& )
	{

	}

	void Dystopia::BossCombat::OnCollisionStay(const CollisionEvent& )
	{

	}

	void Dystopia::BossCombat::OnCollisionExit(const CollisionEvent& )
	{

	}

	void Dystopia::BossCombat::OnTriggerEnter(GameObject * const )
	{
	}

	void Dystopia::BossCombat::OnTriggerStay(GameObject * const )
	{
	}

	void Dystopia::BossCombat::OnTriggerExit(GameObject * const )
	{
	}

	BossCombat * BossCombat::Duplicate() const
	{
		return new BossCombat{*this};
	}

	void BossCombat::Serialise(TextSerialiser& ) const
	{
	}

	void BossCombat::Unserialise(TextSerialiser& )
	{
	}


	const char * const BossCombat::GetBehaviourName() const
	{
		return BossCombat::BehaviourName;
	}

	void BossCombat::EditorUI(void) noexcept
	{
		
		
	}
	
	TypeErasure::TypeEraseMetaData BossCombat::GetMetaData()
	{
		static MetaData<Dystopia::BossCombat> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const BossCombat::GetMetaData() const
	{
		static MetaData<Dystopia::BossCombat> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
}




