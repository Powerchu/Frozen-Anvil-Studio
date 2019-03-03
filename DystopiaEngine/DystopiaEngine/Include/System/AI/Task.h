/* HEADER *********************************************************************************/
/*!
\file	Task.h
\author Aaron Chu (100%)
\par    email: m.chu\@digipen.edu
\brief
Builder Template Interface for all Neural Trees (Behaviour Tree). Referred to
https://www.gamasutra.com/blogs/ChrisSimpson/20140717/221339/Behavior_trees_for_AI_How_they_work.php
for basic understanding.
This contain basic Task nodes.

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _TASK_H_
#define _TASK_H_
#include <utility>
#include "NeuralTree.h"
#include "Utility/DebugAssert.h"

namespace Dystopia
{
	namespace NeuralTree
	{
		class Wait : public Task
		{
		public:
			HashString GetEditorName(void) const override { return "Wait Task"; }

			explicit Wait(float _waitTime) : mWaitTime(_waitTime), mMaxWaitTime(_waitTime) {}

			void Init() override
			{
				mWaitTime = mMaxWaitTime;
			}

			eStatus Update(float _deltaTime) override
			{
				if (mWaitTime > 0.f)
				{
					mWaitTime -= _deltaTime;

					if (mWaitTime <= 0.f)
					{
						mWaitTime = 0.0f;
						return eStatus::SUCCESS;
					}

					return eStatus::RUNNING;
				}
				
				return eStatus::FAIL;

			}

		private:
			float mWaitTime;
			float mMaxWaitTime;
		};

		class WaitBlackBoardTime : public Task
		{
		public:
			HashString GetEditorName(void) const override { return "Wait Blackboard Time Task"; }

			explicit WaitBlackBoardTime(HashString key, Blackboard::Ptr _bb) : Task(Ut::Move(_bb)), mWaitKey(Ut::Move(key)), mWaitTime(0)
			{
			}

			void Init() override
			{
				mWaitTime = mpBlackboard->GetFloat(mWaitKey);
			}

			eStatus Update(float _deltaTime) override
			{
				if (mWaitTime > 0.f)
				{
					mWaitTime -= _deltaTime;

					if (mWaitTime <= 0.f)
					{
						mWaitTime = 0.0f;
						return eStatus::SUCCESS;
					}

					return eStatus::RUNNING;
				}

				return eStatus::FAIL;
			}

		private:
			HashString mWaitKey;
			float mWaitTime;

		};
	}
}


#endif
