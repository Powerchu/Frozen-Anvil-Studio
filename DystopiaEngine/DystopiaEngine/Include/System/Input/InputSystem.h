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


namespace Dystopia
{
	class CharacterController;
	class Window;
	class TextSerialiser;

	class  InputManager : public Systems
	{
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

		bool Init(void);
		void Update(float _dt);
		void Shutdown(void);

		void PostUpdate() override;

		void LoadDefaults(void) override;
		void LoadSettings(DysSerialiser_t&) override;
		void SaveSettings(DysSerialiser_t&) override;

		_DLL_EXPORT void  MapUserButton(eUserButton, eButton);
		_DLL_EXPORT void  MapButton(const char* _name, eButton _Button);

		// Returns true while the user holds down the key identified by _Key.
		_DLL_EXPORT bool  GetKey(eButton _Key)   const noexcept;
		// Returns true during the frame the user starts pressing down the key identified by _Key.
		_DLL_EXPORT bool  GetKeyDown(eButton _Key) const noexcept;
		// Returns true during the frame the user releases the key identified by _Key.
		_DLL_EXPORT bool  GetKeyUp(eButton _Key)  const noexcept;

		// Returns true while the virtual button identified by _BtnName is held down.
		_DLL_EXPORT bool  GetButton(const char* _KeyName)   const noexcept;
		// Returns true during the frame the user pressed down the virtual button identified by _BtnName.
		_DLL_EXPORT bool  GetButtonDown(const char* _BtnName) const noexcept;
		// Returns true the first frame the user releases the virtual button identified by _BtnName.
		_DLL_EXPORT bool  GetButtonUp(const char* _BtnName)  const noexcept;

		// Returns the value of the virtual axis identified by axisName.
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
		struct KeyBinding
		{
			unsigned mnKey;

			bool mbTriggered;
			bool mbPressed;
			bool mbReleased;

			KeyBinding& operator = (eButton);
		};

		struct VirtualButton
		{
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
			float mfDeadRange		= 0.001F;
			float mfSensitivity		= 3.000F;

			bool mbSnapping	= false;
			bool mbInvert	= false;

			void Modify(const float& _val)
			{
				float tempV = _val;
				if (tempV <= -1.f) tempV = -1.f;
				else if (tempV >= 1.f) tempV = 1.f;

				mfRetValue += tempV;
			}

			void Update(const float& _dt)
			{
				if (mfRetValue <= mfDeadRange)
				{
					mfRetValue = 0.000F;
				}
			}
		};

		MouseData mMouseInput;

		KeyboardState mKeyBoardState;
		KeyboardState mPrevKeyBoardState;

		void LoadDefaultUserKeys(void);

		std::map<std::string, eButton> mButtonMapping;
		std::map<HashString, VirtualButton> mAxisMapping;

		XGamePad mGamePad;

	};

}



#endif		// INCLUDE GUARD

