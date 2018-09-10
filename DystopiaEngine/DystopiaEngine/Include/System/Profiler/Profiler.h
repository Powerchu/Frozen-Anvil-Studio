/* HEADER *********************************************************************************/
/*!
\file	Profiler.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Profiler System

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _PROFILER_SYS_H_
#define _PROFILER_SYS_H_

#include "System\Base\Systems.h"
#include "System\Profiler\ProfileInfo.h"
#include "DataStructure\AutoArray.h"

// Temporary std lib for fast prototyping
#include <map>
#include <chrono>


namespace Dystopia
{
	class Profiler : public Systems
	{
	public:
		Profiler(void);

		bool Init(void) override;
		void Update(float) override;
		void PostUpdate(void) override;
		void Shutdown(void) override;

		std::map<std::string, ProfileInfo>& GetInfo(void);

		float GetCPUPercentageIdle(void) const;
		float GetCPUPercentageBusy(void) const;
		float GetCPUPercentageOS(void) const;
		float GetCPUPercentageProcess(void) const;

		size_t GetNumPageFaults(void) const noexcept;
		size_t GetUsedMemory(void) const noexcept;
		size_t GetUsedPhysicalMemory(void) const noexcept;
		size_t GetAvailablePhysicalMemory(void) const noexcept;
		float  GetSystemMemoryLoad(void) const noexcept;

	private:

		// CPU
		float mfCPUTime;
		uint64_t mnProcIdle, mnProcBusy, mnProcOS;

		// Memory
		float mfMemLoad;
		size_t mnPageFaults, mnUsedMem, mnUsedRAM, mnAvailableMem;

		std::map<std::string, ProfileInfo> mData;

		void CalculateCPUUsage(void) noexcept;
		void CalculateMemoryUsage(void) noexcept;
	};
}



#endif		// INCLUDE GUARD

