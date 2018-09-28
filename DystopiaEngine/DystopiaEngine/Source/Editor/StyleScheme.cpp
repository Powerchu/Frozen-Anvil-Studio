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

namespace Dystopia
{
	static StyleScheme* gpInstance = 0;

	StyleScheme* StyleScheme::GetInstance()
	{
		if (gpInstance) return gpInstance;

		gpInstance = new StyleScheme{};
		return gpInstance;
	}
}

static constexpr float gGenericMax = 10.f;
Dystopia::StyleScheme::StyleScheme()
	: EditorTab{ false },
	mLabel{ "Style Scheme" },
	mArrVecData{}, mArrFData{}
{}


Dystopia::StyleScheme::~StyleScheme()
{}

void Dystopia::StyleScheme::Init()
{
	auto& style = ImGui::GetStyle();

	mArrVecData[eWINDOW_PADDING]		 = Dystopia::StyleScheme::StyleVarsV{style.WindowPadding, 0.1f, gGenericMax};
	mArrVecData[eWINDOW_MIN_SIZE]		 = Dystopia::StyleScheme::StyleVarsV{ style.WindowMinSize, 0.1f, gGenericMax };
	mArrVecData[eWINDOW_TITLE_ALIGNMENT] = Dystopia::StyleScheme::StyleVarsV{ style.WindowTitleAlign, 0.1f, gGenericMax };
	mArrVecData[eFRAME_PADDING]			 = Dystopia::StyleScheme::StyleVarsV{ style.FramePadding, 0.1f, gGenericMax };
	mArrVecData[eITEM_SPACING]			 = Dystopia::StyleScheme::StyleVarsV{ style.ItemSpacing, 0.1f, gGenericMax };
	mArrVecData[eITEM_SPACING_INNER]	 = Dystopia::StyleScheme::StyleVarsV{ style.ItemInnerSpacing, 0.1f, gGenericMax };
	mArrVecData[eBUTTON_TEXT_ALIGNMENT]  = Dystopia::StyleScheme::StyleVarsV{ style.ButtonTextAlign, 0.1f, gGenericMax };

	mArrFData[eALPHA]					 = Dystopia::StyleScheme::StyleVarsF{style.Alpha, 0.1f, 1.0f};
	mArrFData[eWINDOW_ROUNDING]			 = Dystopia::StyleScheme::StyleVarsF{style.WindowRounding, 0.f, gGenericMax };
	mArrFData[eWINDOW_BORDER]			 = Dystopia::StyleScheme::StyleVarsF{style.WindowBorderSize, 0.f, gGenericMax };
	mArrFData[eCHILD_ROUNDING]			 = Dystopia::StyleScheme::StyleVarsF{style.ChildRounding, 0.f, gGenericMax};
	mArrFData[eCHILD_BORDER]			 = Dystopia::StyleScheme::StyleVarsF{style.ChildBorderSize, 0.f, gGenericMax};
	mArrFData[ePOPUP_ROUNDING]			 = Dystopia::StyleScheme::StyleVarsF{style.PopupRounding, 0.f, gGenericMax};
	mArrFData[ePOPUP_BORDER]			 = Dystopia::StyleScheme::StyleVarsF{style.PopupBorderSize, 0.f, gGenericMax};
	mArrFData[eFRAME_ROUNDING]			 = Dystopia::StyleScheme::StyleVarsF{style.FrameRounding, 0.f, gGenericMax };
	mArrFData[eFRAME_BORDER]			 = Dystopia::StyleScheme::StyleVarsF{style.FrameBorderSize, 0.f, gGenericMax};
	mArrFData[eINDENT_SPACE]			 = Dystopia::StyleScheme::StyleVarsF{style.IndentSpacing, 0.f,gGenericMax};
	mArrFData[eSCROLL_SIZE]				 = Dystopia::StyleScheme::StyleVarsF{style.ScrollbarSize, 0.f, gGenericMax};
	mArrFData[eSCROLL_ROUNDING]			 = Dystopia::StyleScheme::StyleVarsF{style.ScrollbarRounding, 0.f, gGenericMax};
	mArrFData[eGRAB_SIZE]				 = Dystopia::StyleScheme::StyleVarsF{style.GrabMinSize, 0.f, gGenericMax};
	mArrFData[eGRAB_ROUNDING]			 = Dystopia::StyleScheme::StyleVarsF{style.GrabRounding, 0.f, gGenericMax};
}

void Dystopia::StyleScheme::Update(const float&)
{}

void Dystopia::StyleScheme::EditorUI()
{
	//if (EGUI::Display::Button("Apply"))
	//{
	//	Apply();
	//}
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
		auto v = EGUI::Display::VectorFields("", &mArrVecData[i].mValue, 0.1f, mArrVecData[i].mMin, mArrVecData[i].mMax);
		for (auto e : v)
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

void Dystopia::StyleScheme::Shutdown()
{}

std::string Dystopia::StyleScheme::GetLabel() const
{
	return mLabel;
}

void Dystopia::StyleScheme::Apply() const
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

void Dystopia::StyleScheme::Remove() const
{
}

void Dystopia::StyleScheme::SaveSettings(Dystopia::TextSerialiser& _out) const
{
	_out.InsertStartBlock("Style");
	_out << mArrFData.size();
	_out << mArrVecData.size();
	for (auto& e : mArrFData)
	{
		_out << e.mValue;
	}
	for (auto& e : mArrVecData)
	{
		for (unsigned int i = 0; i < 2; ++i)
		{
			_out << e.mValue[i];
		}
	}
	_out.InsertEndBlock("Style");
}

void Dystopia::StyleScheme::LoadSettings(Dystopia::TextSerialiser& _in)
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

std::string Dystopia::StyleScheme::ToName(eStyleDataVectors _i)
{
	switch (_i)
	{
	case eWINDOW_PADDING:				return "Window Padding";
	case eWINDOW_MIN_SIZE:				return "Window Minimum Size";
	case eWINDOW_TITLE_ALIGNMENT:		return "Window Title Alignemnt";
	case eFRAME_PADDING:				return "Frame Padding";
	case eITEM_SPACING:					return "Item Spacing";
	case eITEM_SPACING_INNER:			return "Item Spacing-Inner";
	case eBUTTON_TEXT_ALIGNMENT:		return "Button Text Alignemnt";
	default: return "Error";
	}
}

std::string Dystopia::StyleScheme::ToName(eStyleDataFloats _i)
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
	case eGRAB_SIZE:			return "Scroll's Bar Size";
	case eGRAB_ROUNDING:		return "Scroll's Bar Rounding";
	default: return "Error";
	}
}

#endif // EDITOR

