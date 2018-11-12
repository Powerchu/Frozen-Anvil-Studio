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




