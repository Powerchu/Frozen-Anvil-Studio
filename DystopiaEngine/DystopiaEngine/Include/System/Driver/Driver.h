/* HEADER *********************************************************************************/
/*!
\file	Driver.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Internal part of Game Engine that runs everything

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _ENGINE_DRIVER_H_
#define _ENGINE_DRIVER_H_

#include "DataStructure/AutoArray.h"
#include "DataStructure/Queue.h"

#include "System/SystemTypes.h"
#include "System/Base/Systems.h"
#include "System/Time/Timer.h"

#include "Utility/Meta.h"
#include "Utility/MetaAlgorithms.h"
#include "Globals.h"


namespace Dystopia
{
	class _DLL_EXPORT EngineCore final
	{
	public:
		using AllSys = Ut::MetaSortT_t <Ut::MetaLessThan, Ut::Collection <
			Ut::Indexer<eSYSTEMS::BEHAVIOUR_SYSTEM, class BehaviourSystem>,
			Ut::Indexer<eSYSTEMS::TIME_SYSTEM     , class TimeSystem     >,
			Ut::Indexer<eSYSTEMS::INPUT_SYSTEM    , class InputManager   >,
			Ut::Indexer<eSYSTEMS::WINDOW_SYSTEM   , class WindowManager  >,
			Ut::Indexer<eSYSTEMS::SOUND_SYSTEM    , class SoundSystem    >, 
			Ut::Indexer<eSYSTEMS::SCENE_SYSTEM    , class SceneSystem    >,
			Ut::Indexer<eSYSTEMS::CAMERA_SYSTEM   , class CameraSystem   >,
			Ut::Indexer<eSYSTEMS::GRAPHIC_SYSTEM  , class GraphicsSystem >,
			Ut::Indexer<eSYSTEMS::EVENT_SYSTEM    , class EventSystem    >,
			Ut::Indexer<eSYSTEMS::COLLISION_SYSTEM, class CollisionSystem>,
			Ut::Indexer<eSYSTEMS::PHYSICS_SYSTEM  , class PhysicsSystem  >,
			Ut::Indexer<eSYSTEMS::PROFILER_SYSTEM , class Profiler       >
		>>;

		using SubSys = typename Ut::MetaAutoIndexer <
			class MeshSystem,
			class FileSystem,
			class FontSystem,
			class LoggerSystem,
			class TextureSystem
		>::result;

		static EngineCore* GetInstance(void) noexcept;

		~EngineCore(void) = default;

		template <class T>
		T* const Get(void) const;

		template <class T>
		T* const GetSystem(void) const;

		template <unsigned N, class T = typename Ut::MetaExtract_t<N, AllSys>::type>
		T* const GetSystem(void) const;

		template <class T>
		T* const GetSubSystem(void) const;

		void BroadcastMessage(const eSysMessage&);

		void LoadSettings(void);
		void Init(void);
		void FixedUpdate(void);
		void Update(void);
		void Shutdown(void);

		void Interrupt(void);
		void InterruptContinue(void);

		void PostUpdate(void);

	private:

		Timer mTime;
		Timer mTimeFixed;

		Queue<eSysMessage> mMessageQueue;

		AutoArray<void*>	mSubSystems;
		AutoArray<Systems*> mSystemList;
		AutoArray<Systems*> mSystemTable;

		std::int64_t mAccumulatedTime;

		EngineCore(void);

		void SendMessage(void);
		void ParseMessage(const eSysMessage&);
	};

}






// ============================================ FUNCTION DEFINITIONS ============================================ // 


template <class T>
inline T* const Dystopia::EngineCore::Get(void) const
{
	if constexpr (Ut::MetaFind<T, AllSys>::value)
	{
		return GetSystem<T>();
	}
	else
	{
		return GetSubSystem<T>();
	}
}

template <class T>
inline T* const Dystopia::EngineCore::GetSystem(void) const
{
	using type = Ut::MetaFind<T, AllSys>;
	static_assert(type::value, "Error: System not found!");

	return static_cast<T*>(mSystemTable[type::result::value]);
}

template <unsigned _N, class T>
inline T* const Dystopia::EngineCore::GetSystem(void) const
{
	return static_cast<T*>(mSystemTable[_N]);
}

template <class T>
inline T* const Dystopia::EngineCore::GetSubSystem(void) const
{
	using type = Ut::MetaFind<T, SubSys>;
	static_assert(type::value, "Error: Sub-System not found!");

	return static_cast<T*>(mSubSystems[type::result::value]);
}



#endif		// INCLUDE GUARD

