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
#include "DataStructure/Array.h"    // Array
#include "Math/Vector2.h"           // Vector2
//#include "Math\Vector4.h"		    // Vector4
#include "System/Input/InputMap.h"
#include "System/Input/MouseData.h"
#include "Component/Component.h"
#include "System/Base/ComponentDonor.h"
#include "XGamePad.h"
#include <map>


namespace Dystopia
{
	class CharacterController;
	class Window;
	class TextSerialiser;

	class InputManager : public Systems, public ComponentDonor<CharacterController>
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

	public:
		InputManager(void);
		~InputManager(void);

		bool Init(void);
		void Update(float _dt);
		void Shutdown(void);

		void PostUpdate() override;

		void LoadDefaults(void);
		void LoadSettings(TextSerialiser&);

		void MapUserButton(eUserButton, eButton);

		void MapButton(std::string const & _name, eButton _Button);

		bool IsKeyTriggered(eButton) const noexcept;
		bool IsKeyPressed(eButton)   const noexcept;
		bool IsKeyReleased(eButton)  const noexcept;
	
		bool IsKeyTriggered(std::string const & _ButtonName) const noexcept;
		bool IsKeyPressed(std::string const & _ButtonName)   const noexcept;
		bool IsKeyReleased(std::string const & _ButtonName)  const noexcept;

		bool IsController() const;

		// left is low frequency motor, right is high frequency motor
		void SetVibrate(unsigned short _ltrg = 32000, unsigned short _rtrg = 16000); //0-65534
		void StopVibrate();

		float GetAnalogY(int) const;	//0 for left analog, all others for right
		float GetAnalogX(int) const;	//0 for left analog, all others for right

		float GetTriggers(int) const;	// 0 for left trigger, all others for right
										// values returned are between: mfTriggerThresh ~ 255

		Math::Vector2 GetMousePosition(void) const;
		Math::Vector2 GetMousePosition(const Window&) const;
		Math::Vector2 GetMouseDelta(void) const noexcept;
		float GetMouseWheel(void) const noexcept;

	private:
		struct KeyBinding
		{
			unsigned mnKey;

			bool mbTriggered;
			bool mbPressed;
			bool mbReleased;

			KeyBinding& operator = (eButton);
		};

		MouseData mMouseInput;

		KeyboardState mKeyBoardState;
		KeyboardState mPrevKeyBoardState;

		void LoadDefaultUserKeys(void);

		std::map<std::string, eButton> mButtonMapping;
		XGamePad mGamePad;
	};

}



#endif		// INCLUDE GUARD

