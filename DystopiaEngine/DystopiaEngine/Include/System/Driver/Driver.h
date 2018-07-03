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



namespace Dystopia
{
	class EngineCore final  
	{
	public:

		static SharedPtr<EngineCore> GetInstance(void);

		template <class T>
		SharedPtr<T> GetSystem(void) const;
		template<unsigned eSYSTEMS>
		auto GetSystem(void) const;
		template <class T>
		T* GetSubSystem(void) const;

		void Init(void);

	private:

		AutoArray<Systems*> SystemList;
		AutoArray<Systems*> SystemTable;

		EngineCore(void);
	};

}



template<class T>
inline SharedPtr<T> Dystopia::EngineCore::GetSystem(void) const
{
	return SystemTable[0];
}

template<unsigned eSYSTEMS>
inline auto Dystopia::EngineCore::GetSystem(void) const
{
	return &SystemTable[eSYSTEMS];
}

template<class T>
inline T* Dystopia::EngineCore::GetSubSystem(void) const
{
	return nullptr;
}



#endif		// INCLUDE GUARD

