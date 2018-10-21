/* HEADER *********************************************************************************/
/*!
\file	Chaser.cpp
\author Dan (100%)
\par    email: dan.kang\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "Chaser.h"
#include "Math/Vector2.h"
#include "System/Input/InputSystem.h"
#include "System/Input/InputMap.h"
#include "System/Driver/Driver.h"
#include "Editor/EGUI.h"
#include "Utility/DebugAssert.h"
#include "System/Logger/LoggerSystem.h"
#include "Object/GameObject.h"
#include "System/Scene/SceneSystem.h"
#include "System/Scene/Scene.h"

namespace Dystopia
{
	Chaser::Chaser()
	{
	}

	Chaser::~Chaser()
	{
	}

	void Chaser::Load()
	{
	}

	void Chaser::Init()
	{
		float testF = 10.0f;
		const auto ownerName = GetOwner()->GetName().c_str();
		LoggerSystem::ConsoleLog(eLog::MESSAGE, "gameobjectName: %s", ownerName);
	}

	void Chaser::Update(const float _fDeltaTime)
	{
	}

	void Chaser::FixedUpdate(const float _fDeltaTime)
	{
	}
	
	void Chaser::PostUpdate(void)
	{
	}

	void Chaser::GameObjectDestroy(void)
	{
	}

	void Chaser::Unload(void)
	{
	}

	Chaser * Chaser::Duplicate() const
	{
		return new Chaser{};
	}

	void Chaser::Serialise(TextSerialiser& _ser) const
	{
	}

	void Chaser::Unserialise(TextSerialiser& _ser)
	{
	}


	const char * const Chaser::GetBehaviourName() const
	{
		return Chaser::BehaviourName;
	}

	void Chaser::EditorUI(void) noexcept
	{
		
		
	}
}




