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

enum eEditorButton : unsigned short
{
	KEY_LMOUSE		,
	KEY_RMOUSE		,
	KEY_MMOUSE		,
	KEY_A			,
	KEY_B			,
	KEY_C			,
	KEY_D			,
	KEY_E			,
	KEY_F			,
	KEY_G			,
	KEY_H			,
	KEY_I			,
	KEY_J			,
	KEY_K			,
	KEY_L			,
	KEY_M			,
	KEY_N			,
	KEY_O			,
	KEY_P			,
	KEY_Q			,
	KEY_R			,
	KEY_S			,
	KEY_T			,
	KEY_U			,
	KEY_V			,
	KEY_W			,
	KEY_X			,
	KEY_Y			,
	KEY_Z			,
	KEY_F1			,
	KEY_F2			,
	KEY_F3			,
	KEY_F4			,
	KEY_F5			,
	KEY_F6			,
	KEY_F7			,
	KEY_F8			,
	KEY_F9			,
	KEY_F10			,
	KEY_F11			,
	KEY_F12			,
	KEY_0			,
	KEY_1			,
	KEY_2			,
	KEY_3			,
	KEY_4			,
	KEY_5			,
	KEY_6			,
	KEY_7			,
	KEY_8			,
	KEY_9			,
	KEY_NUMPAD_0	,
	KEY_NUMPAD_1	,
	KEY_NUMPAD_2	,
	KEY_NUMPAD_3	,
	KEY_NUMPAD_4	,
	KEY_NUMPAD_5	,
	KEY_NUMPAD_6	,
	KEY_NUMPAD_7	,
	KEY_NUMPAD_8	,
	KEY_NUMPAD_9	,
	KEY_BACKSPACE	,
	KEY_TAB			,
	KEY_ENTER		,
	KEY_SHIFT		,
	KEY_CTRL		,
	KEY_ALT			,
	KEY_ESCAPE		,
	KEY_LSHIFT		,
	KEY_RSHIFT		,
	KEY_LCTRL		,
	KEY_RCTRL		,
	KEY_LALT		,
	KEY_RALT		,
	KEY_SPACEBAR	,
	KEY_PAGEUP		,
	KEY_PAGEDOWN	,
	KEY_END			,
	KEY_HOME		,
	KEY_PRNTSRC		,
	KEY_INSERT		,
	KEY_DELETE		,
	KEY_LEFT		,
	KEY_UP			,
	KEY_RIGHT		,
	KEY_DOWN		,

	TOTAL_EDITOR_BUTTONS
};

#endif		// INCLUDE GUARD

