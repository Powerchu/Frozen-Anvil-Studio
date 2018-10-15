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

#include "System/Time/TimeDefs.h"


namespace Dystopia
{
	struct ScopedTimerDefaultAction
	{
		using Precision_t = std::chrono::milliseconds;

		static void PostDuration(Precision_t);
	};

	template <typename Ty = ScopedTimerDefaultAction>
	class ScopedTimer
	{
	public:

		template <typename ... Ts>
		explicit ScopedTimer(Ts&& ...);

		~ScopedTimer(void) noexcept;


	private:

		Ty ActionFunctor;
		TimePoint_t mMarker;

		ScopedTimer(ScopedTimer&&) = delete;
		ScopedTimer(const ScopedTimer&) = delete;
		ScopedTimer& operator = (ScopedTimer&&) = delete;
		ScopedTimer& operator = (const ScopedTimer&&) = delete;
	};
}






// ============================================ FUNCTION DEFINITIONS ============================================ // 


template <typename Ty> template <typename ... Ts>
inline Dystopia::ScopedTimer<Ty>::ScopedTimer(Ts&& ... Args)
	: mMarker{ Clock::now() }, ActionFunctor{ Ut::Forward<Ts>(Args) ... }
{

}

template<typename Ty>
inline Dystopia::ScopedTimer<Ty>::~ScopedTimer(void) noexcept
{
	ActionFunctor.PostDuration(
		std::chrono::duration_cast<typename Ty::Precision_t>(
			Clock::now() - mMarker
	));
}



#endif		// INCLUDE GUARD

