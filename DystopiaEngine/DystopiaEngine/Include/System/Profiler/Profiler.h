/* HEADER *********************************************************************************/
/*!
\file	Profiler.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Profiler System

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _PROFILER_SYS_H_
#define _PROFILER_SYS_H_

// Temporary std lib for fast prototyping
#include <map>
#include <chrono>


namespace Dystopia
{
	struct ProfileInfo
	{
		std::chrono::milliseconds mFixedUpdate;
		std::chrono::milliseconds mUpdate;
		std::chrono::milliseconds mPostUpdate;
	};

	class Profiler
	{
	public:

		const std::map<std::string, ProfileInfo>& GetInfo(void) const;

	private:


		std::map<std::string, ProfileInfo> mData;
	};
}



#endif		// INCLUDE GUARD

