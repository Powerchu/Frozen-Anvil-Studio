/* HEADER *********************************************************************************/
/*!
\file	EventSystem.h
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _EVENTSYSTEM_H_
#define _EVENTSYSTEM_H_

namespace Dystopia
{
	typedef unsigned int EventID;
	typedef void*		 EventHandle;
	constexpr EventID FNV_PRIME = 16777619u;
	constexpr EventID OFFSET_BASIS = 2166136261u;

	template<unsigned int N>
	constexpr EventID EventHashC(const char(&_eventName)[N], unsigned int I = N)
	{
		return I == 1 ? (OFFSET_BASIS ^ _eventName[0]) * FNV_PRIME :
						(EventHashC(_eventName, I - 1) ^ (_eventName[I - 1])) * FNV_PRIME;
	}

	class Event
	{
	public:


	private:

	};

	class EventSystem
	{
	public:

		bool Bind(EventID);
		bool Bind(const char*);
		void Fire(EventID);
		void Fire(const char*);
		EventID GetEvent(const char*);

	private:

	};
}

#endif //_EVENTSYSTEM_H_

