/* HEADER *********************************************************************************/
/*!
\file	TimeSystem.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _TIME_SYSTEM_H_
#define _TIME_SYSTEM_H_

#include "System/Base/Systems.h"	 // Base class
#include "System/Time/Timer.h"
#include "DataStructure/Heap.h"
#include "DataStructure/AutoArray.h"
#include "Utility/Utility.h"		 // Forward

namespace Dystopia
{
	class TimeSystem : public Systems
	{
	public:

		TimeSystem(void);

		bool Init(void) override;
		void PostInit(void) override;
		
		void Update(float) override;
		void Shutdown(void) override;

		float GetDeltaTime(void) const noexcept;
		float GetFixedDeltaTime(void) const noexcept;

		void StopTime(void) noexcept;
		void ResumeTime(void) noexcept;
		void AdvanceTime(void) noexcept;
		bool ConsumeFixedDT(void) noexcept;

		void FlushQueue(void);

	private:

		struct QueueObject
		{
			Timer mDelay;
			virtual void Invoke(void) = 0;
			virtual ~QueueObject()    = 0;
		};

		template <class Invokable>
		struct InvokeMe : public QueueObject
		{
			Invokable mData;
			unsigned mnRepeat;

			InvokeMe(float, unsigned, Invokable&&);
			inline void Invoke(void);

			~InvokeMe() = default;
		};

		struct TimeZone
		{
			float mTimeScale;
		};

		bool mbSimulateTime;
		float mfSimulatedDT;
		int64_t mSimulatedDT;

		int64_t mFixedDT;
		int64_t mAccumulatedDT;

		Timer mTimeKeep;
		AutoArray<TimeZone> mTimeZones;

		Heap<QueueObject*> mPQueue;

		// Not Implemented!
		template <typename T> friend void Invoke(float _fSeconds, T&& _pInvokable);
		template <typename T> friend void InvokeRepeating(float _fSeconds, unsigned _nIterations, T&& _pInvokable);

		template <typename T>
		void TestFunction(float _fSeconds, T&& _pInvokable)
		{
			TestFunction(_fSeconds, 0, Ut::Forward<T>(_pInvokable));
		}

		template <typename T>
		void TestFunction(float _fSeconds, unsigned _nIterations, T&& _pInvokable)
		{
			mPQueue.Insert(new InvokeMe<T>{ _fSeconds, _nIterations, Ut::Forward<T>(_pInvokable) },
				[](const QueueObject* _lhs, const QueueObject* _rhs)->bool
				{
					return _lhs->mDelay.Time() < _rhs->mDelay.Time();
				}
			);
		}
	};


	/*! =================================================================
	\brief
	Calls any invokable object after a given delay.

	\param _fSeconds
	The approximate delay in seconds between this function call
	and the time of excution.

	\param _pInvokable
	Any invokable object, probably. Maybe except member functions.
	eg. function (pointers), functors, lambda expressions, etc.

	\return void
	No return value.
	=================================================================  */
	template <typename T>
	void Invoke(float _fSeconds, T&& _pInvokable);

	/*! =================================================================
	\brief
	Calls any invokable object after a given delay.

	\param _fSeconds
	The approximate delay in seconds between this function call
	and the time of excution and between each iteration.

	\param _nIterations
	Number of times to repeat.

	\param _pInvokable
	Any invokable object, probably. Maybe except member functions.
	eg. function (pointers), functors, lambda expressions, etc.

	\return void
	No return value.
	=================================================================  */
	template <typename T>
	void InvokeRepeating(float _fSeconds, unsigned _nIterations, T&& _pInvokable);
}






// ============================================ FUNCTION DEFINITIONS ============================================ // 


template <typename Invokable>
inline Dystopia::TimeSystem::InvokeMe<Invokable>::InvokeMe(float _fSeconds, unsigned _nRepeat, Invokable&& _pInvokable) :
	QueueObject{  }, mData{ _pInvokable }, mnRepeat{ _nRepeat }
{
	mDelay.Countdown(_fSeconds);
}

template <typename Invokable>
inline void Dystopia::TimeSystem::InvokeMe<Invokable>::Invoke(void)
{
	mData();

	if (mnRepeat)
	{
		// Invoke again
	}
}



#endif		// INCLUDE GUARD

