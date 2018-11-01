#include "EditorMain.h"
#include "EInput.h"
#include "Allocator/DefaultAlloc.h"

namespace
{
	template <typename Ty, typename ... T>
	AutoArray<Ty> MakeAutoArray(Ut::TypeList<T...>)
	{
		return AutoArray<Ty>{ static_cast<Ty>(Dystopia::DefaultAllocator<T>::ConstructAlloc())...};
	}
}

Editor::EditorMain* Editor::EditorMain::GetInstance(void)
{
	static EditorMain instance{};
	return &instance;
}

Editor::EditorMain::EditorMain(void)
	: mArrSystems{ MakeAutoArray<EditorSystem*>(Ut::MakeTypeList_t<Ut::TypeList, ESystems>{}) },
	  mArrPanels {}
{
}

Editor::EditorMain::~EditorMain(void)
{
}

void Editor::EditorMain::Init(void)
{
	for (auto& s : mArrSystems)
		s->Load();
	for (auto& s : mArrSystems)
		s->Init();

	for (auto& p : mArrPanels)
		p->Load();
	for (auto& p : mArrPanels)
		p->Init();
}

void Editor::EditorMain::StartFrame(void)
{
	for (auto& s : mArrSystems)
		s->StartFrame();
}

void Editor::EditorMain::Update(void)
{
	for (auto& s : mArrSystems)
		s->Update(mDelta);

	for (auto& p : mArrPanels)
		p->Update(mDelta);

	for (auto& p : mArrPanels)
		p->EditorUI();
}

void Editor::EditorMain::EndFrame(void)
{
	for (auto& s : mArrSystems)
		s->EndFrame();

	if (mCurState != mNextState)
	{
		mCurState = mNextState;
		for (auto& s : mArrSystems)
			s->Message(eEMessage::STATE_CHANGED);
		for (auto& p : mArrPanels)
			p->Message(eEMessage::STATE_CHANGED);
	}
}

void Editor::EditorMain::Shutdown(void)
{
	for (auto& s : mArrSystems)
		s->Shutdown();

	for (auto& p : mArrPanels)
		p->Shutdown();
}

void Editor::EditorMain::ChangeState(eState e)
{
	mNextState = e;
}

float Editor::EditorMain::GetDeltaTime(void) const
{
	return mDelta;
}

Editor::eState Editor::EditorMain::GetCurState(void) const
{
	return mCurState;
}

bool Editor::EditorMain::IsClosing(void)
{
	return mCurState == eState::EXIT;
}
