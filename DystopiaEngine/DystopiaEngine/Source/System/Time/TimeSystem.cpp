/* HEADER *********************************************************************************/
/*!
\file	TimeSystem.cpp
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "System/Time/TimeSystem.h"		// File Header
#include "System/Driver/Driver.h"

#include "Utility/Meta.h"
#include "Utility/MetaDataStructures.h"

#include "Math/MathUtility.h"


Dystopia::TimeSystem::TimeSystem(void) :
	mTimeKeep{}, mTimeZones{ Ut::SizeofList<EngineCore::AllSys>::value }, mPQueue{},
	mbSimulateTime{ false }, mfSimulatedDT{ 1.f / 60.f }, mSimulatedDT{ 16666666667ll }, 
	mFixedDT{ 20000000ll }, mAccumulatedDT{ 0 }
{
}

bool Dystopia::TimeSystem::Init(void)
{
	return true;
}

void Dystopia::TimeSystem::PostInit(void)
{
	mTimeKeep.Lap();
	mAccumulatedDT = 0;
}


void Dystopia::TimeSystem::StopTime(void) noexcept
{
	AdvanceTime();
}

void Dystopia::TimeSystem::ResumeTime(void) noexcept
{
	mTimeKeep.Lap();
}

void Dystopia::TimeSystem::AdvanceTime(void) noexcept
{
	auto prevTime = mTimeKeep.Time();
	mTimeKeep.Lap();

	auto additionalDT = mbSimulateTime ? mSimulatedDT : (mTimeKeep.Time() - prevTime).count();
	mAccumulatedDT += (additionalDT < 1000000000ll) * additionalDT;
}

bool Dystopia::TimeSystem::ConsumeFixedDT(void) noexcept
{
	bool ret = mAccumulatedDT > mFixedDT;
	mAccumulatedDT -= ret * mFixedDT;
	return ret;
}


void Dystopia::TimeSystem::Update(float)
{
	while (!mPQueue.IsEmpty() && mPQueue.First()->mDelay.Complete())
	{
		mPQueue.First()->Invoke();

		delete mPQueue.First();

		mPQueue.Pop([](const QueueObject* _lhs, const QueueObject* _rhs)->bool
		{
			return _lhs->mDelay.Time() < _rhs->mDelay.Time();
		});
	}
}

void Dystopia::TimeSystem::Shutdown(void)
{
	FlushQueue();
}


float Dystopia::TimeSystem::GetDeltaTime(void) const noexcept
{
	return mbSimulateTime ? mfSimulatedDT : mTimeKeep.Elapsed();
}

float Dystopia::TimeSystem::GetFixedDeltaTime(void) const noexcept
{
	return mFixedDT * 1e-9f;
}


void Dystopia::TimeSystem::FlushQueue(void)
{
	mPQueue.Clear();
}

