/* HEADER *********************************************************************************/
/*!
\file	InputSystem.cpp
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Creates and manages Win32 windows.

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "System\Input\InputSystem.h"
#include "System\Input\InputMap.h"
#include "System\Window\WindowManager.h"
#include "System\Window\Window.h"
#include "System\Driver\Driver.h"
#include "Math\Vector2.h"
#include "Math\Vector4.h"

#define WIN32_LEAN_AND_MEAN			// Exclude rare stuff from Window's header
#include <Windows.h>
#undef  WIN32_LEAN_AND_MEAN			// Stop define from spilling into code


void Dystopia::InputManager::LoadDefaultUserKeys(void)
{
	mButtonMap[eUserButton::BUTTON_UP    ] = eButton::KEYBOARD_UP;
	mButtonMap[eUserButton::BUTTON_DOWN  ] = eButton::KEYBOARD_DOWN;
	mButtonMap[eUserButton::BUTTON_LEFT  ] = eButton::KEYBOARD_LEFT;
	mButtonMap[eUserButton::BUTTON_RIGHT ] = eButton::KEYBOARD_RIGHT;

	mButtonMap[eUserButton::MOUSE_L] = eButton::MOUSE_LEFT;
	mButtonMap[eUserButton::MOUSE_R] = eButton::MOUSE_RIGHT;
	mButtonMap[eUserButton::MOUSE_M] = eButton::MOUSE_MIDDLE;

	mButtonMap[eUserButton::BUTTON_PAUSE ] = eButton::KEYBOARD_P;
	mButtonMap[eUserButton::BUTTON_CANCEL] = eButton::KEYBOARD_ESCAPE;
}

Dystopia::InputManager::InputManager(void) :
	mButtonMap{ static_cast<unsigned>(eUserButton::TOTAL_USERBUTTONS) }
{

}

Dystopia::InputManager::~InputManager(void)
{

}


bool Dystopia::InputManager::Init(void)
{
	return true;
}

void Dystopia::InputManager::Update(float)
{
	for (unsigned n = 0; n < eUserButton::TOTAL_USERBUTTONS; ++n)
	{
		bool bState = (GetAsyncKeyState(mButtonMap[n].mnKey) & 0x8000) != 0;

		mButtonMap[n].mbTriggered =  bState && !mButtonMap[n].mbPressed;
		mButtonMap[n].mbReleased  = !bState &&  mButtonMap[n].mbPressed;
		mButtonMap[n].mbPressed   =  bState;
	}
}

void Dystopia::InputManager::Shutdown(void)
{

}

void Dystopia::InputManager::LoadDefaults(void)
{
	mButtonMap.clear();

	LoadDefaultUserKeys();
}

void Dystopia::InputManager::LoadSettings(TextSerialiser&)
{
}

void Dystopia::InputManager::MapUserButton(eUserButton _nBtn, eButton _nTo)
{
	mButtonMap[_nBtn] = _nTo;
}

bool Dystopia::InputManager::IsKeyTriggered(eUserButton _nBtn)
{
	return mButtonMap[_nBtn].mbTriggered;
}

bool Dystopia::InputManager::IsKeyPressed(eUserButton _nBtn)
{
	return mButtonMap[_nBtn].mbPressed;
}

bool Dystopia::InputManager::IsKeyReleased(eUserButton _nBtn)
{
	return mButtonMap[_nBtn].mbReleased;
}

Math::Vector2 Dystopia::InputManager::GetMousePosition(void)
{
	POINT pos;
	GetCursorPos(&pos);
	ScreenToClient(
		EngineCore::GetInstance()->GetSystem<WindowManager>()->GetMainWindow().GetWindowHandle(), 
		&pos
	);

	return Math::Vector2(pos.x * 1.f, pos.y * 1.f);
}

Math::Vector2 Dystopia::InputManager::GetMousePosition(const Dystopia::Window& _activeWindow)
{
	POINT pos;
	GetCursorPos(&pos);
	ScreenToClient(_activeWindow.GetWindowHandle(), &pos);

	return Math::Vector2(pos.x * 1.f, pos.y * 1.f);
}

Dystopia::InputManager::KeyBinding& Dystopia::InputManager::KeyBinding::operator = (eButton _nBtn)
{
	mnKey = _nBtn;
	return *this;
}


