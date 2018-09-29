/* HEADER *********************************************************************************/
/*!
\file	vvv.cpp
\author v (100%)
\par    email: v\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "vvv.h"
#include "Math\Vector2.h"
#include "System\Input\InputSystem.h"
#include "System\Input\InputMap.h"
#include "System\Driver\Driver.h"
#include "Editor\ConsoleLog.h"
#include "Object\GameObject.h"
#include "Component\Transform.h"
namespace Dystopia
{
	vvv::vvv()
	{                    
	}     

	vvv::~vvv()
	{
	}

	void vvv::Load()
	{   
	}

	void vvv::Init()
	{
	}

	void vvv::Update(const float _fDeltaTime)
	{
        this->GetOwner()->GetComponent<Transform>()->SetPosition(1000,50,0);
	}

	void vvv::FixedUpdate(const float _fDeltaTime)
	{
	}
	
	void vvv::PostUpdate(void)
	{
	}

	void vvv::GameObjectDestroy(void)
	{
	}

	void vvv::Unload(void)
	{
	}

	vvv * vvv::Duplicate() const
	{
		return new vvv{};
	}

	void vvv::Serialise(TextSerialiser& _ser) const
	{
	}

	void vvv::Unserialise(TextSerialiser& _ser)
	{
	}


	const char * const vvv::GetBehaviourName() const
	{
		return vvv::BehaviourName;
	}

	

}




