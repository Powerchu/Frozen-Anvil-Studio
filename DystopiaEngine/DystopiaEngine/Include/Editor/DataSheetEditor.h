/* HEADER *********************************************************************************/
/*!
\file	DataSheetEditor.h
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#ifndef _DATA_SHEET_EDITOR_H_
#define _DATA_SHEET_EDITOR_H_

#include "DataStructure/AutoArray.h"
#include "DataStructure/HashString.h"
#include "Utility/Utility.h"

#include "Editor/EditorPanel.h"
#include "Editor/EGUI.h"

namespace Dystopia
{
	class DatabaseSystem;
	class DataSheet;
}

namespace Editor
{
	class DataSheetEditor : public EditorPanel
	{
	public:
		DataSheetEditor(void);
		~DataSheetEditor(void);

		void Load(void);
		bool Init(void);
		void Update(float);
		void EditorUI(void);
		void Shutdown(void);
		void Message(eEMessage);
		void SaveSettings(Dystopia::TextSerialiser& _out) const;
		void LoadSettings(Dystopia::TextSerialiser& _in);
		const HashString& GetLabel(void) const;

		struct UIVisitor
		{
			template<typename T>
			void operator()(const T& _variant);
			template<>
			void operator()(const int& _variant);
			template<>
			void operator()(const float& _variant);
			template<>
			void operator()(const bool& _variant);
			template<>
			void operator()(const char& _variant);
			template<>
			void operator()(const HashString& _variant);
		};

	private:

		HashString mLabel;
		HashString mNewAdditionName;
		int mnCurrentSheet;
		int mnSheetSelection;
		int mnTypeSelection;
		int mnRemoveSheetSelection;
		int mnRemoveElementSelection;
		Dystopia::DatabaseSystem *mpDB;
		UIVisitor mUIVisitor;

		void HandleSheets(void);
		bool HandleInserting(const char*, unsigned);
		void HandleRemove(void);
		void TableHeaders(void);
		void HandleData(const HashString&, Dystopia::DataSheet&);

	};


	// ============================================ DEFINITIONS ============================================ // 


	template<typename T>
	inline void DataSheetEditor::UIVisitor::operator()(const T& _variant)
	{
		EGUI::Display::Label("ERROR in DataSheetEditor::UIVisitor");
	}
	template<>
	inline void DataSheetEditor::UIVisitor::operator()(const int& _variant)
	{
		EGUI::Display::Label("%d", _variant);
	}
	template<>
	inline void DataSheetEditor::UIVisitor::operator()(const float& _variant)
	{
		EGUI::Display::Label("%f", _variant);
	}
	template<>
	inline void DataSheetEditor::UIVisitor::operator()(const bool& _variant)
	{
		EGUI::Display::Label("%s", _variant ? "true" : "false");
	}
	template<>
	inline void DataSheetEditor::UIVisitor::operator()(const char& _variant)
	{
		EGUI::Display::Label("%c", _variant);
	}
	template<>
	inline void DataSheetEditor::UIVisitor::operator()(const HashString& _variant)
	{
		EGUI::Display::Label("%s", _variant.c_str());
	}


}


#endif
#endif











