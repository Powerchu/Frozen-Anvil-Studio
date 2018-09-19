/* HEADER *********************************************************************************/
/*!
\file	Profiler.cpp
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Profiler System
	Define DISABLE_PROFILER in the compiler flags to disable

	References:
	https://docs.microsoft.com/en-us/windows/desktop/api/processthreadsapi/
	https://docs.microsoft.com/en-us/windows/desktop/api/Psapi/

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "System\Profiler\Profiler.h"
#include "System\Profiler\ProfileInfo.h"
#include "System\Time\TimeDefs.h"
#include "System\Time\Timer.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <Psapi.h>
#include <iostream>

namespace
{
	inline static constexpr size_t ByteToMiB(size_t _bytes)
	{
		return _bytes >> 20;
	}

	uint64_t ConvertFiletime(FILETIME _t)
	{
		// In case of endianess issues
		// Use shifts instead of reinterpret cast
		return (static_cast<uint64_t>(_t.dwHighDateTime) << 32) | _t.dwLowDateTime;
	}
}

void Dystopia::ProfileInfo::Clear(void) noexcept
{
	for (auto& e : mTimes)
		e.second = 0;

	mTotal = 0;
}

Dystopia::Profiler::Profiler(void) :
	mData{},
	mfCPUTime{ 1.f }, mnProcIdle { 0 }, mnProcBusy{ 0 }, mnProcOS{ 0 },
	mfMemLoad{ 100.f }, mnPageFaults{ 0 }, mnUsedMem{ 0 }, mnAvailableMem{ 0 }
{
}

std::map<std::string, Dystopia::ProfileInfo>& Dystopia::Profiler::GetInfo(void)
{
	return mData;
}

bool Dystopia::Profiler::Init(void)
{
	return true;
}

void Dystopia::Profiler::Update(float)
{
#if !defined(DISABLE_PROFILER)
	static Timer timekeep;

	// Don't update every frame
	if (timekeep.Elapsed() < .0625f)
		return;
	timekeep.Lap();

	CalculateCPUUsage();
	CalculateMemoryUsage();

#endif
}

void Dystopia::Profiler::PostUpdate(void)
{
	for (auto& e : mData)
		e.second.Clear();
}

void Dystopia::Profiler::Shutdown(void)
{
}


bool Dystopia::Profiler::Is64bitMachine(void) const
{
#if defined(_WIN64)
	return true;
#else
	using IsWow64FuncPtr = int (WINAPI *) (HANDLE, PBOOL);

	int result;
	auto addr = reinterpret_cast<IsWow64FuncPtr>(GetProcAddress(GetModuleHandle(TEXT("kernel32")), "IsWow64Process"));

	if (addr && addr(GetCurrentProcess(), &result))
	{
		return result != 0;
	}

	return false;
#endif
}


float Dystopia::Profiler::GetCPUPercentageIdle(void) const
{
	return mnProcIdle * mfCPUTime;
}

float Dystopia::Profiler::GetCPUPercentageBusy(void) const
{
	return (mnProcBusy + mnProcOS) * mfCPUTime;
}

float Dystopia::Profiler::GetCPUPercentageOS(void) const
{
	return mnProcOS * mfCPUTime;
}

float Dystopia::Profiler::GetCPUPercentageProcess(void) const
{
	return mnProcBusy * mfCPUTime;
}

void Dystopia::Profiler::CalculateCPUUsage(void) noexcept
{
	FILETIME idle, os, user;
	static uint64_t prevIdle, prevOS, prevUser;

	if (GetSystemTimes(&idle, &os, &user))
	{
		mnProcIdle = prevIdle;
		prevIdle = ConvertFiletime(idle);

		mnProcOS = prevOS;
		prevOS = ConvertFiletime(os) - prevIdle;

		mnProcBusy = prevUser;
		prevUser = ConvertFiletime(user);

		mnProcOS   = prevOS   - mnProcOS;
		mnProcIdle = prevIdle - mnProcIdle;
		mnProcBusy = prevUser - mnProcBusy;

		mfCPUTime = 100.f / (mnProcBusy + mnProcIdle + mnProcOS);
	}
}


size_t Dystopia::Profiler::GetNumPageFaults(void) const noexcept
{
	return mnPageFaults;
}

size_t Dystopia::Profiler::GetUsedMemory(void) const noexcept
{
	return mnUsedMem;
}

size_t Dystopia::Profiler::GetUsedPhysicalMemory(void) const noexcept
{
	return mnUsedRAM;
}

size_t Dystopia::Profiler::GetAvailablePhysicalMemory(void) const noexcept
{
	return mnAvailableMem;
}

float Dystopia::Profiler::GetSystemMemoryLoad(void) const noexcept
{
	return mfMemLoad;
}

void Dystopia::Profiler::CalculateMemoryUsage(void) noexcept
{
	static MEMORYSTATUSEX memState{ sizeof(memState) };
	static PROCESS_MEMORY_COUNTERS memInfo;

	GlobalMemoryStatusEx(&memState);
	GetProcessMemoryInfo(GetCurrentProcess(), &memInfo, sizeof(memInfo));

	mnPageFaults   = memInfo.PageFaultCount;
	mnUsedRAM      = ByteToMiB(memInfo.WorkingSetSize);
	mnUsedMem      = ByteToMiB(memInfo.PagefileUsage );
	mnAvailableMem = ByteToMiB(memState.ullAvailPhys );
	mfMemLoad      = 100.f * (1.f - static_cast<float>(memState.ullAvailPhys) / static_cast<float>(memState.ullTotalPhys));
}


