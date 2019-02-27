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
#include <random>

namespace Dystopia
{
	namespace NeuralTree
	{
		/* The Selector composite ticks each child node in order.
		*  If a child succeeds or runs, the selector returns the same status.
		*  In the next tick, it will try to run each child in order again.
		*  If all children fails, only then does the selector fail.
		*  Stops when SUCCESS
		*  
		*  \detail:
		*  A selector is a branch task that runs each of its child behaviors in turn. It will 
		*  return immediately with a success status code when one of its children runs successfully. 
		*  As long as its children are failing, it will keep on trying. If it runs out of children 
		*  completely, it will return a failure status code.
		*  
		*  Selectors are used to choose the first of a set of possible actions that is successful. 
		*  
		*  For example, a selector might represent a character wanting to reach safety. There may 
		*  be multiple ways to do that: take cover, leave a dangerous area, and find backup. 
		*  Such a selector will first try to take cover; if that fails, it will leave the area. 
		*  If that succeeds, it will stop since there's no point also finding backup, as we've solved 
		*  the character's goal of reaching safety. If we exhaust all options without success, then 
		*  the selector itself has failed.
		*/
		class Selector : public Composite
		{
		public:
			void Init() override
			{
				iter = mparrChildren.begin();
			}

			eStatus Update(float _deltaTime) override
			{
				assert(HasChildren() && "Composite has no children");

				while (iter != mparrChildren.end()) {
					const auto status = (*iter)->Tick(_deltaTime);

					if (status != eStatus::FAIL) {
						return status;
					}
					++iter;
				}
				return eStatus::FAIL;
			}

			HashString GetEditorName(void) const override { return "Selector"; }
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

			eStatus Update(float _deltaTime) override
			{
				assert(HasChildren() && "Composite has no children");

				while (iter != mparrChildren.end()) {
					const auto status = (*iter)->Tick(_deltaTime);

					if (status != eStatus::SUCCESS) {
						return status;
					}

					++iter;
				}
				return eStatus::SUCCESS;
			}

			HashString GetEditorName(void) const override { return "Sequence"; }

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
			eStatus Update(float _deltaTime) override
			{
				assert(HasChildren() && "Composite has no children");

				while (iter != mparrChildren.end()) {
					const auto status = (*iter)->Tick(_deltaTime);

					if (status != eStatus::FAIL) {
						return status;
					}

					++iter;
				}

				iter = mparrChildren.begin();
				return eStatus::FAIL;
			}

			HashString GetEditorName(void) const override { return "Stateful Selector"; }

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
			eStatus Update(float _deltaTime) override
			{
				assert(HasChildren() && "Composite has no children");

				while (iter != mparrChildren.end()) {
					const auto status = (*iter)->Tick(_deltaTime);

					if (status != eStatus::SUCCESS) {
						return status;
					}

					++iter;
				}

				iter = mparrChildren.begin();
				return eStatus::SUCCESS;
			}

			HashString GetEditorName(void) const override { return "Stateful Sequence"; }
		};

		/*
		 *  RandomSelector does exactly what a Selector does: Runs till SUCCEED
		 *  However, this runs in a random order.
		 */
		class RandomSelector : public Composite
		{
		public:
			void Init() override
			{
				iter = mparrChildren.begin();

				std::random_device random_device;
				std::mt19937 engine{ random_device() };
				const std::uniform_int_distribution<size_t> dist(0, mparrChildren.size() - 1);

				mpChild = mparrChildren[dist(engine)];
			}

			eStatus Update(float _deltaTime) override
			{
				assert(HasChildren() && "Composite has no children");

				const auto status = mpChild->Tick(_deltaTime);

				if (status != eStatus::FAIL) 
				{
					return status;
				}
				
				return eStatus::FAIL;
			}

			HashString GetEditorName(void) const override { return "Random Selector"; }
		};

		class RandomSequence : public Composite
		{
		public:
			void Init() override
			{
				iter = mparrChildren.begin();

				std::random_device random_device;
				std::mt19937 engine{ random_device() };
				const std::uniform_int_distribution<size_t> dist(0, mparrChildren.size() - 1);

				mpChild = mparrChildren[dist(engine)];
			}

			eStatus Update(float _deltaTime) override
			{
				assert(HasChildren() && "Composite has no children");

				const auto status = mpChild->Tick(_deltaTime);

				if (status != eStatus::SUCCESS)
				{
					return status;
				}

				return eStatus::SUCCESS;
			}

			HashString GetEditorName(void) const override { return "Random Sequence"; }
		};

	}  // NeuralTree
} // Dystopia



#endif

