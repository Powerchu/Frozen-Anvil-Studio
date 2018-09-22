/* HEADER *********************************************************************************/
/*!
\file	creating.cpp
\author shannon (100%)
\par    email: tshannon\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "creating.h"
#include "Math\Vector2.h"
#include "System\Input\InputSystem.h"
#include "System\Input\InputMap.h"
#include "System\Driver\Driver.h"

namespace Dystopia
{
	creating::creating()
	{
	}

	creating::~creating()
	{
	}

	void creating::Load()
	{
	}

	void creating::Init()
	{
	}

	void creating::Update(const float _fDeltaTime)
	{
	}

	void creating::FixedUpdate(const float _fDeltaTime)
	{
	}
	
	void creating::PostUpdate(void)
	{
	}

	void creating::GameObjectDestroy(void)
	{
	}

	void creating::Unload(void)
	{
	}

	creating * creating::Duplicate() const
	{
		return nullptr;
	}

	void creating::Serialise(TextSerialiser& _ser) const
	{
	}

	void creating::Unserialise(TextSerialiser& _ser)
	{
	}


	const char * const creating::GetBehaviourName() const
	{
		return creating::BehaviourName;
	}

	

}




