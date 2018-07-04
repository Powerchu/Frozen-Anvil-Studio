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

#include "DataStructure\SharedPtr.h"
#include "DataStructure\AutoArray.h"

#include "System\SystemTypes.h"
#include "System\Base\Systems.h"
#include "System\Time\Timer.h"

#include "Utility\Meta.h"
#include "Utility\MetaAlgorithms.h"


namespace Dystopia
{
	class TimeSystem;
	class InputManager;
	class WindowManager;
	class SoundSystem;
	class GraphicsSystem;

	class EngineCore final  
	{
		using AllSys = Utility::Collection <
			Utility::Indexer<eSYSTEMS::TIME_SYSTEM    , TimeSystem     >,
			Utility::Indexer<eSYSTEMS::INPUT_SYSTEM   , InputManager   >,
			Utility::Indexer<eSYSTEMS::WINDOW_SYSTEM  , WindowManager  >,
			Utility::Indexer<eSYSTEMS::SOUND_SYSTEM   , SoundSystem    >,
			Utility::Indexer<eSYSTEMS::GRAPHIC_SYSTEM , GraphicsSystem >
		>;

	public:

		static SharedPtr<EngineCore> GetInstance(void);

		template <class T>
		T* const GetSystem(void) const;

		template <class T>
		T* const GetSubSystem(void) const;

		void Init(void);
		void Update(void);

	private:

		Timer mTime;
		AutoArray<Systems*> SystemList;
		AutoArray<Systems*> SystemTable;

		EngineCore(void);
	};

}



template<class T>
inline T* const Dystopia::EngineCore::GetSystem(void) const
{
	return static_cast<T*const>(SystemTable[Utility::Find_t<T, AllSys>::value]);
}

template<class T>
inline T* const Dystopia::EngineCore::GetSubSystem(void) const
{
	return nullptr;
}



#endif		// INCLUDE GUARD

