/* HEADER *********************************************************************************/
/*!
\file	TimeDefs.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Contains the declaration for commonly used Time types

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _TIMEDEFS_H_
#define _TIMEDEFS_H_

#include <chrono>


namespace Dystopia
{
	using Clock = std::chrono::high_resolution_clock;
	using Duration = std::chrono::duration<float>;
	using TimePoint_t = std::chrono::time_point<std::chrono::high_resolution_clock>;
}



#endif		// INCLUDE GUARD

