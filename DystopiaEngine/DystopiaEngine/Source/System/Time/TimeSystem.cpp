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
#include "System\Time\TimeSystem.h"		// File Header

bool Dystopia::TimeSystem::Init(void)
{
	return true;
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

void Dystopia::TimeSystem::FlushQueue(void)
{
	mPQueue.Clear();
}

