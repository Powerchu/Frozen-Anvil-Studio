/* HEADER *********************************************************************************/
/*!
\file	InputManager.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Creates and manages Win32 windows.

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _INPUT_MANAGER_H_
#define _INPUT_MANAGER_H_

#include "System/Base/Systems.h"    // Base Class
#include "Math/Vector2.h"           // Vector2
//#include "Math\Vector4.h"		    // Vector4
#include "System/Input/InputMap.h"
#include "System/Input/MouseData.h"
#include "System/Base/ComponentDonor.h"

#include "XGamePad.h"
#include "Globals.h"
#include <map>
#include <utility>
#include "Editor/EGUI.h"


namespace Dystopia
{
	class CharacterController;
	class Window;
	class TextSerialiser;

	class  InputManager : public Systems
	{
		struct VirtualButton;
	public:

		struct KeyboardState
		{
			using u32int = unsigned long;

			/*Cover all 256 Keys*/
			u32int mKeyPressFlags[8];

			operator void*();
			operator u32int*();
			operator const u32int* () const;
		};

		

		InputManager(void);
		~InputManager(void);

		bool Init(void) override;
		void Update(float _dt) override;
		void Shutdown(void) override;

		void PostUpdate() override;

		void LoadDefaults(void) override;
		void LoadSettings(DysSerialiser_t&) override;
		void SaveSettings(DysSerialiser_t&) override;

		_DLL_EXPORT void  MapUserButton(eUserButton, eButton) const;
		_DLL_EXPORT void  MapButton(const char* _name, eButton _posBtn, eButton _negBtn = NONE, eButton _altPosBtn = NONE, eButton _altNegBtn = NONE);
		_DLL_EXPORT void  MapButton(const char* _name, int _type, int _axis);
		_DLL_EXPORT void  MapButton(const VirtualButton& _vBtn);

		// Returns true while the user holds down the key identified by _Key.
		_DLL_EXPORT bool  GetKey(eButton _Key)   const noexcept;
		// Returns true during the frame the user starts pressing down the key identified by _Key.
		_DLL_EXPORT bool  GetKeyDown(eButton _Key) const noexcept;
		// Returns true during the frame the user releases the key identified by _Key.
		_DLL_EXPORT bool  GetKeyUp(eButton _Key)  const noexcept;

		// Returns true while the virtual button identified by _BtnName is held down.
		_DLL_EXPORT bool  GetButton(const char* _KeyName, bool isNeg = false)   const noexcept;
		// Returns true during the frame the user pressed down the virtual button identified by _BtnName (single).
		_DLL_EXPORT bool  GetButtonDown(const char* _BtnName, bool isNeg = false) const noexcept;
		// Returns true the first frame the user releases the virtual button identified by _BtnName (single).
		_DLL_EXPORT bool  GetButtonUp(const char* _BtnName, bool isNeg = false)  const noexcept;

		/*
		 *  Returns the value of the virtual axis identified by _BtnName.
		 *  Neutral is 0
		 *  For Key/Mouse/Joystick Buttons, value return will always be from -1 to 1
		 *  For Mouse Movement, value will 0 to any positive float value (Mouse Delta)
		 *  For Joystick, value will also be from -1 to 1
		 */
		_DLL_EXPORT float GetAxis(const char* _BtnName) const noexcept;

		_DLL_EXPORT bool IsController() const;

		// left is low frequency motor, right is high frequency motor
		_DLL_EXPORT void SetVibrate(unsigned short _ltrg = 32000, unsigned short _rtrg = 16000); //0-65534
		_DLL_EXPORT void StopVibrate();

		_DLL_EXPORT float GetAnalogY(int) const;	//0 for left analog, all others for right
		_DLL_EXPORT float GetAnalogX(int) const;	//0 for left analog, all others for right

		/* 0 for left trigger, all others for right
		   values returned are between: mfTriggerThresh ~ 255
		*/
		_DLL_EXPORT float GetTriggers(int) const;	

		_DLL_EXPORT Math::Vector2 GetMousePosition(void) const;
		_DLL_EXPORT Math::Vector2 GetMousePosition(const Window&) const;
		_DLL_EXPORT Math::Vector2 GetMouseDelta(void) const noexcept;
		_DLL_EXPORT float GetMouseWheel(void) const noexcept;

#if EDITOR	
		void EditorUI(void);
#endif

	private:
		void LoadDefaultUserKeys(void);

		struct KeyBinding
		{
			unsigned mnKey;

			bool mbTriggered;
			bool mbPressed;
			bool mbReleased;

			KeyBinding& operator = (eButton);
		};

		MouseData mMouseInput;
		XGamePad mGamePad;

		KeyboardState mKeyBoardState;
		KeyboardState mPrevKeyBoardState;

#if EDITOR	
		struct ComboStruct final
		{
			EGUI::Display::ComboFilterState PosFilter;
			EGUI::Display::ComboFilterState NegFilter;
			EGUI::Display::ComboFilterState AltPosFilter;
			EGUI::Display::ComboFilterState AltNegFilter;


			int TypeSelectedInd = 0;
			int AxisSelectedInd = 0;
		};
#endif

		struct VirtualButton
		{
			VirtualButton()
				: mName("")
				, mPosDescription("Description for Positive Button")
				, mNegDescription("Description for Negative Button")
			{
				
			}

			VirtualButton(const char* _name, eButton _posBtn = NONE, eButton _negBtn = NONE, eButton _altPosBtn = NONE, eButton _altNegBtn = NONE)
				: mName(_name)
				, mPosDescription("Description for Positive Button")
				, mNegDescription("Description for Negative Button")
				, mPosBtn(_posBtn)
				, mNegBtn(_negBtn)
				, mAltPosBtn(_altPosBtn)
				, mAltNegBtn(_altNegBtn)
			{
				
			}

			VirtualButton(HashString _name, HashString _posDesc, HashString _negDesc
						 , eButton _posBtn = NONE, eButton _negBtn = NONE
						 , eButton _altPosBtn = NONE, eButton _altNegBtn = NONE
						 , float _grav = 3.0f, float _deadR = 0.001f, float _sens = 3.0f
						 , int _type = 0, int _axis = 0)
				: mName(_name)
				, mPosDescription(_posDesc)
				, mNegDescription(_negDesc)
				, mPosBtn(_posBtn)
				, mNegBtn(_negBtn)
				, mAltPosBtn(_altPosBtn)
				, mAltNegBtn(_altNegBtn)
				, mfGravity(_grav)
				, mfDeadRange(_deadR)
				, mfSensitivity(_sens)
				, TypeSelectedInd(_type)
				, AxisSelectedInd(_axis)
			{

			}

			VirtualButton(const char* _name, int _type, int _axis)
				: mName(_name)
				, mPosDescription("Desc")
				, mNegDescription("Desc")
				, TypeSelectedInd(_type)
				, AxisSelectedInd(_axis)
			{
				
			}

			HashString mName;
			HashString mPosDescription;
			HashString mNegDescription;

			eButton mPosBtn		= NONE;
			eButton mNegBtn		= NONE;
			eButton mAltPosBtn	= NONE;
			eButton mAltNegBtn	= NONE;

			// A RetValue of 0.f means neutral, and is in a range from -1 to 1
			float mfRetValue		= 0.000f;
			float mfGravity			= 3.000F;
			float mfDeadRange		= 0.010F;
			float mfSensitivity		= 3.000F;

			bool mbSnapping = false;
			bool mbInvert = false;

			int TypeSelectedInd = 0;
			int AxisSelectedInd = 0;

			void UpdateName(const char* _newName)
			{
				mName = _newName;
			}

			// Update either Positive or Negative Btn Description (0 = +ve, 1 = -ve)
			void UpdateBtnDesc(const char* _newDesc, int _neg = 0)
			{
				if (_neg) mNegDescription = _newDesc;
				else mPosDescription = _newDesc;
			}

			// Update either +ve or -ve Button Mapping
			void MapBtn(eButton _newBtn, int _neg = 0)
			{
				if (_neg) mNegBtn = _newBtn;
				else mPosBtn = _newBtn;
			}

			// Update either +ve or -ve alternate Button Mapping
			void MapAltBtn(eButton _newBtn, int _neg = 0)
			{
				if (_neg) mAltNegBtn = _newBtn;
				else mAltPosBtn = _newBtn;
			}

			void Modify(const float& _val)
			{
				mfRetValue += _val;

				if (mfRetValue < -1.0f) mfRetValue = -1.f;
				else if (mfRetValue > 1.0f) mfRetValue = 1.f;
			}

			void Update(const float& _dt)
			{
				if (mfRetValue <= mfDeadRange && mfRetValue >= -mfDeadRange)
				{
					mfRetValue = 0.000F;
				}

				if (mfRetValue < 0.f)
				{
					mfRetValue += mfGravity * _dt;
				}
				else if (mfRetValue > 0.f)
				{
					mfRetValue -= mfGravity * _dt;
				}
			}
		};


		// ComboFilterStates Array
		AutoArray<ComboStruct> marrCombos;

		// Virtual Button/Axis Mapping
		std::map<HashString, VirtualButton> mAxisMapping;

		// Button Strings
		std::map<eButton, HashString> mButtonNames
		{
			{ NONE,							"NONE" },
			
			{ KEYBOARD_A,					"KEYBOARD_A" },
			{ KEYBOARD_B,					"KEYBOARD_B" },
			{ KEYBOARD_C,					"KEYBOARD_C" },
			{ KEYBOARD_D,					"KEYBOARD_D" },
			{ KEYBOARD_E,					"KEYBOARD_E" },
			{ KEYBOARD_F,					"KEYBOARD_F" },
			{ KEYBOARD_G,					"KEYBOARD_G" },
			{ KEYBOARD_H,					"KEYBOARD_H" },
			{ KEYBOARD_I,					"KEYBOARD_I" },
			{ KEYBOARD_J,					"KEYBOARD_J" },
			{ KEYBOARD_K,					"KEYBOARD_K" },
			{ KEYBOARD_L,					"KEYBOARD_L" },
			{ KEYBOARD_M,					"KEYBOARD_M" },
			{ KEYBOARD_N,					"KEYBOARD_N" },
			{ KEYBOARD_O,					"KEYBOARD_O" },
			{ KEYBOARD_P,					"KEYBOARD_P" },
			{ KEYBOARD_Q,					"KEYBOARD_Q" },
			{ KEYBOARD_R,					"KEYBOARD_R" },
			{ KEYBOARD_S,					"KEYBOARD_S" },
			{ KEYBOARD_T,					"KEYBOARD_T" },
			{ KEYBOARD_U,					"KEYBOARD_U" },
			{ KEYBOARD_V,					"KEYBOARD_V" },
			{ KEYBOARD_W,					"KEYBOARD_W" },
			{ KEYBOARD_X,					"KEYBOARD_X" },
			{ KEYBOARD_Y,					"KEYBOARD_Y" },
			{ KEYBOARD_Z,					"KEYBOARD_Z" },
			{ KEYBOARD_F1,					"KEYBOARD_F1" },
			{ KEYBOARD_F2,					"KEYBOARD_F2" },
			{ KEYBOARD_F3,					"KEYBOARD_F3" },
			{ KEYBOARD_F4,					"KEYBOARD_F4" },
			{ KEYBOARD_F5,					"KEYBOARD_F5" },
			{ KEYBOARD_F6,					"KEYBOARD_F6" },
			{ KEYBOARD_F7,					"KEYBOARD_F7" },
			{ KEYBOARD_F8,					"KEYBOARD_F8" },
			{ KEYBOARD_F9,					"KEYBOARD_F9" },
			{ KEYBOARD_F10,					"KEYBOARD_F10" },
			{ KEYBOARD_F11,					"KEYBOARD_F11" },
			{ KEYBOARD_F12,					"KEYBOARD_F12" },
			{ KEYBOARD_0,					"KEYBOARD_0" },
			{ KEYBOARD_1,					"KEYBOARD_1" },
			{ KEYBOARD_2,					"KEYBOARD_2" },
			{ KEYBOARD_3,					"KEYBOARD_3" },
			{ KEYBOARD_4,					"KEYBOARD_4" },
			{ KEYBOARD_5,					"KEYBOARD_5" },
			{ KEYBOARD_6,					"KEYBOARD_6" },
			{ KEYBOARD_7,					"KEYBOARD_7" },
			{ KEYBOARD_8,					"KEYBOARD_8" },
			{ KEYBOARD_9,					"KEYBOARD_9" },
			{ KEYBOARD_NUMPAD_0,			"KEYBOARD_NUMPAD_0" },
			{ KEYBOARD_NUMPAD_1,			"KEYBOARD_NUMPAD_1" },
			{ KEYBOARD_NUMPAD_2,			"KEYBOARD_NUMPAD_2" },
			{ KEYBOARD_NUMPAD_3,			"KEYBOARD_NUMPAD_3" },
			{ KEYBOARD_NUMPAD_4,			"KEYBOARD_NUMPAD_4" },
			{ KEYBOARD_NUMPAD_5,			"KEYBOARD_NUMPAD_5" },
			{ KEYBOARD_NUMPAD_6,			"KEYBOARD_NUMPAD_6" },
			{ KEYBOARD_NUMPAD_7,			"KEYBOARD_NUMPAD_7" },
			{ KEYBOARD_NUMPAD_8,			"KEYBOARD_NUMPAD_8" },
			{ KEYBOARD_NUMPAD_9,			"KEYBOARD_NUMPAD_9" },

			{ KEYBOARD_OEM_1,				"KEYBOARD_OEM_1" },
			{ KEYBOARD_OEM_PLUS,			"KEYBOARD_OEM_PLUS" },
			{ KEYBOARD_OEM_COMMA,			"KEYBOARD_OEM_COMMA" },
			{ KEYBOARD_OEM_MINUS,			"KEYBOARD_OEM_MINUS" },
			{ KEYBOARD_OEM_PERIOD,			"KEYBOARD_OEM_PERIOD" },

			{ KEYBOARD_BACKSPACE,			"KEYBOARD_BACKSPACE" },
			{ KEYBOARD_TAB,					"KEYBOARD_TAB" },
			{ KEYBOARD_ENTER,				"KEYBOARD_ENTER" },
			{ KEYBOARD_SHIFT,				"KEYBOARD_SHIFT" },
			{ KEYBOARD_CTRL,				"KEYBOARD_CTRL" },
			{ KEYBOARD_ALT,					"KEYBOARD_ALT" },
			{ KEYBOARD_ESCAPE,				"KEYBOARD_ESCAPE" },
			{ KEYBOARD_LSHIFT,				"KEYBOARD_LSHIFT" },
			{ KEYBOARD_RSHIFT,				"KEYBOARD_RSHIFT" },
			{ KEYBOARD_LCTRL,				"KEYBOARD_LCTRL" },
			{ KEYBOARD_RCTRL,				"KEYBOARD_RCTRL" },
			{ KEYBOARD_LALT,				"KEYBOARD_LALT" },
			{ KEYBOARD_RALT,				"KEYBOARD_RALT" },
			{ KEYBOARD_SPACEBAR,			"KEYBOARD_SPACEBAR" },
			{ KEYBOARD_PAGEUP,				"KEYBOARD_PAGEUP" },
			{ KEYBOARD_PAGEDOWN,			"KEYBOARD_PAGEDOWN" },
			{ KEYBOARD_END,					"KEYBOARD_END" },
			{ KEYBOARD_HOME,				"KEYBOARD_HOME" },
			{ KEYBOARD_PRNTSRC,				"KEYBOARD_PRNTSRC" },
			{ KEYBOARD_INSERT,				"KEYBOARD_INSERT" },
			{ KEYBOARD_DELETE,				"KEYBOARD_DELETE" },
			{ KEYBOARD_LEFT,				"KEYBOARD_LEFT" },
			{ KEYBOARD_UP,					"KEYBOARD_UP" },
			{ KEYBOARD_RIGHT,				"KEYBOARD_RIGHT" },
			{ KEYBOARD_DOWN,				"KEYBOARD_DOWN" },

			{ MOUSE_LEFT,					"MOUSE_LEFT" },
			{ MOUSE_RIGHT,					"MOUSE_RIGHT" },
			{ MOUSE_MIDDLE,					"MOUSE_MIDDLE" },
			{ MOUSE_X1,						"MOUSE_X1" },
			{ MOUSE_X2,						"MOUSE_X2" },

			{ XBUTTON_DPAD_UP,				"XBUTTON_DPAD_UP" },
			{ XBUTTON_DPAD_DOWN,			"XBUTTON_DPAD_DOWN" },
			{ XBUTTON_DPAD_LEFT,			"XBUTTON_DPAD_LEFT" },
			{ XBUTTON_DPAD_RIGHT,			"XBUTTON_DPAD_RIGHT" },
			{ XBUTTON_START,				"XBUTTON_START" },
			{ XBUTTON_BACK,					"XBUTTON_BACK" },
			{ XBUTTON_LEFT_THUMB,			"XBUTTON_LEFT_THUMB" },
			{ XBUTTON_RIGHT_THUMB,			"XBUTTON_RIGHT_THUMB" },
			{ XBUTTON_LEFT_SHOULDER,		"XBUTTON_LEFT_SHOULDER" },
			{ XBUTTON_RIGHT_SHOULDER,		"XBUTTON_RIGHT_SHOULDER" },
			{ XBUTTON_A,					"XBUTTON_A" },
			{ XBUTTON_B,					"XBUTTON_B" },
			{ XBUTTON_X,					"XBUTTON_X" },
			{ XBUTTON_Y,					"XBUTTON_Y" },
			{ XBUTTON_LAST,					"XBUTTON_LAST" }
		};

	};

}



#endif		// INCLUDE GUARD

