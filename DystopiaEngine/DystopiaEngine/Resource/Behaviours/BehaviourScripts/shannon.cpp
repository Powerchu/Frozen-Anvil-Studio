/* HEADER *********************************************************************************/
/*!
\file	shannon.cpp
\author s (100%)
\par    email: s\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "shannon.h"
#include "Math\Vector2.h"
#include "System\Input\InputSystem.h"
#include "System\Input\InputMap.h"
#include "System\Driver\Driver.h"
#include "Object\GameObject.h"
#include "Component\Transform.h"
#include "Editor/ConsoleLog.h"
#include "Utility/DebugAssert.h"
#include <string>

namespace Dystopia
{
	shannon::shannon()
	{
	}

	shannon::~shannon()
	{
		
	}

	void shannon::Load()
	{
		 
	}

	void shannon::Init()
	{
	}

	void shannon::Update(const float _fDeltaTime)
	{
		//char ch[] = "SHANNON";
		//PrintToConsoleLog(std::string{ch});
		//auto x = this->GetOwner();
		//auto y = x->GetComponent<Transform>();
		//
		//DEBUG_PRINT(eLog::ERROR, "%p, %p, %d\n", x, y, sizeof(std::string));
		
		//this->GetOwner()->GetComponent<Transform>()->SetScale(Math::Vector4{50,50,0,1});
		//this->GetOwner()->GetComponent<Transform>()->SetPosition(Math::Vector4{150,150,0,1});
	}

	void shannon::FixedUpdate(const float _fDeltaTime)
	{
	}
	
	void shannon::PostUpdate(void)
	{
	}

	void shannon::GameObjectDestroy(void)
	{
	}

	void shannon::Unload(void)
	{
	}

	shannon * shannon::Duplicate() const
	{
		return new shannon{};
	}

	void shannon::Serialise(TextSerialiser& _ser) const
	{
	}

	void shannon::Unserialise(TextSerialiser& _ser)
	{
	}


	const char * const shannon::GetBehaviourName() const
	{
		return shannon::BehaviourName;
	}

	

}




