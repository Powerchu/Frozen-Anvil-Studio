/* HEADER *********************************************************************************/
/*!
\file	GUID.cpp
\author Digipen (100%)
\par    email: keith.goh\@digipen.edu
\brief
	The Base class for generation of unique IDs.
	IDs are generated based on time stamps


All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "Utility/GUID.h"
#include <chrono>
namespace Dystopia
{
	GUID_t GUIDGenerator::GetUniqueID()
	{
		using TimeUnit = std::chrono::milliseconds;
		/*The number of times this function is called within 1 ms*/
		static unsigned long count = 0;
		/*currTime will be updated everytime this function is called*/
		/*start time will be update when the duration between each GetUniqueID call is more than 1 ms*/
		std::chrono::duration<double> start = std::chrono::duration_cast<TimeUnit>(std::chrono::system_clock::now().time_since_epoch());

		/*Update the current time*/
		const std::chrono::duration<double> currTime = std::chrono::duration_cast<TimeUnit>(std::chrono::system_clock::now().time_since_epoch());
		
		/*If the current time of the function called is within 1ms of the last function call*/
		if ( !std::chrono::duration_cast<TimeUnit>(currTime - start).count() )
		{
			/*Shift the ID by 45 bits*/
			GUID_t ID = static_cast<long>(std::chrono::duration_cast<TimeUnit>(currTime).count()) << (NUM_EMPTY_BITS + NUM_MSCOUNT_BITS);
			/*Make the next 10 bits the number of time this function has been called.
			  Append the the 2 numbers together and return it as ID*/
			return static_cast<GUID_t>(ID | (count++ << NUM_EMPTY_BITS));
		}
		/*Reset the count*/
		count = 0;
		/*Update the time of last function call*/
		start = std::chrono::duration_cast<TimeUnit>(currTime);
		/*Return the ID*/
		return static_cast<GUID_t>(std::chrono::duration_cast<TimeUnit>(currTime).count()) << (NUM_EMPTY_BITS + NUM_MSCOUNT_BITS);
	}
}