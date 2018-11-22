/* HEADER *********************************************************************************/
/*!
\file	Fireball.cpp
\author keith.goh (100%)
\par    email: keith.goh\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "Fireball.h"
#include "System/Input/InputSystem.h"
#include "System/Input/InputMap.h"
#include "System/Driver/Driver.h"
#include "Editor/EGUI.h"
#include "Utility/DebugAssert.h"
#include "System/Scene/SceneSystem.h"
#include "Object/GameObject.h"
#include "System/Collision/CollisionEvent.h"
#include "System/Behaviour/BehaviourSystem.h"
#include <cstdint>

namespace Dystopia
{
	namespace Fireball_MSG
	{
		template<typename ... Ts>
		void SendInternalMessage(Behaviour * ptr, const char * _FuncName, Ts ... _Params)
		{
			EngineCore::GetInstance()->Get<BehaviourSystem>()->SendInternalMessage(ptr, _FuncName, _Params...);
		}
		template<typename ... Ts>
		void SendExternalMessage(uint64_t _ObjectID, const char * _FuncName, Ts ... _Params)
		{
			EngineCore::GetInstance()->Get<BehaviourSystem>()->SendExternalMessage(_ObjectID, _FuncName, _Params...);
		}
		template<typename ... Ts>
		void SendExternalMessage(GameObject * const _ptr, const char * _FuncName, Ts ... _Params)
		{
			//if(!_ptr) return;
			if(auto ptr = EngineCore::GetInstance()->Get<BehaviourSystem>())
				ptr->SendExternalMessage(_ptr, _FuncName, _Params...);
		}
		template<typename ... Ts>
		void SendExternalMessage(GameObject const * _ptr, const char * _FuncName, Ts ... _Params)
		{
			//if(!_ptr) return;
			if(auto ptr = EngineCore::GetInstance()->Get<BehaviourSystem>())
				ptr->SendExternalMessage(_ptr, _FuncName, _Params...);
		}
		template<typename ... Ts>
		void SendAllMessage(const char * _FuncName, Ts ... _Params)
		{
			EngineCore::GetInstance()->Get<BehaviourSystem>()->SendAllMessage(_FuncName, _Params...);
		}
	}
	
	Fireball::Fireball()
	{
	}

	Fireball::~Fireball()
	{
	}

	void Fireball::Load()
	{
        
	}
	
	void Fireball::Awake()
	{
		SetFlags(FLAG_ACTIVE);
	}

	void Fireball::Init()
	{
		SetFlags(FLAG_ACTIVE);
	}

	void Fireball::Update(const float _fDeltaTime)
	{
        Fireball_MSG::SendInternalMessage(this,"TEST", 10.f);
	}

	void Fireball::FixedUpdate(const float _fDeltaTime)
	{
	}
	
	void Fireball::PostUpdate(void)
	{
	}

	void Fireball::GameObjectDestroy(void)
	{
	}

	void Fireball::Unload(void)
	{
	}

	void Dystopia::Fireball::OnCollisionEnter(const CollisionEvent& _colEvent)
	{
		auto * ptr = EngineCore::GetInstance()->Get<SceneSystem>()->FindGameObject(_colEvent.mOtherID);
		if(ptr)
		{
			//const char * name = ptr->Get
			const char * name = ptr->GetNamePtr();
			if(strcmp(name,"PlayerAttackTrig") &&
			   strcmp(name, "Fireball")&& strcmp(name, "Missle")
			   && strcmp(name, "AudioTrig")) 
			{
				GetOwner()->Destroy();
			}
		}
	}

	void Dystopia::Fireball::OnCollisionStay(const CollisionEvent& _colEvent)
	{
	
	}

	void Dystopia::Fireball::OnCollisionExit(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::Fireball::OnTriggerEnter(GameObject * const _obj)
	{
		if(!_obj) 
			return;
		
		auto * ptr = EngineCore::GetInstance()->Get<SceneSystem>()->FindGameObject(_obj->GetID());
		if(ptr)
		{
			//const char * name = ptr->Get
			const char * name = ptr->GetNamePtr();
			if(strcmp(name,"PlayerAttackTrig") &&
			   strcmp(name, "Fireball")&& strcmp(name, "Missle")
			   && strcmp(name, "AudioTrig"))
			{
				if(GetOwner())
					GetOwner()->Destroy();
			}
			if(!strcmp(name,"Goblin"))
			{
				ptr->GetID();
				Fireball_MSG::SendExternalMessage(ptr,"TEST", 1000.f);  
			}
		}
	}

	void Dystopia::Fireball::OnTriggerStay(GameObject * const _obj)
	{
	
	}

	void Dystopia::Fireball::OnTriggerExit(GameObject * const _obj)
	{
	}

	Fireball * Fireball::Duplicate() const
	{
		return new Fireball{};
	}

	void Fireball::Serialise(TextSerialiser& _ser) const
	{
	}

	void Fireball::Unserialise(TextSerialiser& _ser)
	{
	}


	const char * const Fireball::GetBehaviourName() const
	{
		return Fireball::BehaviourName;
	}

	void Fireball::EditorUI(void) noexcept
	{
		
		
	}
	
	TypeErasure::TypeEraseMetaData Fireball::GetMetaData()
	{
		/*TO DO*/
		/*REMEMBER TO RETURN YOUR REFLECTED DATA HERE*/
		static MetaData<Dystopia::Fireball> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const Fireball::GetMetaData() const
	{
		/*TO DO*/
		/*REMEMBER TO RETURN YOUR REFLECTED DATA HERE*/
		return TypeErasure::TypeEraseMetaData{};
	}
    
    void Fireball::ReceiveMessage(const char * const _FuncName, BehaviourMessage _msg)
    {
		for(auto & elem : mMemberFunc)
		{
			if(!std::strcmp(elem.first, _FuncName))
			{ 
				elem.second.Invoke(this, _msg); 
			}
		}
    }
}




