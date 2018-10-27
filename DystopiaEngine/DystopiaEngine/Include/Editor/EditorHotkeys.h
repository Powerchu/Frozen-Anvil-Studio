/* HEADER *********************************************************************************/
/*!
\file	EditorHotkeys.h
\author Digipen (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#ifndef _EDITOR_HOTKEYS_H_
#define _EDITOR_HOTKEYS_H_
#include "System/Input/InputMap.h"
#define g_CountDefault 2
#define g_CountSaveAs 3
#define g_CountGizmo 3

namespace Dystopia
{
	enum eHotkeyState
	{
		eHOTKEY_HOLD,
		eHOTKEY_RELEASED,
		eHOTKEY_TRIGGERED,
		eHOTKEY_NEGATE
	};

	namespace Hotkey
	{
		static constexpr unsigned short g_KeyUndo[g_CountDefault] = { eButton::KEYBOARD_CTRL, eButton::KEYBOARD_Z };
		static constexpr eHotkeyState g_StateUndo[g_CountDefault] = { eHotkeyState::eHOTKEY_HOLD, eHotkeyState::eHOTKEY_TRIGGERED };

		static constexpr unsigned short g_KeyRedo[g_CountDefault] = { eButton::KEYBOARD_CTRL, eButton::KEYBOARD_Y };
		static constexpr eHotkeyState g_StateRedo[g_CountDefault] = { eHotkeyState::eHOTKEY_HOLD, eHotkeyState::eHOTKEY_TRIGGERED };

		static constexpr unsigned short g_KeyCopy[g_CountDefault] = { eButton::KEYBOARD_CTRL, eButton::KEYBOARD_C };
		static constexpr eHotkeyState g_StateCopy[g_CountDefault] = { eHotkeyState::eHOTKEY_HOLD, eHotkeyState::eHOTKEY_TRIGGERED };

		static constexpr unsigned short g_KeyPaste[g_CountDefault] = { eButton::KEYBOARD_CTRL, eButton::KEYBOARD_V };
		static constexpr eHotkeyState g_StatePaste[g_CountDefault] = { eHotkeyState::eHOTKEY_HOLD, eHotkeyState::eHOTKEY_TRIGGERED };

		static constexpr unsigned short g_KeyOpen[g_CountDefault] = { eButton::KEYBOARD_CTRL, eButton::KEYBOARD_O };
		static constexpr eHotkeyState g_StateOpen[g_CountDefault] = { eHotkeyState::eHOTKEY_HOLD, eHotkeyState::eHOTKEY_TRIGGERED };

		static constexpr unsigned short g_KeyPlayStop[g_CountDefault] = { eButton::KEYBOARD_CTRL, eButton::KEYBOARD_P };
		static constexpr eHotkeyState g_StatePlayStop[g_CountDefault] = { eHotkeyState::eHOTKEY_HOLD, eHotkeyState::eHOTKEY_TRIGGERED };

		static constexpr unsigned short g_KeySave[g_CountDefault] = { eButton::KEYBOARD_CTRL, eButton::KEYBOARD_S };
		static constexpr eHotkeyState g_StateSave[g_CountDefault] = { eHotkeyState::eHOTKEY_HOLD, eHotkeyState::eHOTKEY_TRIGGERED };

		static constexpr unsigned short g_KeySaveAs[g_CountSaveAs] = { eButton::KEYBOARD_CTRL, eButton::KEYBOARD_SHIFT, eButton::KEYBOARD_S };
		static constexpr eHotkeyState g_StateSaveAs[g_CountSaveAs] = { eHotkeyState::eHOTKEY_HOLD, eHotkeyState::eHOTKEY_HOLD, eHotkeyState::eHOTKEY_TRIGGERED };

		static constexpr unsigned short g_KeyDelete[g_CountDefault] = { eButton::KEYBOARD_CTRL, eButton::KEYBOARD_DELETE };
		static constexpr eHotkeyState g_StateDelete[g_CountDefault] = { eHotkeyState::eHOTKEY_NEGATE, eHotkeyState::eHOTKEY_TRIGGERED };

		static constexpr unsigned short g_KeyGizmoTrans[g_CountGizmo] = { eButton::KEYBOARD_CTRL, eButton::KEYBOARD_ALT, eButton::KEYBOARD_W };
		static constexpr eHotkeyState g_StateGizmoTrans[g_CountGizmo] = { eHotkeyState::eHOTKEY_NEGATE, eHotkeyState::eHOTKEY_NEGATE, eHotkeyState::eHOTKEY_TRIGGERED };

		static constexpr unsigned short g_KeyGizmoScale[g_CountGizmo] = { eButton::KEYBOARD_CTRL, eButton::KEYBOARD_ALT, eButton::KEYBOARD_E };
		static constexpr eHotkeyState g_StateGizmoScale[g_CountGizmo] = { eHotkeyState::eHOTKEY_NEGATE, eHotkeyState::eHOTKEY_NEGATE, eHotkeyState::eHOTKEY_TRIGGERED };
	}
}

#endif //_EDITOR_HOTKEYS_H_
#endif //EDITOR
