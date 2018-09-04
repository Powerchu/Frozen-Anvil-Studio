/* HEADER *********************************************************************************/
/*!
\file	ScopedTimer.cpp
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Timer that counts the duration from when it was created till it is destroyed

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "System\Time\ScopedTimer.h"
#include "System\Time\TimeDefs.h"
#include "System\Time\TimeSystem.h"
#include "System\Driver\Driver.h"

#include <iostream>


void Dystopia::ScopedTimerDefaultAction::PostDuration(Dur_t _dur)
{
	//std::cout << _dur.count() << std::endl;
}


