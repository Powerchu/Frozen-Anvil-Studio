/* HEADER *********************************************************************************/
/*!
\file	EditorTab.cpp
\author Digipen (100%)
\par    email: t.shannon\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "Editor\EditorEvents.h"
#include <string>

namespace Dystopia
{
	EditorEventHandler::EventMap::EventMap()
		: mEventID{ 0 }, mEventEnum{ eEditorEvents::EDITOR_EVENT_LAST }
	{}

	EditorEventHandler::EditorEventHandler()
		: mpEventSystem{ new EventSystem{} }, mEventsMap{}
	{
	}

	EditorEventHandler::~EditorEventHandler()
	{
		if (mpEventSystem) Shutdown();
	}

	void EditorEventHandler::Init()
	{
		std::string name = "EditorSpecificEvent_";
		for (unsigned int i = 0; i < EDITOR_EVENT_LAST; ++i)
		{
			std::string eventName = name;
			eventName += i;
			mEventsMap[i].mEventID = mpEventSystem->CreateEvent(eventName.c_str());
			mEventsMap[i].mEventEnum = static_cast<eEditorEvents>(i);
		}
	}

	void EditorEventHandler::Update(float)
	{
	}

	void EditorEventHandler::Shutdown()
	{
		mpEventSystem->Shutdown();
		delete mpEventSystem;
		mpEventSystem = nullptr;
	}

	void EditorEventHandler::Fire(eEditorEvents _e)
	{
		GetEvent(_e)->Fire();
	}

	void EditorEventHandler::FireNow(eEditorEvents _e)
	{
		mpEventSystem->Fire(GetEvent(_e)->GetID());
	}

	void EditorEventHandler::FireAllPending()
	{
		mpEventSystem->FireAllPending();
	}

	Event* EditorEventHandler::GetEvent(eEditorEvents _e)
	{
		for (unsigned int i = 0; i < EDITOR_EVENT_LAST; ++i)
		{
			if (mEventsMap[i].mEventEnum == _e)
				return (mpEventSystem->GetEvent(mEventsMap[i].mEventID));
		}
		return nullptr;
	}

}

