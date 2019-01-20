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
#include "Editor/EditorCommands.h"
#include "Editor/EditorResource.h"
#include "Editor/EditorFactory.h"
#include "Editor/EInput.h"
#include "Editor/EHotkey.h"
#include "Editor/Payloads.h"

#include "System/Input/InputSystem.h"
#include "System/Scene/SceneSystem.h"
#include "System/Scene/Scene.h"
#include "System/Driver/Driver.h"
#include "System/File/FileSystem.h"

#include "Object/GameObject.h"
#include "Object/ObjectFlags.h"

#include "Utility/Utility.h"
#include "Utility/GUID.h"
#include "Utility/DebugAssert.h"

#include "DataStructure/Array.h"

Editor::EditorClipboard::EditorClipboard(void)
	: mArrSelectedIDs{}, mArrCopiedBufer{}, mnCopyID{ nPos }, mnDeleteID{ nPos }, mnPasteID{ nPos }, mnDupliID{ nPos }
	, mpPrefabFocus{ nullptr }
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
	if (EditorMain::GetInstance()->GetCurState() != eState::MAIN)
		return;

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
{
}

void Editor::EditorClipboard::EndFrame(void)
{}

void Editor::EditorClipboard::Shutdown(void)
{}

void Editor::EditorClipboard::Message(eEMessage _msg)
{
	if (_msg == eEMessage::SCENE_CHANGED)
	{
		auto& curScene = Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::SceneSystem>()->GetCurrentScene();
		for (size_t i = 0; i < mArrSelectedIDs.size(); i++)
		{
			if (!curScene.FindGameObject(mArrSelectedIDs[i]))
			{
				mArrSelectedIDs.FastRemove(i);
				--i;
			}
		}
	}
}

void Editor::EditorClipboard::SaveSettings(Dystopia::TextSerialiser&) const
{}

void Editor::EditorClipboard::LoadSettings(Dystopia::TextSerialiser&)
{}

void Editor::EditorClipboard::AddGameObject(const uint64_t& _id)
{
	auto inputMgr = EditorMain::GetInstance()->GetSystem<EInput>()->GetInputManager();
	if (!inputMgr->GetKey(eButton::KEYBOARD_CTRL) || mpPrefabFocus)
		ClearAll();
	mArrSelectedIDs.Insert(_id);
}

void Editor::EditorClipboard::RemoveGameObject(const uint64_t& _id)
{
	for (unsigned int i = 0; i < mArrSelectedIDs.size(); ++i)
	{
		if (mArrSelectedIDs[i] == _id)
		{
			mArrSelectedIDs.FastRemove(i);
			return;
		}
	}
}

void Editor::EditorClipboard::RemoveGameObjectP(const uint64_t& _objID)
{
	RemoveGameObject(_objID);
	auto& scene = Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::SceneSystem>()->GetCurrentScene();
	if (auto o = scene.FindGameObject(_objID))
	{
		auto& childrens = o->GetComponent<Dystopia::Transform>()->GetAllChild();
		for (auto& c : childrens)
			RemoveGameObjectP(c->GetOwnerID());
	}
}

void Editor::EditorClipboard::ClearAll(void)
{
	mArrSelectedIDs.clear();
	mpPrefabFocus = nullptr;
}

void Editor::EditorClipboard::Copy(void)
{
	mArrCopiedBufer.clear();
	for (const auto& id : mArrSelectedIDs)
		mArrCopiedBufer.push_back(id);
}

void Editor::EditorClipboard::Delete(void)
{
	EditorMain::GetInstance()->GetSystem<EditorCommands>()->RemoveGameObject(mArrSelectedIDs);
}

void Editor::EditorClipboard::Duplicate(void)
{
	Copy();
	Paste();
}

void Editor::EditorClipboard::Paste(void)
{
	auto& curScene = Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::SceneSystem>()->GetCurrentScene();
	for (size_t i = 0; i < mArrCopiedBufer.size(); ++i)
	{
		if (!curScene.FindGameObject(mArrCopiedBufer[i]))
		{
			mArrCopiedBufer.FastRemove(i);
			i--;
		}
	}
	EditorMain::GetInstance()->GetSystem<EditorCommands>()->DuplicateGameObject(mArrCopiedBufer);
}

AutoArray<uint64_t>& Editor::EditorClipboard::GetSelectedIDs(void)
{
	return mArrSelectedIDs;
}

void Editor::EditorClipboard::AddPrefab(Editor::File* _file)
{
	ClearAll();

	if (!_file) 
		return;

	EditorMain::GetInstance()->GetSystem<EditorFactory>()->FindMasterPrefab(_file->mName, mpPrefabFocus);
}

Editor::EditorFactory::PrefabData* Editor::EditorClipboard::GetPrefab(void) const
{
	return mpPrefabFocus;
}

void Editor::EditorClipboard::RemovePrefab(void)
{
	mpPrefabFocus = nullptr;
}

#endif

