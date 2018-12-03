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

#include "IO/TextSerialiser.h"

#include "Editor/EGUI.h"


Dystopia::TimeSystem::TimeSystem(void) :
	mTimeKeep{}, mPQueue{}, mAccumulatedDT{ 0 }
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
	mAccumulatedDT += (additionalDT < 1000000001ll) * additionalDT;
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
	return mfTimeScale * (mbSimulateTime ? mfSimulatedDT : mTimeKeep.Elapsed());
}

float Dystopia::TimeSystem::GetFixedDeltaTime(void) const noexcept
{
	return mfTimeScale * mFixedDT * 1e-9f;
}


void Dystopia::TimeSystem::SetTimeScale(float _fTimeScale) noexcept
{
	mfTimeScale = _fTimeScale;
}


void Dystopia::TimeSystem::FlushQueue(void)
{
	mPQueue.Clear();
}

void Dystopia::TimeSystem::LoadDefaults(void)
{
	mbSimulateTime = false;

	mfTimeScale   = 1.f;
	mFixedDT      = 20000000ll;
	mSimulatedDT  = 16666667ll;
	mfSimulatedDT = 1.f / 60.f;
}

void Dystopia::TimeSystem::LoadSettings(DysSerialiser_t& _in)
{
	_in >> mbSimulateTime;
	_in >> mfTimeScale;
	_in >> mFixedDT;
	_in >> mSimulatedDT;
	_in >> mfSimulatedDT;
}

void Dystopia::TimeSystem::SaveSettings(DysSerialiser_t& _out)
{
	_out << mbSimulateTime;
	_out << mfTimeScale;
	_out << mFixedDT;
	_out << mSimulatedDT;
	_out << mfSimulatedDT;
}

#if EDITOR
void Dystopia::TimeSystem::EditorUI(void)
{
	static float fixedDT = 0.02f;

	EGUI::Display::CheckBox("Simulate Time", &mbSimulateTime);

	if (mbSimulateTime)
	{
		switch (EGUI::Display::DragFloat("Simulated Timestep ", &mfSimulatedDT, .001f, .0001f, 1.f))
		{
		case EGUI::eDragStatus::eSTART_DRAG:
			//EGUI::GetCommandHND()->Make_FunctionModWrapper()
			break;
		case EGUI::eDragStatus::eDRAGGING:
			mSimulatedDT = static_cast<int64_t>(mfSimulatedDT * 1e9f);
			break;
		case EGUI::eDragStatus::eENTER:
		case EGUI::eDragStatus::eTABBED:
		case EGUI::eDragStatus::eEND_DRAG:
		case EGUI::eDragStatus::eDEACTIVATED:
			//EGUI::GetCommandHND()->EndRecording();
		case EGUI::eDragStatus::eNO_CHANGE:
			break;
		}
	}

	switch (EGUI::Display::DragFloat("Fixed Timestep     ", &fixedDT, .001f, .0001f, 1.f))
	{
	case EGUI::eDragStatus::eSTART_DRAG:
		break;
	case EGUI::eDragStatus::eDRAGGING:
	case EGUI::eDragStatus::eENTER:
	case EGUI::eDragStatus::eTABBED:
	case EGUI::eDragStatus::eEND_DRAG:
	case EGUI::eDragStatus::eDEACTIVATED:
		mFixedDT = static_cast<int64_t>(fixedDT * 1e9f);
		mAccumulatedDT = 0;
	case EGUI::eDragStatus::eNO_CHANGE:
		break;
	}

	switch (EGUI::Display::DragFloat("Time Scale         ", &mfTimeScale, .01f, .0001f, 10.f))
	{
	case EGUI::eDragStatus::eSTART_DRAG:
		//EGUI::GetCommandHND()->StartRecording(&TimeSystem::mfTimeScale);
		break;
	case EGUI::eDragStatus::eENTER:
	case EGUI::eDragStatus::eTABBED:
	case EGUI::eDragStatus::eEND_DRAG:
	case EGUI::eDragStatus::eDEACTIVATED:
		//EGUI::GetCommandHND()->EndRecording();
	case EGUI::eDragStatus::eDRAGGING:
	case EGUI::eDragStatus::eNO_CHANGE:
		break;
	}
}
#endif 
