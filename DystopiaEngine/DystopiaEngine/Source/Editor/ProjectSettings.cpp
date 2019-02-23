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
#include "System/Tag/TagSystem.h"
#include "System/Time/TimeSystem.h"
#include "System/Collision/CollisionSystem.h"


Editor::ProjectSettings::ProjectSettings(void)
	:
	mLabel{ "Project Settings" },
	mpGfxSys{ nullptr },
	mpPhySys{ nullptr },
	mpInputSys(nullptr),
	mpTagSystem(nullptr),
	mpColSystem{nullptr}
{
}

Editor::ProjectSettings::~ProjectSettings(void)
{
	marrSysName.clear();
}

void Editor::ProjectSettings::Load(void)
{}

bool Editor::ProjectSettings::Init(void)
{
	mpGfxSys = Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::GraphicsSystem>();
	mpPhySys = Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::PhysicsSystem>();
	mpColSystem = Dystopia::EngineCore::GetInstance()->Get<Dystopia::CollisionSystem>();
	mpInputSys = Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::InputManager>();
	mpTagSystem = Dystopia::EngineCore::GetInstance()->Get<Dystopia::TagSystem>();

	marrSysName.push_back("Graphics");
	marrSysName.push_back("Physics");
	marrSysName.push_back("Input");
	marrSysName.push_back("Time Settings");
	return true;
}

void Editor::ProjectSettings::Update(float)
{}




void Editor::ProjectSettings::EditorUI(void)
{
	static bool enabled[EGUISYSCOUNT] = {true, false, false, false};
	// Header
	EGUI::Indent(5);
	EGUI::Display::Label("Project Settings");
	EGUI::UnIndent(5);
	EGUI::Display::HorizontalSeparator();

	VerticalTabs(enabled);
	EGUI::SameLine();
	RightPanel(enabled);
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

void Editor::ProjectSettings::VerticalTabs(bool(&enabled)[EGUISYSCOUNT])
{
	ImGui::BeginGroup();
	if (EGUI::Display::GoxTab(marrSysName[0].c_str(), &enabled[0]))
	{
		enabled[0] = true;
		enabled[1] = false;
		enabled[2] = false;
		enabled[3] = false;
	}

	if (EGUI::Display::GoxTab(marrSysName[1].c_str(), &enabled[1]))
	{
		enabled[0] = false;
		enabled[1] = true;
		enabled[2] = false;
		enabled[3] = false;
	}
	if (EGUI::Display::GoxTab(marrSysName[2].c_str(), &enabled[2]))
	{
		enabled[0] = false;
		enabled[1] = false;
		enabled[2] = true;
		enabled[3] = false;
	}
	if (EGUI::Display::GoxTab(marrSysName[3].c_str(), &enabled[3]))
	{
		enabled[0] = false;
		enabled[1] = false;
		enabled[2] = false;
		enabled[3] = true;
	}
	ImGui::EndGroup();
}

void Editor::ProjectSettings::RightPanel(bool(&enabled)[4]) const
{
	ImGui::BeginChild("RightPanel", ImGui::GetContentRegionAvail(), true);
	if (enabled[0])
	{
		if (EGUI::Display::CollapsingHeader("Graphics Settings"))
		{
			EGUI::Indent(10);
			mpGfxSys->EditorUI();
			EGUI::UnIndent(10);
		}
		EGUI::Display::HorizontalSeparator();
	}

	if (enabled[1])
	{
		if (EGUI::Display::CollapsingHeader("Physics Settings"))
		{
			EGUI::Indent(10);
			mpPhySys->EditorUI();
			EGUI::UnIndent(10);
		}
		EGUI::Display::HorizontalSeparator();

		if (EGUI::Display::CollapsingHeader("Collision Table", false))
		{
			EGUI::Indent(10);
			mpColSystem->EditorUI();
			EGUI::UnIndent(10);
		}
		EGUI::Display::HorizontalSeparator();
	}

	if (enabled[2])
	{
		if (EGUI::Display::CollapsingHeader("Input Mapping"))
		{
			EGUI::Indent(10);
			mpInputSys->EditorUI();
			EGUI::UnIndent(10);
		}
		EGUI::Display::HorizontalSeparator();
	}

	if (enabled[3])
	{
		auto mpTimeSys = Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::TimeSystem>();

		if (EGUI::Display::CollapsingHeader("Time System"))
		{
			EGUI::Indent(10);
			mpTimeSys->EditorUI();
			EGUI::UnIndent(10);
		}
		EGUI::Display::HorizontalSeparator();
	}
	ImGui::EndChild();

	//if (EGUI::Display::StartTreeNode("Tag Manager", nullptr, false, false, true, true))
	//{
	//	//mpTagSystem->EditorUI();
	//	EGUI::Display::EndTreeNode();
	//}
}

#endif






