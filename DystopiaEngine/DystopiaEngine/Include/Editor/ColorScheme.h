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
#include "Math/Vector4.h"
#include "DataStructure/Array.h"
#include "Editor/EditorTab.h"

namespace Dystopia
{
	class TextSerialiser;
	class ColorScheme : public EditorTab
	{
	public:
		static ColorScheme* GetInstance();
		~ColorScheme();

		/* Init() is called immediately after the creation of the object */
		void		Init();
		/* Update() is called before Window(), so alter most variables (frame based) here to be printed in Window() later */
		void		Update(const float&);
		/* Window() is where you do the EGUI/IMGUI functions. GUI variable changes will be recorded here */
		void		EditorUI();
		/* Shutdown() is called right before deleting this object */
		void		Shutdown();
		/* GetLabel() returns the string to identify this class. EditorTab requires this to create a tab for you using the label */
		std::string GetLabel() const;

		void SaveSettings(TextSerialiser& _out) const;
		void LoadSettings(TextSerialiser& _in);

		void Apply() const;
		void Remove() const;

	private:
		ColorScheme();
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

		int									mBarSize;
		int									mAlignment;
		std::string							mLabel;
		Array<Math::Vec4, eCOLOR_DATA_Last> mArrColors;

		std::string ToName(eColorData);
	};
}

#endif //_COLOR_SCHEME_H_
#endif //EDITOR
