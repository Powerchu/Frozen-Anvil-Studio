/* HEADER *********************************************************************************/
/*!
\file	Leaf.h
\author Aaron Chu (100%)
\par    email: m.chu\@digipen.edu
\brief
Builder Template Interface for all Neural Trees (Behaviour Tree). Referred to
https://www.gamasutra.com/blogs/ChrisSimpson/20140717/221339/Behavior_trees_for_AI_How_they_work.php
for basic understanding.

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _LEAF_H_
#define _LEAF_H_
#include <utility>
#include "NeuralTree.h"
#include "Utility/DebugAssert.h"

namespace Dystopia
{
	namespace NeuralTree
	{
		class HungerCheck : public Task
		{
		public:
			HungerCheck(Blackboard::Ptr _ptr) : Task(std::move(_ptr)) {}
			eStatus Update() override
			{
				const auto a = mpBlackboard->GetFloat("Hunger");
				if (a < 10.0F)
				{
					DEBUG_PRINT(eLog::MESSAGE, "I am hungry. Hunger: %f", a);
					return eStatus::SUCCESS;
				}
				else
				{
					///DEBUG_PRINT(eLog::MESSAGE, "I am not hungry. Hunger: %f", a);
					return eStatus::FAIL;
				}
			}
		};

		class HaveFood : public Task
		{
		public:
			HaveFood(Blackboard::Ptr _ptr) : Task(std::move(_ptr)) {}
			eStatus Update() override
			{
				if (mpBlackboard->GetInt("Food") == 0)
				{
					DEBUG_PRINT(eLog::MESSAGE, "I have no food.");
					return eStatus::FAIL;
				}
				else
				{
					DEBUG_PRINT(eLog::MESSAGE, "I will eat now.");
					return eStatus::SUCCESS;
				}
			}
		};

		class FindFood : public Task
		{
		public:
			FindFood(Blackboard::Ptr _ptr) : Task(std::move(_ptr)) {}
			eStatus Update() override
			{
				DEBUG_PRINT(eLog::MESSAGE, "Finding Food");
				const auto a = mpBlackboard->GetInt("Food") + 3;
				mpBlackboard->SetInt("Food", a);
				DEBUG_PRINT(eLog::MESSAGE, "I have %d food now.", a);
				return eStatus::SUCCESS;
			}
		};

		class EatFood : public Task
		{
		public:
			EatFood(NeuralTree::Blackboard::Ptr _ptr) : Task(std::move(_ptr)) {}
			eStatus Update() override
			{
				DEBUG_PRINT(eLog::MESSAGE, "Eating food");
				const auto a = mpBlackboard->GetFloat("Hunger") + 15.0F;
				const auto b = mpBlackboard->GetInt("Food") - 1;

				mpBlackboard->SetInt("Food", b);
				mpBlackboard->SetFloat("Hunger", a);
				DEBUG_PRINT(eLog::MESSAGE, "Hunger: %f", a);

				return eStatus::SUCCESS;
			}
		};
	}
}


#endif
