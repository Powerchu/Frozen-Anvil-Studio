/* HEADER *********************************************************************************/
/*!
\file	EditorCommands.cpp
\author Digipen (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#include "Editor/EditorCommands.h"
#include "Editor/EditorMain.h"
#include "Editor/EInput.h"
#include "Editor/EHotkey.h"

#include "System/Driver/Driver.h"
#include "System/Scene/Scene.h"
#include "System/Scene/SceneSystem.h"
#include "System/Input/InputSystem.h"

#include "Utility/GUID.h"

#include "DataStructure/Array.h"

Editor::EditorCommands::EditorCommands(void)
	: mDeqRedo{}, mDeqUndo{}, mbChangesMade{ false }, mbDisableCommands{ false }, mnUndo{ nPos }, mnRedo{ nPos }
{}

Editor::EditorCommands::~EditorCommands(void)
{}

void Editor::EditorCommands::Load(void)
{
}

bool Editor::EditorCommands::Init(void)
{
	Hotkey<2> undo{ Array<eButton, 2>{ eButton::KEYBOARD_CTRL, eButton::KEYBOARD_Z },
					Array<eHKState, 2>{ eHKState::eHK_HOLD, eHKState::eHK_TRIGGERED } };
	Hotkey<2> redo{ Array<eButton, 2>{ eButton::KEYBOARD_CTRL, eButton::KEYBOARD_Y },
					Array<eHKState, 2>{ eHKState::eHK_HOLD, eHKState::eHK_TRIGGERED } };

	auto input = EditorMain::GetInstance()->GetSystem<EInput>();
	mnUndo = input->RegisterHotkey(undo);
	mnRedo = input->RegisterHotkey(redo);

	return true;
}

void Editor::EditorCommands::StartFrame(void)
{
	auto input = EditorMain::GetInstance()->GetSystem<EInput>();
	if (input->IsHotkeyTriggered(mnUndo))
	{
		Undo();
	}
	else if (input->IsHotkeyTriggered(mnRedo))
	{
		Redo();
	}
}

void Editor::EditorCommands::Update(float)
{}

void Editor::EditorCommands::EndFrame(void)
{}

void Editor::EditorCommands::Shutdown(void)
{
	for (auto& r : mDeqRedo)
		Dystopia::DefaultAllocator<Command>::DestructFree(r);
	for (auto& u : mDeqUndo)
		Dystopia::DefaultAllocator<Command>::DestructFree(u);
}

void Editor::EditorCommands::Message(eEMessage _msg)
{
	if (eEMessage::STATE_CHANGED == _msg)
	{
		mbDisableCommands = (EditorMain::GetInstance()->GetCurState() == eState::PLAY);
	}
}

void Editor::EditorCommands::SaveSettings(Dystopia::TextSerialiser&) const
{}

void Editor::EditorCommands::LoadSettings(Dystopia::TextSerialiser&)
{}

void Editor::EditorCommands::SavedChanges(void)
{
	mbChangesMade = false;
}

bool Editor::EditorCommands::HasChanges(void) const
{
	return mbChangesMade;
}

void Editor::EditorCommands::Undo(void)
{
	//EndRecording();
	if (!mDeqUndo.size()) return;

	if (!mDeqUndo.back()->Undo())
	{
		RemoveStray(mDeqUndo);
		return;
	}
	mbChangesMade = true;

	mDeqRedo.push_back(mDeqUndo.back());
	mDeqUndo.pop_back();
}

void Editor::EditorCommands::Redo(void)
{
	//EndRecording();
	if (!mDeqRedo.size()) return;

	if (!mDeqRedo.back()->Do())
	{
		RemoveStray(mDeqRedo);
		return;
	}

	mbChangesMade = true;

	mDeqUndo.push_back(mDeqRedo.back());
	mDeqRedo.pop_back();
}

void Editor::EditorCommands::InsertNewGameObject(void)
{
	if (mbDisableCommands)
		return;

	Command *pCommand = Dystopia::DefaultAllocator<InsertGameObject>::ConstructAlloc(Dystopia::GUIDGenerator::GetUniqueID());
	if (pCommand->Do())
		mDeqUndo.push_back(pCommand);
	else
		Dystopia::DefaultAllocator<Command>::DestructFree(pCommand);
}

void Editor::EditorCommands::RemoveGameObject(const uint64_t& _objID)
{
	if (mbDisableCommands)
		return;

	Command *pCommand = Dystopia::DefaultAllocator<DeleteGameObject>::ConstructAlloc(_objID);
	if (pCommand->Do())
		mDeqUndo.push_back(pCommand);
	else
		Dystopia::DefaultAllocator<Command>::DestructFree(pCommand);
}

void Editor::EditorCommands::RemoveStray(std::deque<Command*>& _targetDeque)
{
	Command *pTemp = _targetDeque.back();
	_targetDeque.pop_back();
	Dystopia::DefaultAllocator<Command>::DestructFree(pTemp);
}


#endif














