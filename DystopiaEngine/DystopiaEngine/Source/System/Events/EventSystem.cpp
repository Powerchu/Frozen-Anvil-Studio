/* HEADER *********************************************************************************/
/*!
\file	EventSystem.cpp
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "System\Events\EventSystem.h"

EventID EventHash(const char* _eventName)
{
	EventID hash = OFFSET_BASIS;
	size_t len = strlen(_eventName) + 1;
	for (size_t i = 0; i < len; ++i)
	{
		hash ^= *_eventName++;
		hash *= FNV_PRIME;
	}
	return hash;
}

/*********************************************************** Event Callback ***********************************************************/

/*********************************************************** Event Data ***********************************************************/

/*********************************************************** Event System ***********************************************************/
