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


#define WIN32_LEAN_AND_MEAN					// Exclude rarely used stuff from Windows headers
#define NOMINMAX							// Disable window's min & max macros
#include <windows.h>						// Windows Header
#include <DbgHelp.h>
#include <cstdio>							// FILE, freopen_s
#include <cstdlib>
#include <iostream>
#include <exception>

#undef  WIN32_LEAN_AND_MEAN					// Stop defines from spilling into code
#undef  NOMINMAX
#pragma warning(push)
#pragma warning(disable : 4996)
#define STACKFRAMES 32

namespace
{
	[[noreturn]] void ProgramTerminate(void)
	{
		FILE* log;
		if (0 == fopen_s(&log, "CrashReport.log", "w+"))
		{
			SymSetOptions(SYMOPT_DEFERRED_LOADS | SYMOPT_UNDNAME);
			if (SymInitialize(GetCurrentProcess(), NULL, TRUE))
			{
				void* data[STACKFRAMES];
				int nFrames = CaptureStackBackTrace(2, STACKFRAMES, data, NULL);
				char strBuffer[1023 + sizeof(SYMBOL_INFO)]{};
				SYMBOL_INFO* symbol = reinterpret_cast<SYMBOL_INFO*>(strBuffer);
				DWORD64 disp;

				symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
				symbol->MaxNameLen = 1024;

				for (int n = 0; n < nFrames; ++n)
				{
					int x = SymFromAddr(GetCurrentProcess(), reinterpret_cast<DWORD64>(data[n]), &disp, symbol);

					if (!x)
					{
						x = GetLastError();
					}

					if (symbol->NameLen)
						fprintf(log, "%s\n", symbol->Name);
					else
						fprintf(log, "Function %p\n", data[n]);
				}

				SymCleanup(GetCurrentProcess());
			}

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

}



#pragma warning(pop)

