/* HEADER *********************************************************************************/
/*!
\file	EditorInput.cpp
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Creates and manages Win32 windows.

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "Editor\EditorInputs.h"
#include "System\Window\WindowManager.h"
#include "System\Window\Window.h"
#include "System\Driver\Driver.h"

#define WIN32_LEAN_AND_MEAN			// Exclude rare stuff from Window's header
#include <Windows.h>
#undef  WIN32_LEAN_AND_MEAN			// Stop define from spilling into code


namespace
{
	constexpr float MOUSEWHEEL_SCALE = 1.f / 120.f;
}

Dystopia::EditorInput::EditorInput(void) :
	mButtonMap{ static_cast<unsigned>(eEditorButton::TOTAL_EDITOR_BUTTONS) }
{
}

Dystopia::EditorInput::~EditorInput(void)
{
}

void Dystopia::EditorInput::Init(void)
{
	mButtonMap[eEditorButton::KEY_LMOUSE]		= eButton::MOUSE_LEFT;
	mButtonMap[eEditorButton::KEY_RMOUSE]		= eButton::MOUSE_RIGHT;
	mButtonMap[eEditorButton::KEY_MMOUSE]		= eButton::MOUSE_MIDDLE;
	mButtonMap[eEditorButton::KEY_A]			= eButton::KEYBOARD_A;
	mButtonMap[eEditorButton::KEY_B]			= eButton::KEYBOARD_B;
	mButtonMap[eEditorButton::KEY_C]			= eButton::KEYBOARD_C;
	mButtonMap[eEditorButton::KEY_D]			= eButton::KEYBOARD_D;
	mButtonMap[eEditorButton::KEY_E]			= eButton::KEYBOARD_E;
	mButtonMap[eEditorButton::KEY_F]			= eButton::KEYBOARD_F;
	mButtonMap[eEditorButton::KEY_G]			= eButton::KEYBOARD_G;
	mButtonMap[eEditorButton::KEY_H]			= eButton::KEYBOARD_H;
	mButtonMap[eEditorButton::KEY_I]			= eButton::KEYBOARD_I;
	mButtonMap[eEditorButton::KEY_J]			= eButton::KEYBOARD_J;
	mButtonMap[eEditorButton::KEY_K]			= eButton::KEYBOARD_K;
	mButtonMap[eEditorButton::KEY_L]			= eButton::KEYBOARD_L;
	mButtonMap[eEditorButton::KEY_M]			= eButton::KEYBOARD_M;
	mButtonMap[eEditorButton::KEY_N]			= eButton::KEYBOARD_N;
	mButtonMap[eEditorButton::KEY_O]			= eButton::KEYBOARD_O;
	mButtonMap[eEditorButton::KEY_P]			= eButton::KEYBOARD_P;
	mButtonMap[eEditorButton::KEY_Q]			= eButton::KEYBOARD_Q;
	mButtonMap[eEditorButton::KEY_R]			= eButton::KEYBOARD_R;
	mButtonMap[eEditorButton::KEY_S]			= eButton::KEYBOARD_S;
	mButtonMap[eEditorButton::KEY_T]			= eButton::KEYBOARD_T;
	mButtonMap[eEditorButton::KEY_U]			= eButton::KEYBOARD_U;
	mButtonMap[eEditorButton::KEY_V]			= eButton::KEYBOARD_V;
	mButtonMap[eEditorButton::KEY_W]			= eButton::KEYBOARD_W;
	mButtonMap[eEditorButton::KEY_X]			= eButton::KEYBOARD_X;
	mButtonMap[eEditorButton::KEY_Y]			= eButton::KEYBOARD_Y;
	mButtonMap[eEditorButton::KEY_Z]			= eButton::KEYBOARD_Z;
	mButtonMap[eEditorButton::KEY_F1]			= eButton::KEYBOARD_F1;
	mButtonMap[eEditorButton::KEY_F2]			= eButton::KEYBOARD_F2;
	mButtonMap[eEditorButton::KEY_F3]			= eButton::KEYBOARD_F3;
	mButtonMap[eEditorButton::KEY_F4]			= eButton::KEYBOARD_F4;
	mButtonMap[eEditorButton::KEY_F5]			= eButton::KEYBOARD_F5;
	mButtonMap[eEditorButton::KEY_F6]			= eButton::KEYBOARD_F6;
	mButtonMap[eEditorButton::KEY_F7]			= eButton::KEYBOARD_F7;
	mButtonMap[eEditorButton::KEY_F8]			= eButton::KEYBOARD_F8;
	mButtonMap[eEditorButton::KEY_F9]			= eButton::KEYBOARD_F9;
	mButtonMap[eEditorButton::KEY_F10]			= eButton::KEYBOARD_F10;
	mButtonMap[eEditorButton::KEY_F11]			= eButton::KEYBOARD_F11;
	mButtonMap[eEditorButton::KEY_F12]			= eButton::KEYBOARD_F12;
	mButtonMap[eEditorButton::KEY_0]			= eButton::KEYBOARD_0;
	mButtonMap[eEditorButton::KEY_1]			= eButton::KEYBOARD_1;
	mButtonMap[eEditorButton::KEY_2]			= eButton::KEYBOARD_2;
	mButtonMap[eEditorButton::KEY_3]			= eButton::KEYBOARD_3;
	mButtonMap[eEditorButton::KEY_4]			= eButton::KEYBOARD_4;
	mButtonMap[eEditorButton::KEY_5]			= eButton::KEYBOARD_5;
	mButtonMap[eEditorButton::KEY_6]			= eButton::KEYBOARD_6;
	mButtonMap[eEditorButton::KEY_7]			= eButton::KEYBOARD_7;
	mButtonMap[eEditorButton::KEY_8]			= eButton::KEYBOARD_8;
	mButtonMap[eEditorButton::KEY_9]			= eButton::KEYBOARD_9;
	mButtonMap[eEditorButton::KEY_NUMPAD_0]		= eButton::KEYBOARD_NUMPAD_0;
	mButtonMap[eEditorButton::KEY_NUMPAD_1]		= eButton::KEYBOARD_NUMPAD_1;
	mButtonMap[eEditorButton::KEY_NUMPAD_2]		= eButton::KEYBOARD_NUMPAD_2;
	mButtonMap[eEditorButton::KEY_NUMPAD_3]		= eButton::KEYBOARD_NUMPAD_3;
	mButtonMap[eEditorButton::KEY_NUMPAD_4]		= eButton::KEYBOARD_NUMPAD_4;
	mButtonMap[eEditorButton::KEY_NUMPAD_5]		= eButton::KEYBOARD_NUMPAD_5;
	mButtonMap[eEditorButton::KEY_NUMPAD_6]		= eButton::KEYBOARD_NUMPAD_6;
	mButtonMap[eEditorButton::KEY_NUMPAD_7]		= eButton::KEYBOARD_NUMPAD_7;
	mButtonMap[eEditorButton::KEY_NUMPAD_8]		= eButton::KEYBOARD_NUMPAD_8;
	mButtonMap[eEditorButton::KEY_NUMPAD_9]		= eButton::KEYBOARD_NUMPAD_9;
	mButtonMap[eEditorButton::KEY_BACKSPACE]	= eButton::KEYBOARD_BACKSPACE;
	mButtonMap[eEditorButton::KEY_TAB]			= eButton::KEYBOARD_TAB;
	mButtonMap[eEditorButton::KEY_ENTER]		= eButton::KEYBOARD_ENTER;
	mButtonMap[eEditorButton::KEY_SHIFT]		= eButton::KEYBOARD_SHIFT;
	mButtonMap[eEditorButton::KEY_CTRL]			= eButton::KEYBOARD_CTRL;
	mButtonMap[eEditorButton::KEY_ALT]			= eButton::KEYBOARD_ALT;
	mButtonMap[eEditorButton::KEY_ESCAPE]		= eButton::KEYBOARD_ESCAPE;
	mButtonMap[eEditorButton::KEY_LSHIFT]		= eButton::KEYBOARD_LSHIFT;
	mButtonMap[eEditorButton::KEY_RSHIFT]		= eButton::KEYBOARD_RSHIFT;
	mButtonMap[eEditorButton::KEY_LCTRL]		= eButton::KEYBOARD_LCTRL;
	mButtonMap[eEditorButton::KEY_RCTRL]		= eButton::KEYBOARD_RCTRL;
	mButtonMap[eEditorButton::KEY_LALT]			= eButton::KEYBOARD_LALT;
	mButtonMap[eEditorButton::KEY_RALT]			= eButton::KEYBOARD_RALT;
	mButtonMap[eEditorButton::KEY_SPACEBAR]		= eButton::KEYBOARD_SPACEBAR;
	mButtonMap[eEditorButton::KEY_PAGEUP]		= eButton::KEYBOARD_PAGEUP;
	mButtonMap[eEditorButton::KEY_PAGEDOWN]		= eButton::KEYBOARD_PAGEDOWN;
	mButtonMap[eEditorButton::KEY_END]			= eButton::KEYBOARD_END;
	mButtonMap[eEditorButton::KEY_HOME]			= eButton::KEYBOARD_HOME;
	mButtonMap[eEditorButton::KEY_PRNTSRC]		= eButton::KEYBOARD_PRNTSRC;
	mButtonMap[eEditorButton::KEY_INSERT]		= eButton::KEYBOARD_INSERT;
	mButtonMap[eEditorButton::KEY_DELETE]		= eButton::KEYBOARD_DELETE;
	mButtonMap[eEditorButton::KEY_LEFT]			= eButton::KEYBOARD_LEFT;
	mButtonMap[eEditorButton::KEY_UP]			= eButton::KEYBOARD_UP;
	mButtonMap[eEditorButton::KEY_RIGHT]		= eButton::KEYBOARD_RIGHT;
	mButtonMap[eEditorButton::KEY_DOWN]			= eButton::KEYBOARD_DOWN;

	mButtonMap[eEditorButton::KEY_MISC_1]		= eButton::KEYBOARD_OEM_1;
	mButtonMap[eEditorButton::KEY_MISC_PLUS]	= eButton::KEYBOARD_OEM_PLUS;
	mButtonMap[eEditorButton::KEY_MISC_COMMA]	= eButton::KEYBOARD_OEM_COMMA;
	mButtonMap[eEditorButton::KEY_MISC_MINUS]	= eButton::KEYBOARD_OEM_MINUS;
	mButtonMap[eEditorButton::KEY_MISC_PERIOD]	= eButton::KEYBOARD_OEM_PERIOD;

	EngineCore::GetInstance()->GetSystem<WindowManager>()->RegisterMouseData(&mMouseInput);
}

void Dystopia::EditorInput::Update(float)
{
	mMouseInput.mnWheel = 0;
	for (unsigned n = 0; n < eEditorButton::TOTAL_EDITOR_BUTTONS; ++n)
	{
		bool bState = (GetAsyncKeyState(mButtonMap[n].mnKey) & 0x8000) != 0;

		mButtonMap[n].mbTriggered = bState && !mButtonMap[n].mbPressed;
		mButtonMap[n].mbReleased = !bState &&  mButtonMap[n].mbPressed;
		mButtonMap[n].mbPressed = bState;
	}
}

void Dystopia::EditorInput::Shutdown(void)
{

}


bool Dystopia::EditorInput::IsKeyTriggered(eEditorButton _nBtn)
{
	return mButtonMap[_nBtn].mbTriggered;
}

bool Dystopia::EditorInput::IsKeyPressed(eEditorButton _nBtn)
{
	return mButtonMap[_nBtn].mbPressed;
}

bool Dystopia::EditorInput::IsKeyReleased(eEditorButton _nBtn)
{
	return mButtonMap[_nBtn].mbReleased;
}

Math::Vector2 Dystopia::EditorInput::GetMousePosition(void)
{
	POINT pos;
	GetCursorPos(&pos);
	ScreenToClient(
		EngineCore::GetInstance()->GetSystem<WindowManager>()->GetMainWindow().GetWindowHandle(),
		&pos
	);

	return Math::Vector2(pos.x * 1.f, pos.y * 1.f);
}

Math::Vector2 Dystopia::EditorInput::GetMousePosition(const Dystopia::Window& _activeWindow)
{
	POINT pos;
	GetCursorPos(&pos);
	ScreenToClient(_activeWindow.GetWindowHandle(), &pos);

	return Math::Vector2(pos.x * 1.f, pos.y * 1.f);
}

Dystopia::EditorInput::KeyBinding& Dystopia::EditorInput::KeyBinding::operator = (eButton _nBtn)
{
	mnKey = _nBtn;
	return *this;
}

Math::Vector2 Dystopia::EditorInput::GetMouseDelta(void) const noexcept
{
	return Math::Vector2{ mMouseInput.mfDeltaX, mMouseInput.mfDeltaY };
}

float Dystopia::EditorInput::GetMouseWheel(void) const noexcept
{
	return mMouseInput.mnWheel * MOUSEWHEEL_SCALE;
}




