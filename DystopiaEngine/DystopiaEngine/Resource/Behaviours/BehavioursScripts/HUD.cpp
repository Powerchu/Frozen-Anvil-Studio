/* HEADER *********************************************************************************/
/*!
\file	HUD.cpp
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "HUD.h"
#include "System/Input/InputSystem.h"
#include "System/Input/InputMap.h"
#include "System/Driver/Driver.h"
#include "System/Behaviour/BehaviourSystem.h"

#include "System/Scene/SceneSystem.h"
#include "System/Collision/CollisionEvent.h"
#include "System/Collision/CollisionSystem.h"
#include "System/Graphics/GraphicsSystem.h"
#include "System/Graphics/Texture.h"
#include "System/File/FileSystem.h" 

#include "Component/SpriteRenderer.h"
#include "Component/Transform.h"

#include "Object/ObjectFlags.h"
#include "Object/GameObject.h"

#include "Editor/EGUI.h"
#include "Utility/DebugAssert.h"

#include "DataStructure/HashString.h"


namespace Dystopia
{
	namespace HUD_MSG
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
	HUD::HUD()
		: FFMode{1}, 
		FadeMode{0},
		HUDAlpha{1},
		FadeSpeed{0}
	{
	}

	HUD::~HUD()
	{
	}

	void HUD::Load()
	{
	}

	void HUD::Awake()
	{
	}

	void HUD::Init()
	{
		ChangeFF(FFMode);
		SetActive(true);
		HUDAlpha = 0.f;
		auto& children = GetOwner()->GetComponent<Transform>()->GetAllChild();
		for (auto& c : children)
			if (auto s = c->GetOwner()->GetComponent<SpriteRenderer>())
				s->SetAlpha(HUDAlpha);
	}

	void HUD::Update(const float _fDeltaTime)
	{
		if (EngineCore::Get<InputManager>()->IsKeyTriggered(eButton::XBUTTON_LEFT_SHOULDER))
		{
			FFMode = 1;
			ChangeFF(FFMode);
		}
		else if (EngineCore::Get<InputManager>()->IsKeyTriggered(eButton::XBUTTON_RIGHT_SHOULDER))
		{
			FFMode = 2;
			ChangeFF(FFMode);
		}
		//else if (EngineCore::Get<InputManager>()->IsKeyTriggered(eButton::XBUTTON_A))
		//{
		//	auto& children = GetOwner()->GetComponent<Transform>()->GetAllChild();
		//	for (auto& c : children)
		//	{
		//		if (c->GetOwner()->GetName() == HashString{"Health"})
		//		{
		//			DEBUG_PRINT(eLog::MESSAGE, "Health! 0");
		//			HUD_MSG::SendExternalMessage(c->GetOwner(), "HPLerpTo", 0.0f);
		//			return;
		//		}
		//	}
		//}
		//else if (EngineCore::Get<InputManager>()->IsKeyTriggered(eButton::XBUTTON_Y))
		//{
		//	auto& children = GetOwner()->GetComponent<Transform>()->GetAllChild();
		//	for (auto& c : children)
		//	{
		//		if (c->GetOwner()->GetName() == HashString{"FFBar"})
		//		{
		//			DEBUG_PRINT(eLog::MESSAGE, "Bar! 1");
		//			HUD_MSG::SendExternalMessage(c->GetOwner(), "EnergyLerpTo", 1.f);
		//			return;
		//		}
		//	}
		//}
		//else if (EngineCore::Get<InputManager>()->IsKeyTriggered(eButton::XBUTTON_B))
		//{
		//	auto& children = GetOwner()->GetComponent<Transform>()->GetAllChild();
		//	for (auto& c : children)
		//	{
		//		if (c->GetOwner()->GetName() == HashString{"Health"})
		//		{
		//			DEBUG_PRINT(eLog::MESSAGE, "Health! 1");
		//			HUD_MSG::SendExternalMessage(c->GetOwner(), "HPLerpTo", 1.f);
		//			return;
		//		}
		//	}
		//}
		//else if (EngineCore::Get<InputManager>()->IsKeyTriggered(eButton::XBUTTON_X))
		//{
		//	auto& children = GetOwner()->GetComponent<Transform>()->GetAllChild();
		//	for (auto& c : children)
		//	{
		//		if (c->GetOwner()->GetName() == HashString{"FFBar"})
		//		{
		//			DEBUG_PRINT(eLog::MESSAGE, "Bar! 0");
		//			HUD_MSG::SendExternalMessage(c->GetOwner(), "EnergyLerpTo", 0.f);
		//			return;
		//		}
		//	}
		//}
		//else if (auto d = EngineCore::Get<InputManager>()->GetAnalogX(0))
		//{
		//	ChangeHealth(d > 0 ? _fDeltaTime : -1.f * _fDeltaTime);
		//}
		//else if (EngineCore::Get<InputManager>()->GetTriggers(0))
		//{
		//	Fade(true);
		//}
		//else if (EngineCore::Get<InputManager>()->GetTriggers(1))
		//{
		//	Fade(false);
		//}
	    //
		ChangeAllAlpha(_fDeltaTime, FadeSpeed);
	}

	void HUD::FixedUpdate(const float ) 
	{
	}

	void HUD::PostUpdate(void)
	{
	}

	void HUD::GameObjectDestroy(void)
	{
	}

	void HUD::Unload(void)
	{
	}

	void Dystopia::HUD::OnCollisionEnter(const CollisionEvent& )
	{

	}

	void Dystopia::HUD::OnCollisionStay(const CollisionEvent& )
	{

	}

	void Dystopia::HUD::OnCollisionExit(const CollisionEvent& )
	{

	}

	void Dystopia::HUD::OnTriggerEnter(GameObject * const )
	{
	}

	void Dystopia::HUD::OnTriggerStay(GameObject * const )
	{
	}

	void Dystopia::HUD::OnTriggerExit(GameObject * const )
	{
	}

	HUD * HUD::Duplicate() const
	{
		return new HUD{ *this };
	}

	void HUD::Serialise(TextSerialiser& ) const
	{
	}

	void HUD::Unserialise(TextSerialiser& )
	{
	}

	const char * const HUD::GetBehaviourName() const
	{
		return HUD::BehaviourName;
	}

	void HUD::EditorUI(void) noexcept
	{
	}
	
	void HUD::Fade(bool _in)
	{
		FadeMode = _in ? 1 : 2;
	}
		
	void HUD::ChangeFF(int _i)
	{
		auto& children = GetOwner()->GetComponent<Transform>()->GetAllChild();
		for (auto& c : children)
		{
			if (c->GetOwner()->GetName() == HashString{"FFIcon"})
			{ 
				c->GetOwner()->GetComponent<SpriteRenderer>()->SetTexture(
				EngineCore::Get<GraphicsSystem>()->LoadTexture( _i == 1 ? 
				EngineCore::Get<FileSystem>()->GetFromResource("Form.png").c_str() : 
				EngineCore::Get<FileSystem>()->GetFromResource("Force.png").c_str()
				));
			}
			if (c->GetOwner()->GetName() == HashString{"FFBar"})
			{ 
				c->GetOwner()->GetComponent<SpriteRenderer>()->SetTexture(
				EngineCore::Get<GraphicsSystem>()->LoadTexture( _i == 1 ? 
				EngineCore::Get<FileSystem>()->GetFromResource("form bar.png").c_str() : 
				EngineCore::Get<FileSystem>()->GetFromResource("force bar.png").c_str()
				));
			}
		}
	}
	
	void HUD::ChangeHealth(float _dir)
	{
		auto& children = GetOwner()->GetComponent<Transform>()->GetAllChild();
		for (auto& c : children)
		{
			if (c->GetOwner()->GetName() == HashString{"Health"})
			{ 
				HUD_MSG::SendExternalMessage(c->GetOwner(), "ChangeHP", _dir);
				return;
			} 
		}
	}
	
	void HUD::ChangeAllAlpha(float _dt, float _speed)
	{
		if (!FadeMode) 
			return;
		
		if (_dt <= 0.0001f)
			_dt = 0.016f;
		
		float modifier = _dt * _speed;
		
		modifier = FadeMode == 1 ? modifier : modifier * -1.f;
		
		HUDAlpha += modifier;
		
		if (HUDAlpha < 0)
			FadeMode = static_cast<int>(HUDAlpha = 0);
		else if ( HUDAlpha > 1)
		{
			FadeMode = 0;
			HUDAlpha = 1;
		}
		 
		auto& children = GetOwner()->GetComponent<Transform>()->GetAllChild();
		for (auto& c : children)
			if (auto s = c->GetOwner()->GetComponent<SpriteRenderer>())
				s->SetAlpha(HUDAlpha);
	}

	void HUD::UpdateHealth(float percentage)
	{
		auto& children = GetOwner()->GetComponent<Transform>()->GetAllChild();
		for (auto& c : children)
		{
			if (c->GetOwner()->GetName() == HashString{"Health"})
			{
				DEBUG_PRINT(eLog::MESSAGE, "%f", percentage);
				HUD_MSG::SendExternalMessage(c->GetOwner(), "HPLerpTo", percentage);
				return;
			}
		}
	}

	void HUD::UpdateEnergy(float percentage)
	{
		auto& children = GetOwner()->GetComponent<Transform>()->GetAllChild();
		for (auto& c : children)
		{
			if (c->GetOwner()->GetName() == HashString{"FFBar"})
			{
				DEBUG_PRINT(eLog::MESSAGE, "%f", percentage);
				HUD_MSG::SendExternalMessage(c->GetOwner(), "EnergyLerpTo", percentage);
				return;
			}
		}
	}

	TypeErasure::TypeEraseMetaData HUD::GetMetaData()
	{
		static MetaData<Dystopia::HUD> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{ mMetaData };
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const HUD::GetMetaData() const
	{
		static MetaData<Dystopia::HUD> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{ mMetaData };
		return mReturn;
	}
}




