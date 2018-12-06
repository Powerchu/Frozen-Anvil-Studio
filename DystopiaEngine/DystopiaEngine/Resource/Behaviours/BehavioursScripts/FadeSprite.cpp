/* HEADER *********************************************************************************/
/*!
\file	FadeSprite.cpp
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "FadeSprite.h"
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

#include "Component/SpriteRenderer.h"

namespace Dystopia
{
	namespace FadeSprite_MSG
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
	FadeSprite::FadeSprite()
		:duration{0},
		target{1}, fade{false}, startV{1}
	{
	}

	FadeSprite::~FadeSprite()
	{ 
	}

	void FadeSprite::Load()
	{
	}

	void FadeSprite::Awake()
	{
	}

	void FadeSprite::Init()
	{
		if (target < 0.f)
			target = 0.f;
		if (target > 1.f)
			target = 1.f;
		if (auto r = GetOwner()->GetComponent<SpriteRenderer>())
			startV = r->GetTint().w; 
	}

	void FadeSprite::Update(const float _fDeltaTime)
	{
		if (fade && duration > 0.f)
		{
			duration -= _fDeltaTime;
			if (auto r = GetOwner()->GetComponent<SpriteRenderer>())
			{
				r->SetActive(true);
				r->SetAlpha(target + ((startV - target) * duration));
			}
		}
	}

	void FadeSprite::FixedUpdate(const float )
	{
	}
	
	void FadeSprite::PostUpdate(void)
	{
	}

	void FadeSprite::GameObjectDestroy(void)
	{
	}

	void FadeSprite::Unload(void)
	{
	}

	void Dystopia::FadeSprite::OnCollisionEnter(const CollisionEvent& )
	{

	}

	void Dystopia::FadeSprite::OnCollisionStay(const CollisionEvent& )
	{

	}

	void Dystopia::FadeSprite::OnCollisionExit(const CollisionEvent& )
	{

	}

	void Dystopia::FadeSprite::OnTriggerEnter(GameObject * const )
	{
	}

	void Dystopia::FadeSprite::OnTriggerStay(GameObject * const )
	{
	}

	void Dystopia::FadeSprite::OnTriggerExit(GameObject * const )
	{
	}

	FadeSprite * FadeSprite::Duplicate() const
	{
		return new FadeSprite{*this};
	}

	void FadeSprite::Serialise(TextSerialiser& ) const
	{
	}

	void FadeSprite::Unserialise(TextSerialiser& )
	{
	}


	const char * const FadeSprite::GetBehaviourName() const
	{
		return FadeSprite::BehaviourName;
	}

	void FadeSprite::EditorUI(void) noexcept
	{
		
		
	}
	
	void FadeSprite::StartFade()
	{
		fade = true;
	}
	
	TypeErasure::TypeEraseMetaData FadeSprite::GetMetaData()
	{
		static MetaData<Dystopia::FadeSprite> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const FadeSprite::GetMetaData() const
	{
		static MetaData<Dystopia::FadeSprite> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
}




