/* HEADER *********************************************************************************/
/*!
\file	EditorResource.cpp
\author Digipen (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#include "Editor/EditorResource.h"

Editor::EditorResource::EditorResource(void)
	: mCurrProjectPath{ "" }
{
}

Editor::EditorResource::~EditorResource(void)
{
}

void Editor::EditorResource::Load(void)
{
}

bool Editor::EditorResource::Init(void)
{
	return true;
}

void Editor::EditorResource::StartFrame(void)
{
}

void Editor::EditorResource::Update(float)
{
}

void Editor::EditorResource::EndFrame(void)
{
}

void Editor::EditorResource::Shutdown(void)
{
}

void Editor::EditorResource::Message(eEMessage)
{
}

void Editor::EditorResource::SaveSettings(Dystopia::TextSerialiser& _out) const
{
	_out.InsertStartBlock("Editor Resource");
	_out << mCurrProjectPath.c_str();
	_out.InsertEndBlock("Editor Resource");
}

void Editor::EditorResource::LoadSettings(Dystopia::TextSerialiser& _in)
{
	_in.ConsumeStartBlock();
	_in >> mCurrProjectPath;
	_in.ConsumeEndBlock();
}

HashString Editor::EditorResource::GetCurrProjectPath(void) const
{
	return mCurrProjectPath;
}

#endif 











