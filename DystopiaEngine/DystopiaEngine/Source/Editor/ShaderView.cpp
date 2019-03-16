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
#include "Editor/ConsoleLog.h"

#include "System/Graphics/GraphicsSystem.h"
#include "System/Graphics/Shader.h"
#include "System/Graphics/ShaderProgram.h"
#include "System/Graphics/ShaderSystem.h"
#include "System/Driver/Driver.h"
#include "System/File/FileSystem.h"

#include "Globals.h"

#include "Lib/Gfx/OpenGLAPI.h"

#include <fstream>

#define STAGE_RESERVES 48
#define MAX_BUFFER_SIZE 255


static const std::string g_VertPopup = "Behaviour List";
static const std::string g_FragPopup = "Make Frag Program";
static const std::string g_GeoPopup = "Make Geo Program";

#if EDITOR
Editor::ShaderView::ShaderView(void)
	: mLabel{ "Shader View" }, mpShaderSystem{ nullptr },
	mArrStageVert{}, mArrStageFrag{}, mArrStageGeo{}, mArrIndexTracker{},
	mArrShowPopup{}, mArrProgramChosen{}
{
}

Editor::ShaderView::~ShaderView(void)
{
}

void Editor::ShaderView::Load(void)
{
	for (int i = 0; i < 3; i++)
	{
		mArrIndexTracker[i] = 0;
		mArrShowPopup[i] = false;
		mArrProgramChosen[i] = "";
	}
	mArrStageVert.resize(STAGE_RESERVES);
	mArrStageFrag.resize(STAGE_RESERVES);
	mArrStageGeo.resize(STAGE_RESERVES);
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
	static char buffer[MAX_BUFFER_SIZE] = "";
	static constexpr float halfY = 0.7f;
	static constexpr float sides = 5;
	static constexpr float tops = 3;
	auto size = Size();
	size.y -= EGUI::TabsImageOffsetY;

	EGUI::PushLeftAlign(150);
	EGUI::Display::EmptyBox("Vert Shader Program", 150, mArrProgramChosen[0].c_str());
	if (unsigned *n = EGUI::Display::StartPayloadReceiver<unsigned>(EGUI::ePayloadTags::SHADER_VERT))
	{
		auto& allPrograms = mpShaderSystem->GetAllShaderPrograms();
		mArrProgramChosen[0] = allPrograms[*n].GetName();
		EGUI::Display::EndPayloadReceiver();
	}
	EGUI::Display::EmptyBox("Frag Shader Program", 150, mArrProgramChosen[1].c_str());
	if (unsigned *n = EGUI::Display::StartPayloadReceiver<unsigned>(EGUI::ePayloadTags::SHADER_FRAG))
	{
		auto& allPrograms = mpShaderSystem->GetAllShaderPrograms();
		mArrProgramChosen[1] = allPrograms[*n].GetName();
		EGUI::Display::EndPayloadReceiver();
	}
	EGUI::Display::EmptyBox("Geo Shader Program", 150, mArrProgramChosen[2].c_str());
	if (unsigned *n = EGUI::Display::StartPayloadReceiver<unsigned>(EGUI::ePayloadTags::SHADER_GEO))
	{
		auto& allPrograms = mpShaderSystem->GetAllShaderPrograms();
		mArrProgramChosen[2] = allPrograms[*n].GetName();
		EGUI::Display::EndPayloadReceiver();
	}
	EGUI::Display::TextField("Shader Name", buffer, MAX_BUFFER_SIZE, false);
	if (strlen(buffer))
	{
		ImGui::SameLine();
		if (EGUI::Display::Button("Make Shader", { 100, 24 }))
		{
			if (MakeShader(buffer))
				buffer[0] = '\0';
		}
	}
	EGUI::PopLeftAlign();
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
	static char buffer[MAX_BUFFER_SIZE] = "";
	if (EGUI::Display::Button("Create Vert Prog", { 200, 24 }))
		mArrShowPopup[0] = true;

	if (EGUI::Display::StartTreeNode("Vertex Programs"))
	{
		EGUI::Indent();

		auto& allPrograms = mpShaderSystem->GetAllShaderPrograms();
		for (unsigned i = 0; i < mArrIndexTracker[0]; ++i)
		{
			auto& prog = allPrograms[mArrStageVert[i]];
			sprintf_s(buffer, "Program ID [%03d] = %s", prog.GetID(), prog.GetName().c_str());
			EGUI::Display::SelectableTxt(buffer);

			unsigned progIndex = mArrStageVert[i];
			if (EGUI::Display::StartPayload(EGUI::ePayloadTags::SHADER_VERT, &progIndex, sizeof(unsigned), buffer))
				EGUI::Display::EndPayload();
		}

		EGUI::UnIndent();
		EGUI::Display::EndTreeNode();
	}
}

void Editor::ShaderView::StageFrag(void)
{
	static char buffer[MAX_BUFFER_SIZE] = "";
	if (EGUI::Display::Button("Create Frag Prog", { 200, 24 }))
		mArrShowPopup[1] = true;

	if (EGUI::Display::StartTreeNode("Fragment Programs"))
	{
		EGUI::Indent();

		auto& allPrograms = mpShaderSystem->GetAllShaderPrograms();
		for (unsigned i = 0; i < mArrIndexTracker[1]; ++i)
		{
			auto& prog = allPrograms[mArrStageFrag[i]];
			sprintf_s(buffer, "Program ID [%03d] = %s", prog.GetID(), prog.GetName().c_str());
			EGUI::Display::SelectableTxt(buffer);

			unsigned progIndex = mArrStageFrag[i];
			if (EGUI::Display::StartPayload(EGUI::ePayloadTags::SHADER_FRAG, &progIndex, sizeof(unsigned), buffer))
				EGUI::Display::EndPayload();
		}

		EGUI::UnIndent();
		EGUI::Display::EndTreeNode();
	}
}

void Editor::ShaderView::StageGeo(void)
{
	static char buffer[MAX_BUFFER_SIZE] = "";
	if (EGUI::Display::Button("Create Geo Prog", { 200, 24 }))
		mArrShowPopup[2] = true;

	if (EGUI::Display::StartTreeNode("Geometry Programs"))
	{
		EGUI::Indent();

		auto& allPrograms = mpShaderSystem->GetAllShaderPrograms();
		for (unsigned i = 0; i < mArrIndexTracker[2]; ++i)
		{
			auto& prog = allPrograms[mArrStageGeo[i]];
			sprintf_s(buffer, "Program ID [%03d] = %s", prog.GetID(), prog.GetName().c_str());
			EGUI::Display::SelectableTxt(buffer);

			unsigned progIndex = mArrStageGeo[i];
			if (EGUI::Display::StartPayload(EGUI::ePayloadTags::SHADER_GEO, &progIndex, sizeof(unsigned), buffer))
				EGUI::Display::EndPayload();
		}

		EGUI::UnIndent();
		EGUI::Display::EndTreeNode();
	}
}

void Editor::ShaderView::Prompt(void)
{
	static char buffer[MAX_BUFFER_SIZE] = "";
	EGUI::PushID(199);
	if (mArrShowPopup[0])
	{
		EGUI::Display::OpenPopup(g_VertPopup.c_str(), false);
		if (EGUI::Display::StartPopupModal(g_VertPopup.c_str(), "Creating Vertex Shader"))
		{
			EGUI::Display::TextField("File Name :", buffer, MAX_BUFFER_SIZE);

			if (EGUI::Display::Button("Close"))
			{
				mArrShowPopup[0] = false;
				EGUI::Display::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (buffer[0] != '\0' && EGUI::Display::Button("Create"))
			{
				MakeShaderProgram(::Gfx::ShaderStage::VERTEX, buffer);
				mArrShowPopup[0] = false;
				buffer[0] = '\0';
				EGUI::Display::CloseCurrentPopup();
			}

			EGUI::Display::EndPopup();
		}
	}
	if (mArrShowPopup[1])
	{
		EGUI::Display::OpenPopup(g_FragPopup.c_str(), false);
		if (EGUI::Display::StartPopupModal(g_FragPopup.c_str(), "Creating Fragment Shader"))
		{
			EGUI::Display::TextField("File Name :", buffer, MAX_BUFFER_SIZE);

			if (EGUI::Display::Button("Close"))
			{
				mArrShowPopup[1] = false;
				EGUI::Display::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (buffer[0] != '\0' && EGUI::Display::Button("Create"))
			{
				MakeShaderProgram(::Gfx::ShaderStage::FRAGMENT, buffer);
				mArrShowPopup[1] = false;
				buffer[0] = '\0';
				EGUI::Display::CloseCurrentPopup();
			}

			EGUI::Display::EndPopup();
		}
	}
	if (mArrShowPopup[2])
	{
		EGUI::Display::OpenPopup(g_GeoPopup.c_str(), false);
		if (EGUI::Display::StartPopupModal(g_GeoPopup.c_str(), "Creating Geometry Shader"))
		{
			EGUI::Display::TextField("File Name :", buffer, MAX_BUFFER_SIZE);

			if (EGUI::Display::Button("Close"))
			{
				mArrShowPopup[2] = false;
				EGUI::Display::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (buffer[0] != '\0' && EGUI::Display::Button("Create"))
			{
				MakeShaderProgram(::Gfx::ShaderStage::GEOMETRY, buffer);
				mArrShowPopup[2] = false;
				buffer[0] = '\0';
				EGUI::Display::CloseCurrentPopup();
			}

			EGUI::Display::EndPopup();
		}
	}
	EGUI::PopID();
}

void Editor::ShaderView::MakeShaderProgram(::Gfx::ShaderStage _stage, const HashString& _name)
{
	auto fs = Dystopia::CORE::Get<Dystopia::FileSystem>();
	HashString defaultsPath{ "" };
	HashString path{ fs->Normalize(fs->GetProjectFolders<std::string>(Dystopia::eFileDir::eResource)).c_str() };
	HashString name{ "\\Shader\\" };
	name += _name;
	name += '.';

	switch (_stage)
	{
	case ::Gfx::ShaderStage::VERTEX:
		name += Gbl::VERTSHADER_EXT;
		path += name;
		defaultsPath = fs->GetFullPath("DefaultVert.dvert", Dystopia::eFileDir::eSolution).c_str();
		break;
	case ::Gfx::ShaderStage::FRAGMENT:
		name += Gbl::FRAGSHADER_EXT;
		path += name;
		defaultsPath = fs->GetFullPath("DefaultFrag.dfrag", Dystopia::eFileDir::eSolution).c_str();
		break;
	case ::Gfx::ShaderStage::GEOMETRY:
		name += Gbl::GEOGSHADER_EXT;
		path += name;
		defaultsPath = fs->GetFullPath("Particle.dgeog", Dystopia::eFileDir::eSolution).c_str();
		break;
	}

	if (fs->CheckFileExist(name.c_str(), Dystopia::eFileDir::eResource))
		PrintToConsoleLog("Shader already exist!");
	else
	{
		std::filesystem::copy_file(defaultsPath.c_str(), path.c_str());
		PrintToConsoleLog("Shader file created!");
		switch (_stage)
		{
		case ::Gfx::ShaderStage::VERTEX:
			mpShaderSystem->CreateShaderProgram(::Gfx::ShaderStage::VERTEX, path.c_str());
			break;
		case ::Gfx::ShaderStage::FRAGMENT:
			mpShaderSystem->CreateShaderProgram(::Gfx::ShaderStage::FRAGMENT, path.c_str());
			break;
		case ::Gfx::ShaderStage::GEOMETRY:
			mpShaderSystem->CreateShaderProgram(::Gfx::ShaderStage::GEOMETRY, path.c_str());
			break;
		}
	}

}

bool Editor::ShaderView::MakeShader(const HashString& _name)
{
	if (!mArrProgramChosen[0].length() || !mArrProgramChosen[1].length())
	{
		PrintToConsoleLog("Vert and Frag must not be empty!");
		return false;
	}

	if (auto s = mpShaderSystem->CreateShader(_name.c_str(), true))
	{
		for (int i = 0; i < 3; ++i)
			if (auto program = mpShaderSystem->GetShaderProgram(mArrProgramChosen[i].c_str()))
				s->AttachProgram(program);
	}
	else
		PrintToConsoleLog("Failed to create shader");
	mArrProgramChosen[0] = mArrProgramChosen[1] = mArrProgramChosen[2] = "";
	return true;
}

#endif




