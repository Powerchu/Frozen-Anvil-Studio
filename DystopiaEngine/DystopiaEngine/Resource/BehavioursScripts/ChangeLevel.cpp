/* HEADER *********************************************************************************/
/*!
\file	ChangeLevel.cpp
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "ChangeLevel.h"
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

//custom includes
#include "Component/SpriteRenderer.h"

namespace Dystopia
{
	namespace ChangeLevel_MSG
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
	ChangeLevel::ChangeLevel()
		: mnNextLevelID{0},
		mbLoad{false}
	{
	}

	ChangeLevel::~ChangeLevel()
	{
	}

	void ChangeLevel::Load()
	{
	}

	void ChangeLevel::Awake()
	{
	}

	void ChangeLevel::Init()
	{
		mpUICam = EngineCore::Get<SceneSystem>()->FindGameObject("UI Camera");
		mbLoad = false;
	}

	void ChangeLevel::Update(const float _fDeltaTime)
	{
		if (mpFader)
		{
			if (auto sr = mpFader->GetComponent<SpriteRenderer>())
				mbLoad = sr->GetTint().w >= 0.99f;
		}

		if (mbLoad)
		{
			switch (mnNextLevelID)
			{
				case 0:
					EngineCore::Get<SceneSystem>()->LoadScene("MainMenuFinal.dscene");
				break;
				case 1:
					EngineCore::Get<SceneSystem>()->LoadScene("IntroCutScene.dscene");
				break;
				case 2:
					EngineCore::Get<SceneSystem>()->LoadScene("ForestFinal.dscene");
				break;
				case 3:
					EngineCore::Get<SceneSystem>()->LoadScene("Town.dscene");
				break;
			}
		}
	}

	void ChangeLevel::FixedUpdate(const float _fDeltaTime)
	{
	}
	
	void ChangeLevel::PostUpdate(void)
	{
	}

	void ChangeLevel::GameObjectDestroy(void)
	{
	}

	void ChangeLevel::Unload(void)
	{
	}

	void Dystopia::ChangeLevel::OnCollisionEnter(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::ChangeLevel::OnCollisionStay(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::ChangeLevel::OnCollisionExit(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::ChangeLevel::OnTriggerEnter(GameObject * const _obj)
	{
		if (_obj->HasTag("Player") && _obj->GetName().id() == StringHasher("CharacterController"))
		{
			auto& uiChild = mpUICam->GetComponent<Transform>()->GetAllChild();
			for (auto&c : uiChild)
				if (c->GetOwner()->GetName() == HashString{"Fader"})
					mpFader = c->GetOwner();

			if (mpFader)
				ChangeLevel_MSG::SendExternalMessage(mpFader, "StartFade");
		}
	}

	void Dystopia::ChangeLevel::OnTriggerStay(GameObject * const _obj)
	{
	}

	void Dystopia::ChangeLevel::OnTriggerExit(GameObject * const _obj)
	{
	}

	ChangeLevel * ChangeLevel::Duplicate() const
	{
		return new ChangeLevel{*this};
	}

	void ChangeLevel::Serialise(TextSerialiser& _ser) const
	{
	}

	void ChangeLevel::Unserialise(TextSerialiser& _ser)
	{
	}


	const char * const ChangeLevel::GetBehaviourName() const
	{
		return ChangeLevel::BehaviourName;
	}

	void ChangeLevel::EditorUI(void) noexcept
	{
		
		
	}
	
	TypeErasure::TypeEraseMetaData ChangeLevel::GetMetaData()
	{
		static MetaData<Dystopia::ChangeLevel> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const ChangeLevel::GetMetaData() const
	{
		static MetaData<Dystopia::ChangeLevel> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
}




