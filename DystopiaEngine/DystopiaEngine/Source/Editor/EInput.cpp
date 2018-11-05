/* HEADER *********************************************************************************/
/*!
\file	EInput.cpp
\author Digipen (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#include "Editor/EInput.h"
#include "Editor/EditorMain.h"

#include "System/Driver/Driver.h"
#include "System/Input/InputSystem.h"
#include "System/Window/Window.h"
#include "System/Window/WindowManager.h"

#include "Utility/DebugAssert.h"

#include <Windows.h>

Editor::EInput::EInput(void)
	: mpInputMgr{ static_cast<Dystopia::InputManager*>(Dystopia::DefaultAllocator<Dystopia::InputManager>::ConstructAlloc()) }
{
}

Editor::EInput::~EInput(void)
{
}

void Editor::EInput::Load(void)
{
}

bool Editor::EInput::Init(void)
{
	return mpInputMgr->Init();
}

void Editor::EInput::StartFrame(void)
{
	const auto myWinHND = Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::WindowManager>()->GetMainWindow().GetWindowHandle();
	if (myWinHND == GetForegroundWindow())
		mpInputMgr->Update(0.016f);
}

void Editor::EInput::Update(float)
{
}

void Editor::EInput::EndFrame(void)
{
	mpInputMgr->PostUpdate();
}

void Editor::EInput::Shutdown(void)
{
	mpInputMgr->Shutdown();
	for (auto& elem : mArrHotkeyPtrs)
	{
		Dystopia::DefaultAllocator<Editor::EInput::HKBase>::DestructFree(elem);
	}
	Dystopia::DefaultAllocator<Dystopia::InputManager>::DestructFree(mpInputMgr);
}

void Editor::EInput::Message(Editor::eEMessage)
{

}

void Editor::EInput::SaveSettings(Dystopia::TextSerialiser&) const
{

}

void Editor::EInput::LoadSettings(Dystopia::TextSerialiser&)
{

}

Dystopia::InputManager * const Editor::EInput::GetInputManager(void) const
{
	return mpInputMgr;
}

bool Editor::EInput::IsHotkeyTriggered(size_t _id) const
{
	return mArrHotkeyPtrs[_id]->Occured(mpInputMgr);
}

#endif


