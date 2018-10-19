/* HEADER *********************************************************************************/
/*!
\file	EventSystem.h
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief

possible example of a use case : 

struct X
{
	X(EventSystem* e)
		:es{ e }
	{
		es->BindToEvent("EventTester1", &X::goo, this);
		es->BindToEvent("EventTester2", &X::foo, this);
		es->BindToEvent("EventTester3", &X::hoo, this);
	}
	~X()
	{
		es->UnBindFromEvent("EventTester1", this);
		es->UnBindFromEvent("EventTester2", this);
		es->UnBindFromEvent("EventTester3", this);
	}
	EventSystem* es;
	void goo() { std::cout << "Member function goo" << std::endl; }
	void hoo() { std::cout << "Member function hoo" << std::endl; }
	void foo() { std::cout << "Member function foo" << std::endl; }
};

Dystopia::EventSystem *es = Dystopia::EventSystem::GetInstance();
{
	Dystopia::X x1{ es };
	es->Fire("EventTester1");
	es->Fire("EventTester2");
	es->Fire("EventTester3");
	es->FireAllPending();
}
es->Shutdown();
delete es;

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _EVENTSYSTEM_H_
#define _EVENTSYSTEM_H_
#include "System/Events/Events.h"
#include "System/Base/Systems.h"

namespace Dystopia
{
	class EventSystem : public Systems
	{
	public:
		EventSystem();
		~EventSystem();

		void		PreInit(void);
		bool		Init(void);
		void		PostInit(void);

		void		FixedUpdate(float);
		void		Update(float);
		void		Shutdown(void);

		void		LoadDefaults(void);
		void		LoadSettings(TextSerialiser&);

		EventID		CreateEvent(const char* _eventName);
		void		Fire(EventID _byID);
		void		Fire(const char* _byName);
		void		FireAllPending();
		bool		BindToEvent(const char* _byName, void(&_fn)(void));

		void		FireNow(EventID _byID)								const;
		void		FireNow(const char* _byName)						const;
		Event*		GetEvent(EventID _byID)								const;
		Event*		GetEvent(const char* _byName)						const;
		bool		BindToEvent(EventID _byID, void(&_fn)(void))		const;
		void		UnBindAllFromEvent(EventID _byID)					const;
		void		UnBindAllFromEvent(const char* _byName)				const;

		template<class Caller>
		bool BindToEvent(EventID _byID, void(Caller::*_fn)(void), Caller* const _user) const
		{
			Event* p = GetEvent(_byID);
			if (p)
			{
				p->Bind(_fn, _user);
				return true;
			}
			return false;
		}

		template<class Caller>
		bool BindToEvent(const char* _byName, void(Caller::*_fn)(void), Caller* const _user)
		{
			EventID id = EventHash(_byName);
			for (auto& e : mArrAllIDs)
			{
				if (id == e)
				{
					return BindToEvent(id, _fn, _user);
				}
			}
			CreateEvent(_byName);
			return BindToEvent(id, _fn, _user);
		}

		template<class Caller>
		bool UnBindFromEvent(EventID _byID, Caller * const _user) const
		{
			Event* p = GetEvent(_byID);
			if (p)
			{
				p->Unbind(_user);
				return true;
			}
			return false;
		}

		template<class Caller>
		bool UnBindFromEvent(const char* _byName, Caller * const _user) const
		{
			return UnBindFromEvent(EventHash(_byName), _user);
		}

	private:
		AutoArray<Event*>	mArrEvents;
		AutoArray<EventID>	mArrAllIDs;
		AutoArray<EventID>	mArrPendingFireIDs;
	};
}


#endif //_EVENTSYSTEM_H_

