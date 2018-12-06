/* HEADER *********************************************************************************/
/*!
\file	TransitLevel.cpp
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "TransitLevel.h"
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
	namespace TransitLevel_MSG
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
	TransitLevel::TransitLevel()
		: start{false}, curDur{1.f}, duration{1.f}
	{
	}

	TransitLevel::~TransitLevel()
	{
	}

	void TransitLevel::Load()
	{
	}

	void TransitLevel::Awake()
	{
		start = false;
		curDur = duration;
		if (auto r = GetOwner()->GetComponent<SpriteRenderer>())
		{
			r->SetActive(false);
			r->SetAlpha(0.f);
		}
	}

	void TransitLevel::Init()
	{
		start = false;
		curDur = duration;
		
		if (auto r = GetOwner()->GetComponent<SpriteRenderer>())
		{
			r->SetActive(false);
			r->SetAlpha(0.f);
		}
	}

	void TransitLevel::Update(const float _fDeltaTime)
	{
		if (start)
		{
			curDur -= _fDeltaTime;
			
			float ratio = curDur / duration;
			if (auto r = GetOwner()->GetComponent<SpriteRenderer>())
			{
				r->SetAlpha(1.f - ratio);
			}
			if (curDur < -0.1f)
			{
				start = false;
				EngineCore::GetInstance()->GetSystem<SceneSystem>()->LoadScene("Forest.dscene");
			}
		}
	}

	void TransitLevel::FixedUpdate(const float )
	{
	}
	
	void TransitLevel::PostUpdate(void)
	{
	}

	void TransitLevel::GameObjectDestroy(void)
	{
	}

	void TransitLevel::Unload(void)
	{
	}

	void Dystopia::TransitLevel::OnCollisionEnter(const CollisionEvent& )
	{

	}

	void Dystopia::TransitLevel::OnCollisionStay(const CollisionEvent& )
	{

	}

	void Dystopia::TransitLevel::OnCollisionExit(const CollisionEvent& )
	{

	}

	void Dystopia::TransitLevel::OnTriggerEnter(GameObject * const )
	{
	}

	void Dystopia::TransitLevel::OnTriggerStay(GameObject * const )
	{
	}

	void Dystopia::TransitLevel::OnTriggerExit(GameObject * const )
	{
	}

	TransitLevel * TransitLevel::Duplicate() const
	{
		return new TransitLevel{*this};
	}

	void TransitLevel::Serialise(TextSerialiser& ) const
	{
	}

	void TransitLevel::Unserialise(TextSerialiser& )
	{
	}


	const char * const TransitLevel::GetBehaviourName() const
	{
		return TransitLevel::BehaviourName;
	}

	void TransitLevel::EditorUI(void) noexcept
	{
		
		
	}
	
	void TransitLevel::TransitToLevel()
	{
		start = true;
		if (auto r = GetOwner()->GetComponent<SpriteRenderer>())
			r->SetActive(true);
	}
	
	TypeErasure::TypeEraseMetaData TransitLevel::GetMetaData()
	{
		static MetaData<Dystopia::TransitLevel> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const TransitLevel::GetMetaData() const
	{
		static MetaData<Dystopia::TransitLevel> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
}




