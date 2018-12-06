/* HEADER *********************************************************************************/
/*!
\file	TownLoader.cpp
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "TownLoader.h"
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
	namespace TownLoader_MSG
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
	TownLoader::TownLoader()
		: closeupDuration{1.5f}, countdown{false}, currDuration{0}, sidewaysDuration{1.5f}, goalX{0}
	{
	}

	TownLoader::~TownLoader()
	{
	}

	void TownLoader::Load()
	{
	}

	void TownLoader::Awake()
	{
	}

	void TownLoader::Init()
	{
		currDuration = closeupDuration;
		currXDuration = sidewaysDuration;
		SetActive(true);
		
		player = nullptr;
		mainCam = nullptr;
		wallsOfBlack = nullptr;
		slider = nullptr;
		if(auto obj = EngineCore::Get<SceneSystem>()->FindGameObject_cstr("Main Camera"))
		{
			mainCam = obj->GetComponent<Transform>();
		}
		if(auto obj = EngineCore::Get<SceneSystem>()->FindGameObject_cstr("WallOfBlack"))
		{
			obj->SetActive(false);
			wallsOfBlack = obj->GetComponent<Transform>();
			originalY = wallsOfBlack->GetGlobalScale().y;
		}
		if(auto obj = EngineCore::Get<SceneSystem>()->FindGameObject_cstr("BlackSlider"))
		{
			slider = obj->GetComponent<Transform>();
			origingalX = slider->GetGlobalPosition().x;
		} 
	}

	void TownLoader::Update(const float _fDeltaTime)
	{
		if (countdown && currDuration > 0.f)
		{
			currDuration -= _fDeltaTime;
			float ratio = currDuration/closeupDuration;
			
			if (wallsOfBlack)
			{
				//auto v = wallsOfBlack->GetGlobalPosition();
				auto s = wallsOfBlack->GetGlobalScale();
				//if(player)
				//{
				//	v.y = player->GetComponent<Transform>()->GetGlobalPosition().y;
				//}
				
				s.y = goalBlackStuff + ((originalY - goalBlackStuff) * ratio); 
				//wallsOfBlack->SetGlobalPosition(v);
				wallsOfBlack->SetGlobalScale(s);
				
				//if (mainCam)
				//{
				//	auto ds = mainCam->GetGlobalScale();
				//	float sz = Math::Lerp<float>(camResultSize, camStartSize, ratio);
				//}
			}
		}
		else if (currDuration <= 0.f && countdown && currXDuration > 0.f)
		{
			currXDuration -= _fDeltaTime;
			float ratio = currXDuration/sidewaysDuration;
			if (slider)
			{
				auto p = slider->GetGlobalPosition();
				p.x = goalX + ((origingalX-goalX) * ratio);	
				slider->SetGlobalPosition(p);
			}
		}
		else if(currXDuration < 0.f && currDuration < 0.f)
		{
			countdown = false;
			EngineCore::Get<SceneSystem>()->LoadScene("Town.dscene");
		}
	}

	void TownLoader::FixedUpdate(const float )
	{
	}
	
	void TownLoader::PostUpdate(void)
	{
	}

	void TownLoader::GameObjectDestroy(void)
	{
	}

	void TownLoader::Unload(void)
	{
	}

	void Dystopia::TownLoader::OnCollisionEnter(const CollisionEvent& )
	{

	}

	void Dystopia::TownLoader::OnCollisionStay(const CollisionEvent& )
	{

	}

	void Dystopia::TownLoader::OnCollisionExit(const CollisionEvent& )
	{

	}

	void Dystopia::TownLoader::OnTriggerEnter(GameObject * const _obj)
	{
		if (_obj->GetName() == HashString{"Player"})
		{
			countdown = true;
			TownLoader_MSG::SendExternalMessage(_obj, "DisableControls", true);
			
			if(wallsOfBlack) 
			{
				//auto v = wallsOfBlack->GetGlobalPosition();
				//v.y = _obj->GetComponent<Transform>()->GetGlobalPosition().y;
				//wallsOfBlack->SetGlobalPosition(v);
				//player = _obj;
				wallsOfBlack->GetOwner()->SetActive(true);
			}
			
			if (mainCam)
			{
				camStartSize = mainCam->GetGlobalScale().x;
			}
			
			if(auto obj = EngineCore::Get<SceneSystem>()->FindGameObject_cstr("HUD"))
			{
				TownLoader_MSG::SendExternalMessage(obj, "Fade", false);
			} 
		}
	}

	void Dystopia::TownLoader::OnTriggerStay(GameObject * const )
	{
	}

	void Dystopia::TownLoader::OnTriggerExit(GameObject * const )
	{
	}

	TownLoader * TownLoader::Duplicate() const
	{
		return new TownLoader{*this};
	}

	void TownLoader::Serialise(TextSerialiser& ) const
	{
	}

	void TownLoader::Unserialise(TextSerialiser& )
	{
	}


	const char * const TownLoader::GetBehaviourName() const
	{
		return TownLoader::BehaviourName;
	}

	void TownLoader::EditorUI(void) noexcept
	{
		
		
	}
	
	TypeErasure::TypeEraseMetaData TownLoader::GetMetaData()
	{
		static MetaData<Dystopia::TownLoader> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const TownLoader::GetMetaData() const
	{
		static MetaData<Dystopia::TownLoader> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
}




