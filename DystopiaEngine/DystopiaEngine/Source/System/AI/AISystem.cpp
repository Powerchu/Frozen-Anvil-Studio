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
			cont.Update(_dt);
		}
	}

	void AISystem::PostUpdate()
	{
	}

	void AISystem::Shutdown()
	{
	}

	void AISystem::LoadDefaults()
	{
	}

	void AISystem::LoadSettings(DysSerialiser_t& _in)
	{
	}

	void AISystem::SaveSettings(DysSerialiser_t& _out)
	{
	}

	void AISystem::ReceiveMessage(const eSysMessage& _msg)
	{

	}

	void AISystem::EditorUI()
	{

	}

	AISystem::~AISystem()
	{
	}
}


