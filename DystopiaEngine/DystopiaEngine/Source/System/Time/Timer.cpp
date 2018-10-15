/* HEADER *********************************************************************************/
/*!
\file	Timer.cpp
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Implementation of the Generic timer class

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "System/Time/Timer.h"	  // File Header
#include "System/Time/TimeDefs.h"
#include "Math/MathUtility.h"	  // epsilon

namespace Dystopia
{
	Timer::Timer(void) :
		mCurrent{ Clock::now() }
	{

	}

	Timer::Timer(float _fSeconds) :
		mCurrent{ Future(_fSeconds) }
	{

	}

	void Timer::Lap(void)
	{
		mCurrent = Clock::now();
	}

	float Timer::Elapsed(void) const
	{
		return std::chrono::duration_cast<Duration>(Clock::now() - mCurrent).count();
	}

	void Timer::Countdown(float _fSeconds)
	{
		mCurrent = Future(_fSeconds);  
	}

	float Timer::Remaining(void) const
	{
		return -Elapsed();
	}

	bool Timer::Complete(void) const
	{
		return Remaining() < Math::epsilon;
	}

	TimePoint_t Timer::Time(void) const
	{
		return mCurrent;
	}

	TimePoint_t Timer::Future(float _fSeconds) const
	{
		return Clock::now() + std::chrono::duration_cast<Clock::duration>(Duration(_fSeconds));
	}
}

