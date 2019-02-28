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
	: mLabel{ "DataSheets" }, mNewAdditionName{}, mnCurrentSheet{ 0 }, mUIVisitor{}, mnSheetSelection{ 0 }, mnTypeSelection{ 0 },
	mnRemoveSheetSelection{ 0 }, mnRemoveElementSelection{ 0 }
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
	auto db = Dystopia::EngineCore::Get<Dystopia::DatabaseSystem>();

	static char buffer[256];

	EGUI::PushLeftAlign(150.f);
	EGUI::Display::TextField("Make a Data sheet", buffer, 256, true);
	ImGui::SameLine();
	if (EGUI::Display::Button("Create") && strlen(buffer))
	{
		HashString name{ static_cast<const char*>(buffer) };
		if (!db->NewData(name))
			DEBUG_PRINT(eLog::MESSAGE, "Failed to make (already existed?) a data sheet %s", name.c_str());
		else
		{
			db->OpenData(name);
			strcpy_s(buffer, 256, "");
		}
	}

	EGUI::Display::EmptyBox("Load Data sheet ", 150, mNewAdditionName.length() ? mNewAdditionName.c_str() : "-empty-", true);
	if (File *t = EGUI::Display::StartPayloadReceiver<File>(EGUI::DATA_SHEET))
	{
		mNewAdditionName = t->mName;
		EGUI::Display::EndPayloadReceiver();
	}

	if (mNewAdditionName.length())
	{
		ImGui::SameLine();
		if (EGUI::Display::Button("Load"))
		{
			if (!db->NewData(mNewAdditionName))
				DEBUG_PRINT(eLog::MESSAGE, "Failed to load (already loaded?) data sheet %s", mNewAdditionName.c_str());
			else
				db->OpenData(mNewAdditionName);
			mNewAdditionName.clear();
		}
	}
	HandleSheets();
	EGUI::PopLeftAlign();

	EGUI::Display::HorizontalSeparator();

	TableHeaders();
	auto& sheets = db->GetAllSheets();
	for (int i = 0; i < sheets.size(); i++)
	{
		EGUI::PushID(i);
		HandleData(sheets[i].first, sheets[i].second);
		EGUI::PopID();
	}
}

void Editor::DataSheetEditor::Shutdown(void)
{}

void Editor::DataSheetEditor::Message(eEMessage)
{}

void Editor::DataSheetEditor::SaveSettings(Dystopia::TextSerialiser&) const
{}

void Editor::DataSheetEditor::LoadSettings(Dystopia::TextSerialiser&)
{}

const HashString& Editor::DataSheetEditor::GetLabel(void) const
{
	return mLabel;
}

void Editor::DataSheetEditor::HandleSheets(void)
{
	static char buffer1[256];
	auto db = Dystopia::EngineCore::Get<Dystopia::DatabaseSystem>();
	mnSheetSelection = Math::Clamp(mnSheetSelection, 0, static_cast<int>(db->GetAllNames().size()));
	mnTypeSelection = Math::Clamp(mnTypeSelection, 0, static_cast<int>(Dystopia::DataSheet::NElements));
	EGUI::Display::HorizontalSeparator();
	EGUI::Display::DropDownSelection("Add To Sheet", mnSheetSelection, db->GetAllNames(), 150.f);
	if (mnSheetSelection >= 0 && mnSheetSelection < db->GetAllNames().size())
	{
		EGUI::Display::TextField("Element Name", buffer1, 256, true);
		if (EGUI::Display::DropDownSelection("Element Type", mnTypeSelection, AcceptableTypeStrings))
		{

		}
		if (HandleInserting(buffer1, mnTypeSelection))
			strcpy_s(buffer1, 256, "");
	}
	EGUI::Display::HorizontalSeparator();
	HandleRemove();
}

bool Editor::DataSheetEditor::HandleInserting(const char* _name, unsigned _n)
{
	auto db = Dystopia::EngineCore::Get<Dystopia::DatabaseSystem>();
	static char buffer1[2];
	static char buffer2[256];
	static int i = 0;
	static float f = 0.f;
	static bool b = false;
	switch (_n)
	{
	case 0: EGUI::Display::DragInt("Element Value", &i, 1.f, -INT_MAX, INT_MAX); break;
	case 1: EGUI::Display::DragFloat("Element Value", &f, 0.1f, -FLT_MIN, FLT_MAX); break;
	case 2: EGUI::Display::CheckBox("Element Value", &b); break;
	case 3: EGUI::Display::TextField("Element Value", buffer1, 2, true, 50.f); break;
	case 4: EGUI::Display::TextField("Element Value", buffer2, 256, true); break;
	}
	EGUI::SameLine();
	if (EGUI::Display::Button("Insert Element"))
	{
		auto& sheet = db->GetAllSheets()[mnSheetSelection].second;
		if (strlen(_name) && (_n == 4 ? strlen(buffer2) : true))
		{
			bool success = false;
			switch (_n)
			{
			case 0: success = sheet.InsertElement(_name, i); break;
			case 1: success = sheet.InsertElement(_name, f); break;
			case 2: success = sheet.InsertElement(_name, b); break;
			case 3: success = sheet.InsertElement(_name, *buffer1); break;
			case 4: success = sheet.InsertElement(_name, HashString{ static_cast<const char *>(buffer2) }); break;
			}
			if (success)
			{
				DEBUG_PRINT(eLog::MESSAGE, "Successfully inserted element OR updated existing element");
				strcpy_s(buffer2, 256, "");
				i = 0;
				f = 0.f;
				b = false;
				return true;
			}
			else
				DEBUG_PRINT(eLog::MESSAGE, "Failed to inserted element (element exist? sheet closed?)");
		}
		else
			DEBUG_PRINT(eLog::MESSAGE, "Failed to inserted element (missing fields?)");
	}
	return false;
}

void Editor::DataSheetEditor::HandleRemove(void)
{
	auto db = Dystopia::EngineCore::Get<Dystopia::DatabaseSystem>();
	
	mnRemoveSheetSelection = Math::Clamp(mnRemoveSheetSelection, 0, static_cast<int>(db->GetAllNames().size()));
	auto prev = mnRemoveSheetSelection;
	if (EGUI::Display::DropDownSelection("Remove From Sheet", mnRemoveSheetSelection, db->GetAllNames(), 150.f))
	{
		if (prev != mnRemoveSheetSelection)
			mnRemoveElementSelection = 0;
	}

	if (db->GetAllSheets().size() < 1)
		return;

	auto& sheet = db->GetAllSheets()[mnRemoveSheetSelection].second;
	auto& elems = sheet.GetAllElements();

	mnRemoveElementSelection = Math::Clamp(mnRemoveElementSelection, 0, static_cast<int>(elems.size()));
	if (!sheet.IsOpened())
	{
		EGUI::Display::Label("Sheet is <closed>");
		return;
	}
	else if (elems.size() < 0)
	{
		EGUI::Display::Label("Sheet is <empty>");
		return;
	}
	EGUI::Display::DropDownSelection("The Element Named", mnRemoveElementSelection, sheet.GetAllNames(), 150.f);
	EGUI::SameLine();
	if (EGUI::Display::Button("Delete") && mnRemoveElementSelection < elems.size())
	{
		if (sheet.RemoveElement(elems[mnRemoveElementSelection]->mName))
			DEBUG_PRINT(eLog::MESSAGE, "Successfully deleted element");
		else
			DEBUG_PRINT(eLog::MESSAGE, "Failed to delete element");
	}
}

void Editor::DataSheetEditor::TableHeaders(void)
{
	ImGui::BeginGroup();
	ImGui::Columns(4, "Headers");
	EGUI::Display::Label("%s", "Sheet");
	ImGui::NextColumn();
	EGUI::Display::Label("%s", "Element Name");
	ImGui::NextColumn();
	EGUI::Display::Label("%s", "Type");
	ImGui::NextColumn();
	EGUI::Display::Label("%s", "Value");
	ImGui::NextColumn();
	ImGui::EndGroup();
	ImGui::Separator();
}

void Editor::DataSheetEditor::HandleData(const HashString& _name, Dystopia::DataSheet& _dat)
{
	static const Math::Vec2 btnSize{50.f, 18.f};
	auto db = Dystopia::EngineCore::Get<Dystopia::DatabaseSystem>();
	auto& allElements = _dat.GetAllElements();
	auto midpoint = allElements.size() / 2;

	if (!_dat.IsOpened() || !allElements.size())
	{
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 0.f, 0.f });
		ImGui::BeginGroup();
		ImGui::Columns(4, _dat.GetOpened().c_str());
		if (EGUI::Display::Button("Unload", btnSize))
			db->UnloadData(_name);
		EGUI::SameLine();
		EGUI::Display::Label("%s <%s>", _name.c_str(), _dat.IsOpened() ? "empty" : "closed");
		if (!_dat.IsOpened())
		{
			EGUI::SameLine();
			if (EGUI::Display::Button("Open", btnSize))
				_dat.OpenSheet();
		}
		ImGui::EndGroup();
		ImGui::PopStyleVar();
	}
	else
	{
		bool close = false;
		bool unload = false;
		ImGui::BeginGroup();
		ImGui::Columns(4, _dat.GetOpened().c_str());
		for (int i = 0; i < allElements.size(); ++i)
		{
			auto v = _dat.MagicGet(allElements[i]->mName);
			if (!ImGui::GetColumnIndex() && i == midpoint)
			{
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 0.f, 0.f });
				unload = EGUI::Display::Button("Unload", btnSize);
				EGUI::SameLine();
				EGUI::Display::Label("%s", _name.c_str());
				EGUI::SameLine();
				close = EGUI::Display::Button("close", btnSize);
				ImGui::PopStyleVar();
			}
			ImGui::NextColumn();
			EGUI::Display::Label("%s", allElements[i]->mName.c_str());
			ImGui::NextColumn();
			EGUI::Display::Label("%d", allElements[i]->GetTypeN());
			ImGui::NextColumn();
			v.Visit(mUIVisitor);
			ImGui::NextColumn();
		}
		ImGui::EndGroup();
		if (close)
			_dat.CloseSheet();
		if (unload)
			db->UnloadData(_name);
	}
	ImGui::Separator();
}

#endif 












