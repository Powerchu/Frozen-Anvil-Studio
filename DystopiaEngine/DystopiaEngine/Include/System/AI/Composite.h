/* HEADER *********************************************************************************/
/*!
\file	Composite.h
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
#ifndef _COMPOSITE_H_
#define _COMPOSITE_H_
#include "System/AI/NeuralTree.h"
#include <cassert>


namespace Dystopia
{
	namespace NeuralTree
	{
		/* The Selector composite ticks each child node in order.
		*  If a child succeeds or runs, the selector returns the same status.
		*  In the next tick, it will try to run each child in order again.
		*  If all children fails, only then does the selector fail.
		*  Stops when SUCCESS
		*/
		class Selector : public Composite
		{
		public:
			void Init() override
			{
				iter = mparrChildren.begin();
			}

			eStatus Update() override
			{
				assert(HasChildren() && "Composite has no children");

				while (iter != mparrChildren.end()) {
					const auto status = (*iter)->Tick();

					if (status != eStatus::FAIL) {
						return status;
					}
					++iter;
				}
				return eStatus::FAIL;
			}
		};

		/* The Sequence composite ticks each child node in order.
		 * If a child fails or runs, the sequence returns the same status.
		 * In the next tick, it will try to run each child in order again.
		 * If all children succeeds, only then does the sequence succeed.
		 * Stops when FAIL
		*/
		class Sequence : public Composite
		{
		public:
			void Init() override
			{
				iter = mparrChildren.begin();
			}

			eStatus Update() override
			{
				assert(HasChildren() && "Composite has no children");

				while (iter != mparrChildren.end()) {
					const auto status = (*iter)->Tick();

					if (status != eStatus::SUCCESS) {
						return status;
					}

					++iter;
				}
				return eStatus::SUCCESS;
			}
		};

		/* The StatefulSelector composite ticks each child node in order, 
		 * and remembers what child it previously tried to tick.
		 * If a child succeeds or runs, the stateful selector returns the same status.
		 * In the next tick, it will try to run the next child or start from the beginning again.
		 * If all children fails, only then does the stateful selector fail.
		*/
		class StatefulSelector : public Composite
		{
		public:
			eStatus Update() override
			{
				assert(HasChildren() && "Composite has no children");

				while (iter != mparrChildren.end()) {
					const auto status = (*iter)->Tick();

					if (status != eStatus::FAIL) {
						return status;
					}

					++iter;
				}

				iter = mparrChildren.begin();
				return eStatus::FAIL;
			}
		};

		/* The StatefulSequence composite ticks each child node in order,
		 * and remembers what child it previously tried to tick.
		 * If a child succeeds or runs, the stateful selector returns the same status.
		 * In the next tick, it will try to run the next child or start from the beginning again.
		 * If all children succeed, only then does the Stateful Sequence succeed.
		*/
		class StatefulSequence : public Composite
		{
		public:
			eStatus Update() override
			{
				assert(HasChildren() && "Composite has no children");

				while (iter != mparrChildren.end()) {
					const auto status = (*iter)->Tick();

					if (status != eStatus::SUCCESS) {
						return status;
					}

					++iter;
				}

				iter = mparrChildren.begin();
				return eStatus::SUCCESS;
			}
		};


	}  // NeuralTree
} // Dystopia



#endif

