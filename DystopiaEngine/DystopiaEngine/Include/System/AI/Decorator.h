/* HEADER *********************************************************************************/
/*!
\file	Decorator.h
\author Aaron Chu (100%)
\par    email: m.chu\@digipen.edu
\brief
Builder Template Interface for all Neural Trees (Behaviour Tree). Referred to
https://www.gamasutra.com/blogs/ChrisSimpson/20140717/221339/Behavior_trees_for_AI_How_they_work.php
for basic understanding.

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
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
			eStatus Update(float _deltaTime) override
			{
				mpChild->Tick(_deltaTime);
				return eStatus::SUCCESS;
			}

			HashString GetEditorName(void) const override { return "Succeeder"; }

		};

		// The Failer decorator returns failure, regardless of what happens to the mpChild.
		class Failer : public Decorator
		{
		public:
			eStatus Update(float _deltaTime) override
			{
				mpChild->Tick(_deltaTime);
				return eStatus::FAIL;
			}

			HashString GetEditorName(void) const override { return "Failer"; }
		};

		// The Inverter decorator inverts the mpChild node's status, 
		// i.e. failure becomes success and success becomes failure.
		// If the mpChild runs, the Inverter returns running status.
		class Inverter : public Decorator
		{
		public:
			eStatus Update(float _deltaTime) override
			{
				const auto status = mpChild->Tick(_deltaTime);

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

			HashString GetEditorName(void) const override { return "Inverter"; }
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

			eStatus Update(float _deltaTime) override
			{
				mpChild->Tick(_deltaTime);

				if (limit > 0 && ++counter == limit) {
					return eStatus::SUCCESS;
				}

				return eStatus::RUNNING;
			}

			HashString GetEditorName(void) const override { return "Repeater"; }

		protected:
			int limit;
			int counter = 0;
		};

		// The UntilSuccess decorator repeats until the mpChild returns success and then returns success.
		class UntilSuccess : public Decorator
		{
		public:
			eStatus Update(float _deltaTime) override
			{
				const auto status = mpChild->Tick(_deltaTime);

				if (status == eStatus::SUCCESS) 
				{
					return eStatus::SUCCESS;
				}

				return eStatus::RUNNING;
			}

			HashString GetEditorName(void) const override { return "Until Success"; }
		};

		// The UntilFailure decorator repeats until the mpChild returns fail and then returns success.
		class UntilFailure : public Decorator
		{
		public:
			eStatus Update(float _deltaTime) override
			{
				const auto status = mpChild->Tick(_deltaTime);

				if (status == eStatus::FAIL) 
				{
					return eStatus::SUCCESS;
				}
				return eStatus::RUNNING;
			}

			HashString GetEditorName(void) const override { return "Until Failure"; }
		};

		// The HardLimiter decorator limits the number of times its child is ticked, based on a Max Count. 
		// When down to 0, the decorator returns a FAIL always and the child will never be run again.
		class HardLimiter : public Decorator
		{
		public:
			explicit HardLimiter(unsigned max_count = 1)
				: maxCount(max_count)
			{
			}

			void Init() override
			{
				if (maxCount > 0) 
					--maxCount;
			}

			eStatus Update(float _deltaTime) override
			{
				if (maxCount == 0) return  eStatus::FAIL;
				return mpChild->Tick(_deltaTime);
			}

			HashString GetEditorName(void) const override { return "Hard Limiter"; }

		private:
			unsigned maxCount;
		};

		// The Limiter decorator limits the number of times its child is ticked, based on a Max Count. 
		// If the children all returned a failure, and the count will be reset to the original max count - repeating the cycle.
		class Limiter : public Decorator
		{
		public:
			explicit Limiter(unsigned max_count = 1)
				: maxCount(max_count), currCount(max_count)
			{
			}

			void Init() override
			{
				if (currCount > 0)
					--currCount;
			}

			eStatus Update(float _deltaTime) override
			{
				if (currCount == 0) return eStatus::FAIL;
				return mpChild->Tick(_deltaTime);
			}

			void Exit(eStatus _status) override
			{
				if (_status == eStatus::FAIL) currCount = maxCount;
			}

			HashString GetEditorName(void) const override { return "Limiter"; }


		private:
			unsigned maxCount;
			unsigned currCount;
		};


		// The TimeHolder decorator forces a wait before ticking its child
		// Use this to mimic "thinking"
		class TimeHolder : public Decorator
		{
		public:
			explicit TimeHolder(float _timeLimit = 1.0f) : mCurrTime(_timeLimit), mTimeLimit(_timeLimit) {}

			eStatus Update(float _deltaTime) override
			{
				if (mCurrTime > 0.0f)
				{
					mCurrTime -= _deltaTime;
					return eStatus::RUNNING;
				}

				return mpChild->Tick(_deltaTime);
			}

			void Exit (eStatus) override
			{
				if (mCurrTime <= 0.0f) mCurrTime = mTimeLimit;
			}

			HashString GetEditorName(void) const override { return "Time Holder"; }

		private:
			float mCurrTime;
			float mTimeLimit;
		};
	}
}


#endif
