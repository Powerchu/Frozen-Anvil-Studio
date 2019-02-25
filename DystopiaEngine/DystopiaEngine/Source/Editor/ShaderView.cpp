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

#include <fstream>

#define STAGE_RESERVES 48
#define MAX_BUFFER_SIZE 255


static const std::string g_VertPopup = "Behaviour List";
static const std::string g_FragPopup = "Make Frag Program";
static const std::string g_GeoPopup = "Make Geo Program";

Editor::ShaderView::ShaderView(void)
	: mLabel{ "Shader View" }, mpShaderSystem{ nullptr },
	mArrStageVert{}, mArrStageFrag{}, mArrStageGeo{}, mArrIndexTracker{},
	mArrShowPopup{}
{
}

Editor::ShaderView::~ShaderView(void)
{
}

void Editor::ShaderView::Load(void)
{
	mArrIndexTracker[0] = mArrIndexTracker[1] = mArrIndexTracker[2] = 0;
	mArrShowPopup[0] = mArrShowPopup[1] = mArrShowPopup[2] = false;
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
		mArrShowPopup[0] = true;

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
		mArrShowPopup[1] = true;

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
	if (EGUI::Display::Button("Create Geo Prog", { 100, 24 }))
		mArrShowPopup[2] = true;

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
	HashString name{ "/Shader/" };
	name += _name;
	name += '.';

	switch (_stage)
	{
	case ::Gfx::ShaderStage::VERTEX:
		name += Gbl::VERTSHADER_EXT;
		path += name;
		mpShaderSystem->CreateShaderProgram(::Gfx::ShaderStage::VERTEX, path.c_str());
		defaultsPath = fs->GetFullPath("DefaultVert.dvert", Dystopia::eFileDir::eSolution).c_str();
		break;
	case ::Gfx::ShaderStage::FRAGMENT:
		name += Gbl::FRAGSHADER_EXT;
		path += name;
		mpShaderSystem->CreateShaderProgram(::Gfx::ShaderStage::FRAGMENT, path.c_str());
		defaultsPath = fs->GetFullPath("DefaultFrag.dfrag", Dystopia::eFileDir::eSolution).c_str();
		break;
	case ::Gfx::ShaderStage::GEOMETRY:
		name += Gbl::GEOGSHADER_EXT;
		path += name;
		mpShaderSystem->CreateShaderProgram(::Gfx::ShaderStage::GEOMETRY, path.c_str());
		defaultsPath = fs->GetFullPath("Particle.dgeog", Dystopia::eFileDir::eSolution).c_str();
		break;
	}

	if (fs->CheckFileExist(name.c_str(), Dystopia::eFileDir::eResource))
		PrintToConsoleLog("Shader already exist!");
	else
	{
		std::filesystem::copy_file(defaultsPath.c_str(), path.c_str());
		PrintToConsoleLog("Shader file created!");
	}
}

void Editor::ShaderView::MakeShader(const HashString& _name)
{

}






