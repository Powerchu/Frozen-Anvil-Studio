/* HEADER *********************************************************************************/
/*!
\file	EClipboard.cpp
\author Digipen (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#include "Editor/EditorClipboard.h"
#include "Editor/EditorMain.h"
#include "Editor/EInput.h"
#include "Editor/EHotkey.h"

#include "System/Input/InputSystem.h"
#include "System/Scene/SceneSystem.h"
#include "System/Scene/Scene.h"

#include "Utility/GUID.h"

#include "DataStructure/Array.h"

Editor::EditorClipboard::EditorClipboard(void)
	: mArrSelectedIDs{ 100 }, mnCopyID{ nPos }, mnDeleteID{ nPos }, mnPasteID{ nPos }, mnDupliID{ nPos },
	mArrCopied{ 100 }
{}

Editor::EditorClipboard::~EditorClipboard(void)
{}

void Editor::EditorClipboard::Load(void)
{}

bool Editor::EditorClipboard::Init(void)
{
	Hotkey<2> copy{ Array<eButton, 2>{ eButton::KEYBOARD_CTRL, eButton::KEYBOARD_C }, 
					Array<eHKState, 2>{ eHKState::eHK_HOLD, eHKState::eHK_TRIGGERED } };
	Hotkey<2> paste{ Array<eButton, 2>{ eButton::KEYBOARD_CTRL, eButton::KEYBOARD_V },
					 Array<eHKState, 2>{ eHKState::eHK_HOLD, eHKState::eHK_TRIGGERED } };
	Hotkey<1> del{ Array<eButton, 1>{ eButton::KEYBOARD_DELETE },
				   Array<eHKState, 1>{ eHKState::eHK_TRIGGERED } };
	Hotkey<2> dup{ Array<eButton, 2>{ eButton::KEYBOARD_CTRL, eButton::KEYBOARD_D },
				   Array<eHKState, 2>{ eHKState::eHK_HOLD, eHKState::eHK_TRIGGERED } };

	auto input = EditorMain::GetInstance()->GetSystem<EInput>();
	mnCopyID	= input->RegisterHotkey(copy);
	mnPasteID	= input->RegisterHotkey(paste);
	mnDeleteID	= input->RegisterHotkey(del);
	mnDupliID	= input->RegisterHotkey(dup);
	return true;
}

void Editor::EditorClipboard::StartFrame(void)
{
	auto input = EditorMain::GetInstance()->GetSystem<EInput>();
	if (input->IsHotkeyTriggered(mnCopyID))
	{
		Copy();
	}
	else if (input->IsHotkeyTriggered(mnPasteID))
	{
		Paste();
	}
	else if (input->IsHotkeyTriggered(mnDeleteID))
	{
		Delete();
	}
	else if (input->IsHotkeyTriggered(mnDupliID))
	{
		Duplicate();
	}
}

void Editor::EditorClipboard::Update(float)
{}

void Editor::EditorClipboard::EndFrame(void)
{}

void Editor::EditorClipboard::Shutdown(void)
{}

void Editor::EditorClipboard::Message(eEMessage)
{}

void Editor::EditorClipboard::SaveSettings(Dystopia::TextSerialiser& _out) const
{}

void Editor::EditorClipboard::LoadSettings(Dystopia::TextSerialiser& _in)
{}

void Editor::EditorClipboard::AddGameObject(const uint64_t& _id)
{
	auto inputMgr = EditorMain::GetInstance()->GetSystem<EInput>()->GetInputManager();
	if (!inputMgr->IsKeyPressed(eButton::KEYBOARD_CTRL))
		ClearAll();
	mArrSelectedIDs.Insert(_id);
}

void Editor::EditorClipboard::ClearAll(void)
{
	mArrSelectedIDs.clear();
}

void Editor::EditorClipboard::Copy(void)
{
	mArrCopied.clear();
	for (auto& id : mArrSelectedIDs)
		mArrCopied.Insert(id);
}

void Editor::EditorClipboard::Delete(void)
{

}

void Editor::EditorClipboard::Duplicate(void)
{

}

void Editor::EditorClipboard::Paste(void)
{

}

#endif

