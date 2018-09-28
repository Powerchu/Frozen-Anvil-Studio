/* HEADER *********************************************************************************/
/*!
\file	EditorInput.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Creates and manages Win32 windows.

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#ifndef _EDITOR_INPUT_H_
#define _EDITOR_INPUT_H_
#include "DataStructure\Array.h"    // Array
#include "Math\Vector2.h"           // Vector2
#include "System\Input\InputMap.h"
#include "System\Input\MouseData.h"

namespace Dystopia
{
	class Window;
	class EditorInput
	{
	public:
		EditorInput(void);
		~EditorInput(void);

		void Init(void);
		void Update(float _dt);
		void Shutdown(void);

		bool IsKeyTriggered(eEditorButton);
		bool IsKeyPressed(eEditorButton);
		bool IsKeyReleased(eEditorButton);

		Math::Vector2 GetMousePosition(void);
		Math::Vector2 GetMousePosition(const Window&);
		Math::Vector2 GetMouseDelta(void) const noexcept;
		float GetMouseWheel(void) const noexcept;

	private:

		struct KeyBinding
		{
			unsigned mnKey;

			bool mbTriggered;
			bool mbPressed;
			bool mbReleased;

			KeyBinding& operator = (eButton);
		};

		MouseData mMouseInput;
		Array<KeyBinding, eEditorButton::TOTAL_EDITOR_BUTTONS> mButtonMap;
	};
}

#endif		// _EDITOR_INPUT_H_
#endif		// EDITOR
