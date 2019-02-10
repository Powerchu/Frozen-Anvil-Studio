/* HEADER *********************************************************************************/
/*!
\file	SortChildOrder.cpp
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "SortChildOrder.h"
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

// custom includes
#include "Component/Transform.h"

namespace Dystopia
{
	namespace SortChildOrder_MSG
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
	SortChildOrder::SortChildOrder()
		: mfValue{0, -0.25f, -0.5f, -0.75f},
		mbAlternate{true},
		mbTransform_X{false},
		mbTransform_Y{false},
		mbTransform_Z{true}
	{ 
	}

	SortChildOrder::~SortChildOrder()
	{
	}
	
	void SortChildOrder::Awake()
	{
	}
 
	void SortChildOrder::Init()
	{
		auto& children = GetOwner()->GetComponent<Transform>()->GetAllChild();
		for (unsigned i = 0; i < children.size(); i ++)
		{
			float val = mfValue[i%4];
			auto pos = children[i]->GetPosition();
			if (mbTransform_X) 
				pos.x = val; 
			if (mbTransform_Y)
				pos.y = val;
			if (mbTransform_Z) 
				pos.z = val; 
			children[i]->SetPosition(pos);
			DEBUG_PRINT(eLog::MESSAGE, "GE");
		}
	}

	void SortChildOrder::Update(const float )
	{
		
	}

	void SortChildOrder::GameObjectDestroy(void)
	{
		Behaviour::GameObjectDestroy();
	}

	SortChildOrder * SortChildOrder::Duplicate() const
	{
		return new SortChildOrder{*this};
	}

	const char * const SortChildOrder::GetBehaviourName() const
	{
		return SortChildOrder::BehaviourName;
	}
	
	TypeErasure::TypeEraseMetaData SortChildOrder::GetMetaData()
	{
		static MetaData<Dystopia::SortChildOrder> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const SortChildOrder::GetMetaData() const
	{
		static MetaData<Dystopia::SortChildOrder> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
}




