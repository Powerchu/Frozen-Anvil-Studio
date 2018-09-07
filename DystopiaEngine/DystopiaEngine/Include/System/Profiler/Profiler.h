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
		void Shutdown(void) override;

//		const std::map<std::string, ProfileInfo>& GetInfo(void) const;

		float GetCPUPercentageIdle(void) const;
		float GetCPUPercentageBusy(void) const;
		float GetCPUPercentageOS(void) const;
		float GetCPUPercentageProcess(void) const;

	private:

		uint64_t mnProcIdle, mnProcBusy, mnProcOS;
		float mfCPU;
//		std::map<std::string, ProfileInfo> mData;

		size_t mnPageFaults, mnUsedMem, mnUsedRAM, mnAvailableMem;
		float mfMemLoad;

		void CalculateCPUUsage(void) noexcept;
		void CalculateMemoryUsage(void) noexcept;
	};
}



#endif		// INCLUDE GUARD

