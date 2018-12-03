/* HEADER *********************************************************************************/
/*!
\file	Decorator.h
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
#ifndef _DECORATOR_H_
#define _DECORATOR_H_
#include "System/AI/NeuralTree.h"


namespace Dystopia
{
	namespace NeuralTree
	{
		// The Succeeder decorator returns success, regardless of what happens to the mpChild.
		class Succeeder : public Decorator
		{
		public:
			eStatus Update() override
			{
				mpChild->Tick();
				return eStatus::SUCCESS;
			}

			std::string GetEditorName(void) const override { return "Succeeder"; }

		};

		// The Failer decorator returns failure, regardless of what happens to the mpChild.
		class Failer : public Decorator
		{
		public:
			eStatus Update() override
			{
				mpChild->Tick();
				return eStatus::FAIL;
			}
			std::string GetEditorName(void) const override { return "Failer"; }
		};

		// The Inverter decorator inverts the mpChild node's status, 
		// i.e. failure becomes success and success becomes failure.
		// If the mpChild runs, the Inverter returns running status.
		class Inverter : public Decorator
		{
		public:
			eStatus Update() override
			{
				const auto status = mpChild->Tick();

				switch (status)
				{
				case eStatus::SUCCESS:
					return eStatus::FAIL;
				case eStatus::FAIL:
					return eStatus::SUCCESS;
				default: // returns running status
					return status;
				}
			}
			std::string GetEditorName(void) const override { return "Inverter"; }
		};

		// The Repeater decorator repeats infinitely or to a limit until the mpChild returns success.
		class Repeater : public Decorator
		{
		public:
			explicit Repeater(const int limit = 0) : limit(limit) {}

			void Init() override
			{
				counter = 0;
			}

			eStatus Update() override
			{
				mpChild->Tick();

				if (limit > 0 && ++counter == limit) {
					return eStatus::SUCCESS;
				}

				return eStatus::RUNNING;
			}

			std::string GetEditorName(void) const override { return "Repeater"; }

		protected:
			int limit;
			int counter = 0;
		};

		// The UntilSuccess decorator repeats until the mpChild returns success and then returns success.
		class UntilSuccess : public Decorator
		{
		public:
			eStatus Update() override
			{
				while (true) {
					const auto status = mpChild->Tick();

					if (status == eStatus::SUCCESS) {
						return eStatus::SUCCESS;
					}
				}
			}
			std::string GetEditorName(void) const override { return "Until Success"; }
		};

		// The UntilFailure decorator repeats until the mpChild returns fail and then returns success.
		class UntilFailure : public Decorator
		{
		public:
			eStatus Update() override
			{
				while (true) {
					const auto status = mpChild->Tick();

					if (status == eStatus::FAIL) {
						return eStatus::SUCCESS;
					}
				}
			}
			std::string GetEditorName(void) const override { return "Until Failure"; }
		};

		// TODO WIP GAM250
		class Limiter : public Decorator
		{
		public:

		private:
			unsigned maxCount;
		};
	}
}


#endif
