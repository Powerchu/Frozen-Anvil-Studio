/* HEADER *********************************************************************************/
/*!
\file	ColorScheme.h
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#ifndef _COLOR_SCHEME_H_
#define _COLOR_SCHEME_H_
#include "Math/Vectors.h"
#include "DataStructure/Array.h"
#include "Editor/EditorPanel.h"

namespace Dystopia
{
	class TextSerialiser;
}

namespace Editor
{
	class ColorScheme : public EditorPanel//EditorTab
	{
	public:
		//static ColorScheme* GetInstance();
		ColorScheme(void);
		~ColorScheme(void);

		void Load(void);
		bool Init(void);
		void Update(float);
		void EditorUI(void);
		void Shutdown(void);
		void Message(eEMessage);
		void SaveSettings(Dystopia::TextSerialiser& _out) const;
		void LoadSettings(Dystopia::TextSerialiser& _in);
		HashString GetLabel(void) const;

		void Apply(void) const;
		void Remove(void) const;

	private:
		enum eColorData : unsigned int
		{
			eCOLOR_DATA_Text = 0,
			eCOLOR_DATA_TextDisabled,
			eCOLOR_DATA_TextSelected,
			eCOLOR_DATA_WindowBG,
			eCOLOR_DATA_ChildWindowBG,
			eCOLOR_DATA_PopupBG,
			eCOLOR_DATA_Border,
			eCOLOR_DATA_BorderShadow,
			eCOLOR_DATA_Widget,
			eCOLOR_DATA_WidgetHovered,
			eCOLOR_DATA_WidgetActive,
			eCOLOR_DATA_Title,
			eCOLOR_DATA_TitleHovered,
			eCOLOR_DATA_TitleActive,
			eCOLOR_DATA_MenuBar,
			eCOLOR_DATA_ScrollBG,
			eCOLOR_DATA_Scroll,
			eCOLOR_DATA_ScrollHovered,
			eCOLOR_DATA_ScrollActive,
			eCOLOR_DATA_CheckMark,
			eCOLOR_DATA_Slider,
			eCOLOR_DATA_SliderActive,
			eCOLOR_DATA_Button,
			eCOLOR_DATA_ButtonHovered,
			eCOLOR_DATA_ButtonActive,
			eCOLOR_DATA_Header,
			eCOLOR_DATA_HeaderHovered,
			eCOLOR_DATA_HeaderActive,
			eCOLOR_DATA_Separator,
			eCOLOR_DATA_SeparatorHovered,
			eCOLOR_DATA_SeparatorActive,
			eCOLOR_DATA_Resizer,
			eCOLOR_DATA_ResizerHovered,
			eCOLOR_DATA_ResizerActive,
			eCOLOR_DATA_Graph,
			eCOLOR_DATA_GraphHovered,
			eCOLOR_DATA_Payload,

			eCOLOR_DATA_Last
		};

		float								mBarSize;
		float								mAlignment;
		HashString							mLabel;
		Array<Math::Vec4, eCOLOR_DATA_Last> mArrColors;

		HashString ToName(eColorData);
	};
}

#endif //EDITOR
#endif //_COLOR_SCHEME_H_
