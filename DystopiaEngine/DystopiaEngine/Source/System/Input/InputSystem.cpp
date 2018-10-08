/* HEADER *********************************************************************************/
/*!
\file	InputSystem.cpp
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Creates and manages Win32 windows.

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "System/Input/InputSystem.h"
#include "System/Input/InputMap.h"
#include "System/Window/WindowManager.h"
#include "System/Window/Window.h"
#include "System/Driver/Driver.h"
#include "Component/CharacterController.h"

#include "Math/Vector2.h"
#include "Math/Vector4.h"

#include <utility>

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN			// Exclude rare stuff from Window's header
#include <Windows.h>
#include "Object/ObjectFlags.h"
#undef  WIN32_LEAN_AND_MEAN			// Stop define from spilling into code
#undef NOMINMAX

#if !_INPUT_MANAGER_TEST_CODE_
namespace
{
	constexpr float MOUSEWHEEL_SCALE = 1.f / 120.f;
}

void Dystopia::InputManager::LoadDefaultUserKeys(void)
{
	mButtonMap[eUserButton::BUTTON_UP    ] = eButton::KEYBOARD_UP;
	mButtonMap[eUserButton::BUTTON_DOWN  ] = eButton::KEYBOARD_DOWN;
	mButtonMap[eUserButton::BUTTON_LEFT  ] = eButton::KEYBOARD_LEFT;
	mButtonMap[eUserButton::BUTTON_RIGHT ] = eButton::KEYBOARD_RIGHT;

	mButtonMap[eUserButton::BUTTON_PAUSE] = eButton::KEYBOARD_P;
	mButtonMap[eUserButton::BUTTON_CANCEL] = eButton::KEYBOARD_ESCAPE;

	mButtonMap[eUserButton::MOUSE_L] = eButton::MOUSE_LEFT;
	mButtonMap[eUserButton::MOUSE_R] = eButton::MOUSE_RIGHT;
	mButtonMap[eUserButton::MOUSE_M] = eButton::MOUSE_MIDDLE;

	mButtonMap[eUserButton::BUTTON_SPACEBAR] = eButton::KEYBOARD_SPACEBAR;
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
#if !EDITOR
	EngineCore::GetInstance()->GetSystem<WindowManager>()->RegisterMouseData(&mMouseInput);
#endif 
	LoadDefaults();
	return true;
}

void Dystopia::InputManager::Update(float _dt)
{
	mMouseInput.mnWheel = 0;
	for (unsigned n = 0; n < eUserButton::TOTAL_USERBUTTONS; ++n)
	{
		bool bState = (GetAsyncKeyState(mButtonMap[n].mnKey) & 0x8000) != 0;

		mButtonMap[n].mbTriggered =  bState && !mButtonMap[n].mbPressed;
		mButtonMap[n].mbReleased  = !bState &&  mButtonMap[n].mbPressed;
		mButtonMap[n].mbPressed   =  bState;
	}

	for (auto& control : mComponents)
	{
		if (control.GetOwner())
		{
			control.MovePlayer(_dt);
		}
	}
}

void Dystopia::InputManager::Shutdown(void)
{

}

void Dystopia::InputManager::PostUpdate()
{
	for (auto& control : mComponents)
	{
		if (control.GetFlags() & FLAG_REMOVE)
		{
			mComponents.Remove(&control);
		}
	}
}

void Dystopia::InputManager::LoadDefaults(void)
{
	LoadDefaultUserKeys();
}

void Dystopia::InputManager::LoadSettings(TextSerialiser&)
{
}

void Dystopia::InputManager::MapUserButton(eUserButton _nBtn, eButton _nTo)
{
	mButtonMap[_nBtn] = _nTo;
}

bool Dystopia::InputManager::IsKeyTriggered(eUserButton _nBtn) const noexcept
{
	return mButtonMap[_nBtn].mbTriggered;
}

bool Dystopia::InputManager::IsKeyPressed(eUserButton _nBtn) const noexcept
{
	return mButtonMap[_nBtn].mbPressed;
}

bool Dystopia::InputManager::IsKeyReleased(eUserButton _nBtn) const noexcept
{
	return mButtonMap[_nBtn].mbReleased;
}

Math::Vector2 Dystopia::InputManager::GetMousePosition(void) const
{
	POINT pos;
	GetCursorPos(&pos);
	ScreenToClient(
		EngineCore::GetInstance()->GetSystem<WindowManager>()->GetMainWindow().GetWindowHandle(), 
		&pos
	);

	return Math::Vector2(pos.x * 1.f, pos.y * 1.f);
}

Math::Vector2 Dystopia::InputManager::GetMousePosition(const Dystopia::Window& _activeWindow) const
{
	POINT pos;
	GetCursorPos(&pos);
	ScreenToClient(_activeWindow.GetWindowHandle(), &pos);

	return Math::Vector2(pos.x * 1.f, pos.y * 1.f);
}

Math::Vector2 Dystopia::InputManager::GetMouseDelta(void) const noexcept
{
	return Math::Vector2{ mMouseInput.mfDeltaX, mMouseInput.mfDeltaY };
}

float Dystopia::InputManager::GetMouseWheel(void) const noexcept
{
	return mMouseInput.mnWheel * MOUSEWHEEL_SCALE;
}


Dystopia::InputManager::KeyBinding& Dystopia::InputManager::KeyBinding::operator = (eButton _nBtn)
{
	mnKey = _nBtn;
	return *this;
}
#else
namespace
{
	constexpr float MOUSEWHEEL_SCALE = 1.f / 120.f;
}


void Dystopia::InputManager::LoadDefaultUserKeys(void)
{
	mButtonMap[eUserButton::BUTTON_UP] = eButton::KEYBOARD_UP;
	mButtonMap[eUserButton::BUTTON_DOWN] = eButton::KEYBOARD_DOWN;
	mButtonMap[eUserButton::BUTTON_LEFT] = eButton::KEYBOARD_LEFT;
	mButtonMap[eUserButton::BUTTON_RIGHT] = eButton::KEYBOARD_RIGHT;

	mButtonMap[eUserButton::BUTTON_PAUSE] = eButton::KEYBOARD_P;
	mButtonMap[eUserButton::BUTTON_CANCEL] = eButton::KEYBOARD_ESCAPE;

	mButtonMap[eUserButton::MOUSE_L] = eButton::MOUSE_LEFT;
	mButtonMap[eUserButton::MOUSE_R] = eButton::MOUSE_RIGHT;
	mButtonMap[eUserButton::MOUSE_M] = eButton::MOUSE_MIDDLE;

	mButtonMap[eUserButton::BUTTON_SPACEBAR] = eButton::KEYBOARD_SPACEBAR;
}


Dystopia::InputManager::KeyboardState::operator void*()
{
	return reinterpret_cast<void*>(this);
}

Dystopia::InputManager::KeyboardState::operator unsigned long*()
{
	return reinterpret_cast<u64int*>(mKeyPressFlags);
}

Dystopia::InputManager::KeyboardState::operator const unsigned long*() const
{
	return reinterpret_cast<u64int const*>(mKeyPressFlags);
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
#if !EDITOR
	EngineCore::GetInstance()->GetSystem<WindowManager>()->RegisterMouseData(&mMouseInput);
#endif 
	LoadDefaults();
	return true;
}

void Dystopia::InputManager::Update(float _dt)
{
	using Type = BYTE[256];
	static Type storage{ 0 };

	/*DEPRECATED - REMEMBER TO REMOVE*/
	mMouseInput.mnWheel = 0;
	for (unsigned n = 0; n < eUserButton::TOTAL_USERBUTTONS; ++n)
	{
		bool bState = (GetAsyncKeyState(mButtonMap[n].mnKey) & 0x8000) != 0;

		mButtonMap[n].mbTriggered = bState && !mButtonMap[n].mbPressed;
		mButtonMap[n].mbReleased = !bState &&  mButtonMap[n].mbPressed;
		mButtonMap[n].mbPressed = bState;
	}
	/*******************************************************************************/

	if (GetKeyboardState(storage))
	{
		KeyboardState::u64int * ptr      = static_cast<KeyboardState::u64int*>(mKeyBoardState);
		//KeyboardState::u64int * prev_ptr = static_cast<KeyboardState::u64int*>(mPrevKeyBoardState);
		for(auto const & elem : storage)
		{
			unsigned GroupIndex = elem / sizeof(KeyboardState::u64int);
			/*Generate the Input Keypress Flags*/
			*(ptr + GroupIndex) |= (0x0000000000000000Ui64 | elem) << (elem % sizeof(KeyboardState::u64int));
		}
	}

	for (auto& control : mComponents)
	{
		if (control.GetOwner())
		{
			control.MovePlayer(_dt);
		}
	}
}

void Dystopia::InputManager::Shutdown(void)
{

}

void Dystopia::InputManager::PostUpdate()
{
	for (auto& control : mComponents)
	{
		if (control.GetFlags() & FLAG_REMOVE)
		{
			mComponents.Remove(&control);
		}
	}
}

void Dystopia::InputManager::LoadDefaults(void)
{
	LoadDefaultUserKeys();
}

void Dystopia::InputManager::LoadSettings(TextSerialiser&)
{
}

void Dystopia::InputManager::MapUserButton(eUserButton _nBtn, eButton _nTo)
{
	mButtonMap[_nBtn] = _nTo;
}

bool Dystopia::InputManager::IsKeyTriggered(eUserButton _nBtn) const noexcept
{
	return mButtonMap[_nBtn].mbTriggered;
}

bool Dystopia::InputManager::IsKeyPressed(eUserButton _nBtn) const noexcept
{
	return mButtonMap[_nBtn].mbPressed;
}

bool Dystopia::InputManager::IsKeyReleased(eUserButton _nBtn) const noexcept
{
	return mButtonMap[_nBtn].mbReleased;
}

bool Dystopia::InputManager::IsKeyTriggered(eButton _Btn) const noexcept
{
	KeyboardState::u64int const * prev_ptr = static_cast<KeyboardState::u64int const *>(mPrevKeyBoardState);
	KeyboardState::u64int const * curr_ptr = static_cast<KeyboardState::u64int const *>(mKeyBoardState);

	KeyboardState::u64int BitChecker = (0x0000000000000001i64 << _Btn % sizeof(KeyboardState::u64int));
	KeyboardState::u64int GrpIndex   = _Btn / sizeof(KeyboardState::u64int);

	return !(*(prev_ptr + GrpIndex) & BitChecker) & (*(curr_ptr + GrpIndex)) & BitChecker;
}

bool Dystopia::InputManager::IsKeyPressed(eButton _Btn) const noexcept
{
	KeyboardState::u64int const * prev_ptr = static_cast<KeyboardState::u64int const *>(mPrevKeyBoardState);
	KeyboardState::u64int const * curr_ptr = static_cast<KeyboardState::u64int const *>(mKeyBoardState);

	KeyboardState::u64int BitChecker = (0x0000000000000001i64 << _Btn % sizeof(KeyboardState::u64int));
	KeyboardState::u64int GrpIndex   = _Btn / sizeof(KeyboardState::u64int);

	return (*(prev_ptr + GrpIndex) & BitChecker) & (*(curr_ptr + GrpIndex)) & BitChecker;
}

bool Dystopia::InputManager::IsKeyReleased(eButton _Btn) const noexcept
{
	KeyboardState::u64int const * prev_ptr = static_cast<KeyboardState::u64int const *>(mPrevKeyBoardState);
	KeyboardState::u64int const * curr_ptr = static_cast<KeyboardState::u64int const *>(mKeyBoardState);

	KeyboardState::u64int BitChecker = (0x0000000000000001i64 << _Btn % sizeof(KeyboardState::u64int));
	KeyboardState::u64int GrpIndex = _Btn / sizeof(KeyboardState::u64int);

	return (*(prev_ptr + GrpIndex) & BitChecker) & (*(curr_ptr + GrpIndex)) & BitChecker;
}

Math::Vector2 Dystopia::InputManager::GetMousePosition(void) const
{
	POINT pos;
	GetCursorPos(&pos);
	ScreenToClient(
		EngineCore::GetInstance()->GetSystem<WindowManager>()->GetMainWindow().GetWindowHandle(),
		&pos
	);

	return Math::Vector2(pos.x * 1.f, pos.y * 1.f);
}

Math::Vector2 Dystopia::InputManager::GetMousePosition(const Dystopia::Window& _activeWindow) const
{
	POINT pos;
	GetCursorPos(&pos);
	ScreenToClient(_activeWindow.GetWindowHandle(), &pos);

	return Math::Vector2(pos.x * 1.f, pos.y * 1.f);
}

Math::Vector2 Dystopia::InputManager::GetMouseDelta(void) const noexcept
{
	return Math::Vector2{ mMouseInput.mfDeltaX, mMouseInput.mfDeltaY };
}

float Dystopia::InputManager::GetMouseWheel(void) const noexcept
{
	return mMouseInput.mnWheel * MOUSEWHEEL_SCALE;
}

Dystopia::InputManager::KeyBinding& Dystopia::InputManager::KeyBinding::operator = (eButton _nBtn)
{
	mnKey = _nBtn;
	return *this;
}
#endif
