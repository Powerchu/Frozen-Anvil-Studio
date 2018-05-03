/* HEADER *********************************************************************************/
/*!
\file	Timer.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Generic Timer

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _TIMER_H_
#define _TIMER_H_

#include <chrono>

namespace Dystopia
{
	// Because the whole thing is a pain to type
	using TimePoint_t = std::chrono::time_point<std::chrono::high_resolution_clock>;

	class Timer
	{
	public:
		/*! =================================================================
		\brief
			Default constructor
		=================================================================  */
		Timer(void);

		/*! =================================================================
		\brief
			Constructor for a count down Timer
		=================================================================  */
		explicit Timer(float _fSeconds);

		/*! =================================================================
		\brief
			Resets Elapsed to zero.

		\warning
			Cannot be used in conjunction with Countdown mode.

		\return void
			No return value.
		=================================================================  */
		void Lap(void);

		/*! =================================================================
		\brief
			Gets the time since the last lap

		\warning
			If timer is in countdown mode, 
			return value is the negative of Remaining.

		\return float
			Time since the last lap in seconds.
		=================================================================  */
		float Elapsed(void) const;
		
		/*! =================================================================
		\brief
			Starts a new countdown starting from the time this function
			is called.

		\warning
			Cannot be used in conjunction with Lap mode.

		\param _fSeconds
			The duration to count down.

		\return void
			No return value.
		=================================================================  */
		void Countdown(float _fSeconds);
		
		/*! =================================================================
		\brief
			Gets remaining duration of the countdown

		\warning
			If timer is in Lap mode, 
			return value is the negative of Elapsed.

		\return float
			-- If -- 
			Countdown has yet to be over, returns a positive
			number representing the remaining time in seconds.
			-- ELSE --
			Countdown is over, returns a negative number
			representing how long ago the countdown ended in seconds.
		=================================================================  */
		float Remaining(void) const;
		
		/*! =================================================================
		\brief
			Checks if the count down has ended.

		\warning
			If timer is in Lap mode, this will always return true.

		\return bool
			Returns true if count down has ended. 
			Returns false otherwise.
		=================================================================  */
		bool Complete(void) const;

		
		/*! =================================================================
		\brief
			Gets the current anchor time point.

		\warning
			If you have no idea what this is, please don't use it

		\return TimePoint_t
			Returns the internal time anchor.
		=================================================================  */
		TimePoint_t Time(void) const;

	private:
		TimePoint_t mCurrent;

		TimePoint_t Future(float) const;
	};
}



#endif		// INCLUDE GUARD

