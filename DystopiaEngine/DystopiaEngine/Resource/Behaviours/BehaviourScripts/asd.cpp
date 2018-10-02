/* HEADER *********************************************************************************/
/*!
\file	asd.cpp
\author asd (100%)
\par    email: asd\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "asd.h"
#include "Math/Vector2.h"
#include "System/Input/InputSystem.h"
#include "System/Input/InputMap.h"
#include "System/Driver/Driver.h"
#include "Editor/EGUI.h"
#include "Utility/DebugAssert.h"

namespace Dystopia
{
	asd::asd()
	{
	}

	asd::~asd()
	{
	}

	void asd::Load()
	{
	}

	void asd::Init()
	{
	}

	void asd::Update(const float _fDeltaTime)
	{
	}

	void asd::FixedUpdate(const float _fDeltaTime)
	{
	}
	
	void asd::PostUpdate(void)
	{
	}

	void asd::GameObjectDestroy(void)
	{
	}

	void asd::Unload(void)
	{
	}

	asd * asd::Duplicate() const
	{
		return new asd{};
	}

	void asd::Serialise(TextSerialiser& _ser) const
	{
	}

	void asd::Unserialise(TextSerialiser& _ser)
	{
	}


	const char * const asd::GetBehaviourName() const
	{
		return asd::BehaviourName;
	}

	void asd::EditorUI(void) noexcept
	{
		
		
	}
}




