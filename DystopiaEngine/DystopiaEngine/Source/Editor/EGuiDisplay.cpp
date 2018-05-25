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
#include <iostream>

namespace Dystopia{
namespace EGUI{
namespace Display{


void MainMenuBar()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			std::cout << "1\n";
			if (ImGui::MenuItem("New"))
			{
				// TODO: Some actual function
			}
			if (ImGui::MenuItem("Open", "Ctrl+O"))
			{
				// TODO: Some actual function
			}
			if (ImGui::BeginMenu("Open Recent"))
			{
				if (ImGui::MenuItem("some_recent_crap.cpp"))
				{
					// TODO: Some actual function
				}
				if (ImGui::MenuItem("more_recent_crap.h"))
				{
					// TODO: Some actual function
				}
				if (ImGui::BeginMenu("More.."))
				{
					ImGui::MenuItem("surprise_theres_more_crap.h");
					ImGui::EndMenu();
				}
				ImGui::EndMenu();
			}
			if (ImGui::MenuItem("Save", "Ctrl+S"))
			{
				// TODO: Some actual function
			}
			if (ImGui::MenuItem("Save As.."))
			{
				// TODO: Some actual function
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Quit", "Alt+F4"))
			{
				// TODO: Some actual function
			}
			ImGui::EndMenu();
		}
		// TODO: Some actual function for all the bottom
		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
			if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
			ImGui::Separator();
			if (ImGui::MenuItem("Cut", "CTRL+X")) {}
			if (ImGui::MenuItem("Copy", "CTRL+C")) {}
			if (ImGui::MenuItem("Paste", "CTRL+V")) {}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}

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
	ImGui::Checkbox("", _outputBool);
}

bool DragFloat(const char* _label, float* _outputFloat, float _dragSpeed, float _min, float _max)
{
	Label(_label);
	ImGui::SameLine();
	ImGui::DragFloat("", _outputFloat, _dragSpeed, _min, _max);
}

bool DragInt(const char* _label, int* _outputInt, float _dragSpeed, int _min, int _max)
{
	Label(_label);
	ImGui::SameLine();
	ImGui::DragInt("", _outputInt, _dragSpeed, _min, _max);
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
}

void CollapseHeader(const char* _header)
{
	ImGui::CollapsingHeader(_header, ImGuiTreeNodeFlags_DefaultOpen);
}

bool SelectableTxt(const char* _label, bool* _outputBool)
{
	ImGui::Selectable(_label, _outputBool);
}

}}}	// NAMESPACE DYSTOPIA::EGUI::DISPLAY

#endif // EDITOR

