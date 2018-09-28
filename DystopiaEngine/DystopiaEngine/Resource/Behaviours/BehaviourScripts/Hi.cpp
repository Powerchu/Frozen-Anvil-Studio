/* HEADER *********************************************************************************/
/*!
\file	Hi.cpp
\author Keith (100%)
\par    email: keith.goh\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "Hi.h"
#include "Math\Vector2.h"
#include "System\Input\InputSystem.h"
#include "System\Input\InputMap.h"
#include "System\Driver\Driver.h"
#include "Editor\ConsoleLog.h"
namespace Dystopia
{
	Hi::Hi()
	{
	}

	Hi::~Hi()
	{
	}

	void Hi::Load()
	{
	}

	void Hi::Init()
	{
	}

	void Hi::Update(const float _fDeltaTime)
	{
        PrintToConsoleLog("Test_1");
	}

	void Hi::FixedUpdate(const float _fDeltaTime)
	{
	}
	
	void Hi::PostUpdate(void)
	{
	}

	void Hi::GameObjectDestroy(void)
	{
	}

	void Hi::Unload(void)
	{
	}

	Hi * Hi::Duplicate() const
	{
		return new Hi{};
	}

	void Hi::Serialise(TextSerialiser& _ser) const
	{
	}

	void Hi::Unserialise(TextSerialiser& _ser)
	{
	}


	const char * const Hi::GetBehaviourName() const
	{
		return Hi::BehaviourName;
	}

	

}




