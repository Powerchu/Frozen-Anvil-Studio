#include "EInput.h"

#include "System/Input/InputSystem.h"

Editor::EInput::EInput(void)
	: mpInputMgr{ static_cast<Dystopia::InputManager*>(Dystopia::DefaultAllocator<Dystopia::InputManager>::ConstructAlloc()) }
{
}

Editor::EInput::~EInput(void)
{
}

void Editor::EInput::Load(void)
{
	mpInputMgr->LoadDefaults();
}

bool Editor::EInput::Init(void)
{
	return mpInputMgr->Init();
}

void Editor::EInput::StartFrame(void)
{
	mpInputMgr->Update(0.016f);
}

void Editor::EInput::Update(float _f)
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

void Editor::EInput::Message(Editor::eEMessage _msg)
{

}

void Editor::EInput::SaveSettings(Dystopia::TextSerialiser& _out) const
{

}

void Editor::EInput::LoadSettings(Dystopia::TextSerialiser& _in)
{

}

Dystopia::InputManager * const Editor::EInput::GetInputManager(void) const
{
	return mpInputMgr;
}

bool Editor::EInput::IsHotkeyTriggered(size_t _id) const
{
	DEBUG_ASSERT(_id >= mArrHotkeyPtrs.size(), "Out of range");

	return mArrHotkeyPtrs[_id]->Occured(mpInputMgr);
}



