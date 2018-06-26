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


namespace Dystopia
{
	class EngineCore final  
	{
	public:

		template <class T>
		T* GetSystem(void) const;

	private:
		EngineCore(void);
	};

}



template<class T>
inline T* Dystopia::EngineCore::GetSystem(void) const
{
	return nullptr;
}



#endif		// INCLUDE GUARD

