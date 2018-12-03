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


