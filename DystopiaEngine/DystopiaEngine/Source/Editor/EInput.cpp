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

#include "../../Dependancies/ImGui/imgui.h"

Editor::EInput::EInput(void)
	: mpInputMgr{ static_cast<Dystopia::InputManager*>(Dystopia::DefaultAllocator<Dystopia::InputManager>::ConstructAlloc()) }
{
}

Editor::EInput::~EInput(void)
{
}

void Editor::EInput::Load(void)
{
	mpInputMgr->Init();
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

	ImGuiIO&	io = ImGui::GetIO();
	const auto&	iquene = Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::WindowManager>()->GetMainWindow().GetInputQueue();
	bool		shft = mpInputMgr->IsKeyPressed(eButton::KEYBOARD_SHIFT);
	float		scrollV = mpInputMgr->GetMouseWheel();

	io.KeysDown[eButton::KEYBOARD_ENTER] = false;
	io.KeysDown[eButton::KEYBOARD_ESCAPE] = false;
	for (int i = eButton::KEYBOARD_BACKSPACE; i <= eButton::KEYBOARD_TAB; ++i)
		io.KeysDown[i] = false;
	for (int i = eButton::KEYBOARD_SPACEBAR; i <= eButton::KEYBOARD_HOME; ++i)
		io.KeysDown[i] = false;
	for (int i = eButton::KEYBOARD_LEFT; i <= eButton::KEYBOARD_DOWN; ++i)
		io.KeysDown[i] = false;
	for (int i = eButton::KEYBOARD_INSERT; i <= eButton::KEYBOARD_DELETE; ++i)
		io.KeysDown[i] = false;

	for (const auto& k : iquene)
	{
		if (k >= eButton::KEYBOARD_0 && k <= eButton::KEYBOARD_9)
			io.AddInputCharacter(k);
		else if (k >= eButton::KEYBOARD_A && k <= eButton::KEYBOARD_Z)
			io.AddInputCharacter(shft ? k : k + 32);
		else if (k >= eButton::KEYBOARD_NUMPAD_0 && k <= eButton::KEYBOARD_NUMPAD_9)
			io.AddInputCharacter(k - 48);
		else if (k == eButton::KEYBOARD_OEM_PERIOD)
			io.AddInputCharacter(46);
		else if (k == eButton::KEYBOARD_OEM_MINUS)
			io.AddInputCharacter(shft ? 95 : 45);
		else if (k == eButton::KEYBOARD_SPACEBAR)
			io.AddInputCharacter(32);
		else
			io.KeysDown[k] = true;
	}

	if (scrollV)
		io.MouseWheel += scrollV;

	io.KeyShift = shft;
	io.KeyAlt = mpInputMgr->IsKeyPressed(eButton::KEYBOARD_ALT);
	io.KeyCtrl = mpInputMgr->IsKeyPressed(eButton::KEYBOARD_CTRL);
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


