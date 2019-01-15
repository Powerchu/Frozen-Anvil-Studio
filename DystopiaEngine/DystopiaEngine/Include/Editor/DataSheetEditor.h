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
#include "Editor/EditorPanel.h"

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
		HashString GetLabel(void) const;

	private:

		HashString mLabel;
		HashString mNewAdditionName;
		int mnCurrentSheet;
		Dystopia::DatabaseSystem *mpDB;

		void HandleData(Dystopia::DataSheet&);

	};
}


#endif
#endif











