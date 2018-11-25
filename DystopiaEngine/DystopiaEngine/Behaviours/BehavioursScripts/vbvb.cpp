/* HEADER *********************************************************************************/
/*!
\file	vbvb.cpp
\author vbvb (100%)
\par    email: vbv\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "vbvb.h"
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
	namespace vbvb_MSG
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
	vbvb::vbvb()
	{
	}

	vbvb::~vbvb()
	{
	}

	void vbvb::Load()
	{
	}

	void vbvb::Awake()
	{
	}

	void vbvb::Init()
	{
	}

	void vbvb::Update(const float _fDeltaTime)
	{
	}

	void vbvb::FixedUpdate(const float _fDeltaTime)
	{
	}
	
	void vbvb::PostUpdate(void)
	{
	}

	void vbvb::GameObjectDestroy(void)
	{
	}

	void vbvb::Unload(void)
	{
	}

	void Dystopia::vbvb::OnCollisionEnter(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::vbvb::OnCollisionStay(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::vbvb::OnCollisionExit(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::vbvb::OnTriggerEnter(const GameObject * _obj)
	{
	}

	void Dystopia::vbvb::OnTriggerStay(const GameObject * _obj)
	{
	}

	void Dystopia::vbvb::OnTriggerExit(const GameObject * _obj)
	{
	}

	vbvb * vbvb::Duplicate() const
	{
		return new vbvb{};
	}

	void vbvb::Serialise(TextSerialiser& _ser) const
	{
	}

	void vbvb::Unserialise(TextSerialiser& _ser)
	{
	}


	const char * const vbvb::GetBehaviourName() const
	{
		return vbvb::BehaviourName;
	}

	void vbvb::EditorUI(void) noexcept
	{
		
		
	}
	
	TypeErasure::TypeEraseMetaData vbvb::GetMetaData()
	{
		static MetaData<Dystopia::vbvb> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const vbvb::GetMetaData() const
	{
		static MetaData<Dystopia::vbvb> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
}




