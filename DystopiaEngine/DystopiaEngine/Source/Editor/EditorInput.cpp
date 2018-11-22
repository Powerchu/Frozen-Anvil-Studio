/* HEADER *********************************************************************************/
/*!
\file	EditorInput.cpp
\author Digipen (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#include "Editor/EditorInput.h"
#include "Allocator/DefaultAlloc.h"
#include "System/Input/InputSystem.h"
#include "System/Input/InputMap.h"

#include "IO/TextSerialiser.h"

Dystopia::EditorInput::EditorInput(void)
	: mpInput{ new InputManager }
{
}

bool Dystopia::EditorInput::Init(void)
{
	return mpInput->Init();
}

void Dystopia::EditorInput::Poll(void)
{
	mpInput->Update(0.016f);
}
void Dystopia::EditorInput::Clear(void)
{
	mpInput->PostUpdate();
}

void Dystopia::EditorInput::Shutdown(void)
{
	mpInput->Shutdown();
	delete mpInput;
}

bool Dystopia::EditorInput::Hotkeys(const unsigned short* _arrKeys, const eHotkeyState* _arrState, unsigned int _size) const
{
	for (unsigned int i = 0; i < _size; ++i)
	{
		auto k = static_cast<eButton>(*(_arrKeys + i));
		switch (*(_arrState + i))
		{
		case eHOTKEY_HOLD:
			if (!mpInput->IsKeyPressed(k))
				return false;
			break;
		case eHOTKEY_RELEASED:
			if (!mpInput->IsKeyReleased(k))
				return false;
			break;
		case eHOTKEY_TRIGGERED:
			if (!mpInput->IsKeyTriggered(k))
				return false;
			break;
		case eHOTKEY_NEGATE:
			if (mpInput->IsKeyTriggered(k) || mpInput->IsKeyReleased(k) || mpInput->IsKeyPressed(k))
				return false;
			break;
		}
	}
	return true;
}

Dystopia::InputManager* Dystopia::EditorInput::Get(void)
{
	return mpInput;
}

#endif //EDITOR

