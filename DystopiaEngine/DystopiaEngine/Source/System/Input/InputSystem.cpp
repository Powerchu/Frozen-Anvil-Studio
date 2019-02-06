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
#include "System/Time/TimeSystem.h"
#include "Math/Vectors.h"

#include <utility>

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN			// Exclude rare stuff from Window's header
#include <Windows.h>
#undef ERROR
#include "Object/ObjectFlags.h"
#include "System/Profiler/ProfilerAction.h"
#include "System/Time/ScopedTimer.h"
#if EDITOR
#include "Editor/EGUI.h"
#include "Editor/EditorMain.h"
#include "Editor/RuntimeMeta.h"
#endif
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
	MapButton("Horizontal", KEYBOARD_RIGHT, KEYBOARD_LEFT, XBUTTON_DPAD_RIGHT, XBUTTON_DPAD_LEFT);
	MapButton("Vertical", KEYBOARD_UP, KEYBOARD_DOWN, XBUTTON_DPAD_UP, XBUTTON_DPAD_DOWN);
	MapButton("L Stick Horizontal", 2, 0);
	MapButton("L Stick Vertical", 2, 1);
	MapButton("Camera X", 2, 2);
	MapButton("Camera Y", 2, 3);
	MapButton("C_Roll", 2, 4);
	MapButton("Right Trigger", 2, 5);
	//MapButton("SetForm", KEYBOARD_Q, NONE, XBUTTON_LEFT_SHOULDER);
	//MapButton("SetForce", KEYBOARD_E, NONE, XBUTTON_RIGHT_SHOULDER);
	MapButton("ChangeSkill", KEYBOARD_Q, NONE, XBUTTON_LEFT_SHOULDER);
	MapButton("Attack", KEYBOARD_C, NONE, XBUTTON_X);
	MapButton("Jump", KEYBOARD_SPACEBAR, NONE, XBUTTON_A);
	MapButton("Skill Y", KEYBOARD_Z, NONE, XBUTTON_Y);
	MapButton("Skill B", KEYBOARD_X, NONE, XBUTTON_B);
	MapButton("Pause", KEYBOARD_ESCAPE, NONE, XBUTTON_START);
	MapButton("Select", KEYBOARD_ENTER, NONE, XBUTTON_A);
	MapButton("Back", KEYBOARD_ESCAPE, NONE, XBUTTON_B);
	MapButton("Roll", KEYBOARD_CTRL, NONE, XBUTTON_RIGHT_SHOULDER);
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
	:mGamePad{ 0 }, mbCanVibrate{ true }
{

}

Dystopia::InputManager::~InputManager(void)
{

}


bool Dystopia::InputManager::Init(void)
{
	EngineCore::GetInstance()->GetSystem<WindowManager>()->RegisterMouseData(&mMouseInput);
	mGamePad.PollInputs();
	mGamePad.StopVibrate();
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
			if (btn.mbSnapping && btn.mfRetValue < 0.f)
			{
				btn.mfRetValue = 0.0f;
			}
			btn.Modify(1 * btn.mfSensitivity * _dt);
		}

		if (GetKey(btn.mNegBtn) || GetKey(btn.mAltNegBtn))
		{
			if (btn.mbSnapping && btn.mfRetValue > 0.f)
			{
				btn.mfRetValue = 0.0f;
			}
			btn.Modify(-1 * btn.mfSensitivity * _dt);
		}

		if (!GetKey(btn.mPosBtn) && !GetKey(btn.mAltPosBtn)
			&& !GetKey(btn.mNegBtn) && !GetKey(btn.mAltNegBtn))
		{
			btn.Update(_dt);
		}

	}

	if (mGamePad.mfTimer >= 0.0f)
		mGamePad.mfTimer -= mfDecay * _dt;

	if (mbCanVibrate)
		mGamePad.VibrateHelper();

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
	{
		elem = 0;
	}
	mMouseInput.mnWheel = 0;
}

void Dystopia::InputManager::LoadDefaults(void)
{
	LoadDefaultUserKeys();
}

void Dystopia::InputManager::LoadSettings(DysSerialiser_t& _in)
{
	mAxisMapping.clear();
	int mapSize = 0;
	_in >> mapSize;

	for(int i = 0; i < mapSize; ++i)
	{
		HashString name;
		HashString mPosDes;
		HashString mNegDes;
		int mPosBtn;
		int mNegBtn;
		int mAltPosBtn;
		int mAltNegBtn;
		float grav;
		float deadR;
		float sens;
		int type;
		int axis;
		bool snap;
		bool invert;

		_in >> name;
		_in >> mPosDes;
		_in >> mNegDes;
		_in >> mPosBtn;
		_in >> mNegBtn;
		_in >> mAltPosBtn;
		_in >> mAltNegBtn;
		_in >> grav;
		_in >> deadR;
		_in >> sens;
		_in >> type;
		_in >> axis;
		_in >> snap;
		_in >> invert;

		MapButton(VirtualButton(name, mPosDes, mNegDes, static_cast<eButton>(mPosBtn), static_cast<eButton>(mNegBtn)
								, static_cast<eButton>(mAltPosBtn), static_cast<eButton>(mAltNegBtn), grav, deadR, sens, type, axis, snap, invert));
	}
}

void Dystopia::InputManager::SaveSettings(DysSerialiser_t& _out)
{
	_out << mAxisMapping.size();
	for (auto &[key, val] : mAxisMapping)
	{
		_out << val.mName;
		_out << val.mPosDescription;
		_out << val.mNegDescription;
		_out << static_cast<int>(val.mPosBtn);
		_out << static_cast<int>(val.mNegBtn);
		_out << static_cast<int>(val.mAltPosBtn);
		_out << static_cast<int>(val.mAltNegBtn);
		_out << val.mfGravity;
		_out << val.mfDeadRange;
		_out << val.mfSensitivity;
		_out << val.TypeSelectedInd;
		_out << val.AxisSelectedInd;
		_out << val.mbSnapping;
		_out << val.mbInvert;
	}
}


void Dystopia::InputManager::MapUserButton(eUserButton, eButton ) const
{
	
}

void Dystopia::InputManager::MapButton(const char* _name, eButton _posBtn, eButton _negBtn, eButton _altPosBtn, eButton _altNegBtn)
{
	//mButtonMapping[_name] = _button;
	mAxisMapping[_name] = VirtualButton(_name, _posBtn, _negBtn, _altPosBtn, _altNegBtn);
}

void Dystopia::InputManager::MapButton(const char* _name, int _type, int _axis)
{
	//mButtonMapping[_name] = _button;
	mAxisMapping[_name] = VirtualButton(_name, _type,_axis);
}

void Dystopia::InputManager::MapButton(const VirtualButton& _vBtn)
{
	mAxisMapping[_vBtn.mName] = _vBtn;
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

_DLL_EXPORT bool Dystopia::InputManager::GetButtonDown(const char* _KeyName, bool isNeg) const noexcept
{
	const auto iterator = mAxisMapping.find(_KeyName);
	if (iterator == mAxisMapping.end())
	{
		return false;
	}
	/*if (iterator->second.mPosBtn >= eButton::XBUTTON_DPAD_UP)
	{
		return mGamePad.IsKeyTriggered(static_cast<eButton>(iterator->second.mPosBtn - eButton::XBUTTON_DPAD_UP));
	}
	if (iterator->second.mAltPosBtn >= eButton::XBUTTON_DPAD_UP)
	{
		return mGamePad.IsKeyTriggered(static_cast<eButton>(iterator->second.mAltPosBtn - eButton::XBUTTON_DPAD_UP));
	}*/
	if (!isNeg)
	{
		return GetKeyDown(iterator->second.mPosBtn) || GetKeyDown(iterator->second.mAltPosBtn)
			|| mGamePad.IsKeyTriggered(static_cast<eButton>(iterator->second.mPosBtn - eButton::XBUTTON_DPAD_UP))
			|| mGamePad.IsKeyTriggered(static_cast<eButton>(iterator->second.mAltPosBtn - eButton::XBUTTON_DPAD_UP));
	}
	else
	{
		return GetKeyDown(iterator->second.mNegBtn) || GetKeyDown(iterator->second.mAltNegBtn)
			|| mGamePad.IsKeyTriggered(static_cast<eButton>(iterator->second.mNegBtn - eButton::XBUTTON_DPAD_UP))
			|| mGamePad.IsKeyTriggered(static_cast<eButton>(iterator->second.mAltNegBtn - eButton::XBUTTON_DPAD_UP));
	}

}

_DLL_EXPORT bool Dystopia::InputManager::GetButton(const char* _KeyName, bool isNeg) const noexcept
{
	const auto iterator = mAxisMapping.find(_KeyName);
	if (iterator == mAxisMapping.end())
	{
		return false;
	}
	/*if (iterator->second.mPosBtn >= eButton::XBUTTON_DPAD_UP)
	{
		return mGamePad.IsKeyPressed(static_cast<eButton>(iterator->second.mPosBtn - eButton::XBUTTON_DPAD_UP));
	}
	if (iterator->second.mAltPosBtn >= eButton::XBUTTON_DPAD_UP)
	{
		return mGamePad.IsKeyPressed(static_cast<eButton>(iterator->second.mAltPosBtn - eButton::XBUTTON_DPAD_UP));
	}*/
	if (!isNeg)
	{
		return GetKey(iterator->second.mPosBtn) || GetKey(iterator->second.mAltPosBtn)
			|| mGamePad.IsKeyPressed(static_cast<eButton>(iterator->second.mPosBtn - eButton::XBUTTON_DPAD_UP))
			|| mGamePad.IsKeyPressed(static_cast<eButton>(iterator->second.mAltPosBtn - eButton::XBUTTON_DPAD_UP));
	}
	else
	{
		return GetKey(iterator->second.mNegBtn) || GetKey(iterator->second.mAltNegBtn)
			|| mGamePad.IsKeyPressed(static_cast<eButton>(iterator->second.mNegBtn - eButton::XBUTTON_DPAD_UP))
			|| mGamePad.IsKeyPressed(static_cast<eButton>(iterator->second.mAltNegBtn - eButton::XBUTTON_DPAD_UP));
	}
	
}

_DLL_EXPORT bool Dystopia::InputManager::GetButtonUp(const char* _KeyName, bool isNeg) const noexcept
{
	const auto iterator = mAxisMapping.find(_KeyName);
	if (iterator == mAxisMapping.end())
	{
		return false;
	}
	/*if (iterator->second.mPosBtn >= eButton::XBUTTON_DPAD_UP)
	{
		return mGamePad.IsKeyReleased(static_cast<eButton>(iterator->second.mPosBtn - eButton::XBUTTON_DPAD_UP));
	}
	if (iterator->second.mAltPosBtn >= eButton::XBUTTON_DPAD_UP)
	{
		return mGamePad.IsKeyReleased(static_cast<eButton>(iterator->second.mAltPosBtn - eButton::XBUTTON_DPAD_UP));
	}*/
	if (!isNeg)
	{
		return GetKeyUp(iterator->second.mPosBtn) || GetKeyUp(iterator->second.mAltPosBtn)
			|| mGamePad.IsKeyReleased(static_cast<eButton>(iterator->second.mPosBtn - eButton::XBUTTON_DPAD_UP))
			|| mGamePad.IsKeyReleased(static_cast<eButton>(iterator->second.mAltPosBtn - eButton::XBUTTON_DPAD_UP));
	}
	else
	{
		return GetKeyUp(iterator->second.mNegBtn) || GetKeyUp(iterator->second.mAltNegBtn)
			|| mGamePad.IsKeyReleased(static_cast<eButton>(iterator->second.mNegBtn - eButton::XBUTTON_DPAD_UP))
			|| mGamePad.IsKeyReleased(static_cast<eButton>(iterator->second.mAltNegBtn - eButton::XBUTTON_DPAD_UP));
	}
	
}

_DLL_EXPORT float Dystopia::InputManager::GetAxis(const char * _BtnName) const noexcept
{
	const auto it = mAxisMapping.find(_BtnName);
	if (it == mAxisMapping.end())
	{
		return false;
	}
	if (it->second.TypeSelectedInd == 0) // Is Keyboard/Mouse/Controller Button (1 or 0 only)
	{
		return it->second.mfRetValue;
	}

	else if (it->second.TypeSelectedInd == 1) // Is Mouse Movement (X, Y or Scroll)
	{
		switch (it->second.AxisSelectedInd)
		{
		case 0: // X Axis
			return GetMouseDelta().x;
		case 1: // Y Axis
			return GetMouseDelta().y;
		case 2: // ScrollWheel
			return GetMouseWheel();
		default:
			break;
		}
	}

	// Is Joystick
	else
	{
		switch (it->second.AxisSelectedInd)
		{
		case 0: // Left Thumb X
			return GetAnalogX(0);
		case 1: // Left Thumb Y Axis
			return GetAnalogY(0);
		case 2: // Right Thumb X
			return GetAnalogX(1);
		case 3: // Right Thumb Y
			return GetAnalogY(1);
		case 4: // Left Trigger
			return GetTriggers(0);
		case 5: // Right Trigger
			return GetTriggers(1);
		default:
			break;
		}
	}

	return false;
}

bool Dystopia::InputManager::IsController() const
{
	return mGamePad.IsConnected();
}

void Dystopia::InputManager::SetVibrate(unsigned short _ltrg, unsigned short _rtrg) const
{
	mGamePad.Vibrate(_ltrg, _rtrg); //0-65534
}

void Dystopia::InputManager::StopVibrate() const
{
	mGamePad.StopVibrate();
}

void Dystopia::InputManager::InvokeVibration(float _intensity, float _decay, float _maxTime, float _balance)
{
	mfDecay = _decay;
	mGamePad.SetVibrationSetting(_intensity, _maxTime, _balance);
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

_DLL_EXPORT bool Dystopia::InputManager::IsVibrateOn() const
{
	return mbCanVibrate;
}

_DLL_EXPORT void Dystopia::InputManager::ToggleVibrate(bool _b)
{
	mbCanVibrate = _b;
}

#if EDITOR
void Dystopia::InputManager::EditorUI()
{
	if (EGUI::Display::Button("Add New Axis", { 128,24 }))
	{
		MapButton("[NEW AXIS]");
	}

	EGUI::SameLine();
	if (EGUI::Display::Button("Restore Defaults", {128,24}))
	{
		mAxisMapping.clear();
		LoadDefaults();
	}

	if (EGUI::Display::CollapsingHeader("Axes"))
	{
		if (ImGui::BeginPopupContextItem())
		{
			if (EGUI::Display::SelectableTxt("Remove All"))
			{
				mAxisMapping.clear();
			}
			ImGui::EndPopup();
		}

		static char buffer1[512];

		const auto tempComboStateCount = mAxisMapping.size();
		if (marrCombos.size() < tempComboStateCount)
		{
			for (auto i = 0; i < tempComboStateCount; i++)
			{
				marrCombos.Insert(ComboStruct());
			}
		}

		EGUI::Indent();

		int mapCount = 0;
		for (auto &[key, val] : mAxisMapping)
		{
			EGUI::PushID(val.mPosBtn);
			if (EGUI::Display::CollapsingHeader(key.c_str()))
			{
				/*if (ImGui::BeginPopupContextItem())
				{
					if (EGUI::Display::SelectableTxt("Remove This Key"))
					{
						mAxisMapping.erase(key);
					}
					ImGui::EndPopup();
				}*/

				marrCombos[mapCount].TypeSelectedInd = val.TypeSelectedInd;
				marrCombos[mapCount].AxisSelectedInd = val.AxisSelectedInd;

				EGUI::Indent();
				EGUI::PushLeftAlign(140.f);
				// Name
				strcpy_s(buffer1, 512, key.c_str());
				if (EGUI::Display::TextField("Name", buffer1, 128, true, 220))
				{
					val.mName = static_cast<const char *>(buffer1);
					auto nodeHandler = mAxisMapping.extract(key);
					nodeHandler.key() = val.mName;
					mAxisMapping.insert(Ut::Move(nodeHandler));
				}

				// Description
				strcpy_s(buffer1, 512, val.mPosDescription.c_str());
				if (EGUI::Display::TextField("(+) Desc", buffer1, 128, true, 220))
				{
					val.mPosDescription = static_cast<const char *>(buffer1);
				}
				strcpy_s(buffer1, 512, val.mNegDescription.c_str());
				if (EGUI::Display::TextField("(-) Desc", buffer1, 128, true, 220))
				{
					val.mNegDescription = static_cast<const char *>(buffer1);
				}

				/*
				 * Combo States
				 */
				static char buf1[128] = "Type Button Name Here...";

				/*
				 * Positive Button
				 */

				auto mBtnTemp = mButtonNames.find(val.mPosBtn);
				static HashString mBtnTempName;
				if (mBtnTemp != mButtonNames.end())
				{
					mBtnTempName = mBtnTemp->second;
					strcpy_s(buf1, 128, mBtnTempName.c_str());
				}

				EGUI::Display::Label("(+) Button");
				EGUI::SameLine(24.f);
				EGUI::PushID(static_cast<int>(mBtnTempName.id()));
				if (EGUI::Display::ComboFilter("btnCombo", buf1, IM_ARRAYSIZE(buf1), hints, IM_ARRAYSIZE(hints), marrCombos[mapCount].PosFilter))
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
				EGUI::PopID();


				/*
				* Negative Button
				*/

				mBtnTemp = mButtonNames.find(val.mNegBtn);
				if (mBtnTemp != mButtonNames.end())
				{
					mBtnTempName = mBtnTemp->second;
					strcpy_s(buf1, 128, mBtnTempName.c_str());
				}

				EGUI::Display::Label("(-) Button");
				EGUI::SameLine(24.f);
				EGUI::PushID(static_cast<int>(mBtnTempName.id()));
				if (EGUI::Display::ComboFilter("btnNegCombo", buf1, IM_ARRAYSIZE(buf1), hints, IM_ARRAYSIZE(hints), marrCombos[mapCount].NegFilter))
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
				EGUI::PopID();

				/*
				* Alt Positive Button
				*/

				mBtnTemp = mButtonNames.find(val.mAltPosBtn);
				if (mBtnTemp != mButtonNames.end())
				{
					mBtnTempName = mBtnTemp->second;
					strcpy_s(buf1, 128, mBtnTempName.c_str());
				}

				EGUI::Display::Label("Alt (+) Btn");
				EGUI::SameLine(16.f);
				EGUI::PushID(static_cast<int>(mBtnTempName.id()));
				if (EGUI::Display::ComboFilter("btnAltPosCombo", buf1, IM_ARRAYSIZE(buf1), hints, IM_ARRAYSIZE(hints), marrCombos[mapCount].AltPosFilter))
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
				EGUI::PopID();

				/*
				* Alt Negative Button
				*/

				mBtnTemp = mButtonNames.find(val.mAltNegBtn);
				if (mBtnTemp != mButtonNames.end())
				{
					mBtnTempName = mBtnTemp->second;
					strcpy_s(buf1, 128, mBtnTempName.c_str());
				}

				EGUI::Display::Label("Alt (-) Btn");
				EGUI::SameLine(16.f);
				EGUI::PushID(static_cast<int>(mBtnTempName.id()));
				if (EGUI::Display::ComboFilter("btnAltNegCombo", buf1, IM_ARRAYSIZE(buf1), hints, IM_ARRAYSIZE(hints), marrCombos[mapCount].AltNegFilter))
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
				EGUI::PopID();


				/* Other Settings */
				EGUI::Display::DragFloat("Gravity", &val.mfGravity, 0.05f, -100.f, 100.f,false,64.f);
				EGUI::Display::DragFloat("Dead", &val.mfDeadRange, 0.005f, -100.f, 100.f, false, 64.f);
				EGUI::Display::DragFloat("Sensitivity", &val.mfSensitivity, 0.05f, -100.f, 100.f, false, 64.f);

				// Snapping Checkbox
				EGUI::Display::CheckBox("Snap", &val.mbSnapping,true
										, "if we have input in opposite direction of current, \ndo we jump to neutral and continue from there?");
	
				// Invert Checkbox
				EGUI::Display::CheckBox("Invert", &val.mbInvert, true, "flip positive and negative?");

				static std::string textStates[3]{ "Key or Mouse Button", "Mouse Movement", "Joystick Axis" };
				if (EGUI::Display::DropDownSelection("Type", marrCombos[mapCount].TypeSelectedInd, textStates, 220.f))
				{
					val.TypeSelectedInd = marrCombos[mapCount].TypeSelectedInd;

					// If selected Type is not Joystick
					if (marrCombos[mapCount].TypeSelectedInd != 2)
					{
						val.mfGravity = 3.0f;
						val.mfDeadRange = 0.001f;
						val.mfSensitivity = 3.0f;
					}
					else
					{
						val.mfGravity = 0.0f;
						val.mfDeadRange = 0.19f;
						val.mfSensitivity = 1.0f;
					}
					
				};

				static std::string ncAxisStates[3]{ "X axis", "Y axis", "Scrollwheel"};
				static std::string cAxisStates[6]{ "Left Thumb X", "Left Thumb Y", "Right Thumb X", "Right Thumb Y", "L Trigger", "R Trigger" };

				// If selected Type is not Joystick
				if (marrCombos[mapCount].TypeSelectedInd != 2)
				{
					if (EGUI::Display::DropDownSelection("Axis", marrCombos[mapCount].AxisSelectedInd, ncAxisStates, 220.f))
					{
						val.AxisSelectedInd = marrCombos[mapCount].AxisSelectedInd, ncAxisStates;
					};
				}
				else // if selected Type is Joystick
				{
					if (EGUI::Display::DropDownSelection("Axis", marrCombos[mapCount].AxisSelectedInd, cAxisStates, 220.f))
					{
						val.AxisSelectedInd = marrCombos[mapCount].AxisSelectedInd, ncAxisStates;
					};
				}

				EGUI::PopLeftAlign();
				EGUI::UnIndent();
			}
			/*if (ImGui::BeginPopupContextItem())
			{
				if (EGUI::Display::SelectableTxt("Remove This Key"))
				{
					mAxisMapping.erase(key);
				}
				ImGui::EndPopup();
			}*/

			mapCount++;
			EGUI::PopID();
		}
		EGUI::UnIndent();
	}
}
#endif

Dystopia::InputManager::KeyBinding& Dystopia::InputManager::KeyBinding::operator = (eButton _nBtn)
{
	mnKey = _nBtn;
	return *this;
}
