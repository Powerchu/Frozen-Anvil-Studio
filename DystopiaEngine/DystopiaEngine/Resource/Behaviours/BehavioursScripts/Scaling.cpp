/* HEADER *********************************************************************************/
/*!
\file	Scaling.cpp
\author dan (100%)
\par    email: dan\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "Scaling.h"
#include "Math/Vector2.h"
#include "System/Input/InputSystem.h"
#include "System/Input/InputMap.h"
#include "System/Driver/Driver.h"
#include "Editor/EGUI.h"
#include "Utility/DebugAssert.h"

namespace Dystopia
{
	Scaling::Scaling()
	{
	}

	Scaling::~Scaling()
	{
	}

	void Scaling::Load()
	{
	}

	void Scaling::Init()
	{
	}

	void Scaling::Update(const float _fDeltaTime)
	{
		GetOwner()->GetComponent<Transform>()->SetScale(1000, 1000, 0);
	}

	void Scaling::FixedUpdate(const float _fDeltaTime)
	{
	}
	
	void Scaling::PostUpdate(void)
	{
	}

	void Scaling::GameObjectDestroy(void)
	{
	}

	void Scaling::Unload(void)
	{
	}

	Scaling * Scaling::Duplicate() const
	{
		return new Scaling{};
	}

	void Scaling::Serialise(TextSerialiser& _ser) const
	{
	}

	void Scaling::Unserialise(TextSerialiser& _ser)
	{
	}


	const char * const Scaling::GetBehaviourName() const
	{
		return Scaling::BehaviourName;
	}

	void Scaling::EditorUI(void) noexcept
	{
		
		
	}
}




