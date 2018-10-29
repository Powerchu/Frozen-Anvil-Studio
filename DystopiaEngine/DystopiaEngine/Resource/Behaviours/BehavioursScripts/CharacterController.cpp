/* HEADER *********************************************************************************/
/*!
\file	CharacterController.cpp
\author Aaron Chu (100%)
\par    email: m.chu\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "CharacterController.h"
#include "System/Input/InputSystem.h"
#include "System/Input/InputMap.h"
#include "System/Driver/Driver.h"
#include "Editor/EGUI.h"
#include "Utility/DebugAssert.h"
#include "Object/GameObject.h"

namespace Dystopia
{
	CharacterController::CharacterController()
	{
	}

	CharacterController::~CharacterController()
	{
	}

	void CharacterController::Load()
	{
	}

	void CharacterController::Init()
	{
	}

	void CharacterController::Update(const float _fDeltaTime)
	{
	}

	void CharacterController::FixedUpdate(const float _fDeltaTime)
	{
	}
	
	void CharacterController::PostUpdate(void)
	{
	}

	void CharacterController::GameObjectDestroy(void)
	{
	}

	void CharacterController::Unload(void)
	{
	}

	void Dystopia::CharacterController::OnCollisionEnter(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::CharacterController::OnCollisionStay(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::CharacterController::OnCollisionExit(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::CharacterController::OnTriggerEnter(const GameObject * _obj)
	{
	}

	void Dystopia::CharacterController::OnTriggerStay(const GameObject * _obj)
	{
	}

	void Dystopia::CharacterController::OnTriggerExit(const GameObject * _obj)
	{
	}

	CharacterController * CharacterController::Duplicate() const
	{
		return new CharacterController{};
	}

	void CharacterController::Serialise(TextSerialiser& _ser) const
	{
	}

	void CharacterController::Unserialise(TextSerialiser& _ser)
	{
	}


	const char * const CharacterController::GetBehaviourName() const
	{
		return CharacterController::BehaviourName;
	}

	void CharacterController::EditorUI(void) noexcept
	{
		
		
	}
	
	TypeErasure::TypeEraseMetaData CharacterController::GetMetaData()
	{
		/*TO DO*/
		/*REMEMBER TO RETURN YOUR REFLECTED DATA HERE*/
		return TypeErasure::TypeEraseMetaData{};
	}
	TypeErasure::TypeEraseMetaData const CharacterController::GetMetaData() const
	{
		/*TO DO*/
		/*REMEMBER TO RETURN YOUR REFLECTED DATA HERE*/
		return TypeErasure::TypeEraseMetaData{};
	}
}




