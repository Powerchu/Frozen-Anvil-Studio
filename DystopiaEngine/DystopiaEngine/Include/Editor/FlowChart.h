/* HEADER *********************************************************************************/
/*!
\file	FlowChart.h
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#ifndef _FLOW_CHART_H_
#define _FLOW_CHART_H_
#include "DataStructure/AutoArray.h"
#include "DataStructure/HashString.h"
#include "Math/Vector2.h"
#include "Editor/EditorPanel.h"
#include "Editor/FlowChartData.h"

#include <vector>
#include <string>

namespace Editor
{
	struct File;
	class FlowChart : public EditorPanel
	{
	public:
		FlowChart(void);
		~FlowChart(void);

		void Load(void);
		bool Init(void);
		void Update(float);
		void EditorUI(void);
		void Shutdown(void);
		void Message(eEMessage);
		void SaveSettings(Dystopia::TextSerialiser& _out) const;
		void LoadSettings(Dystopia::TextSerialiser& _in);
		HashString GetLabel(void) const;
		void PanelClosing(void);

	private:

		HashString mLabel;
		HashString mCurrentDataFile;
		int mnWidth;
		int mnHeight;

		AutoArray<UserItem> mItems;
		AutoArray<UserLinks> mLinks;

		HashString GetWord(const char *);
		void ProcessData(File*);
		void ParseItems(FILE*);
		void ParseSubItem(int, FILE*);
		int AddItem(const HashString&);
		int FindItem(const HashString&) const;
		int FindItem(const uint64_t&) const;
		void ProcessLinks(void);
		void MakeFiles(void);

		void ParseHeaders(AutoArray<HashString>&);
		void ParseBodies(AutoArray<HashString>&, AutoArray<HashString>&);
		void ParseVars(AutoArray<HashString>&);
		void ParseConditions(std::vector<std::string>&, AutoArray<HashString>&, const UserItem*);
		void ParseStates();
		void ParseLoadState(AutoArray<HashString>&);
		AutoArray<HashString> ExtractAllLines(void* _stream, const char* _exitCode);
		std::vector<std::string> ClearBetweenCodes(const char* _file, const char * _entry, const char* _exit);
	};
}


#endif // _SPRITE_EDITOR_H_
#endif // EDITOR









