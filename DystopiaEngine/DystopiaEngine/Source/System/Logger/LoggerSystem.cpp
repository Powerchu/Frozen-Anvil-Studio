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
#include "System/Logger/LoggerSystem.h"
#include "System/Logger/FileLogger.h"

#include "Editor/ConsoleLog.h"
#include "System/Driver/Driver.h"

#if defined(DEBUGALLOC)
#include "Allocator/Allocator.h"
#endif

#define WIN32_LEAN_AND_MEAN					// Exclude rarely used stuff from Windows headers
#define NOMINMAX							// Disable window's min & max macros
#include <ctime>
#include <string>
#include <cstdio>							// FILE, freopen_s
#include <cstdlib>
#include <exception>
#include <Windows.h>						// Windows Header
#include <DbgHelp.h>

#undef  WIN32_LEAN_AND_MEAN					// Stop defines from spilling into code
#undef  NOMINMAX
#define STACKFRAMES 32


namespace
{
	using SymCleanup_t    = decltype(SymCleanup)*;
	using SymFromAddr_t   = decltype(SymFromAddr)*;
	using SymInitialize_t = decltype(SymInitialize)*;
	using SymSetOptions_t = decltype(SymSetOptions)*;

	void WriteCallStack(Dystopia::FileLogger& _out, unsigned nNumSkip)
	{
		auto lib = LoadLibrary(L"dbghelp.dll");

		SymCleanup_t    CleanupF  = reinterpret_cast<SymCleanup_t   >(GetProcAddress(lib, "SymCleanup")   );
		SymFromAddr_t   FromAddrF = reinterpret_cast<SymFromAddr_t  >(GetProcAddress(lib, "SymFromAddr")  );
		SymInitialize_t InitF     = reinterpret_cast<SymInitialize_t>(GetProcAddress(lib, "SymInitialize"));
		SymSetOptions_t SetOptF   = reinterpret_cast<SymSetOptions_t>(GetProcAddress(lib, "SymSetOptions"));

		if (CleanupF && FromAddrF && InitF && SetOptF)
		{
			SetOptF(SYMOPT_DEFERRED_LOADS | SYMOPT_UNDNAME);
			if (InitF(GetCurrentProcess(), NULL, TRUE))
			{
				void* data[STACKFRAMES];
				int nFrames = CaptureStackBackTrace(nNumSkip, STACKFRAMES, data, NULL);
				char strBuffer[1023 + sizeof(SYMBOL_INFO)]{};
				SYMBOL_INFO* symbol = reinterpret_cast<SYMBOL_INFO*>(strBuffer);

				symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
				symbol->MaxNameLen = 1024;

				for (int n = 0; n < nFrames; ++n)
				{
					FromAddrF(GetCurrentProcess(), reinterpret_cast<DWORD64>(data[n]), NULL, symbol);

					if (symbol->NameLen)
						_out.Write("%s (%p)\n", symbol->Name, data[n]);
					else
						_out.Write("Function %p\n", data[n]);
				}

				CleanupF(GetCurrentProcess());
			}
		}

		if (lib) FreeLibrary(lib);
	}

	[[noreturn]] void ProgramTerminate(void)
	{{
		Dystopia::FileLogger log{ "CrashReport.dystor", "w+" };
		if (log.Good())
		{
			log.Write("Crash Report %zu\n\nCall Stack:\n", time(0));

			WriteCallStack(log, 3);

#       if defined(DEBUGALLOC)
			Dystopia::Alloc::WriteActiveAllocations(log);
			Dystopia::Alloc::WriteFreeMemory(log);
#       endif
		}

		// Let it crash
		} std::abort();
	}
}

#if EDITOR
#define DEFAULT_PRINT_FUNC PrintToConsoleLog
#else
#define DEFAULT_PRINT_FUNC nullptr
#endif

Dystopia::LoggerSystem::LoggerSystem(void) noexcept
	: mpOut{ DEFAULT_PRINT_FUNC }, mActiveFlags{ eLog::DEFAULT }
{
	std::set_terminate(ProgramTerminate);


#if defined(COMMANDPROMPT)

	if (AllocConsole())
	{
		FILE* file;

		freopen_s(&file, "CONOUT$", "wt", stdout);
		freopen_s(&file, "CONOUT$", "wt", stderr);
//		freopen_s(&file, "CONOUT$", "wt", stdin);

		SetConsoleTitle(L"Dystopia Engine");
	}

#endif	// Show Command Prompt
}

Dystopia::LoggerSystem::~LoggerSystem(void) noexcept
{
	// Clean exit
	//std::set_terminate(nullptr);

	mpOut = nullptr;
	mActiveFlags = eLog::NONE;

#if defined(COMMANDPROMPT)
	FreeConsole();
#endif
}

Dystopia::LoggerSystem* Dystopia::LoggerSystem::GetInstance(void) noexcept
{
	return EngineCore::GetInstance()->GetSubSystem<LoggerSystem>();
}


void Dystopia::LoggerSystem::RedirectOutput(void(*_pOut)(const std::string&))
{
	if (!mpOut)
		return;
	mpOut = _pOut;
	mActiveFlags = mpOut ? mActiveFlags : eLog::NONE;
}

void Dystopia::LoggerSystem::ParseInput(const std::string&)
{
}

Dystopia::FileLogger Dystopia::LoggerSystem::FileLog(const std::string& _strFilename)
{
	return FileLogger{ _strFilename };
}

void Dystopia::LoggerSystem::SendOutput(const char* _strOutput)
{
	if (!mpOut)
		return;
	std::string output = _strOutput;
	mpOut(output);
}



