/* HEADER *********************************************************************************/
/*!
\file	GameOver.cpp
\author Dan (100%)
\par    email: dan.kang\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "GameOver.h"
#include "System/Input/InputSystem.h"
#include "System/Input/InputMap.h"
#include "System/Driver/Driver.h"
#include "System/Behaviour/BehaviourSystem.h"   
#include "System/Time/TimeSystem.h"

#include "System/Scene/SceneSystem.h"
#include "System/Collision/CollisionEvent.h"
#include "System/Collision/CollisionSystem.h"
#include "System/Graphics/VertexDefs.h"

#include "Object/ObjectFlags.h"
#include "Object/GameObject.h"

#include "Editor/EGUI.h"
#include "Utility/DebugAssert.h"
  
#include "Component/SpriteRenderer.h"
#include "Component/TextRenderer.h"

 
namespace Dystopia
{
	namespace GameOver_MSG
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
	GameOver::GameOver()
	:mFadeToBlack{false},
	 mFadeAmt{0.f}
	{
	}

	GameOver::~GameOver()
	{
	}

	void GameOver::Load()
	{
	}

	void GameOver::Awake()
	{
		if(EngineCore::Get<InputManager>()->IsController())
		{
			EngineCore::Get<InputManager>()->MapButton("Accept", eButton::XBUTTON_A);
		}
		else
		{
			EngineCore::Get<InputManager>()->MapButton("Accept", eButton::KEYBOARD_ENTER);
		}
		
		EngineCore::Get<TimeSystem>()->SetTimeScale(1.f);
		GetOwner()->GetComponent<SpriteRenderer>()->SetAlpha(0.f);
		GetOwner()->GetComponent<Transform>()->SetOpacity(0);
		//GetOwner()->GetComponent<TextRenderer>()->SetAlpha(0.f);
		//GetOwner()->GetComponent<Transform>()->GetAllChild()[0]->GetOwner()->GetComponent<TextRenderer>()->SetAlpha(0.f);
		// if(auto ptr = GetOwner()->GetComponent<Transform>()->GetAllChild()[0])
		// {
			// if(ptr->GetOwner() && ptr->GetOwner()->GetComponent<TextRenderer>())
			// {
				// auto Textptr = ptr->GetOwner()->GetComponent<TextRenderer>();
				// if(Textptr)
				// {
					// Textptr->SetAlpha(0.f);
				// }
			// }

		//}
	}

	void GameOver::Init()
	{
		EngineCore::Get<TimeSystem>()->SetTimeScale(1.f);
		GetOwner()->GetComponent<SpriteRenderer>()->SetAlpha(0.f);
		GetOwner()->GetComponent<Transform>()->SetOpacity(0);
	}

	void GameOver::Update(const float )
	{
		if(mFadeToBlack)
		{
			GetOwner()->GetComponent<SpriteRenderer>()->SetAlpha(mFadeAmt);
			GetOwner()->GetComponent<Transform>()->SetOpacity(mFadeAmt);
			mFadeAmt += 0.015f;
		}
		if(mFadeAmt > 1.f)
		{
			if(EngineCore::Get<InputManager>()->IsKeyTriggered("Accept"))
			{
				EngineCore::Get<TimeSystem>()->SetTimeScale(1.f);
				EngineCore::Get<SceneSystem>()->RestartScene();

			}
		}
	}

	void GameOver::FixedUpdate(const float )
	{
	}
	
	void GameOver::ActivateGameOver()
	{
		mFadeToBlack = true;
	    GetOwner()->GetComponent<SpriteRenderer>()->SetColorA(Math::Vec3D{0,0,0,0});
		//GetOwner()->GetComponent<TextRenderer>()->SetAlpha(0.f); 
		EngineCore::Get<TimeSystem>()->SetTimeScale(0.f);
		GetOwner()->GetComponent<Transform>()->SetOpacity(0.f);
	}
	void GameOver::PostUpdate(void)
	{
	}

	void GameOver::GameObjectDestroy(void)
	{
	}

	void GameOver::Unload(void)
	{
	}

	void Dystopia::GameOver::OnCollisionEnter(const CollisionEvent& )
	{

	}

	void Dystopia::GameOver::OnCollisionStay(const CollisionEvent& )
	{

	}

	void Dystopia::GameOver::OnCollisionExit(const CollisionEvent& )
	{

	} 

	void Dystopia::GameOver::OnTriggerEnter(GameObject * const )
	{
	}

	void Dystopia::GameOver::OnTriggerStay(GameObject * const )
	{
	}

	void Dystopia::GameOver::OnTriggerExit(GameObject * const )
	{
	}

	GameOver * GameOver::Duplicate() const
	{
		return new GameOver{*this};
	}

	void GameOver::Serialise(TextSerialiser& ) const
	{
	}

	void GameOver::Unserialise(TextSerialiser& )
	{
	}


	const char * const GameOver::GetBehaviourName() const
	{
		return GameOver::BehaviourName;
	}

	void GameOver::EditorUI(void) noexcept
	{  
		
		
	}
	
	TypeErasure::TypeEraseMetaData GameOver::GetMetaData()
	{
		static MetaData<Dystopia::GameOver> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const GameOver::GetMetaData() const
	{
		static MetaData<Dystopia::GameOver> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;  
	}
}




