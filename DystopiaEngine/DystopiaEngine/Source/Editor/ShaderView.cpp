/* HEADER *********************************************************************************/
/*!
\file	ShaderView.cpp
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "Editor/ShaderView.h"
#include "Editor/EGUI.h"
#include "Editor/EditorMain.h"
#include "Editor/Payloads.h"

#include "System/Graphics/GraphicsSystem.h"
#include "System/Graphics/Shader.h"
#include "System/Graphics/ShaderProgram.h"
#include "System/Graphics/ShaderSystem.h"
#include "System/Driver/Driver.h"

#define STAGE_RESERVES 48

Editor::ShaderView::ShaderView(void)
	: mLabel{ "Shader View" }, mpShaderSystem{ nullptr },
	mArrStageVert{}, mArrStageFrag{}, mArrStageGeo{}, mArrIndexTracker{}
{
}

Editor::ShaderView::~ShaderView(void)
{
}

void Editor::ShaderView::Load(void)
{
	mArrIndexTracker[0] = mArrIndexTracker[1] = mArrIndexTracker[2] = 0;
	mArrStageVert.reserve(STAGE_RESERVES);
	mArrStageFrag.reserve(STAGE_RESERVES);
	mArrStageGeo.reserve(STAGE_RESERVES);
}

bool Editor::ShaderView::Init(void)
{
	mpShaderSystem = Dystopia::CORE::Get<Dystopia::ShaderSystem>();
	return mpShaderSystem != nullptr;
}

void Editor::ShaderView::Update(float)
{
}

void Editor::ShaderView::EditorUI(void)
{
	mArrIndexTracker[0] = mArrIndexTracker[1] = mArrIndexTracker[2] = 0;

	ShaderUI();
	EGUI::Display::HorizontalSeparator();
	ProgramUI();

	EGUI::Display::HorizontalSeparator();

	ImGui::Columns(3);
	StageVert();
	ImGui::NextColumn();
	StageFrag();
	ImGui::NextColumn();
	StageGeo();
	ImGui::NextColumn();
	ImGui::Columns(1);
	
	Prompt();
}

void Editor::ShaderView::Shutdown(void)
{
}

void Editor::ShaderView::Message(eEMessage)
{
}

void Editor::ShaderView::SaveSettings(Dystopia::TextSerialiser&) const
{
}

void Editor::ShaderView::LoadSettings(Dystopia::TextSerialiser&)
{
}

const HashString& Editor::ShaderView::GetLabel(void) const
{
	return mLabel;
}

void Editor::ShaderView::ShaderUI(void)
{
	//auto& allShaders = mpShaderSystem->GetAllShaders();
}

void Editor::ShaderView::ProgramUI(void)
{
	auto& allPrograms = mpShaderSystem->GetAllShaderPrograms();
	for (unsigned i = 0; i < allPrograms.size(); ++i)
	{
		switch (allPrograms[i].GetStage())
		{
		case ::Gfx::ShaderStage::VERTEX:
			mArrStageVert[mArrIndexTracker[0]++] = i;
			break;
		case ::Gfx::ShaderStage::FRAGMENT:
			mArrStageFrag[mArrIndexTracker[1]++] = i;
			break;
		case ::Gfx::ShaderStage::GEOMETRY:
			mArrStageGeo[mArrIndexTracker[2]++] = i;
			break;
		}
	}
}

void Editor::ShaderView::StageVert(void)
{
	if (EGUI::Display::Button("Create Vert Prog", { 100, 24 }))
	{

	}

	if (EGUI::Display::StartTreeNode("Vertex Programs"))
	{
		EGUI::Indent();

		auto& allPrograms = mpShaderSystem->GetAllShaderPrograms();
		for (unsigned i = 0; i < mArrIndexTracker[0]; ++i)
		{
			auto& prog = allPrograms[mArrStageVert[i]];
			EGUI::Display::Label("Program ID [%03d] = %s", prog.GetID(), prog.GetName().c_str());
		}

		EGUI::UnIndent();
		EGUI::Display::EndTreeNode();
	}
}

void Editor::ShaderView::StageFrag(void)
{
	if (EGUI::Display::Button("Create Frag Prog", { 100, 24 }))
	{

	}

	if (EGUI::Display::StartTreeNode("Fragment Programs"))
	{
		EGUI::Indent();

		auto& allPrograms = mpShaderSystem->GetAllShaderPrograms();
		for (unsigned i = 0; i < mArrIndexTracker[1]; ++i)
		{
			auto& prog = allPrograms[mArrStageFrag[i]];
			EGUI::Display::Label("Program ID [%03d] = %s", prog.GetID() , prog.GetName().c_str());
		}

		EGUI::UnIndent();
		EGUI::Display::EndTreeNode();
	}
}

void Editor::ShaderView::StageGeo(void)
{
	if (EGUI::Display::Button("Create Geom Prog", { 100, 24 }))
	{

	}

	if (EGUI::Display::StartTreeNode("Geometry Programs"))
	{
		EGUI::Indent();

		auto& allPrograms = mpShaderSystem->GetAllShaderPrograms();
		for (unsigned i = 0; i < mArrIndexTracker[2]; ++i)
		{
			auto& prog = allPrograms[mArrStageGeo[i]];
			EGUI::Display::Label("Program ID [%03d] = %s", prog.GetID(), prog.GetName().c_str());
		}

		EGUI::UnIndent();
		EGUI::Display::EndTreeNode();
	}
}


void Editor::ShaderView::Prompt(void)
{
	EGUI::PushID(199);
	if (EGUI::Display::StartPopupModal("Make Vert Program", "Creating Vertex Shader"))
	{
		if (EGUI::Display::Button("Close"))
		{
			EGUI::Display::CloseCurrentPopup();
		}
		EGUI::Display::EndPopup();
	}
	else if (EGUI::Display::StartPopupModal("Make Frag Program", "Creating Fragment Shader"))
	{
		if (EGUI::Display::Button("Close"))
		{
			EGUI::Display::CloseCurrentPopup();
		}
		EGUI::Display::EndPopup();
	}
	else if (EGUI::Display::StartPopupModal("Make Geo Program", "Creating Geometry Shader"))
	{
		if (EGUI::Display::Button("Close"))
		{
			EGUI::Display::CloseCurrentPopup();
		}
		EGUI::Display::EndPopup();
	}
	EGUI::PopID();
}









