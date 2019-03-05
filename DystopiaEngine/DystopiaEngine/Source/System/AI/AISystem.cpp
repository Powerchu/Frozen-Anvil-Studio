/* HEADER *********************************************************************************/
/*!
\file	AISystem.cpp
\author Aaron Chu (100%)
\par    email: m.chu\@digipen.edu
\brief


All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "System/AI/AISystem.h"
#include "System/Profiler/ProfilerAction.h"
#include "System/Time/ScopedTimer.h"
#include "Component/AiController.h"

namespace Dystopia
{
	AISystem::AISystem()
	{

	}

	void AISystem::PreInit()
	{
	}

	bool AISystem::Init()
	{
		return true;
	}

	void AISystem::PostInit()
	{
	}

	void AISystem::PreFixedUpdate(float)
	{
	}

	void AISystem::FixedUpdate(float)
	{
	}

	void AISystem::Update(float _dt)
	{
		ScopedTimer<ProfilerAction> timeKeeper{ "AI System", "Update" };

		for (auto& cont : mComponents)
		{
			if (cont.GetOwner() == nullptr) continue;
#if EDITOR
			if (cont.GetOwner()->GetFlags() & eObjFlag::FLAG_EDITOR_OBJ 
				|| cont.GetFlags() & eObjFlag::FLAG_EDITOR_OBJ) continue;
#endif 
			cont.Update(_dt);
		}

	}

	void AISystem::PostUpdate()
	{
		for (auto& cont : mComponents)
		{
			if (nullptr == cont.GetOwner()) continue;

			if (cont.GetFlags() & FLAG_REMOVE)
			{
				cont.ClearTree();
				mComponents.Remove(&cont);
			}
		}
	}

	void AISystem::Shutdown()
	{
	}

	void AISystem::LoadDefaults()
	{
	}

	void AISystem::LoadSettings(DysSerialiser_t& )
	{
	}

	void AISystem::SaveSettings(DysSerialiser_t& )
	{
	}

	void AISystem::EditorUI()
	{

	}

	AISystem::~AISystem()
	{
	}
}


