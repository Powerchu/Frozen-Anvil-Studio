/* HEADER *********************************************************************************/
/*!
\file	ColorScheme.cpp
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
#include "Editor/ColorScheme.h"
#include "IO/BinarySerializer.h"
#include "../../Dependancies/ImGui/imgui.h"

static constexpr unsigned int dimensions = 4;
namespace Dystopia
{
	static ColorScheme* gpInstance = 0;

	ColorScheme* ColorScheme::GetInstance()
	{
		if (gpInstance) return gpInstance;

		gpInstance = new ColorScheme{};
		return gpInstance;
	}
}

Dystopia::ColorScheme::ColorScheme()
	: EditorTab{ false },
	mLabel{ "Color Scheme" },
	mArrColors{}, mAlignment{ 150 }, mBarSize{ 500 }
{}

Dystopia::ColorScheme::~ColorScheme()
{}

void Dystopia::ColorScheme::Init()
{
	auto& style = ImGui::GetStyle();
	auto arrayOfColors = style.Colors;

	mArrColors[eCOLOR_DATA_Text]				 = arrayOfColors[ImGuiCol_Text];				
	mArrColors[eCOLOR_DATA_TextDisabled]		 = arrayOfColors[ImGuiCol_TextDisabled];		
	mArrColors[eCOLOR_DATA_TextSelected]		 = arrayOfColors[ImGuiCol_WindowBg];			
	mArrColors[eCOLOR_DATA_WindowBG]			 = arrayOfColors[ImGuiCol_ChildBg];				
	mArrColors[eCOLOR_DATA_ChildWindowBG]		 = arrayOfColors[ImGuiCol_PopupBg];				
	mArrColors[eCOLOR_DATA_PopupBG]				 = arrayOfColors[ImGuiCol_Border];				
	mArrColors[eCOLOR_DATA_Border]				 = arrayOfColors[ImGuiCol_BorderShadow];		
	mArrColors[eCOLOR_DATA_BorderShadow]		 = arrayOfColors[ImGuiCol_FrameBg];				
	mArrColors[eCOLOR_DATA_Widget]				 = arrayOfColors[ImGuiCol_FrameBgHovered];		
	mArrColors[eCOLOR_DATA_WidgetHovered]		 = arrayOfColors[ImGuiCol_FrameBgActive];		
	mArrColors[eCOLOR_DATA_WidgetActive]		 = arrayOfColors[ImGuiCol_TitleBg];				
	mArrColors[eCOLOR_DATA_Title]				 = arrayOfColors[ImGuiCol_TitleBgActive];		
	mArrColors[eCOLOR_DATA_TitleHovered]		 = arrayOfColors[ImGuiCol_TitleBgCollapsed];	
	mArrColors[eCOLOR_DATA_TitleActive]			 = arrayOfColors[ImGuiCol_MenuBarBg];			
	mArrColors[eCOLOR_DATA_MenuBar]				 = arrayOfColors[ImGuiCol_ScrollbarBg];			
	mArrColors[eCOLOR_DATA_ScrollBG]			 = arrayOfColors[ImGuiCol_ScrollbarGrab];		
	mArrColors[eCOLOR_DATA_Scroll]				 = arrayOfColors[ImGuiCol_ScrollbarGrabHovered];
	mArrColors[eCOLOR_DATA_ScrollHovered]		 = arrayOfColors[ImGuiCol_ScrollbarGrabActive];	
	mArrColors[eCOLOR_DATA_ScrollActive]		 = arrayOfColors[ImGuiCol_CheckMark];			
	mArrColors[eCOLOR_DATA_CheckMark]			 = arrayOfColors[ImGuiCol_SliderGrab];			
	mArrColors[eCOLOR_DATA_Slider]				 = arrayOfColors[ImGuiCol_SliderGrabActive];	
	mArrColors[eCOLOR_DATA_SliderActive]		 = arrayOfColors[ImGuiCol_Button];				
	mArrColors[eCOLOR_DATA_Button]				 = arrayOfColors[ImGuiCol_ButtonHovered];		
	mArrColors[eCOLOR_DATA_ButtonHovered]		 = arrayOfColors[ImGuiCol_ButtonActive];		
	mArrColors[eCOLOR_DATA_ButtonActive]		 = arrayOfColors[ImGuiCol_Header];				
	mArrColors[eCOLOR_DATA_Header]				 = arrayOfColors[ImGuiCol_HeaderHovered];		
	mArrColors[eCOLOR_DATA_HeaderHovered]		 = arrayOfColors[ImGuiCol_HeaderActive];		
	mArrColors[eCOLOR_DATA_HeaderActive]		 = arrayOfColors[ImGuiCol_Separator];			
	mArrColors[eCOLOR_DATA_Separator]			 = arrayOfColors[ImGuiCol_SeparatorHovered];	
	mArrColors[eCOLOR_DATA_SeparatorHovered]	 = arrayOfColors[ImGuiCol_SeparatorActive];		
	mArrColors[eCOLOR_DATA_SeparatorActive]		 = arrayOfColors[ImGuiCol_ResizeGrip];			
	mArrColors[eCOLOR_DATA_Resizer]				 = arrayOfColors[ImGuiCol_ResizeGripHovered];	
	mArrColors[eCOLOR_DATA_ResizerHovered]		 = arrayOfColors[ImGuiCol_ResizeGripActive];	
	mArrColors[eCOLOR_DATA_ResizerActive]		 = arrayOfColors[ImGuiCol_PlotLines];			
	mArrColors[eCOLOR_DATA_Graph]				 = arrayOfColors[ImGuiCol_PlotLinesHovered];	
	mArrColors[eCOLOR_DATA_GraphHovered]		 = arrayOfColors[ImGuiCol_TextSelectedBg];		
	mArrColors[eCOLOR_DATA_Payload]				 = arrayOfColors[ImGuiCol_DragDropTarget];	
}

void Dystopia::ColorScheme::Update(const float&)
{}

void Dystopia::ColorScheme::EditorUI()
{
	//if (EGUI::Display::Button("Apply"))
	//{
	//	Apply();
	//}
	ImGui::PushItemWidth(mBarSize);
	for (unsigned int i = 0; i < mArrColors.size(); ++i)
	{
		EGUI::PushID(i);
		EGUI::Display::Label(ToName(static_cast<eColorData>(i)).c_str());
		ImGui::SameLine(mAlignment);
		if (ImGui::ColorEdit4("MyColor##2f", (float*)&mArrColors[i], ImGuiColorEditFlags_Float))
			Apply();
		EGUI::PopID();
	}
	ImGui::PopItemWidth();
}

void Dystopia::ColorScheme::Shutdown()
{}

std::string Dystopia::ColorScheme::GetLabel() const
{
	return mLabel;
}

void Dystopia::ColorScheme::Apply()const
{
	auto& style = ImGui::GetStyle();
	auto arrayOfColors = style.Colors;

    arrayOfColors[ImGuiCol_Text]					= mArrColors[eCOLOR_DATA_Text];			
    arrayOfColors[ImGuiCol_TextDisabled]			= mArrColors[eCOLOR_DATA_TextDisabled];	
    arrayOfColors[ImGuiCol_TextSelectedBg]			= mArrColors[eCOLOR_DATA_TextSelected];
	arrayOfColors[ImGuiCol_WindowBg]				= mArrColors[eCOLOR_DATA_WindowBG];
    arrayOfColors[ImGuiCol_ChildBg]					= mArrColors[eCOLOR_DATA_ChildWindowBG];	
    arrayOfColors[ImGuiCol_PopupBg]					= mArrColors[eCOLOR_DATA_PopupBG];			
    arrayOfColors[ImGuiCol_Border]					= mArrColors[eCOLOR_DATA_Border];			
    arrayOfColors[ImGuiCol_BorderShadow]			= mArrColors[eCOLOR_DATA_BorderShadow];	   
    arrayOfColors[ImGuiCol_FrameBg]					= mArrColors[eCOLOR_DATA_Widget];			
    arrayOfColors[ImGuiCol_FrameBgHovered]			= mArrColors[eCOLOR_DATA_WidgetHovered];	
    arrayOfColors[ImGuiCol_FrameBgActive]			= mArrColors[eCOLOR_DATA_WidgetActive];	
    arrayOfColors[ImGuiCol_TitleBg]					= mArrColors[eCOLOR_DATA_Title];			
    arrayOfColors[ImGuiCol_TitleBgActive]			= mArrColors[eCOLOR_DATA_TitleHovered];	
    arrayOfColors[ImGuiCol_TitleBgCollapsed]		= mArrColors[eCOLOR_DATA_TitleActive];		
    arrayOfColors[ImGuiCol_MenuBarBg]				= mArrColors[eCOLOR_DATA_MenuBar];			
    arrayOfColors[ImGuiCol_ScrollbarBg]				= mArrColors[eCOLOR_DATA_ScrollBG];		
    arrayOfColors[ImGuiCol_ScrollbarGrab]			= mArrColors[eCOLOR_DATA_Scroll];			
    arrayOfColors[ImGuiCol_ScrollbarGrabHovered]	= mArrColors[eCOLOR_DATA_ScrollHovered];	
    arrayOfColors[ImGuiCol_ScrollbarGrabActive]		= mArrColors[eCOLOR_DATA_ScrollActive];	
    arrayOfColors[ImGuiCol_CheckMark]				= mArrColors[eCOLOR_DATA_CheckMark];		
    arrayOfColors[ImGuiCol_SliderGrab]				= mArrColors[eCOLOR_DATA_Slider];			
    arrayOfColors[ImGuiCol_SliderGrabActive]		= mArrColors[eCOLOR_DATA_SliderActive];	
    arrayOfColors[ImGuiCol_Button]					= mArrColors[eCOLOR_DATA_Button];			
    arrayOfColors[ImGuiCol_ButtonHovered]			= mArrColors[eCOLOR_DATA_ButtonHovered];	
    arrayOfColors[ImGuiCol_ButtonActive]			= mArrColors[eCOLOR_DATA_ButtonActive];	
    arrayOfColors[ImGuiCol_Header]					= mArrColors[eCOLOR_DATA_Header];			
    arrayOfColors[ImGuiCol_HeaderHovered]			= mArrColors[eCOLOR_DATA_HeaderHovered];	
    arrayOfColors[ImGuiCol_HeaderActive]			= mArrColors[eCOLOR_DATA_HeaderActive];	
    arrayOfColors[ImGuiCol_Separator]				= mArrColors[eCOLOR_DATA_Separator];		
    arrayOfColors[ImGuiCol_SeparatorHovered]		= mArrColors[eCOLOR_DATA_SeparatorHovered];
    arrayOfColors[ImGuiCol_SeparatorActive]			= mArrColors[eCOLOR_DATA_SeparatorActive];	
    arrayOfColors[ImGuiCol_ResizeGrip]				= mArrColors[eCOLOR_DATA_Resizer];			
    arrayOfColors[ImGuiCol_ResizeGripHovered]		= mArrColors[eCOLOR_DATA_ResizerHovered];	
    arrayOfColors[ImGuiCol_ResizeGripActive]		= mArrColors[eCOLOR_DATA_ResizerActive];	
    arrayOfColors[ImGuiCol_PlotLines]				= mArrColors[eCOLOR_DATA_Graph];			
    arrayOfColors[ImGuiCol_PlotLinesHovered]		= mArrColors[eCOLOR_DATA_GraphHovered];	
    arrayOfColors[ImGuiCol_DragDropTarget]			= mArrColors[eCOLOR_DATA_Payload];			
}

void Dystopia::ColorScheme::Remove() const
{
	ImGui::StyleColorsDark();
}

void Dystopia::ColorScheme::SaveSettings(Dystopia::BinarySerializer& _out) const
{
	_out.InsertStartBlock();
	for (auto& e : mArrColors)
	{
		for (unsigned int i = 0; i < dimensions; ++i)
		{
			_out << e[i];
		}
	}
	_out.InsertEndBlock();
}

void Dystopia::ColorScheme::LoadSettings(Dystopia::BinarySerializer& _in)
{
	_in.ConsumeStartBlock();
	for (auto& e : mArrColors)
	{
		for (unsigned int i = 0; i < dimensions; ++i)
		{
			_in >> e[i];
		}
	}
	_in.ConsumeEndBlock();
}

std::string Dystopia::ColorScheme::ToName(eColorData _i)
{
	switch (_i)
	{
		case eCOLOR_DATA_Text:				return "Text";
		case eCOLOR_DATA_TextDisabled:		return "TextDisabled";
		case eCOLOR_DATA_TextSelected:		return "TextSelected";
		case eCOLOR_DATA_WindowBG:			return "WindowBG";
		case eCOLOR_DATA_ChildWindowBG:		return "ChildWindowBG";
		case eCOLOR_DATA_PopupBG:			return "PopupBG";
		case eCOLOR_DATA_Border:			return "Border";
		case eCOLOR_DATA_BorderShadow:		return "BorderShadow";
		case eCOLOR_DATA_Widget:			return "Widget";
		case eCOLOR_DATA_WidgetHovered:		return "WidgetHovered";
		case eCOLOR_DATA_WidgetActive:		return "WidgetActive";
		case eCOLOR_DATA_Title:				return "Title";
		case eCOLOR_DATA_TitleHovered:		return "TitleHovered";
		case eCOLOR_DATA_TitleActive:		return "TitleActive";
		case eCOLOR_DATA_MenuBar:			return "MenuBar";
		case eCOLOR_DATA_ScrollBG:			return "ScrollBG";
		case eCOLOR_DATA_Scroll:			return "Scroll";
		case eCOLOR_DATA_ScrollHovered:		return "ScrollHovered";
		case eCOLOR_DATA_ScrollActive:		return "ScrollActive";
		case eCOLOR_DATA_CheckMark:			return "CheckMark";
		case eCOLOR_DATA_Slider:			return "Slider";
		case eCOLOR_DATA_SliderActive:		return "SliderActive";
		case eCOLOR_DATA_Button:			return "Button";
		case eCOLOR_DATA_ButtonHovered:		return "ButtonHovered";
		case eCOLOR_DATA_ButtonActive:		return "ButtonActive";
		case eCOLOR_DATA_Header:			return "Header";
		case eCOLOR_DATA_HeaderHovered:		return "HeaderHovered";
		case eCOLOR_DATA_HeaderActive:		return "HeaderActive";
		case eCOLOR_DATA_Separator:			return "Separator";
		case eCOLOR_DATA_SeparatorHovered:	return "SeparatorHovered";
		case eCOLOR_DATA_SeparatorActive:	return "SeparatorActive";
		case eCOLOR_DATA_Resizer:			return "Resizer";
		case eCOLOR_DATA_ResizerHovered:	return "ResizerHovered";
		case eCOLOR_DATA_ResizerActive:		return "ResizerActive";
		case eCOLOR_DATA_Graph:				return "Graph";
		case eCOLOR_DATA_GraphHovered:		return "GraphHovered";
		case eCOLOR_DATA_Payload:			return "Payload";
		default: return "Error";
	}
}

#endif //EDITOR
