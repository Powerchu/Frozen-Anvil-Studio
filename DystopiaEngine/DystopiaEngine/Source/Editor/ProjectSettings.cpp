/* HEADER *********************************************************************************/
/*!
\file	ProjectSettings.cpp
\author Digipen (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#include "Editor/ProjectSettings.h"
#include "Editor/EGUI.h"

#include "System/Driver/Driver.h"
#include "System/Graphics/GraphicsSystem.h"
#include "System/Physics/PhysicsSystem.h"
#include "System/Input/InputSystem.h"
#include "System/Time/TimeSystem.h"


Editor::ProjectSettings::ProjectSettings(void)
	:
	mLabel{ "Project Settings" },
	mpGfxSys{ nullptr },
	mpPhySys{ nullptr }
{
}

Editor::ProjectSettings::~ProjectSettings(void)
{
}

void Editor::ProjectSettings::Load(void)
{}

bool Editor::ProjectSettings::Init(void)
{
	mpGfxSys = Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::GraphicsSystem>();
	mpPhySys = Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::PhysicsSystem>();
	return true;
}

void Editor::ProjectSettings::Update(float)
{}

void Editor::ProjectSettings::EditorUI(void)
{
	if (EGUI::Display::StartTreeNode("Graphics Settings"))
	{
		mpGfxSys->EditorUI();
		EGUI::Display::EndTreeNode();
	}
	EGUI::Display::HorizontalSeparator();
	if (EGUI::Display::StartTreeNode("Physics Settings"))
	{
		mpPhySys->EditorUI();
		EGUI::Display::EndTreeNode();
	}
	auto mpTimeSys = Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::TimeSystem>();
	EGUI::Display::HorizontalSeparator();
	if (EGUI::Display::StartTreeNode("Time Settings"))
	{
		mpTimeSys->EditorUI();
		EGUI::Display::EndTreeNode();
	}

}

void Editor::ProjectSettings::Shutdown(void)
{}

void Editor::ProjectSettings::Message(eEMessage)
{}

void Editor::ProjectSettings::SaveSettings(Dystopia::TextSerialiser&) const
{}

void Editor::ProjectSettings::LoadSettings(Dystopia::TextSerialiser&)
{}

HashString Editor::ProjectSettings::GetLabel(void) const
{
	return mLabel;
}

#endif






