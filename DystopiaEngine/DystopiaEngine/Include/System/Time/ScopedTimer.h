/* HEADER *********************************************************************************/
/*!
\file	ScopedTimer.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Timer that counts the duration from when it was created till it is destroyed

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _SCOPED_TIMER_H_
#define _SCOPED_TIMER_H_

#include "System\Time\TimeDefs.h"


namespace Dystopia
{
	struct ScopedTimerDefaultAction
	{
		using Dur_t = std::chrono::milliseconds;

		static void PostDuration(Dur_t);
	};

	template <typename Ty = ScopedTimerDefaultAction>
	class ScopedTimer
	{
	public:

		ScopedTimer(void) noexcept
			: mMarker{ Clock::now() }
		{

		}

		~ScopedTimer(void) noexcept
		{
			Ty::PostDuration(
				std::chrono::duration_cast<typename Ty::Dur_t>(
					Clock::now() - mMarker
			));
		}


	private:

		TimePoint_t mMarker;
	};
}



#endif		// INCLUDE GUARD

