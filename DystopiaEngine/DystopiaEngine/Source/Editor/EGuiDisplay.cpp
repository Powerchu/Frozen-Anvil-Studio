/* HEADER *********************************************************************************/
/*!
\file	EGuiDisplay.cpp
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief
BRIEF HERE

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#include "Editor\EGUI.h"
#include "../../Dependancies/ImGui/imgui.h"
#include "../../Dependancies/ImGui/imgui_internal.h"
#include <iostream>

namespace EGUI
{
	bool StartTab(const char *_pLabel, bool *_pOpen, ImGuiWindowFlags _flags)
	{
		return EGUI::Docking::BeginTabs(_pLabel, _pOpen, _flags);
	}

	void EndTab()
	{
		EGUI::Docking::EndTabs();
	}

	bool StartMainMenuBar()
	{
		return ImGui::BeginMainMenuBar();
	}

	bool StartMenuHeader(const char *_pLabel)
	{
		return ImGui::BeginMenu(_pLabel);
	}

	bool StartMenuBody(const char *_pLabel)
	{
		return ImGui::MenuItem(_pLabel);
	}

	void EndMainMenuBar()
	{
		ImGui::EndMainMenuBar();
	}

	void EndMenuHeader()
	{
		ImGui::EndMenu();
	}

	namespace Display
	{
		void Label(const char* _formatLabel, ...)
		{
			va_list args;
			va_start(args, _formatLabel);
			ImGui::TextV(_formatLabel, args);
			va_end(args);
		}
	
		void TextField(const char* _label, char* _outputbuffer, size_t _size)
		{
			Label(_label);
			ImGui::SameLine();
			ImGui::InputText("", _outputbuffer, _size, ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_AutoSelectAll);
		}
	
		bool CheckBox(const char* _label, bool* _outputBool)
		{
			Label(_label);
			ImGui::SameLine();
			return  (ImGui::Checkbox("", _outputBool));
		}
	
		bool DragFloat(const char* _label, float* _outputFloat, float _dragSpeed, float _min, float _max)
		{
			Label(_label);
			ImGui::SameLine();
			return (ImGui::DragFloat("", _outputFloat, _dragSpeed, _min, _max));
		}
	
		bool DragInt(const char* _label, int* _outputInt, float _dragSpeed, int _min, int _max)
		{
			Label(_label);
			ImGui::SameLine();
			return (ImGui::DragInt("", _outputInt, _dragSpeed, _min, _max));
		}
	
		bool VectorFields(const char* _label, Math::Vector4 *_outputVec, float _dragSpeed, float _min, float _max)
		{
			const float width = 50;
	
			float x, y, z;
			x = _outputVec->x;
			y = _outputVec->y;
			z = _outputVec->z;
	
			ImGui::PushItemWidth(width);
			Label(_label);
			ImGui::SameLine();
			Label("X:");
			ImGui::SameLine();
			if (ImGui::DragFloat("", &x, _dragSpeed, _min, _max)) _outputVec->x = x;
			ImGui::SameLine();
			Label("Y:");
			ImGui::SameLine();
			if (ImGui::DragFloat("", &y, _dragSpeed, _min, _max)) _outputVec->y = y;
			ImGui::SameLine();
			Label("Z:");
			ImGui::SameLine();
			if (ImGui::DragFloat("", &z, _dragSpeed, _min, _max)) _outputVec->z = z;
			ImGui::PopItemWidth();
	
			return false;
		}
	
		bool CollapseHeader(const char* _header)
		{
			return ImGui::CollapsingHeader(_header, ImGuiTreeNodeFlags_DefaultOpen);
		}

		bool SelectableTxt(const char* _label, bool _outputBool)
		{
			return (ImGui::Selectable(_label, _outputBool));
		}

		bool SelectableTxt(const char* _label, bool* _outputBool)
		{
			return (ImGui::Selectable(_label, _outputBool));
		}

		bool SelectableTxtDouble(const char* _label, bool _outputBool)
		{
			return (ImGui::Selectable(_label, _outputBool, ImGuiSelectableFlags_AllowDoubleClick)) && ImGui::IsMouseDoubleClicked(0);
		}

		bool SelectableTxtDouble(const char* _label, bool* _outputBool)
		{
			if ((ImGui::Selectable(_label, _outputBool, ImGuiSelectableFlags_AllowDoubleClick)) && ImGui::IsMouseDoubleClicked(0))
				return true;
			else
				*_outputBool = false;
			return false;
		}
	}
}	// NAMESPACE DYSTOPIA::EGUI::DISPLAY
#endif // EDITOR

