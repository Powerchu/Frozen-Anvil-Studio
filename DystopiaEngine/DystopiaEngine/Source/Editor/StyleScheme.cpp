/* HEADER *********************************************************************************/
/*!
\file	StyleScheme.cpp
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#include "Editor/EGUI.h"
#include "Editor/StyleScheme.h"
#include "IO/TextSerialiser.h"
#include "../../Dependancies/ImGui/imgui.h"

/*namespace Dystopia
{
	static StyleScheme* gpInstance = 0;

	StyleScheme* StyleScheme::GetInstance()
	{
		if (gpInstance) return gpInstance;

		gpInstance = new StyleScheme{};
		return gpInstance;
	}
}*/


static constexpr float gGenericMax = 10.f;

Editor::StyleScheme::StyleScheme(void)
	: //EditorTab{ false },
	mLabel{ "Style Scheme" },
	mArrVecData{}, mArrFData{}
{}


Editor::StyleScheme::~StyleScheme(void)
{}

void Editor::StyleScheme::Load(void)
{}

bool Editor::StyleScheme::Init(void)
{
	auto& style = ImGui::GetStyle();

	mArrVecData[eWINDOW_PADDING] = Editor::StyleScheme::StyleVarsV{ ImVec2{ 5, 10 }, 0.1f, gGenericMax };
	mArrVecData[eWINDOW_MIN_SIZE] = Editor::StyleScheme::StyleVarsV{ ImVec2{256.f,256.f}, 0.1f, gGenericMax };
	mArrVecData[eWINDOW_TITLE_ALIGNMENT] = Editor::StyleScheme::StyleVarsV{ ImVec2{0.005f, 0.5f}, 0.1f, gGenericMax };
	mArrVecData[eFRAME_PADDING] = Editor::StyleScheme::StyleVarsV{ ImVec2{4,4}, 0.1f, gGenericMax };
	mArrVecData[eITEM_SPACING] = Editor::StyleScheme::StyleVarsV{ ImVec2{10,4}, 0.1f, gGenericMax };
	mArrVecData[eITEM_SPACING_INNER] = Editor::StyleScheme::StyleVarsV{ ImVec2{7,5}, 0.1f, gGenericMax };
	mArrVecData[eBUTTON_TEXT_ALIGNMENT] = Editor::StyleScheme::StyleVarsV{ style.ButtonTextAlign, 0.1f, gGenericMax };

	mArrFData[eALPHA] = Editor::StyleScheme::StyleVarsF{ style.Alpha, 0.1f, 1.0f };
	mArrFData[eWINDOW_ROUNDING] = Editor::StyleScheme::StyleVarsF{ 4.f, 0.f, gGenericMax };
	mArrFData[eWINDOW_BORDER] = Editor::StyleScheme::StyleVarsF{ 1.f, 0.f, gGenericMax };
	mArrFData[eCHILD_ROUNDING] = Editor::StyleScheme::StyleVarsF{ 0.f, 0.f, gGenericMax };
	mArrFData[eCHILD_BORDER] = Editor::StyleScheme::StyleVarsF{ 0.f, 0.f, gGenericMax };
	mArrFData[ePOPUP_ROUNDING] = Editor::StyleScheme::StyleVarsF{ 2.f, 0.f, gGenericMax };
	mArrFData[ePOPUP_BORDER] = Editor::StyleScheme::StyleVarsF{ 0.f, 0.f, gGenericMax };
	mArrFData[eFRAME_ROUNDING] = Editor::StyleScheme::StyleVarsF{ 7.f, 0.f, gGenericMax };
	mArrFData[eFRAME_BORDER] = Editor::StyleScheme::StyleVarsF{ 0.f, 0.f, gGenericMax };
	mArrFData[eINDENT_SPACE] = Editor::StyleScheme::StyleVarsF{ 10.f, 0.f,gGenericMax };
	mArrFData[eSCROLL_SIZE] = Editor::StyleScheme::StyleVarsF{ 13.f, 0.f, gGenericMax };
	mArrFData[eSCROLL_ROUNDING] = Editor::StyleScheme::StyleVarsF{ 10.f, 0.f, gGenericMax };
	mArrFData[eGRAB_SIZE] = Editor::StyleScheme::StyleVarsF{ 4.f, 0.f, gGenericMax };
	mArrFData[eGRAB_ROUNDING] = Editor::StyleScheme::StyleVarsF{ 2.f, 0.f, gGenericMax };
	return true;
}

void Editor::StyleScheme::Update(float)
{}

void Editor::StyleScheme::EditorUI(void)
{
	for (unsigned int i = 0; i < mArrFData.size(); ++i)
	{
		EGUI::PushID(i);
		EGUI::Display::Label(ToName(static_cast<eStyleDataFloats>(i)).c_str());
		if (EGUI::Display::DragFloat("FloatVars", &mArrFData[i].mValue, 0.1f, mArrFData[i].mMin, mArrFData[i].mMax, true))
			Apply();
		EGUI::PopID();
	}
	for (unsigned int i = 0; i < mArrVecData.size(); ++i)
	{
		EGUI::PushID(i);
		EGUI::Display::Label(ToName(static_cast<eStyleDataVectors>(i)).c_str());
		ImGui::SameLine(160);
		const auto v = EGUI::Display::VectorFields("", &mArrVecData[i].mValue, 0.1f, mArrVecData[i].mMin, mArrVecData[i].mMax);
		for (const auto& e : v)
		{
			switch (e)
			{
			case EGUI::eDragStatus::eNO_CHANGE:
				break;
			default: Apply(); break;
			}
		}

		EGUI::PopID();
	}
}

void Editor::StyleScheme::Shutdown(void)
{}

void Editor::StyleScheme::Message(eEMessage)
{}

void Editor::StyleScheme::SaveSettings(Dystopia::TextSerialiser& _out) const
{
	_out.InsertStartBlock("Style");
	_out << mArrFData.size();
	_out << mArrVecData.size();
	for (const auto& e : mArrFData)
	{
		_out << e.mValue;
	}
	for (const auto& e : mArrVecData)
	{
		for (unsigned int i = 0; i < 2; ++i)
		{
			_out << e.mValue[i];
		}
	}
	_out.InsertEndBlock("Style");
}

void Editor::StyleScheme::LoadSettings(Dystopia::TextSerialiser& _in)
{
	_in.ConsumeStartBlock();
	size_t size1;
	size_t size2;
	_in >> size1;
	_in >> size2;
	for (unsigned int i = 0; i < size1; ++i)
	{
		_in >> mArrFData[i].mValue;
	}
	for (unsigned int i = 0; i < size2; ++i)
	{
		for (unsigned int j = 0; j < 2; ++j)
		{
			_in >> mArrVecData[i].mValue[j];
		}
	}
	_in.ConsumeEndBlock();
	Apply();
}

const HashString& Editor::StyleScheme::GetLabel(void) const
{
	return mLabel;
}

void Editor::StyleScheme::Apply(void) const
{
	auto& style = ImGui::GetStyle();
	
	style.WindowPadding 		 =	mArrVecData[eWINDOW_PADDING].mValue;		  
	style.WindowMinSize 		 =	mArrVecData[eWINDOW_MIN_SIZE].mValue;		  
	style.WindowTitleAlign 		 =	mArrVecData[eWINDOW_TITLE_ALIGNMENT].mValue;  
	style.FramePadding 			 =	mArrVecData[eFRAME_PADDING].mValue;			  
	style.ItemSpacing 			 =	mArrVecData[eITEM_SPACING].mValue;			  
	style.ItemInnerSpacing 		 =	mArrVecData[eITEM_SPACING_INNER].mValue;	  
	style.ButtonTextAlign 		 =	mArrVecData[eBUTTON_TEXT_ALIGNMENT].mValue;   
								 
	style.Alpha 				 =	mArrFData[eALPHA].mValue;					  
	style.WindowRounding 		 =	mArrFData[eWINDOW_ROUNDING].mValue;
	style.WindowBorderSize 		 =	mArrFData[eWINDOW_BORDER].mValue;
	style.ChildRounding 		 =	mArrFData[eCHILD_ROUNDING].mValue;
	style.ChildBorderSize 		 =	mArrFData[eCHILD_BORDER].mValue;
	style.PopupRounding 		 =	mArrFData[ePOPUP_ROUNDING].mValue;
	style.PopupBorderSize 		 =	mArrFData[ePOPUP_BORDER].mValue;
	style.FrameRounding 		 =	mArrFData[eFRAME_ROUNDING].mValue;
	style.FrameBorderSize 		 =	mArrFData[eFRAME_BORDER].mValue;			  
	style.IndentSpacing 		 =	mArrFData[eINDENT_SPACE].mValue;			  
	style.ScrollbarSize 		 =	mArrFData[eSCROLL_SIZE].mValue;
	style.ScrollbarRounding 	 =	mArrFData[eSCROLL_ROUNDING].mValue;
	style.GrabMinSize 			 =	mArrFData[eGRAB_SIZE].mValue;
	style.GrabRounding 			 =	mArrFData[eGRAB_ROUNDING].mValue;
}

HashString Editor::StyleScheme::ToName(eStyleDataVectors _i)
{
	switch (_i)
	{
	case eWINDOW_PADDING:				return "Window Padding";
	case eWINDOW_MIN_SIZE:				return "Window Minimum Size";
	case eWINDOW_TITLE_ALIGNMENT:		return "Window Title Alignment";
	case eFRAME_PADDING:				return "Frame Padding";
	case eITEM_SPACING:					return "Item Spacing";
	case eITEM_SPACING_INNER:			return "Item Spacing-Inner";
	case eBUTTON_TEXT_ALIGNMENT:		return "Button Text Alignment";
	default: return "Error";
	}
}

HashString Editor::StyleScheme::ToName(eStyleDataFloats _i)
{
	switch (_i)
	{
	case eALPHA:				return "Alpha";
	case eWINDOW_ROUNDING:		return "Window Rounding";
	case eWINDOW_BORDER:		return "Window Border";
	case eCHILD_ROUNDING:		return "Child Window Rounding";
	case eCHILD_BORDER:			return "Child Window Rounding Border";
	case ePOPUP_ROUNDING:		return "Pop-up Rounding";
	case ePOPUP_BORDER:			return "Pop-up Border";
	case eFRAME_ROUNDING:		return "Frame Rounding";
	case eFRAME_BORDER:			return "Frame Border";
	case eINDENT_SPACE:			return "Indent Space";
	case eSCROLL_SIZE:			return "Scroll Size";
	case eSCROLL_ROUNDING:		return "Scroll rounding";
	case eGRAB_SIZE:			return "Corner Grab Size";
	case eGRAB_ROUNDING:		return "Corner Grab Rounding";
	default: return "Error";
	}
}

#endif // EDITOR

