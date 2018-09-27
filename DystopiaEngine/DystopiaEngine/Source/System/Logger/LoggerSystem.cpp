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
#include "System\Driver\Driver.h"
#include "Editor\ConsoleLog.h"

#define WIN32_LEAN_AND_MEAN					// Exclude rarely used stuff from Windows headers
#define NOMINMAX							// Disable window's min & max macros
#include <ctime>
#include <string>
#include <cstdio>							// FILE, freopen_s
#include <cstdlib>
#include <exception>
#include <windows.h>						// Windows Header
#include <DbgHelp.h>

#undef  WIN32_LEAN_AND_MEAN					// Stop defines from spilling into code
#undef  NOMINMAX
#define STACKFRAMES 32

namespace
{
	void WriteCallStack(FILE* out, unsigned nNumSkip)
	{
		SymSetOptions(SYMOPT_DEFERRED_LOADS | SYMOPT_UNDNAME);
		if (SymInitialize(GetCurrentProcess(), NULL, TRUE))
		{
			void* data[STACKFRAMES];
			int nFrames = CaptureStackBackTrace(nNumSkip, STACKFRAMES, data, NULL);
			char strBuffer[1023 + sizeof(SYMBOL_INFO)]{};
			SYMBOL_INFO* symbol = reinterpret_cast<SYMBOL_INFO*>(strBuffer);\

			symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
			symbol->MaxNameLen = 1024;

			for (int n = 0; n < nFrames; ++n)
			{
				SymFromAddr(GetCurrentProcess(), reinterpret_cast<DWORD64>(data[n]), NULL, symbol);

				if (symbol->NameLen)
					fprintf(out, "%s (%p)\n", symbol->Name, data[n]);
				else
					fprintf(out, "Function %p\n", data[n]);
			}

			SymCleanup(GetCurrentProcess());
		}
	}

	[[noreturn]] void ProgramTerminate(void)
	{
		FILE* log;
		if (0 == fopen_s(&log, "CrashReport.log", "w+"))
		{
			fprintf(log, "Crash Report %zu\n\nCall Stack:\n", time(0));

			WriteCallStack(log, 3);
			fclose(log);
		}

		// Let it crash
		std::abort();
	}
}


Dystopia::LoggerSystem::LoggerSystem(void) noexcept
	: mpOut{ PrintToConsoleLog }, mActiveFlags{ eLog::DEFAULT }
{
	std::set_terminate(ProgramTerminate);

#if !EDITOR && defined(COMMAND_PROMPT)

	if (AllocConsole())
	{
		FILE* file;

		freopen_s(&file, "CONOUT$", "wt", stdout);
		freopen_s(&file, "CONOUT$", "wt", stderr);
		//			freopen_s(&file, "CONOUT$", "wt", stdin);

		SetConsoleTitle(ENGINE_NAME);
	}

#endif	// Show Command Prompt
}

Dystopia::LoggerSystem::~LoggerSystem(void) noexcept
{
	// Clean exit
	//std::set_terminate(nullptr);

	mpOut = nullptr;
	mActiveFlags = eLog::NONE;

#if !EDITOR && defined(COMMAND_PROMPT)
	FreeConsole();
#endif
}

Dystopia::LoggerSystem* Dystopia::LoggerSystem::GetInstance(void) noexcept
{
	return EngineCore::GetInstance()->GetSubSystem<LoggerSystem>();
}


void Dystopia::LoggerSystem::RedirectOutput(void(*_pOut)(const std::string&))
{
	mpOut = _pOut;
	mActiveFlags = mpOut ? mActiveFlags : eLog::NONE;
}

void Dystopia::LoggerSystem::ParseInput(const std::string&)
{
}

void Dystopia::LoggerSystem::SendOutput(const char* _strOutput)
{
	std::string output = _strOutput;
	mpOut(output);
}



