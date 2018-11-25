/* HEADER *********************************************************************************/
/*!
\file	Events.cpp
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief
BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/

#include "System/Events/Events.h"
#include "Utility/Utility.h"
#include <cstdlib>

namespace Dystopia
{
	EventID EventHash(const char* _eventName)
	{
		EventID hash = OFFSET_BASIS;
		size_t len = strlen(_eventName) + 1;
		for (size_t i = 0; i < len; ++i)
		{
			hash ^= *_eventName++;
			hash *= FNV_PRIME;
		}
		return hash;
	}

	/*********************************************************** Event Callback ***********************************************************/
	EventCallback::EventCallback()
		: mpModelEvent{ nullptr }, mpGenericComparison{ nullptr }
	{}

	EventCallback::EventCallback(void(&_rhs)(void))
		: mpModelEvent{ new Model<void(void)>{ _rhs } }, mpGenericComparison{ nullptr }
	{}

	EventCallback::~EventCallback()
	{
		if (mpModelEvent) delete mpModelEvent;
		mpGenericComparison = nullptr;
		mpModelEvent = nullptr;
	}

	void EventCallback::Fire() const
	{
		if (mpModelEvent)
			mpModelEvent->Fire();
	}

	EventCallback& EventCallback::operator=(const EventCallback& _rhs)
	{
		Concept* temp = mpModelEvent;
		mpModelEvent = _rhs.mpModelEvent->Duplicate();
		mpGenericComparison = _rhs.mpGenericComparison;
		delete temp;
		return *this;
	}

	EventCallback& EventCallback::operator=(EventCallback&& _rhs)
	{
		Ut::Swap(_rhs.mpModelEvent, mpModelEvent);
		Ut::Swap(_rhs.mpGenericComparison, mpGenericComparison);
		return *this;
	}

	/*********************************************************** Event Data ***********************************************************/
	Event::Event(EventID _byID)
		: mID{ _byID }
	{}

	Event::Event(const char *_ByName)
		: mID{ EventHash(_ByName) }
	{}

	Event::~Event()
	{
		for (auto& e : mArrEventCallbacks)
			delete e;
	}

	EventID Event::GetID() const
	{
		return mID;
	}



	//void Event::Bind(void(&_fn)(void))
	//{
	//	CallBackEvent *p = new CallBackEvent{ _fn };
	//	mArrEventCallbacks.push_back(p);
	//}

	void Event::UnbindAll()
	{
		for (auto& e : mArrEventCallbacks)
			delete e;
		mArrEventCallbacks.clear();
	}

}

