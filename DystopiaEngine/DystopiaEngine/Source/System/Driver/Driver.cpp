/* HEADER *********************************************************************************/
/*!
\file	Driver.cpp
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Internal part of Game Engine that runs everything

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "System/Driver/Driver.h"

#include "Globals.h"
#include "IO/TextSerialiser.h"
#include "Utility/MetaAlgorithms.h"
#include "Utility/MetaDataStructures.h"
#include "DataStructure/Array.h"
#include "DataStructure/Queue.h"
#include "DataStructure/AutoArray.h"
#include "DataStructure/HashString.h"
#include "Allocator/DefaultAlloc.h"

// Systems
#include "System/Time/TimeSystem.h"
#include "System/Scene/SceneSystem.h"
#include "System/Input/InputSystem.h"
#include "System/Sound/SoundSystem.h"
#include "System/Graphics/GraphicsSystem.h"
#include "System/Window/WindowManager.h"
#include "System/Collision/CollisionSystem.h"
#include "System/Physics/PhysicsSystem.h"
#include "System/Camera/CameraSystem.h"
#include "System/Events/EventSystem.h"
#include "System/Profiler/Profiler.h"
#include "System/Behaviour/BehaviourSystem.h"

// SubSystems
#include "System/Graphics/MeshSystem.h"
#include "System/Graphics/FontSystem.h"
#include "System/Graphics/TextureSystem.h"
#include "System/File/FileSystem.h"
#include "System/Logger/LoggerSystem.h"

#include "System/Time/Timer.h"
#include "System/Time/ScopedTimer.h"

// STL Includes
#include <chrono>
#include <string>
#include <filesystem>


#define SETTINGS_DIR  eFileDir::eCurrent
#define SETTINGS_FILE "Settings.dyst"



namespace
{
	template <typename Ty, typename ... T>
	AutoArray<Ty> MakeAutoArray(Ut::TypeList<T...>)
	{
		 return AutoArray<Ty>{ static_cast<Ty>(Dystopia::DefaultAllocator<T>::ConstructAlloc())...};
	}

	template <typename ... T>
	void DeleteSubSys(AutoArray<void*>& _SubSys, Ut::TypeList<T...>)
	{
		void(*deleters[])(void*) {
			[] (void* _p)  { Dystopia::DefaultAllocator<T>::DestructFree(static_cast<T*>(_p));  }...
		};

		auto b = _SubSys.begin();
		for (auto* deleter : deleters)
		{
			deleter(*b);
			++b;
		}
	}

	template <typename T>
	struct ErrorOnDuplicate;

	template <
		template <typename ...> class Set,
		template <unsigned, typename> class Holder,
		typename ... Tys, unsigned ... Ns,
		typename Ty1, typename Ty2, unsigned N1, unsigned N2>
	struct ErrorOnDuplicate <Set<Holder<N1, Ty1>, Holder<N2, Ty2>, Holder<Ns, Tys>...>>
	{
		static_assert(!(N1 == N2), "System Error: Systems cannot have the same index! (Same enum?).");

		using eval = typename ErrorOnDuplicate<Set<Holder<N2, Ty2>, Holder<Ns, Tys>...>>::eval;
	};

	template <
		template <typename ...> class Set,
		template <unsigned, typename> class Holder,
		typename Ty1, typename Ty2, unsigned N1, unsigned N2>
	struct ErrorOnDuplicate <Set<Holder<N1, Ty1>, Holder<N2, Ty2>>>
	{
		static_assert(!(N1 == N2), "System Error: Systems cannot have the same index! (Same enum?).");

		using eval = void;
	};
}


Dystopia::EngineCore* Dystopia::EngineCore::GetInstance(void) noexcept
{
	static EngineCore oInstance{ };
	return &oInstance;
}

Dystopia::EngineCore::EngineCore(void) :
	mMessageQueue{ 60 }, mSystemList{ Ut::SizeofList<AllSys>::value },
	mSubSystems { MakeAutoArray<void*>(Ut::MakeTypeList_t<Ut::TypeList, SubSys>{}) },
	mSystemTable{ MakeAutoArray<Systems*>(Ut::MakeTypeList_t<Ut::TypeList, AllSys>{}) }
{
	using SanityCheck = typename ErrorOnDuplicate<AllSys>::eval;
}


void Dystopia::EngineCore::LoadSettings(void)
{
	if (GetSubSystem<FileSystem>()->CheckFileExist(SETTINGS_FILE, SETTINGS_DIR))
	{
		auto file = Serialiser::OpenFile<TextSerialiser>(
			GetSubSystem<FileSystem>()->GetProjectFolders<std::string>(SETTINGS_DIR) +
			SETTINGS_FILE
		);

		HashString sentry;

		file >> sentry;
		file.ConsumeStartBlock();

		// Check if the file is okay
		if (!file.EndOfInput() && (sentry == "SENTRY"))
		{
			for (auto& e : mSystemTable)
			{
				file.ConsumeStartBlock();
				e->LoadSettings(file);
				file.ConsumeEndBlock();
			}

			return;
		}
	}

	for (auto& e : mSystemTable)
		e->LoadDefaults();
}

void Dystopia::EngineCore::PreInit(void)
{
	for (auto& e : mSystemTable)
		e->PreInit();
}

void Dystopia::EngineCore::Init(void)
{
	for (auto& e : mSystemTable)
	{
		if (e->Init())
		{
			mSystemList.EmplaceBack(e);
		}
		else
		{
			delete e;
			e = nullptr;
		}
	}
}

void Dystopia::EngineCore::PostInit(void)
{
	for (auto& e : mSystemList)
		e->PostInit();
}

void Dystopia::EngineCore::Interrupt(void)
{
	GetSystem<TimeSystem>()->StopTime();
}

void Dystopia::EngineCore::InterruptContinue(void)
{
	GetSystem<TimeSystem>()->ResumeTime();
}

void Dystopia::EngineCore::FixedUpdate(void)
{
	auto TimeSys = GetSystem<TimeSystem>();
	auto FixedDT = TimeSys->GetFixedDeltaTime();

	while (TimeSys->ConsumeFixedDT())
	{
		for (auto& e : mSystemList)
		{
			e->PreFixedUpdate(FixedDT);
		}
		for (auto& e : mSystemList)
		{
			e->FixedUpdate(FixedDT);
		}
	}
}

void Dystopia::EngineCore::Update(void)
{
	auto TimeSys = GetSystem<TimeSystem>();
	TimeSys->AdvanceTime();

	float dt = TimeSys->GetDeltaTime();
	for (auto& e : mSystemList)
	{
		e->Update(dt);
	}
}

void Dystopia::EngineCore::PostUpdate(void)
{
	for (auto& e : mSystemList)
	{
		e->PostUpdate();
	}
}

void Dystopia::EngineCore::Shutdown(void)
{
	GetSubSystem<FileSystem>()->CreateFiles(SETTINGS_FILE, SETTINGS_DIR);
	auto s = Serialiser::OpenFile<DysSerialiser_t>(
		GetSubSystem<FileSystem>()->GetProjectFolders<std::string>(SETTINGS_DIR) +
		SETTINGS_FILE,
		DysSerialiser_t::MODE_WRITE
	);

	s << "SENTRY";
	s.InsertStartBlock("SETTINGS");

	for (auto& e : mSystemList)
	{
		s.InsertStartBlock("SYSTEM");
		e->SaveSettings(s);
		s.InsertEndBlock("SYSTEM");
		e->Shutdown();
	}

	for (auto& e : mSystemList)
		DefaultAllocator<Systems>::DestructFree(e);

	DeleteSubSys(mSubSystems, Ut::MakeTypeList_t<Ut::TypeList, SubSys>{});

	mSystemList.clear();
	mSystemTable.clear();
}


void Dystopia::EngineCore::BroadcastMessage(const eSysMessage& _Message)
{
	mMessageQueue.Insert(_Message);
}

void Dystopia::EngineCore::SendMessage(void)
{
	for (auto& m : mMessageQueue)
	{
		ParseMessage(m);

		for (auto& e : mSystemList)
			e->ReceiveMessage(m);
	}

	mMessageQueue.clear();
}

void Dystopia::EngineCore::ParseMessage(const eSysMessage&)
{
}


#if !EDITOR

int WinMain(HINSTANCE, HINSTANCE, char *, int)
{
	auto CORE = Dystopia::EngineCore::GetInstance();

	/*
	CORE->ExecuteGame();
	*/

	return 0;
}

#endif


