/* HEADER *********************************************************************************/
/*!
\file	LoggerSystem.cpp
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Implmentation of LoggerSystem

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "System\Logger\LoggerSystem.h"

#include <cstdlib>
#include <iostream>
#include <exception>
#pragma warning(push)
#pragma warning(disable : 4996)

namespace
{
	static bool bAbnormalExit = true;

	[[noreturn]] void ProgramTerminate(void)
	{
		if (bAbnormalExit)
		{
			FILE* log = fopen("CrashReport.log", "w+");

			fprintf(log, "lolwhat2\n");

			fclose(log);
		}

		std::abort();
	}
}


Dystopia::LoggerSystem::LoggerSystem(void) noexcept
{
	std::set_terminate(ProgramTerminate);
}

Dystopia::LoggerSystem::~LoggerSystem(void) noexcept
{
	bAbnormalExit = false;
}



#pragma warning(pop)

