/* HEADER *********************************************************************************/
/*!
\file	EditorInput.h
\author Digipen (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#ifndef _EDITOR_INPUT_H_
#define _EDITOR_INPUT_H_
#include "EditorHotkeys.h"

namespace Dystopia
{
	class InputManager;
	class EditorInput
	{
	public:

		EditorInput(void);
		bool Init(void);
		void Poll(void);
		void Clear(void);
		void Shutdown(void);
		/*
		static unsigned short keys[3] = { eButton::KEYBOARD_LCTRL , eButton::KEYBOARD_LALT, eButton::KEYBOARD_LSHIFT };
		static Dystopia::eHotkeyState state[3] = { Dystopia::eHOTKEY_HOLD , Dystopia::eHOTKEY_HOLD, Dystopia::eHOTKEY_TRIGGERED };
		static Dystopia::EditorInput p;
		p.Init(mpInput);
		bool pass = p.Hotkeys(keys, state, 3);
		if (pass)
			int i = 5;
		*/
		InputManager* Get(void);
		bool Hotkeys(const unsigned short* _arrKeys, const eHotkeyState* _arrState, unsigned int _size) const;
	private:
		InputManager *mpInput;
	};
}

#endif //_EDITOR_INPUT_H_
#endif //EDITOR
