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

#include "DataStructure\AutoArray.h"

#include "System\SystemTypes.h"
#include "System\Base\Systems.h"
#include "System\Time\Timer.h"

#include "Utility\Meta.h"
#include "Utility\MetaAlgorithms.h"



namespace Dystopia
{
	class EngineCore final  
	{
	public:
		using AllSys = Utility::MetaSortT_t <Utility::MetaLessThan, Utility::Collection <
			Utility::Indexer<eSYSTEMS::TIME_SYSTEM     , class TimeSystem     >,
			Utility::Indexer<eSYSTEMS::INPUT_SYSTEM    , class InputManager   >,
			Utility::Indexer<eSYSTEMS::WINDOW_SYSTEM   , class WindowManager  >,
//			Utility::Indexer<eSYSTEMS::SOUND_SYSTEM    , class SoundSystem    >,
			Utility::Indexer<eSYSTEMS::SCENE_SYSTEM    , class SceneSystem    >,
			Utility::Indexer<eSYSTEMS::CAMERA_SYSTEM   , class CameraSystem   >,
			Utility::Indexer<eSYSTEMS::GRAPHIC_SYSTEM  , class GraphicsSystem >,
			Utility::Indexer<eSYSTEMS::EVENT_SYSTEM    , class EventSystem    >,
			Utility::Indexer<eSYSTEMS::COLLISION_SYSTEM, class CollisionSystem>,
			Utility::Indexer<eSYSTEMS::PHYSICS_SYSTEM  , class PhysicsSystem  >
		>>;

		using SubSys = typename Utility::MetaAutoIndexer <
			class MeshSystem
		>::result;

		static EngineCore* GetInstance(void) noexcept;

		~EngineCore(void) = default;

		template <class T>
		T* const GetSystem(void) const;

		template <unsigned N, class T = typename Utility::MetaExtract_t<N, AllSys>::type>
		T* const GetSystem(void) const;

		template <class T>
		T* const GetSubSystem(void) const;

		void LoadSettings(void);
		void Init(void);
		void FixedUpdate(void);
		void Update(void);
		void Shutdown(void);

	private:

		Timer mTime;
		Timer mTimeFixed;
		AutoArray<Systems*> mSystemList;
		AutoArray<Systems*> mSystemTable;
		AutoArray<void*>	mSubSystems;

		EngineCore(void);
	};

}






// ============================================ FUNCTION DEFINITIONS ============================================ // 


template<class T>
inline T* const Dystopia::EngineCore::GetSystem(void) const
{
	using type = Utility::MetaFind<T, AllSys>;
	static_assert(type::value, "Error: System not found!");

	return static_cast<T*>(mSystemTable[type::result::value]);
}

template<unsigned _N, class T>
inline T* const Dystopia::EngineCore::GetSystem(void) const
{
	return static_cast<T*>(mSystemTable[_N]);
}

template<class T>
inline T* const Dystopia::EngineCore::GetSubSystem(void) const
{
	using type = Utility::MetaFind<T, SubSys>;
	static_assert(type::value, "Error: Sub-System not found!");

	return static_cast<T*>(mSubSystems[type::result::value]);
}



#endif		// INCLUDE GUARD

