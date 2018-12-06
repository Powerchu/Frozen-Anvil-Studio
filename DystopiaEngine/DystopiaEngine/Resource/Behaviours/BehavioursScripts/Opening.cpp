/* HEADER *********************************************************************************/
/*!
\file	Opening.cpp
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "Opening.h"
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
#include "Component/SpriteRenderer.h"


namespace Dystopia
{
	namespace Opening_MSG 
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
	Opening::Opening()
		: speed{5.f}, off{false}, duration{1.f}
	{
	}

	Opening::~Opening()
	{
	}

	void Opening::Load()
	{
	}

	void Opening::Awake()
	{
		auto& children = GetOwner()->GetComponent<Transform>()->GetAllChild();
		for (auto c : children)
		{
			if (auto r = c->GetOwner()->GetComponent<SpriteRenderer>())
			{
				r->SetActive(true);
				r->SetAlpha(1.f);
			}
		}
	}

	void Opening::Init()
	{
		SetActive(true);
		auto& children = GetOwner()->GetComponent<Transform>()->GetAllChild();
		for (auto c : children)
		{
			if (auto r= c->GetOwner()->GetComponent<SpriteRenderer>())
			{
				r->SetActive(true);
				r->SetAlpha(1.f);
			}
		}
		if (auto o = EngineCore::Get<SceneSystem>()->FindGameObject("Player"))
		{
			Opening_MSG::SendExternalMessage(o, "DisableControls", true);
		}
		off = false;
	}

	void Opening::Update(const float _fDeltaTime)
	{
		if (off)
			return;
		
		if (startDelay > 0.f)
		{
			if (auto o = EngineCore::Get<SceneSystem>()->FindGameObject("Player"))
			{
				Opening_MSG::SendExternalMessage(o, "DisableControls", true);
			}
			startDelay -= _fDeltaTime;
			return;
		}
		
		duration -= _fDeltaTime;
		if (duration < 0.f)
		{
			off = true;
			if (auto o = EngineCore::Get<SceneSystem>()->FindGameObject("Player"))
				Opening_MSG::SendExternalMessage(o, "DisableControls", false);
			auto& children = GetOwner()->GetComponent<Transform>()->GetAllChild();
			for (auto c : children)
			{
				if (auto r= c->GetOwner()->GetComponent<SpriteRenderer>())
				{
					r->SetActive(false);
					r->SetAlpha(0.f);
				}
			}
		}
		else
		{
			auto t = GetOwner()->GetComponent<Transform>();
			auto s = t->GetGlobalScale();
			s.y = s.y + (_fDeltaTime * speed);
			t->SetGlobalScale(s); 
			if (auto o = EngineCore::Get<SceneSystem>()->FindGameObject("Player"))
			{
				Opening_MSG::SendExternalMessage(o, "DisableControls", true);
			}
		}
	}

	void Opening::FixedUpdate(const float )
	{
	}
	
	void Opening::PostUpdate(void)
	{
	}

	void Opening::GameObjectDestroy(void)
	{
	}

	void Opening::Unload(void)
	{
	}

	void Dystopia::Opening::OnCollisionEnter(const CollisionEvent& )
	{

	}

	void Dystopia::Opening::OnCollisionStay(const CollisionEvent& )
	{

	}

	void Dystopia::Opening::OnCollisionExit(const CollisionEvent& )
	{

	}

	void Dystopia::Opening::OnTriggerEnter(GameObject * const )
	{
	}

	void Dystopia::Opening::OnTriggerStay(GameObject * const )
	{
	}

	void Dystopia::Opening::OnTriggerExit(GameObject * const )
	{
	}

	Opening * Opening::Duplicate() const
	{
		return new Opening{*this};
	}

	void Opening::Serialise(TextSerialiser& ) const
	{
	}

	void Opening::Unserialise(TextSerialiser& )
	{
	}


	const char * const Opening::GetBehaviourName() const
	{
		return Opening::BehaviourName;
	}

	void Opening::EditorUI(void) noexcept
	{
		
		
	}
	
	TypeErasure::TypeEraseMetaData Opening::GetMetaData()
	{
		static MetaData<Dystopia::Opening> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const Opening::GetMetaData() const
	{
		static MetaData<Dystopia::Opening> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
}




