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
#include "Editor/ProjectSettings.h"
#include "Editor/EGUI.h"

#include "System/Driver/Driver.h"
#include "System/Graphics/GraphicsSystem.h"
#include "System/Physics/PhysicsSystem.h"


Dystopia::ProjectSettings* gpInstance = 0;
Dystopia::ProjectSettings* Dystopia::ProjectSettings::GetInstance(void)
{
	if (gpInstance) return gpInstance;

	gpInstance = new Dystopia::ProjectSettings{};
	return gpInstance;
}

Dystopia::ProjectSettings::ProjectSettings(void)
	: EditorTab{ false },
	mLabel{ "Project Settings" },
	mpGfxSys{ nullptr },
	mpPhySys{ nullptr }
{
}

Dystopia::ProjectSettings::~ProjectSettings(void)
{
}

void Dystopia::ProjectSettings::Init(void)
{
	mpGfxSys = EngineCore::GetInstance()->GetSystem<GraphicsSystem>();
	mpPhySys = EngineCore::GetInstance()->GetSystem<PhysicsSystem>();
}

void Dystopia::ProjectSettings::Update(const float&)
{

}

void Dystopia::ProjectSettings::EditorUI(void)
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
}

void Dystopia::ProjectSettings::Shutdown(void)
{

}

std::string Dystopia::ProjectSettings::GetLabel(void) const
{
	return mLabel;
}

void Dystopia::ProjectSettings::SaveSettings(TextSerialiser& /*_out*/) const
{

}

void Dystopia::ProjectSettings::LoadSettings(TextSerialiser& /*_in*/)
{

}


