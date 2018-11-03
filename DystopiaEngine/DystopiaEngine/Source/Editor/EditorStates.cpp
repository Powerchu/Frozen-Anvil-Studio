/* HEADER *********************************************************************************/
/*!
\file	EditorStates.cpp
\author Digipen (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#include "Editor/EditorStates.h"
#include "Editor/EditorMain.h"

#include "System/Driver/Driver.h"
#include "System/Logger/LoggerSystem.h"
#include "System/Window/WindowManager.h"
#include "System/Profiler/Profiler.h"
#include "System/Behaviour/BehaviourSystem.h"

Editor::EditorStates::EditorStates(void)
	: mState{ eState::LAUNCHER }
{
}

Editor::EditorStates::~EditorStates(void)
{
}

void Editor::EditorStates::Load(void)
{

}

bool Editor::EditorStates::Init(void)
{
	return true;
}

void Editor::EditorStates::StartFrame(void)
{
	float dt = Editor::EditorMain::GetInstance()->GetDeltaTime();
	switch (mState)
	{
	case eState::LAUNCHER:
		Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::WindowManager>()->Update(dt);
		break;

	case eState::MAIN:
		Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::WindowManager>()->Update(dt);
		Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::Profiler>()->Update(dt);
		Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::BehaviourSystem>()->PollChanges();
		break;
	}
}

void Editor::EditorStates::Update(float)
{
	switch (mState)
	{
	case eState::PLAY:
		Dystopia::EngineCore::GetInstance()->FixedUpdate();
		Dystopia::EngineCore::GetInstance()->Update();
		break;
	}
}

void Editor::EditorStates::EndFrame(void)
{
}

void Editor::EditorStates::Shutdown(void)
{
}

void Editor::EditorStates::Message(eEMessage _msg)
{
	if (_msg == eEMessage::STATE_CHANGED)
		mState = Editor::EditorMain::GetInstance()->GetCurState();
}

void Editor::EditorStates::SaveSettings(Dystopia::TextSerialiser&) const
{

}

void Editor::EditorStates::LoadSettings(Dystopia::TextSerialiser&)
{

}

#endif




