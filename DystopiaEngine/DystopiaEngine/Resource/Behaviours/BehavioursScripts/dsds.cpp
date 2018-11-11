/* HEADER *********************************************************************************/
/*!
\file	dsds.cpp
\author sds (100%)
\par    email: d\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "dsds.h"
#include "System/Input/InputSystem.h"
#include "System/Input/InputMap.h"
#include "System/Driver/Driver.h"
#include "System/Behaviour/BehaviourSystem.h"

#include "Editor/EGUI.h"
#include "Utility/DebugAssert.h"



namespace Dystopia
{
	namespace dsds_MSG
	{
		template<typename ... Ts>
		void SendInternalMessage(Behaviour * _ptr, const char * _FuncName, Ts ... _Params)
		{
			EngineCore::GetInstance()->Get<BehaviourSystem>()->SendInternalMessage(_ptr, _FuncName, _Params...);
		}
	}
	dsds::dsds()
	{
	}

	dsds::~dsds()
	{
	}

	void dsds::Load()
	{
	}

	void dsds::Init()
	{
	}

	void dsds::Update(const float _fDeltaTime)
	{
	}

	void dsds::FixedUpdate(const float _fDeltaTime)
	{
	}
	
	void dsds::PostUpdate(void)
	{
	}

	void dsds::GameObjectDestroy(void)
	{
	}

	void dsds::Unload(void)
	{
	}

	void Dystopia::dsds::OnCollisionEnter(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::dsds::OnCollisionStay(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::dsds::OnCollisionExit(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::dsds::OnTriggerEnter(const GameObject * _obj)
	{
	}

	void Dystopia::dsds::OnTriggerStay(const GameObject * _obj)
	{
	}

	void Dystopia::dsds::OnTriggerExit(const GameObject * _obj)
	{
	}

	dsds * dsds::Duplicate() const
	{
		return new dsds{};
	}

	void dsds::Serialise(TextSerialiser& _ser) const
	{
	}

	void dsds::Unserialise(TextSerialiser& _ser)
	{
	}


	const char * const dsds::GetBehaviourName() const
	{
		return dsds::BehaviourName;
	}

	void dsds::EditorUI(void) noexcept
	{
		
		
	}
	
	TypeErasure::TypeEraseMetaData dsds::GetMetaData()
	{
		/*TO DO*/
		/*REMEMBER TO RETURN YOUR REFLECTED DATA HERE*/
		return TypeErasure::TypeEraseMetaData{};
	}
	TypeErasure::TypeEraseMetaData const dsds::GetMetaData() const
	{
		/*TO DO*/
		/*REMEMBER TO RETURN YOUR REFLECTED DATA HERE*/
		return TypeErasure::TypeEraseMetaData{};
	}
}




