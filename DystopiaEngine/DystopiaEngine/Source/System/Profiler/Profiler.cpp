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
#include "System/Profiler/Profiler.h"
#include "System/Profiler/ProfileInfo.h"
#include "System/Time/TimeDefs.h"
#include "System/Time/Timer.h"
#include "System/Logger/LogPriority.h"
#include "System/Logger/LoggerSystem.h"

#include "DataStructure/Array.h"
#include "DataStructure/AutoArray.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <Psapi.h>
#include <iostream>
#include <intrin.h>
#include <bitset>

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
	// Check for CPU and available instruction set
	int nAvailableID;
	char buf1[128]{ 0 }, buf2[128]{ 0 };
	Array<int, 4> out;
	
	std::bitset<32> data[2];
	AutoArray<Array<int, 4>> ExData;

	// Get CPU Vendor
	__cpuid(out.begin(), 0);
	nAvailableID = out[0];
	reinterpret_cast<int*>(buf1)[0] = out[1];
	reinterpret_cast<int*>(buf1)[1] = out[3];
	reinterpret_cast<int*>(buf1)[2] = out[2];

	if (nAvailableID > 0)
	{
		__cpuid(out.begin(), 1);
		data[0] = out[2];
		data[1] = out[3];
	}

	__cpuid(out.begin(), 0x80000000);
	nAvailableID = out[0];

	for (int n = 0x80000000; n < nAvailableID; ++n)
	{
		__cpuidex(out.begin(), n, 0);
		ExData.push_back(out);
	}

	// If we can get the CPU brand, print it!
	if (nAvailableID > 0x80000003)
	{
		std::memcpy(buf2     , ExData[2].begin(), sizeof(ExData[0]));
		std::memcpy(buf2 + 16, ExData[3].begin(), sizeof(ExData[0]));
		std::memcpy(buf2 + 32, ExData[4].begin(), sizeof(ExData[0]));

		LoggerSystem::ConsoleLog(eLog::SYSINFO, "System: Using %s, %s!\n", buf1, buf2);
	}
	else
	{
		LoggerSystem::ConsoleLog(eLog::SYSINFO, "System: Using %s!", buf1);
	}

#define AVAIL_MSG(_X_) ((_X_) ? "Available" : "Not Supported")
	LoggerSystem::ConsoleLog(eLog::SYSINFO, "System: SSE %s, SSE2 %s, SSE3 %s, SSE4.1 %s, SSE4.2 %s, FMA %s!", 
		AVAIL_MSG(data[1][25]), AVAIL_MSG(data[1][26]), AVAIL_MSG(data[0][0]),
		AVAIL_MSG(data[0][19]), AVAIL_MSG(data[0][20]), AVAIL_MSG(data[0][12])
	);
#undef AVAIL_MSG

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


