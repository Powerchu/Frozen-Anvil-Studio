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

#pragma warning(push)
#pragma warning(disable : 4251)

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
			Ut::Indexer<eSYSTEMS::PROFILER_SYSTEM , class Profiler       >,
			Ut::Indexer<eSYSTEMS::AI_SYSTEM		  , class AISystem		 >,
			Ut::Indexer<eSYSTEMS::INTERMEDIARY_SYSTEM, class EditorIntermediary	>,
			Ut::Indexer<eSYSTEMS::DATABASE_SYSTEM, class DatabaseSystem	>
			Ut::Indexer<eSYSTEMS::FILE_SYSTEM     , class FileSystem     >
		>>;

		using SubSys = typename Ut::MetaAutoIndexer <
			class MeshSystem,
			class FontSystem,
			class LoggerSystem,
			class TextureSystem,
			class TagSystem,
			class Factory
		>::result;

		static EngineCore* GetInstance(void) noexcept;

		~EngineCore(void) = default;

		template <class T>
		T* const GetSystem(void) const;

		template <unsigned N, class T = typename Ut::MetaExtract_t<N, AllSys>::type>
		T* const GetSystem(void) const;

		template <class T>
		T* const GetSubSystem(void) const;

		template <class T>
		static inline T* const Get(void);

		void BroadcastMessage(const eSysMessage&, size_t _nParam);

		void LoadSettings(void);
		void PreInit(void);
		void Init(void);
		void PostInit(void);
		void FixedUpdate(void);
		void Update(void);
		void Shutdown(void);

		void Interrupt(void);
		void InterruptContinue(void);

		void PostUpdate(void);

		void Quit();
		bool GetQuitState() const;

		void ExecuteGame();

	private:

		struct SysMsg
		{
			eSysMessage mMsg;
			size_t mParam;
		};

		Queue<SysMsg> mMessageQueue;
		bool mbQuit;

		AutoArray<void*>	mSubSystems;
		AutoArray<Systems*> mSystemList;
		AutoArray<Systems*> mSystemTable;

		EngineCore(void);

		void SendMessage(void);
		void ParseMessage(const eSysMessage&, size_t);
	};

}






// ============================================ FUNCTION DEFINITIONS ============================================ // 


template <class T>
inline T* const Dystopia::EngineCore::Get(void)
{
	if constexpr (Ut::MetaFind<T, AllSys>::value)
	{
		return GetInstance()->GetSystem<T>();
	}
	else
	{
		return GetInstance()->GetSubSystem<T>();
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



#pragma warning(pop)
#endif		// INCLUDE GUARD

