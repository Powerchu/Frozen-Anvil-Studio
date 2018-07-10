/* HEADER *********************************************************************************/
/*!
\file	InputMap.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Definitions for InputManager. If you need a key that is not here, let me know.
	Since we want to avoid littering windows header everywhere,
	we use raw values instead of the defines.

	It is okay to add more into eUserButton yourself.
	It is NOT okay to add into eButton. Let me know.

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _INPUTMAP_H_
#define _INPUTMAP_H_

enum eUserButton : unsigned short
{
	BUTTON_UP, 
	BUTTON_DOWN,
	BUTTON_LEFT,
	BUTTON_RIGHT,

	BUTTON_PAUSE,
	BUTTON_CANCEL,

	MOUSE_L,
	MOUSE_R,
	MOUSE_M,

	TOTAL_USERBUTTONS
};

enum eButton : unsigned short
{
	MOUSE_LEFT			= 0x01,
	MOUSE_RIGHT,
	MOUSE_MIDDLE		= 0x04,
	MOUSE_X1,			// These exist but god knows what they bind to
	MOUSE_X2,

	KEYBOARD_A			= 0x41,
	KEYBOARD_B,
	KEYBOARD_C,
	KEYBOARD_D,
	KEYBOARD_E,
	KEYBOARD_F,
	KEYBOARD_G,
	KEYBOARD_H,
	KEYBOARD_I,
	KEYBOARD_J,
	KEYBOARD_K,
	KEYBOARD_L,
	KEYBOARD_M,
	KEYBOARD_N,
	KEYBOARD_O,
	KEYBOARD_P,
	KEYBOARD_Q,
	KEYBOARD_R,
	KEYBOARD_S,
	KEYBOARD_T,
	KEYBOARD_U,
	KEYBOARD_V,
	KEYBOARD_W,
	KEYBOARD_X,
	KEYBOARD_Y,
	KEYBOARD_Z,

	KEYBOARD_F1			= 0x70,
	KEYBOARD_F2,
	KEYBOARD_F3,
	KEYBOARD_F4,
	KEYBOARD_F5,
	KEYBOARD_F6,
	KEYBOARD_F7,
	KEYBOARD_F8,
	KEYBOARD_F9,
	KEYBOARD_F10,
	KEYBOARD_F11,
	KEYBOARD_F12,

	KEYBOARD_0			= 0x30,
	KEYBOARD_1,
	KEYBOARD_2,
	KEYBOARD_3,
	KEYBOARD_4,
	KEYBOARD_5,
	KEYBOARD_6,
	KEYBOARD_7,
	KEYBOARD_8,
	KEYBOARD_9,

	KEYBOARD_NUMPAD_0	= 0x60,
	KEYBOARD_NUMPAD_1,
	KEYBOARD_NUMPAD_2,
	KEYBOARD_NUMPAD_3,
	KEYBOARD_NUMPAD_4,
	KEYBOARD_NUMPAD_5,
	KEYBOARD_NUMPAD_6,
	KEYBOARD_NUMPAD_7,
	KEYBOARD_NUMPAD_8,
	KEYBOARD_NUMPAD_9,

	KEYBOARD_BACKSPACE	= 0x08,
	KEYBOARD_TAB,
	KEYBOARD_ENTER		= 0x0D,
	KEYBOARD_SHIFT		= 0x10,
	KEYBOARD_CTRL,
	KEYBOARD_ALT,
	KEYBOARD_ESCAPE		= 0x1B,
	KEYBOARD_LSHIFT		= 0xA0,
	KEYBOARD_RSHIFT,
	KEYBOARD_LCTRL,
	KEYBOARD_RCTRL,
	KEYBOARD_LALT,
	KEYBOARD_RALT,
	KEYBOARD_SPACEBAR	= 0x20,
	KEYBOARD_PAGEUP,
	KEYBOARD_PAGEDOWN,
	KEYBOARD_END,
	KEYBOARD_HOME,
	KEYBOARD_PRNTSRC	= 0x2C,
	KEYBOARD_INSERT,
	KEYBOARD_DELETE,

	KEYBOARD_LEFT		= 0x25,
	KEYBOARD_UP,
	KEYBOARD_RIGHT,
	KEYBOARD_DOWN
};



#endif		// INCLUDE GUARD

