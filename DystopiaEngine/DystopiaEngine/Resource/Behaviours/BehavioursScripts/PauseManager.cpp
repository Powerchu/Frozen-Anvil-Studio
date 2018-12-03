/* HEADER *********************************************************************************/
/*!
\file	PauseManager.cpp
\author Keith Goh (100%)
\par    email: keith.goh\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "PauseManager.h"
#include "System/Input/InputSystem.h"
#include "System/Input/InputMap.h"
#include "System/Driver/Driver.h"
#include "System/Behaviour/BehaviourSystem.h"

#include "System/Scene/SceneSystem.h"
#include "System/Time/TimeSystem.h"
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
	namespace PauseManager_MSG
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
	PauseManager::PauseManager()
	:mPauseState{false},
	mButtons{nullptr},
	selection{0}
	{
	}  

	PauseManager::~PauseManager()
	{
	}

	void PauseManager::Load()
	{
		SetFlags(FLAG_ACTIVE);
		mPauseState = false;
		EngineCore::GetInstance()->Get<TimeSystem>()->SetTimeScale(1.f); 
	}

	void PauseManager::Awake()
	{
		DEBUG_PRINT(eLog::MESSAGE, "Awake");
		SetFlags(FLAG_ACTIVE);
		mPauseState = false;
		EngineCore::GetInstance()->Get<TimeSystem>()->SetTimeScale(1.f); 
		mButtons[0] = EngineCore::GetInstance()->Get<SceneSystem>()->FindGameObject_cstr("Resume_Button");
		mButtons[1] = EngineCore::GetInstance()->Get<SceneSystem>()->FindGameObject_cstr("Restart_Button");
		mButtons[2] = EngineCore::GetInstance()->Get<SceneSystem>()->FindGameObject_cstr("Settings_Button");
		mButtons[3] = EngineCore::GetInstance()->Get<SceneSystem>()->FindGameObject_cstr("Quit_Button");
		mSelector   = EngineCore::GetInstance()->Get<SceneSystem>()->FindGameObject_cstr("Select_Button");
		selection = 0;
		if(mSelector && mButtons[selection])
		{
			mSelector->GetComponent<Transform>()->SetGlobalPosition(mButtons[selection]->GetComponent<Transform>()->GetGlobalPosition() + Math::MakeVector3D(17,0,0));
		}
	}

	void PauseManager::Init()
	{
		DEBUG_PRINT(eLog::MESSAGE, "Init");
		SetFlags(FLAG_ACTIVE);
		mPauseState = false;
		EngineCore::GetInstance()->Get<TimeSystem>()->SetTimeScale(1.f);
		mButtons[0] = EngineCore::GetInstance()->Get<SceneSystem>()->FindGameObject_cstr("Resume_Button");
		mButtons[1] = EngineCore::GetInstance()->Get<SceneSystem>()->FindGameObject_cstr("Restart_Button");
		mButtons[2] = EngineCore::GetInstance()->Get<SceneSystem>()->FindGameObject_cstr("Settings_Button");
		mButtons[3] = EngineCore::GetInstance()->Get<SceneSystem>()->FindGameObject_cstr("Quit_Button");
		mArrScale[0] = mButtons[0]->GetComponent<Transform>()->GetGlobalScale();
		mArrScale[1] = mButtons[1]->GetComponent<Transform>()->GetGlobalScale();
		mArrScale[2] = mButtons[2]->GetComponent<Transform>()->GetGlobalScale();
		mArrScale[3] = mButtons[3]->GetComponent<Transform>()->GetGlobalScale();
		mSelector   = EngineCore::GetInstance()->Get<SceneSystem>()->FindGameObject_cstr("Select_Button");
		selection = 0;
		if(mSelector && mButtons[selection])
		{
			mSelector->GetComponent<Transform>()->SetGlobalPosition(mButtons[selection]->GetComponent<Transform>()->GetGlobalPosition() + Math::MakeVector3D(17,0,0));
			mSelector->RemoveFlags(eObjFlag::FLAG_ACTIVE);
			if (auto sr = mSelector->GetComponent<SpriteRenderer>())
				sr->RemoveFlags(eObjFlag::FLAG_ACTIVE);
			auto& sChildren = mSelector->GetComponent<Transform>()->GetAllChild();
			for (auto&c : sChildren)
			{
				c->GetOwner()->RemoveFlags(eObjFlag::FLAG_ACTIVE);
				if (auto s = c->GetOwner()->GetComponent<SpriteRenderer>())
					s->RemoveFlags(eObjFlag::FLAG_ACTIVE);
			}
		}
		auto& children = GetOwner()->GetComponent<Transform>()->GetAllChild();
		for(auto & c : children)
		{
			c->GetOwner()->RemoveFlags(eObjFlag::FLAG_ACTIVE);
			if (auto s = c->GetOwner()->GetComponent<SpriteRenderer>())
				s->RemoveFlags(eObjFlag::FLAG_ACTIVE);
		}
	}

	void PauseManager::Update(const float)
	{
		bool isPaused = EngineCore::GetInstance()->GetSystem<InputManager>()->IsKeyTriggered(XBUTTON_START) || EngineCore::GetInstance()->GetSystem<InputManager>()->IsKeyTriggered(KEYBOARD_ESCAPE);
		if(isPaused) 
		{
			if(mPauseState)
			{
				DEBUG_PRINT(eLog::MESSAGE, "unpaused!");
				mPauseState = false;
				EngineCore::GetInstance()->Get<TimeSystem>()->SetTimeScale(1.f);
				auto& children = GetOwner()->GetComponent<Transform>()->GetAllChild();
				for(auto & c : children)
				{
					c->GetOwner()->RemoveFlags(eObjFlag::FLAG_ACTIVE);
					if (auto s = c->GetOwner()->GetComponent<SpriteRenderer>())
						s->RemoveFlags(eObjFlag::FLAG_ACTIVE);
				}
				mSelector->RemoveFlags(eObjFlag::FLAG_ACTIVE);
				if (auto sr = mSelector->GetComponent<SpriteRenderer>())
					sr->RemoveFlags(eObjFlag::FLAG_ACTIVE);
				auto& sChildren = mSelector->GetComponent<Transform>()->GetAllChild();
				for (auto&c : sChildren)
				{
					c->GetOwner()->RemoveFlags(eObjFlag::FLAG_ACTIVE);
					if (auto s = c->GetOwner()->GetComponent<SpriteRenderer>())
						s->RemoveFlags(eObjFlag::FLAG_ACTIVE);
				}
				selection = 0;
			} 
			else
			{
				DEBUG_PRINT(eLog::MESSAGE, "paused!"); 
				EngineCore::GetInstance()->Get<TimeSystem>()->SetTimeScale(0.f); 
				mPauseState = true;   
				auto& children = GetOwner()->GetComponent<Transform>()->GetAllChild();
				for(auto & c : children)
				{
					c->GetOwner()->SetFlag(eObjFlag::FLAG_ACTIVE);
					if (auto s = c->GetOwner()->GetComponent<SpriteRenderer>())
						s->SetFlags(eObjFlag::FLAG_ACTIVE);
				}
				mSelector->SetFlag(eObjFlag::FLAG_ACTIVE);
				if (auto sr = mSelector->GetComponent<SpriteRenderer>())
					sr->SetFlags(eObjFlag::FLAG_ACTIVE);
				auto& sChildren = mSelector->GetComponent<Transform>()->GetAllChild();
				for (auto&c : sChildren)
				{
					c->GetOwner()->SetFlag(eObjFlag::FLAG_ACTIVE);
					if (auto s = c->GetOwner()->GetComponent<SpriteRenderer>())
						s->SetFlags(eObjFlag::FLAG_ACTIVE);
				}
				selection = 0;
			} 
			HighlightScale();
		}
		
		if(mPauseState)
		{
			bool isDown    = EngineCore::GetInstance()->GetSystem<InputManager>()->IsKeyTriggered(eButton::XBUTTON_DPAD_DOWN) || EngineCore::GetInstance()->GetSystem<InputManager>()->IsKeyTriggered( eButton::KEYBOARD_DOWN);
			bool isUp      = EngineCore::GetInstance()->GetSystem<InputManager>()->IsKeyTriggered(XBUTTON_DPAD_UP) || EngineCore::GetInstance()->GetSystem<InputManager>()->IsKeyTriggered( eButton::KEYBOARD_UP);
			bool isPress   = EngineCore::GetInstance()->GetSystem<InputManager>()->IsKeyTriggered(XBUTTON_A) || EngineCore::GetInstance()->GetSystem<InputManager>()->IsKeyTriggered( eButton::KEYBOARD_ENTER);
			if(isPress)
			{
				if(selection == 0)
				{
					if(mButtons[selection])
						PauseManager_MSG::SendExternalMessage(mButtons[selection],"ButtonPress");
					
					DEBUG_PRINT(eLog::MESSAGE, "unpaused!"); 
					EngineCore::GetInstance()->Get<TimeSystem>()->SetTimeScale(1.f); 
					mPauseState = false;
					selection = 0;

					auto& children = GetOwner()->GetComponent<Transform>()->GetAllChild();
					for (auto & c : children)
					{
						c->GetOwner()->RemoveFlags(eObjFlag::FLAG_ACTIVE);
						if (auto s = c->GetOwner()->GetComponent<SpriteRenderer>())
							s->RemoveFlags(eObjFlag::FLAG_ACTIVE);
					}
					mSelector->RemoveFlags(eObjFlag::FLAG_ACTIVE);
					if (auto sr = mSelector->GetComponent<SpriteRenderer>())
						sr->RemoveFlags(eObjFlag::FLAG_ACTIVE);
					auto& sChildren = mSelector->GetComponent<Transform>()->GetAllChild();
					for (auto&c : sChildren)
					{
						c->GetOwner()->RemoveFlags(eObjFlag::FLAG_ACTIVE);
						if (auto s = c->GetOwner()->GetComponent<SpriteRenderer>())
							s->RemoveFlags(eObjFlag::FLAG_ACTIVE);
					}
				}
				else
				{
					if(mButtons[selection])
						PauseManager_MSG::SendExternalMessage(mButtons[selection], "ButtonPress");
				}

			}
			if(isUp)
			{
				selection = selection - 1 < 0 ? 3 : selection - 1;
				if(mSelector && mButtons[selection])
				{
					mSelector->GetComponent<Transform>()->SetGlobalPosition(mButtons[selection]->GetComponent<Transform>()->GetGlobalPosition() + Math::MakeVector3D(17,0,0));
				}
				HighlightScale();
			}
			if(isDown)
			{
				selection = selection + 1 > 3 ? 0 : selection + 1;
				if(mSelector && mButtons[selection])
				{
					mSelector->GetComponent<Transform>()->SetGlobalPosition(mButtons[selection]->GetComponent<Transform>()->GetGlobalPosition() + Math::MakeVector3D(17,0,0));
				}
				HighlightScale();
			}
		}
	}

	void PauseManager::FixedUpdate(const float)
	{
	}

	void PauseManager::PostUpdate(void)
	{
	}

	void PauseManager::GameObjectDestroy(void)
	{
	}

	void PauseManager::Unload(void)
	{
	}

	void Dystopia::PauseManager::OnCollisionEnter(const CollisionEvent&)
	{

	}

	void Dystopia::PauseManager::OnCollisionStay(const CollisionEvent&)
	{

	}

	void Dystopia::PauseManager::OnCollisionExit(const CollisionEvent&)
	{

	}

	void Dystopia::PauseManager::OnTriggerEnter(GameObject * const)
	{
	}

	void Dystopia::PauseManager::OnTriggerStay(GameObject * const)
	{
	}

	void Dystopia::PauseManager::OnTriggerExit(GameObject * const)
	{
	}

	PauseManager * PauseManager::Duplicate() const
	{
		return new PauseManager{ *this };
	}

	void PauseManager::Serialise(TextSerialiser&) const
	{
	}

	void PauseManager::Unserialise(TextSerialiser&)
	{
	}


	const char * const PauseManager::GetBehaviourName() const
	{
		return PauseManager::BehaviourName;
	}

	void PauseManager::EditorUI(void) noexcept
	{


	}
	
	void PauseManager::HighlightScale(void)
	{
		float scale = 1.3f;
		for (size_t i = 0; i < 4; ++i)
		{
			if (selection == i)
			{
				auto o = mButtons[selection]->GetComponent<Transform>()->GetGlobalScale();
				o.x = mArrScale[selection].x * scale;
				o.y = mArrScale[selection].y * scale;
				mButtons[selection]->GetComponent<Transform>()->SetGlobalScale(o);
				if (auto s = mButtons[selection]->GetComponent<SpriteRenderer>())
					s->SetColorA(Math::Vector4{1.f, 0.8f, .1f, 1.f});
				DEBUG_PRINT(eLog::MESSAGE, "SetColorA %d", selection);
			}
			else
			{
				auto o = mButtons[i]->GetComponent<Transform>()->GetGlobalScale();
				o.x = mArrScale[i].x;
				o.y = mArrScale[i].y;
				mButtons[i]->GetComponent<Transform>()->SetGlobalScale(o);
				if (auto s2 = mButtons[i]->GetComponent<SpriteRenderer>())
					s2->SetColorA(Math::Vector4{1.f, 1.f, 1.f, 1.f}); 
			}
		}
	}

	TypeErasure::TypeEraseMetaData PauseManager::GetMetaData()
	{
		static MetaData<Dystopia::PauseManager> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{ mMetaData };
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const PauseManager::GetMetaData() const
	{
		static MetaData<Dystopia::PauseManager> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{ mMetaData };
		return mReturn;
	}
}




