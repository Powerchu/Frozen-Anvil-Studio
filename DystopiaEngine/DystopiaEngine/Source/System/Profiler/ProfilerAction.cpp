/* HEADER *********************************************************************************/
/*!
\file	ProfilerAction.cpp
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Implementation

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "System/Profiler/ProfilerAction.h"
#include "System/Driver/Driver.h"
#include "System/Profiler/Profiler.h"

#include <string>

Dystopia::ProfilerAction::ProfilerAction(const std::string& _strModule, const std::string& _strFunc) :
	strModule{ _strModule }, strFunction{ _strFunc }
{
}

void Dystopia::ProfilerAction::PostDuration(ProfilerAction::Precision_t _dur)
{
#if !defined(DISABLE_PROFILER)
	auto& info = EngineCore::GetInstance()->GetSystem<Profiler>()->GetInfo();

	size_t time = _dur.count();
	auto& mod = info[strModule];

	mod.mTotal += time;
	mod.mTimes[strFunction] = time;
#else
	_dur;
#endif
}


