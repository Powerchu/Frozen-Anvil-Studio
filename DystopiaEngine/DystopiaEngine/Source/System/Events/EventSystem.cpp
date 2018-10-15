/* HEADER *********************************************************************************/
/*!
\file	EventSystem.cpp
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "System/Events/EventSystem.h"
#include <functional>

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
		std::swap(_rhs.mpModelEvent, mpModelEvent);
		std::swap(_rhs.mpGenericComparison, mpGenericComparison);
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

	void Event::Fire() const
	{
		for (auto& e : mArrEventCallbacks)
			e->Fire();
	}

	void Event::Bind(void(&_fn)(void))
	{
		EventCallback *p = new EventCallback{ _fn };
		mArrEventCallbacks.push_back(p);
	}

	void Event::UnbindAll()
	{
		for (auto& e : mArrEventCallbacks)
			delete e;
		mArrEventCallbacks.clear();
	}

	/*********************************************************** Event System ***********************************************************/

	EventSystem::EventSystem()
		: mArrEvents{}, mArrAllIDs{}, mArrPendingFireIDs{}
	{}

	EventSystem::~EventSystem()
	{}

	void EventSystem::Shutdown(void)
	{
		for (auto& e : mArrEvents)
			delete e;
		mArrEvents.clear();
		mArrAllIDs.clear();
		mArrPendingFireIDs.clear();
	}

	void EventSystem::PreInit(void)
	{}

	bool EventSystem::Init(void)
	{
		return true;
	}

	void EventSystem::PostInit(void)
	{}

	void EventSystem::FixedUpdate(float)
	{}

	void EventSystem::Update(float)
	{}

	void EventSystem::LoadDefaults(void)
	{}

	void EventSystem::LoadSettings(TextSerialiser& _ser)
	{
		UNUSED_PARAMETER(_ser);
	}

	EventID	EventSystem::CreateEvent(const char* _eventName)
	{
		EventID id = EventHash(_eventName);

		for (auto& e : mArrAllIDs)
		{
			if (e == id) return id;
		}

		Event* p = new Event{ id };
		mArrEvents.push_back(p);
		mArrAllIDs.push_back(id);
		return id;
	}

	void EventSystem::Fire(EventID _byID)
	{
		mArrPendingFireIDs.push_back(_byID);
	}

	void EventSystem::Fire(const char* _byName)
	{
		Fire(EventHash(_byName));
	}

	void EventSystem::FireAllPending()
	{
		for (auto& e : mArrPendingFireIDs)
		{
			FireNow(e);
		}
		mArrPendingFireIDs.clear();
	}

	void EventSystem::FireNow(EventID _byID) const
	{
		for (auto& e : mArrEvents)
		{
			if (_byID == e->GetID()) e->Fire();
		}
	}

	void EventSystem::FireNow(const char* _byName) const
	{
		FireNow(EventHash(_byName));
	}

	Event* EventSystem::GetEvent(EventID _byID) const
	{
		for (auto& e : mArrEvents)
		{
			if (e->GetID() == _byID) return e;
		}
		return nullptr;
	}

	Event* EventSystem::GetEvent(const char* _byName) const
	{
		return GetEvent(EventHash(_byName));
	}

	bool EventSystem::BindToEvent(EventID _byID, void(&_fn)(void)) const
	{
		Event* p = GetEvent(_byID);
		if (p)
		{
			p->Bind(_fn);
			return true;
		}
		return false;
	}

	bool EventSystem::BindToEvent(const char* _byName, void(&_fn)(void))
	{
		EventID id = EventHash(_byName);
		for (auto& e : mArrAllIDs)
		{
			if (id == e)
			{
				return BindToEvent(id, _fn);
			}
		}
		CreateEvent(_byName);
		return BindToEvent(id, _fn);
	}

	void EventSystem::UnBindAllFromEvent(EventID _byID) const
	{
		Event *p = GetEvent(_byID);

		if (p) p->UnbindAll();
	}

	void EventSystem::UnBindAllFromEvent(const char* _byName) const
	{
		UnBindAllFromEvent(EventHash(_byName));
	}

}




