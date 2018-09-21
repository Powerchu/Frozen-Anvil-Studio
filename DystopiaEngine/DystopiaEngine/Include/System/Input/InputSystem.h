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

namespace Dystopia
{
	class Window;
	class TextSerialiser;

	class InputManager : public Systems
	{
	public:
		InputManager(void);
		~InputManager(void);

		bool Init(void);
		void Update(float _dt);
		void Shutdown(void);

		void LoadDefaults(void);
		void LoadSettings(TextSerialiser&);
		
		void MapUserButton(eUserButton, eButton);

		bool IsKeyTriggered(eUserButton) const noexcept;
		bool IsKeyPressed(eUserButton) const noexcept;
		bool IsKeyReleased(eUserButton) const noexcept;

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
		Array<KeyBinding, eUserButton::TOTAL_USERBUTTONS> mButtonMap;
		
		void LoadDefaultUserKeys(void);
	};
}



#endif		// INCLUDE GUARD

