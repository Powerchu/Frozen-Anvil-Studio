/* HEADER *********************************************************************************/
/*!
\file	DataSheetEditor.cpp
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#include "Editor/DataSheetEditor.h"
#include "Editor/EditorMain.h"
#include "Editor/Payloads.h"

#include "Editor/CommandTypes.h"
#include "Editor/EditorCommands.h"

#include "System/Driver/Driver.h"
#include "System/Database/DatabaseSystem.h"

#include "Utility/DebugAssert.h"

Editor::DataSheetEditor::DataSheetEditor(void)
	: mLabel{ "DataSheets" }, mNewAdditionName{}, mnCurrentSheet{ 0 }, mUIVisitor{}
{}

Editor::DataSheetEditor::~DataSheetEditor(void)
{}

void Editor::DataSheetEditor::Load(void)
{}

bool Editor::DataSheetEditor::Init(void)
{
	SetHorizontalEnabled(true);
	mpDB = Dystopia::EngineCore::Get<Dystopia::DatabaseSystem>();
	return mpDB;
}

void Editor::DataSheetEditor::Update(float)
{}

void Editor::DataSheetEditor::EditorUI(void)
{
	EGUI::PushLeftAlign(150.f);
	auto db = Dystopia::EngineCore::Get<Dystopia::DatabaseSystem>();

	static char buffer[256];

	if (EGUI::Display::TextField("Make a Data sheet", buffer, 256, true))
	{
	}
	EGUI::PopLeftAlign();
	ImGui::SameLine();
	if (EGUI::Display::Button("Create") && strlen(buffer))
	{
		HashString name{ buffer };
		if (!db->NewData(name))
			DEBUG_PRINT(eLog::MESSAGE, "Failed to make a data sheet %s", name.c_str());
		else
		{
			db->OpenData(name);
			strcpy_s(buffer, 256, "");
		}
	}
	EGUI::PushLeftAlign(150.f);

	if (EGUI::Display::EmptyBox("Open Data sheet ", 150, mNewAdditionName.length() ? mNewAdditionName.c_str() : "-empty-", true))
	{
	}
	if (File *t = EGUI::Display::StartPayloadReceiver<File>(EGUI::DATA_SHEET))
	{
		mNewAdditionName = t->mName;
		EGUI::Display::EndPayloadReceiver();
	}

	if (mNewAdditionName.length())
	{
		ImGui::SameLine();
		if (EGUI::Display::Button("Open"))
		{
			if (!db->NewData(mNewAdditionName))
				DEBUG_PRINT(eLog::MESSAGE, "Failed to open data sheet %s", mNewAdditionName.c_str());
			else
				db->OpenData(mNewAdditionName);
			mNewAdditionName.clear();
		}
	}

	EGUI::PopLeftAlign();
	EGUI::Display::HorizontalSeparator();

	auto& sheets = db->GetAllSheets();
	for (auto& elemPair : sheets)
		HandleData(elemPair.second);
}

void Editor::DataSheetEditor::Shutdown(void)
{}

void Editor::DataSheetEditor::Message(eEMessage)
{}

void Editor::DataSheetEditor::SaveSettings(Dystopia::TextSerialiser&) const
{}

void Editor::DataSheetEditor::LoadSettings(Dystopia::TextSerialiser&)
{}

HashString Editor::DataSheetEditor::GetLabel(void) const
{
	return mLabel;
}

void Editor::DataSheetEditor::HandleData(Dystopia::DataSheet& _dat)
{
	auto& allElements = _dat.GetAllElements();
	ImGui::Columns(static_cast<int>(allElements.size()), _dat.GetOpened().c_str());
	for (int i = 0; i < allElements.size(); ++i)
	{
		EGUI::Display::Label("%s", allElements[i]->mName.c_str());
		auto v = _dat.MagicGet(allElements[i]->mName);
		v.Visit(mUIVisitor);
		ImGui::NextColumn();
	}
	EGUI::Display::HorizontalSeparator();
}

#endif 












