/* HEADER *********************************************************************************/
/*!
\file	_SF_ClassName_.cpp
\author _SF_CreatorName_ (100%)
\par    email: _SF_CreatorLogin_\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "_SF_ClassName_.h"
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
	namespace _SF_ClassName__MSG
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
	_SF_ClassName_::_SF_ClassName_()
	{
	}

	_SF_ClassName_::~_SF_ClassName_()
	{
	}

	void _SF_ClassName_::Load()
	{
	}

	void _SF_ClassName_::Awake()
	{
	}

	void _SF_ClassName_::Init()
	{
	}

	void _SF_ClassName_::Update(const float _fDeltaTime)
	{
	}

	void _SF_ClassName_::FixedUpdate(const float _fDeltaTime)
	{
	}
	
	void _SF_ClassName_::PostUpdate(void)
	{
	}

	void _SF_ClassName_::GameObjectDestroy(void)
	{
	}

	void _SF_ClassName_::Unload(void)
	{
	}

	void Dystopia::_SF_ClassName_::OnCollisionEnter(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::_SF_ClassName_::OnCollisionStay(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::_SF_ClassName_::OnCollisionExit(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::_SF_ClassName_::OnTriggerEnter(GameObject * const _obj)
	{
	}

	void Dystopia::_SF_ClassName_::OnTriggerStay(GameObject * const _obj)
	{
	}

	void Dystopia::_SF_ClassName_::OnTriggerExit(GameObject * const _obj)
	{
	}

	_SF_ClassName_ * _SF_ClassName_::Duplicate() const
	{
		return new _SF_ClassName_{};
	}

	void _SF_ClassName_::Serialise(TextSerialiser& _ser) const
	{
	}

	void _SF_ClassName_::Unserialise(TextSerialiser& _ser)
	{
	}


	const char * const _SF_ClassName_::GetBehaviourName() const
	{
		return _SF_ClassName_::BehaviourName;
	}

	void _SF_ClassName_::EditorUI(void) noexcept
	{
		
		
	}
	
	TypeErasure::TypeEraseMetaData _SF_ClassName_::GetMetaData()
	{
		static MetaData<Dystopia::_SF_ClassName_> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const _SF_ClassName_::GetMetaData() const
	{
		static MetaData<Dystopia::_SF_ClassName_> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
}




