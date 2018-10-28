/* HEADER *********************************************************************************/
/*!
\file	dsdsd.cpp
\author sd (100%)
\par    email: s\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "dsdsd.h"
#include "System/Input/InputSystem.h"
#include "System/Input/InputMap.h"
#include "System/Driver/Driver.h"
#include "Editor/EGUI.h"
#include "Utility/DebugAssert.h"

namespace Dystopia
{
	dsdsd::dsdsd()
	{
	}

	dsdsd::~dsdsd()
	{
	}

	void dsdsd::Load()
	{
	}

	void dsdsd::Init()
	{
	}

	void dsdsd::Update(const float _fDeltaTime)
	{
	}

	void dsdsd::FixedUpdate(const float _fDeltaTime)
	{
	}
	
	void dsdsd::PostUpdate(void)
	{
	}

	void dsdsd::GameObjectDestroy(void)
	{
	}

	void dsdsd::Unload(void)
	{
	}

	void Dystopia::dsdsd::OnCollisionEnter(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::dsdsd::OnCollisionStay(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::dsdsd::OnCollisionExit(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::dsdsd::OnTriggerEnter(const GameObject * _obj)
	{
	}

	void Dystopia::dsdsd::OnTriggerStay(const GameObject * _obj)
	{
	}

	void Dystopia::dsdsd::OnTriggerExit(const GameObject * _obj)
	{
	}

	dsdsd * dsdsd::Duplicate() const
	{
		return new dsdsd{};
	}

	void dsdsd::Serialise(TextSerialiser& _ser) const
	{
	}

	void dsdsd::Unserialise(TextSerialiser& _ser)
	{
	}


	const char * const dsdsd::GetBehaviourName() const
	{
		return dsdsd::BehaviourName;
	}

	void dsdsd::EditorUI(void) noexcept
	{
		
		
	}
	
	TypeErasure::TypeEraseMetaData dsdsd::GetMetaData()
	{
		/*TO DO*/
		/*REMEMBER TO RETURN YOUR REFLECTED DATA HERE*/
		return TypeErasure::TypeEraseMetaData{};
	}
	TypeErasure::TypeEraseMetaData const dsdsd::GetMetaData() const
	{
		/*TO DO*/
		/*REMEMBER TO RETURN YOUR REFLECTED DATA HERE*/
		return TypeErasure::TypeEraseMetaData{};
	}
}




