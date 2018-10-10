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


namespace
{
	constexpr float MOUSEWHEEL_SCALE = 1.f / 120.f;
}


void Dystopia::InputManager::LoadDefaultUserKeys(void)
{

}


Dystopia::InputManager::KeyboardState::operator void*()
{
	return reinterpret_cast<void*>(this);
}

Dystopia::InputManager::KeyboardState::operator unsigned long*()
{
	return reinterpret_cast<u32int*>(mKeyPressFlags);
}

Dystopia::InputManager::KeyboardState::operator const unsigned long*() const
{
	return reinterpret_cast<u32int const*>(mKeyPressFlags);
}

Dystopia::InputManager::InputManager(void)
	:mGamePad{ 0 }
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
	mGamePad.PollInputs();
	if (GetKeyboardState(storage))
	{
		KeyboardState::u32int * ptr      = static_cast<KeyboardState::u32int*>(mKeyBoardState);
		KeyboardState::u32int count = 0;
		for(auto const & elem : storage)
		{
			unsigned GroupIndex  = count / (sizeof(KeyboardState::u32int) * 8);
			unsigned BitShiftAmt = count % (sizeof(KeyboardState::u32int) * 8);
			/*Generate the Input Keypress Flags*/
			*(ptr + GroupIndex) |= static_cast<bool>(elem & 0x80) ? (0x00000001ui32) << BitShiftAmt: 0;
			++count;
		}
	}

	for (auto& control : mComponents)
	{
		if (control.GetOwner())
		{
			control.MovePlayer(_dt);
		}
	}
	/*Commenting this out makes the input smoother*/
	//SetKeyboardState(Reset);
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

	mPrevKeyBoardState = mKeyBoardState;
	for (auto & elem : mKeyBoardState.mKeyPressFlags)
		elem = 0;
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
	
}

void Dystopia::InputManager::MapButton(std::string const &_name, eButton _button)
{
	mButtonMapping[_name] = _button;
}
bool Dystopia::InputManager::IsKeyTriggered(eButton _Btn) const noexcept
{
	KeyboardState::u32int const * prev_ptr = static_cast<KeyboardState::u32int const *>(mPrevKeyBoardState);
	KeyboardState::u32int const * curr_ptr = static_cast<KeyboardState::u32int const *>(mKeyBoardState);

	KeyboardState::u32int BitChecker = (0x00000001ui32 << _Btn % (sizeof(KeyboardState::u32int) * 8));
	KeyboardState::u32int GrpIndex   = _Btn / (sizeof(KeyboardState::u32int) * 8);

	bool prev = !(*(prev_ptr + GrpIndex) & BitChecker);
	bool cur  = (*(curr_ptr  + GrpIndex) & BitChecker);
	return prev & cur;
}

bool Dystopia::InputManager::IsKeyPressed(eButton _Btn) const noexcept
{
	KeyboardState::u32int const * prev_ptr = static_cast<KeyboardState::u32int const *>(mPrevKeyBoardState);
	KeyboardState::u32int const * curr_ptr = static_cast<KeyboardState::u32int const *>(mKeyBoardState);

	KeyboardState::u32int BitChecker = (0x00000001ui32 << _Btn % (sizeof(KeyboardState::u32int) * 8));
	KeyboardState::u32int GrpIndex   = _Btn / (sizeof(KeyboardState::u32int) * 8);

	return (*(prev_ptr + GrpIndex) & BitChecker) && (*(curr_ptr + GrpIndex) & BitChecker);
}

bool Dystopia::InputManager::IsKeyReleased(eButton _Btn) const noexcept
{
	KeyboardState::u32int const * prev_ptr = static_cast<KeyboardState::u32int const *>(mPrevKeyBoardState);
	KeyboardState::u32int const * curr_ptr = static_cast<KeyboardState::u32int const *>(mKeyBoardState);

	KeyboardState::u32int BitChecker = (0x00000001ui32 << _Btn % (sizeof(KeyboardState::u32int) * 8));
	KeyboardState::u32int GrpIndex = _Btn / (sizeof(KeyboardState::u32int) * 8);

	return (*(prev_ptr + GrpIndex) & BitChecker) & !(*(curr_ptr + GrpIndex)) & BitChecker;
}

bool Dystopia::InputManager::IsKeyTriggered(std::string const & _ButtonName) const noexcept
{
	auto iterator = mButtonMapping.find(_ButtonName);
	if (iterator == mButtonMapping.end())
	{
		return false;
	}
	if (iterator->second >= eButton::XBUTTON_DPAD_UP)
	{
		return mGamePad.IsKeyTriggered(iterator->second);
	}
	return IsKeyTriggered(iterator->second);
}

bool Dystopia::InputManager::IsKeyPressed(std::string const & _ButtonName) const noexcept
{
	auto iterator = mButtonMapping.find(_ButtonName);
	if (iterator == mButtonMapping.end())
	{
		return false;
	}
	if (iterator->second >= eButton::XBUTTON_DPAD_UP)
	{
		return mGamePad.IsKeyPressed(iterator->second);
	}
	return IsKeyPressed(iterator->second);
}

bool Dystopia::InputManager::IsKeyReleased(std::string const & _ButtonName) const noexcept
{
	auto iterator = mButtonMapping.find(_ButtonName);
	if (iterator == mButtonMapping.end())
	{
		return false;
	}
	if (iterator->second >= eButton::XBUTTON_DPAD_UP)
	{
		return mGamePad.IsKeyReleased(iterator->second);
	}
	return IsKeyReleased(iterator->second);
}

float Dystopia::InputManager::GetAnalogY(int _state) const
{
	return mGamePad.GetAnalogY(_state);
}

float Dystopia::InputManager::GetAnalogX(int _state) const
{
	return mGamePad.GetAnalogX(_state);
}

float Dystopia::InputManager::GetTriggers(int _state) const
{
	return mGamePad.GetTriggers(_state);
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
