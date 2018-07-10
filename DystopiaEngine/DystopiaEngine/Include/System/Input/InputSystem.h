/* HEADER *********************************************************************************/
/*!
\file	InputManager.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Creates and manages Win32 windows.

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _INPUT_MANAGER_H_
#define _INPUT_MANAGER_H_

#include "System\Base\Systems.h"		// Base Class
#include "DataStructure\AutoArray.h"	// AutoArray
#include "Math\Vector2.h"               // Vector2
#include "Math\Vector4.h"				// Vector4

enum eButton;
enum eUserButton;

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

		bool IsKeyTriggered(eUserButton);
		bool IsKeyPressed(eUserButton);
		bool IsKeyReleased(eUserButton);

		// Temporary
		Math::Vector2 GetMousePosition(void);
		Math::Vector2 GetMousePosition(Window&);

	private:

		struct KeyBinding
		{
			int mnKey;

			bool mbTriggered;
			bool mbPressed;
			bool mbReleased;

			KeyBinding& operator = (eButton);
		};

		AutoArray<KeyBinding> mButtonMap;

		void LoadDefaultUserKeys(void);
	};
}



#endif		// INCLUDE GUARD

