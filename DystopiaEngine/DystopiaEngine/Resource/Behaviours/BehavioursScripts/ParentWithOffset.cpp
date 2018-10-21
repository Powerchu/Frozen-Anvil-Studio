/* HEADER *********************************************************************************/
/*!
\file	ParentWithOffset.cpp
\author aaron (100%)
\par    email: m.chu\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "ParentWithOffset.h"
#include "Math/Vector2.h"
#include "System/Input/InputSystem.h"
#include "System/Input/InputMap.h"
#include "System/Driver/Driver.h"
#include "Editor/EGUI.h"
#include "Utility/DebugAssert.h"

namespace Dystopia
{
	ParentWithOffset::ParentWithOffset()
	{
	}

	ParentWithOffset::~ParentWithOffset()
	{
	}

	void ParentWithOffset::Load()
	{
	}

	void ParentWithOffset::Init()
	{
	}

	void ParentWithOffset::Update(const float _fDeltaTime)
	{
	}

	void ParentWithOffset::FixedUpdate(const float _fDeltaTime)
	{
	}
	
	void ParentWithOffset::PostUpdate(void)
	{
	}

	void ParentWithOffset::GameObjectDestroy(void)
	{
	}

	void ParentWithOffset::Unload(void)
	{
	}

	void Dystopia::Behaviour::OnCollisionEnter(const CollisionEvent&)
	{

	}

	void Dystopia::Behaviour::OnCollisionStay(const CollisionEvent&)
	{

	}

	void Dystopia::Behaviour::OnCollisionExit(const CollisionEvent&)
	{

	}

	void Dystopia::Behaviour::OnTriggerEnter(const GameObject *)
	{
	}

	void Dystopia::Behaviour::OnTriggerStay(const GameObject *)
	{
	}

	void Dystopia::Behaviour::OnTriggerExit(const GameObject *)
	{
	}

	ParentWithOffset * ParentWithOffset::Duplicate() const
	{
		return new ParentWithOffset{};
	}

	void ParentWithOffset::Serialise(TextSerialiser& _ser) const
	{
	}

	void ParentWithOffset::Unserialise(TextSerialiser& _ser)
	{
	}


	const char * const ParentWithOffset::GetBehaviourName() const
	{
		return ParentWithOffset::BehaviourName;
	}

	void ParentWithOffset::EditorUI(void) noexcept
	{
		
		
	}
}




