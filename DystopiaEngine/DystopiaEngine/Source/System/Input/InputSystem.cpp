/* HEADER *********************************************************************************/
/*!
\file	InputSystem.cpp
\author Goh Kai Jie Keith (100%)
\par    email: keith.goh\@digipen.edu
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

#include "Math/Vector2.h"

#include <utility>

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN			// Exclude rare stuff from Window's header
#include <Windows.h>
#include "Object/ObjectFlags.h"
#include "System/Profiler/ProfilerAction.h"
#include "System/Time/ScopedTimer.h"
#include "Editor/EGUI.h"
#include "Editor/EditorMain.h"
#include "Editor/RuntimeMeta.h"
#undef  WIN32_LEAN_AND_MEAN			// Stop define from spilling into code
#undef NOMINMAX


namespace
{
	constexpr float MOUSEWHEEL_SCALE = 1.f / 120.f;

	// Button Mapping
	const char *hints[] =
	{
		"NONE",

		"KEYBOARD_A",
		"KEYBOARD_B",
		"KEYBOARD_C",
		"KEYBOARD_D",
		"KEYBOARD_E",
		"KEYBOARD_F",
		"KEYBOARD_G",
		"KEYBOARD_H",
		"KEYBOARD_I",
		"KEYBOARD_J",
		"KEYBOARD_K",
		"KEYBOARD_L",
		"KEYBOARD_M",
		"KEYBOARD_N",
		"KEYBOARD_O",
		"KEYBOARD_P",
		"KEYBOARD_Q",
		"KEYBOARD_R",
		"KEYBOARD_S",
		"KEYBOARD_T",
		"KEYBOARD_U",
		"KEYBOARD_V",
		"KEYBOARD_W",
		"KEYBOARD_X",
		"KEYBOARD_Y",
		"KEYBOARD_Z",
		"KEYBOARD_F1",
		"KEYBOARD_F2",
		"KEYBOARD_F3",
		"KEYBOARD_F4",
		"KEYBOARD_F5",
		"KEYBOARD_F6",
		"KEYBOARD_F7",
		"KEYBOARD_F8",
		"KEYBOARD_F9",
		"KEYBOARD_F10",
		"KEYBOARD_F11",
		"KEYBOARD_F12",
		"KEYBOARD_0",
		"KEYBOARD_1",
		"KEYBOARD_2",
		"KEYBOARD_3",
		"KEYBOARD_4",
		"KEYBOARD_5",
		"KEYBOARD_6",
		"KEYBOARD_7",
		"KEYBOARD_8",
		"KEYBOARD_9",
		"KEYBOARD_NUMPAD_0",
		"KEYBOARD_NUMPAD_1",
		"KEYBOARD_NUMPAD_2",
		"KEYBOARD_NUMPAD_3",
		"KEYBOARD_NUMPAD_4",
		"KEYBOARD_NUMPAD_5",
		"KEYBOARD_NUMPAD_6",
		"KEYBOARD_NUMPAD_7",
		"KEYBOARD_NUMPAD_8",
		"KEYBOARD_NUMPAD_9",

		"KEYBOARD_OEM_1",
		"KEYBOARD_OEM_PLUS",
		"KEYBOARD_OEM_COMMA",
		"KEYBOARD_OEM_MINUS",
		"KEYBOARD_OEM_PERIOD",

		"KEYBOARD_BACKSPACE",
		"KEYBOARD_TAB",
		"KEYBOARD_ENTER",
		"KEYBOARD_SHIFT",
		"KEYBOARD_CTRL",
		"KEYBOARD_ALT",
		"KEYBOARD_ESCAPE",
		"KEYBOARD_LSHIFT",
		"KEYBOARD_RSHIFT",
		"KEYBOARD_LCTRL",
		"KEYBOARD_RCTRL",
		"KEYBOARD_LALT",
		"KEYBOARD_RALT",
		"KEYBOARD_SPACEBAR",
		"KEYBOARD_PAGEUP",
		"KEYBOARD_PAGEDOWN",
		"KEYBOARD_END",
		"KEYBOARD_HOME",
		"KEYBOARD_PRNTSRC",
		"KEYBOARD_INSERT",
		"KEYBOARD_DELETE",
		"KEYBOARD_LEFT",
		"KEYBOARD_UP",
		"KEYBOARD_RIGHT",
		"KEYBOARD_DOWN",

		"MOUSE_LEFT",
		"MOUSE_RIGHT",
		"MOUSE_MIDDLE",
		"MOUSE_X1",
		"MOUSE_X2",

		"XBUTTON_DPAD_UP",
		"XBUTTON_DPAD_DOWN",
		"XBUTTON_DPAD_LEFT",
		"XBUTTON_DPAD_RIGHT",
		"XBUTTON_START",
		"XBUTTON_BACK",
		"XBUTTON_LEFT_THUMB",
		"XBUTTON_RIGHT_THUMB",
		"XBUTTON_LEFT_SHOULDER",
		"XBUTTON_RIGHT_SHOULDER",
		"XBUTTON_A",
		"XBUTTON_B",
		"XBUTTON_X",
		"XBUTTON_Y",
		"XBUTTON_LAST",
	};
}


void Dystopia::InputManager::LoadDefaultUserKeys(void)
{
	MapButton("Horizontal", KEYBOARD_RIGHT, KEYBOARD_LEFT);
	//MapButton("Vertical", KEYBOARD_UP, KEYBOARD_DOWN);
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

	for (auto &[key, btn] : mAxisMapping)
	{
		if (GetKey(btn.mPosBtn) || GetKey(btn.mAltPosBtn))
		{
			btn.Modify(1 * btn.mfSensitivity * _dt);
		}

		if (GetKey(btn.mNegBtn) || GetKey(btn.mAltNegBtn))
		{
			btn.Modify(-1 * btn.mfSensitivity * _dt);
		}

		if (!GetKey(btn.mPosBtn) && !GetKey(btn.mAltPosBtn)
			&& !GetKey(btn.mNegBtn) && !GetKey(btn.mAltNegBtn))
		{
			btn.Update(_dt);
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

void Dystopia::InputManager::MapButton(const char* _name, eButton _posBtn, eButton _negBtn)
{
	//mButtonMapping[_name] = _button;
	mAxisMapping[_name] = VirtualButton(_name, _posBtn, _negBtn);
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
	if (EGUI::Display::Button("Add New Axis", { 128,24 }))
	{
		EGUI::Display::OpenPopup("Add Axis", false);
	}

	if (EGUI::Display::StartPopupModal("Add Axis", "Adding New Axis"))
	{
		if (EGUI::Display::Button("Cancel", { 60,24 }))
		{
			EGUI::Display::CloseCurrentPopup();
		}

		EGUI::Display::EndPopup();
	}

	if (EGUI::Display::CollapsingHeader("Axes"))
	{
		
		static char buffer1[512];

		EGUI::Indent();
		for (auto &[key, val] : mAxisMapping)
		{
			if (EGUI::Display::CollapsingHeader(key.c_str()))
			{
				EGUI::Indent(10);
				EGUI::PushLeftAlign(128.f);
				// Name
				strcpy_s(buffer1, 512, key.c_str());
				if (EGUI::Display::TextField("Name", buffer1, 128, true, 150))
				{
					val.mName = static_cast<const char *>(buffer1);
					auto nodeHandler = mAxisMapping.extract(key);
					nodeHandler.key() = val.mName;
					mAxisMapping.insert(Ut::Move(nodeHandler));
				}

				// Description
				strcpy_s(buffer1, 512, val.mPosDescription.c_str());
				if (EGUI::Display::TextField("(+) Desc", buffer1, 128, true, 150))
				{
					val.mPosDescription = static_cast<const char *>(buffer1);
				}
				strcpy_s(buffer1, 512, val.mNegDescription.c_str());
				if (EGUI::Display::TextField("(-) Desc", buffer1, 128, true, 150))
				{
					val.mNegDescription = static_cast<const char *>(buffer1);
				}

				/*
				 * Combo States
				 */
				static EGUI::Display::ComboFilterState sP = { 0 };
				static EGUI::Display::ComboFilterState sN = { 0 };
				static EGUI::Display::ComboFilterState sAP = { 0 };
				static EGUI::Display::ComboFilterState sAN = { 0 };
				static char buf1[256] = "Type Button Name Here...";

				/*
				 * Positive Button
				 */

				auto mBtnTemp = mButtonNames.find(val.mPosBtn);
				static HashString mBtnTempName;
				if (mBtnTemp != mButtonNames.end())
				{
					mBtnTempName = mBtnTemp->second;
					strcpy_s(buf1, 256, mBtnTempName.c_str());
				}

				EGUI::Display::Label("(+) Button");
				EGUI::SameLine();
				if (EGUI::Display::ComboFilter("btnCombo", buf1, IM_ARRAYSIZE(buf1), hints, IM_ARRAYSIZE(hints), sP))
				{
					for (auto &[btn, name] : mButtonNames)
					{
						if (name == buf1)
						{
							val.MapBtn(btn);
							break;
						}
					}
				}

				/*
				* Negative Button
				*/

				mBtnTemp = mButtonNames.find(val.mNegBtn);
				if (mBtnTemp != mButtonNames.end())
				{
					mBtnTempName = mBtnTemp->second;
					strcpy_s(buf1, 256, mBtnTempName.c_str());
				}

				EGUI::Display::Label("(-) Button");
				EGUI::SameLine();
				if (EGUI::Display::ComboFilter("btnNegCombo", buf1, IM_ARRAYSIZE(buf1), hints, IM_ARRAYSIZE(hints), sN))
				{
					for (auto &[btn, name] : mButtonNames)
					{
						if (name == buf1)
						{
							val.MapBtn(btn, 1);
							break;
						}
					}
				}

				/*
				* Alt Positive Button
				*/

				mBtnTemp = mButtonNames.find(val.mAltPosBtn);
				if (mBtnTemp != mButtonNames.end())
				{
					mBtnTempName = mBtnTemp->second;
					strcpy_s(buf1, 256, mBtnTempName.c_str());
				}

				EGUI::Display::Label("Alt (+) Button");
				EGUI::SameLine();
				if (EGUI::Display::ComboFilter("btnAltPosCombo", buf1, IM_ARRAYSIZE(buf1), hints, IM_ARRAYSIZE(hints), sAP))
				{
					for (auto &[btn, name] : mButtonNames)
					{
						if (name == buf1)
						{
							val.MapAltBtn(btn);
							break;
						}
					}
				}

				/*
				* Alt Negative Button
				*/

				mBtnTemp = mButtonNames.find(val.mAltNegBtn);
				if (mBtnTemp != mButtonNames.end())
				{
					mBtnTempName = mBtnTemp->second;
					strcpy_s(buf1, 256, mBtnTempName.c_str());
				}

				EGUI::Display::Label("Alt (-) Button");
				EGUI::SameLine();
				if (EGUI::Display::ComboFilter("btnAltNegCombo", buf1, IM_ARRAYSIZE(buf1), hints, IM_ARRAYSIZE(hints), sAN))
				{
					for (auto &[btn, name] : mButtonNames)
					{
						if (name == buf1)
						{
							val.MapAltBtn(btn,1);
							break;
						}
					}
				}


				/* Other Settings */
				EGUI::Display::DragFloat("Gravity", &val.mfGravity, 0.05f, -100.f, 100.f,false,64.f);
				EGUI::Display::DragFloat("Dead", &val.mfDeadRange, 0.005f, -100.f, 100.f, false, 64.f);
				EGUI::Display::DragFloat("Sensitivity", &val.mfSensitivity, 0.05f, -100.f, 100.f, false, 64.f);

				EGUI::PopLeftAlign();
				EGUI::UnIndent(10);
			}
		}
		EGUI::UnIndent();
	}
}

Dystopia::InputManager::KeyBinding& Dystopia::InputManager::KeyBinding::operator = (eButton _nBtn)
{
	mnKey = _nBtn;
	return *this;
}
