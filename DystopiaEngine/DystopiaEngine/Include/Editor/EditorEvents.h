/* HEADER *********************************************************************************/
/*!
\file	EditorEvents.cpp
\author Digipen (100%)
\par    email: t.shannon\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#ifndef _EDITOR_EVENTS_H_
#define _EDITOR_EVENTS_H_
#include "DataStructure/Array.h"
#include "System/Events/EventSystem.h"

namespace Dystopia
{
	enum eEditorEvents : unsigned int
	{
		EDITOR_LCLICK,
		EDITOR_RCLICK,
		EDITOR_HOTKEY_UNDO,
		EDITOR_HOTKEY_REDO,
		EDITOR_HOTKEY_COPY,
		EDITOR_HOTKEY_CUT,
		EDITOR_HOTKEY_PASTE,
		EDITOR_HOTKEY_DELETE,
		EDITOR_HOTKEY_SAVE,
		EDITOR_HOTKEY_SAVEAS,
		EDITOR_HOTKEY_DLL_CHANGED,
		EDITOR_HOTKEY_PLAY,
		EDITOR_HOTKEY_STOP,
		EDITOR_SCROLL_UP,
		EDITOR_SCROLL_DOWN,
		EDITOR_SCENE_CHANGED,
		EDITOR_EVENT_LAST
	};

	class EditorEventHandler
	{
	public:
		EditorEventHandler();
		~EditorEventHandler();

		void Init();
		void Update(float);
		void Shutdown();
		void Fire(eEditorEvents);
		void FireNow(eEditorEvents);
		void FireAllPending();

		Event* GetEvent(eEditorEvents);

	private:
		struct EventMap
		{
			EventMap();

			EventID mEventID;
			eEditorEvents mEventEnum;
		};

		Array<EventMap, eEditorEvents::EDITOR_EVENT_LAST> mEventsMap;
		EventSystem *mpEventSystem;
	};
}


#endif // _EDITOR_EVENTS_H_
#endif //EDITOR


