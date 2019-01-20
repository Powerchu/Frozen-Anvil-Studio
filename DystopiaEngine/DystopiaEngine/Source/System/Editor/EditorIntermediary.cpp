/* HEADER *********************************************************************************/
/*!
\file	EditorIntermediary.cpp
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "System/Editor/EditorIntermediary.h"
#include "System/SystemMessage.h"

#if EDITOR
#include "Editor/EditorMain.h"
#include "Editor/EditorMessages.h"
#endif

Dystopia::EditorIntermediary::EditorIntermediary(void) 
{}

Dystopia::EditorIntermediary::~EditorIntermediary(void)
{}

bool Dystopia::EditorIntermediary::Init(void)
{
	return true;
}

void Dystopia::EditorIntermediary::Update(float)
{}

void Dystopia::EditorIntermediary::Shutdown(void)
{}

void Dystopia::EditorIntermediary::ReceiveMessage(const eSysMessage& _msg, size_t)
{
#if EDITOR
	auto editor = Editor::EditorMain::GetInstance();
	if (_msg == Dystopia::eSysMessage::QUIT && editor->GetCurState() == Editor::eState::MAIN)
		editor->Broadcast(Editor::eEMessage::WINDOWS_QUIT);
#endif
}