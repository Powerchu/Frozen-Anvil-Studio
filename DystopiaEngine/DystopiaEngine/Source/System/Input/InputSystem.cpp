/* HEADER *********************************************************************************/
/*!
\file	InputSystem.cpp
\author Goh Kai Jie Keith (100%)
\par    email: keith.goh\@digipen.edu
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

#include "Math/Vector2.h"

#include <utility>

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN			// Exclude rare stuff from Window's header
#include <Windows.h>
#include "Object/ObjectFlags.h"
#include "System/Profiler/ProfilerAction.h"
#include "System/Time/ScopedTimer.h"
#include "Editor/EGUI.h"
#undef  WIN32_LEAN_AND_MEAN			// Stop define from spilling into code
#undef NOMINMAX


namespace
{
	constexpr float MOUSEWHEEL_SCALE = 1.f / 120.f;
}


void Dystopia::InputManager::LoadDefaultUserKeys(void)
{
	MapButton("Left", KEYBOARD_LEFT);
	MapButton("Right", KEYBOARD_RIGHT);
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
	EngineCore::GetInstance()->GetSystem<WindowManager>()->RegisterMouseData(&mMouseInput);
	LoadDefaults();
	mGamePad.PollInputs();
	return true;
}

void Dystopia::InputManager::Update(const float _dt)
{
	ScopedTimer<ProfilerAction> timeKeeper{ "Input System", "Update" };
	using Type = BYTE[256];
	static Type storage{ 0 };
	mGamePad.PollInputs();
	if (GetKeyboardState(storage))
	{
		KeyboardState::u32int * ptr      = static_cast<KeyboardState::u32int*>(mKeyBoardState);
		KeyboardState::u32int count = 0;
		for(auto const & elem : storage)
		{
			const unsigned GroupIndex  = count / (sizeof(KeyboardState::u32int) * 8);
			const unsigned BitShiftAmt = count % (sizeof(KeyboardState::u32int) * 8);
			/*Generate the Input Keypress Flags*/
			*(ptr + GroupIndex) |= static_cast<bool>(elem & 0x80) ? (0x00000001ui32) << BitShiftAmt: 0;
			++count;
		}
	}

	for (auto const&[key, btn] : mAxisMapping)
	{
		if (GetKey(btn.mPosBtn) || GetKey(btn.mAltPosBtn))
		{
			btn.Modify(1 * btn.mfSensitivity);
		}

		if (GetKey(btn.mNegBtn) || GetKey(btn.mAltNegBtn))
		{
			
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
	mPrevKeyBoardState = mKeyBoardState;
	for (auto & elem : mKeyBoardState.mKeyPressFlags)
		elem = 0;
	mMouseInput.mnWheel = 0;
}

void Dystopia::InputManager::LoadDefaults(void)
{
	LoadDefaultUserKeys();
}

void Dystopia::InputManager::LoadSettings(DysSerialiser_t&)
{

}

void Dystopia::InputManager::SaveSettings(DysSerialiser_t&)
{

}

void Dystopia::InputManager::MapUserButton(eUserButton, eButton )
{
	
}

void Dystopia::InputManager::MapButton(const char* _name, eButton _button)
{
	mButtonMapping[_name] = _button;
}

_DLL_EXPORT bool Dystopia::InputManager::GetKeyDown(eButton _Key) const noexcept
{
	if (_Key >= eButton::XBUTTON_DPAD_UP)
	{
		return mGamePad.IsKeyTriggered(static_cast<eButton>(_Key - eButton::XBUTTON_DPAD_UP));
	}
	KeyboardState::u32int const * prev_ptr = static_cast<KeyboardState::u32int const *>(mPrevKeyBoardState);
	KeyboardState::u32int const * curr_ptr = static_cast<KeyboardState::u32int const *>(mKeyBoardState);

	const KeyboardState::u32int BitChecker = (0x00000001ui32 << _Key % (sizeof(KeyboardState::u32int) * 8));
	const KeyboardState::u32int GrpIndex   = _Key / (sizeof(KeyboardState::u32int) * 8);

	const bool prev = !(*(prev_ptr + GrpIndex) & BitChecker);
	const bool cur  = (*(curr_ptr  + GrpIndex) & BitChecker);
	
	return prev & cur;
}

_DLL_EXPORT bool Dystopia::InputManager::GetKey(eButton _Key) const noexcept
{
	if (_Key >= eButton::XBUTTON_DPAD_UP)
	{
		return mGamePad.IsKeyPressed(static_cast<eButton>(_Key - eButton::XBUTTON_DPAD_UP));
	}
	KeyboardState::u32int const * prev_ptr = static_cast<KeyboardState::u32int const *>(mPrevKeyBoardState);
	KeyboardState::u32int const * curr_ptr = static_cast<KeyboardState::u32int const *>(mKeyBoardState);

	const KeyboardState::u32int BitChecker = (0x00000001ui32 << _Key % (sizeof(KeyboardState::u32int) * 8));
	const KeyboardState::u32int GrpIndex   = _Key / (sizeof(KeyboardState::u32int) * 8);

	return (*(prev_ptr + GrpIndex) & BitChecker) && (*(curr_ptr + GrpIndex) & BitChecker);
}

_DLL_EXPORT bool Dystopia::InputManager::GetKeyUp(eButton _Key) const noexcept
{
	if (_Key >= eButton::XBUTTON_DPAD_UP)
	{
		return mGamePad.IsKeyReleased(static_cast<eButton>(_Key - eButton::XBUTTON_DPAD_UP));
	}
	KeyboardState::u32int const * prev_ptr = static_cast<KeyboardState::u32int const *>(mPrevKeyBoardState);
	KeyboardState::u32int const * curr_ptr = static_cast<KeyboardState::u32int const *>(mKeyBoardState);

	const KeyboardState::u32int BitChecker = (0x00000001ui32 << _Key % (sizeof(KeyboardState::u32int) * 8));
	const KeyboardState::u32int GrpIndex = _Key / (sizeof(KeyboardState::u32int) * 8);

	return ((*(prev_ptr + GrpIndex) & BitChecker) && !(*(curr_ptr + GrpIndex) & BitChecker));
}

_DLL_EXPORT bool Dystopia::InputManager::GetButtonDown(const char* _KeyName) const noexcept
{
	const auto iterator = mAxisMapping.find(_KeyName);
	if (iterator == mAxisMapping.end())
	{
		return false;
	}
	if (iterator->second.mPosBtn >= eButton::XBUTTON_DPAD_UP)
	{
		return mGamePad.IsKeyTriggered(static_cast<eButton>(iterator->second.mPosBtn - eButton::XBUTTON_DPAD_UP));
	}
	if (iterator->second.mAltPosBtn >= eButton::XBUTTON_DPAD_UP)
	{
		return mGamePad.IsKeyTriggered(static_cast<eButton>(iterator->second.mAltPosBtn - eButton::XBUTTON_DPAD_UP));
	}
	return GetKeyDown(iterator->second.mPosBtn) || GetKeyDown(iterator->second.mAltPosBtn);
}

_DLL_EXPORT bool Dystopia::InputManager::GetButton(const char* _KeyName) const noexcept
{
	const auto iterator = mAxisMapping.find(_KeyName);
	if (iterator == mAxisMapping.end())
	{
		return false;
	}
	if (iterator->second.mPosBtn >= eButton::XBUTTON_DPAD_UP)
	{
		return mGamePad.IsKeyPressed(static_cast<eButton>(iterator->second.mPosBtn - eButton::XBUTTON_DPAD_UP));
	}
	if (iterator->second.mAltPosBtn >= eButton::XBUTTON_DPAD_UP)
	{
		return mGamePad.IsKeyPressed(static_cast<eButton>(iterator->second.mAltPosBtn - eButton::XBUTTON_DPAD_UP));
	}
	return GetKey(iterator->second.mPosBtn) || GetKey(iterator->second.mAltPosBtn);
}

_DLL_EXPORT bool Dystopia::InputManager::GetButtonUp(const char* _KeyName) const noexcept
{
	const auto iterator = mAxisMapping.find(_KeyName);
	if (iterator == mAxisMapping.end())
	{
		return false;
	}
	if (iterator->second.mPosBtn >= eButton::XBUTTON_DPAD_UP)
	{
		return mGamePad.IsKeyReleased(static_cast<eButton>(iterator->second.mPosBtn - eButton::XBUTTON_DPAD_UP));
	}
	if (iterator->second.mAltPosBtn >= eButton::XBUTTON_DPAD_UP)
	{
		return mGamePad.IsKeyReleased(static_cast<eButton>(iterator->second.mAltPosBtn - eButton::XBUTTON_DPAD_UP));
	}
	return GetKeyUp(iterator->second.mPosBtn) || GetKeyUp(iterator->second.mAltPosBtn);
}

_DLL_EXPORT float Dystopia::InputManager::GetAxis(const char * _BtnName) const noexcept
{
	const auto it = mAxisMapping.find(_BtnName);
	if (it == mAxisMapping.end())
	{
		return false;
	}
	return it->second.mfRetValue;
}

bool Dystopia::InputManager::IsController() const
{
	return mGamePad.IsConnected();
}

void Dystopia::InputManager::SetVibrate(unsigned short _ltrg, unsigned short _rtrg)
{
	mGamePad.Vibrate(_ltrg, _rtrg); //0-65534
}

void Dystopia::InputManager::StopVibrate()
{
	mGamePad.StopVibrate();
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

void Dystopia::InputManager::EditorUI()
{
	if (EGUI::Display::CollapsingHeader("Axes"))
	{
		for (auto const&[key, val] : mButtonMapping)
		{
			EGUI::Display::CollapsingHeader(key.c_str());
		}
	}
}

Dystopia::InputManager::KeyBinding& Dystopia::InputManager::KeyBinding::operator = (eButton _nBtn)
{
	mnKey = _nBtn;
	return *this;
}
