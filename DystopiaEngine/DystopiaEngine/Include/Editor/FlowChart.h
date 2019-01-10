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
#include "DataStructure/HashString.h"
#include "Math/Vector2.h"
#include "Editor/EditorPanel.h"

namespace Editor
{
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

	private:
		HashString mLabel;
		HashString mCurrentDataFile;

		void ProcessData(const HashString&);
	};
}


#endif // _SPRITE_EDITOR_H_
#endif // EDITOR









