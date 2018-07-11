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
#include "Editor\Commands.h"
#include "Editor\CommandList.h"
#include "../../Dependancies/ImGui/imgui.h"
#include "../../Dependancies/ImGui/imgui_internal.h"
#include <stdlib.h>
#include <iostream>

constexpr float Default_VectorField_Alignment_Left = 100.f;
constexpr float Default_VectorField_Alignment_Height = 2.f;

Dystopia::CommandHandler *gContextComdHND = nullptr;

namespace EGUI
{
	void SetContext(Dystopia::CommandHandler *_pContext)
	{
		gContextComdHND = _pContext;
	}

	void RemoveContext()
	{
		gContextComdHND = nullptr;
	}

	bool StartTab(const std::string& _label, bool *_pOpen, ImGuiWindowFlags _flags)
	{
		return EGUI::Docking::BeginTabs(_label.c_str(), _pOpen, _flags);
	}

	void EndTab()
	{
		EGUI::Docking::EndTabs();
	}

	bool StartMainMenuBar()
	{
		return ImGui::BeginMainMenuBar();
	}

	bool StartMenuHeader(const std::string& _label)
	{
		return ImGui::BeginMenu(_label.c_str());
	}

	bool StartMenuBody(const std::string& _label, const std::string& _shortcut)
	{
		return ImGui::MenuItem(_label.c_str(), _shortcut.c_str());
	}

	void EndMainMenuBar()
	{
		ImGui::EndMainMenuBar();
	}

	void EndMenuHeader()
	{
		ImGui::EndMenu();
	}

	bool StartChild(const std::string& _label, const Math::Vec4& _size)
	{
		return ImGui::BeginChild(_label.c_str(), ImVec2{ _size.x, _size.y }, false);
	}

	bool StartChild(const std::string& _label, const float& _x, const float& _y)
	{
		return ImGui::BeginChild(_label.c_str(), ImVec2{ _x, _y }, false);
	}

	void EndChild()
	{
		ImGui::EndChild();
	}

	void SameLine()
	{
		ImGui::SameLine();
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
	
		void TextField(const std::string& _label, char* _outputbuffer, size_t _size)
		{
			ImGuiInputTextFlags flags = ImGuiInputTextFlags_CharsHexadecimal |
										ImGuiInputTextFlags_AutoSelectAll |
										ImGuiInputTextFlags_EnterReturnsTrue;

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + Default_VectorField_Alignment_Height);
			Label(_label.c_str());
			ImGui::SameLine();
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - Default_VectorField_Alignment_Height);
			if (ImGui::InputText(("###TextField" + _label).c_str(), _outputbuffer, _size, flags))
			{
			}
		}
	
		bool CheckBox(const std::string& _label, bool* _outputBool)
		{
			bool changed = false;
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + Default_VectorField_Alignment_Height);
			Label(_label.c_str());
			ImGui::SameLine();
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - Default_VectorField_Alignment_Height);
			if (ImGui::Checkbox(("###CheckBox" + _label).c_str(), _outputBool))
			{
				if (gContextComdHND && !gContextComdHND->IsRecording() && ImGui::IsMouseDown(0))
				{
					gContextComdHND->StartRecording(_outputBool);
				}
				changed = true;
			}
			if (gContextComdHND && gContextComdHND->IsRecording() && !ImGui::IsMouseDown(0))
			{
				gContextComdHND->EndRecording();
			}
			return changed;
		}
	
		bool DragFloat(const std::string& _label, float* _outputFloat, float _dragSpeed, float _min, float _max)
		{
			bool changed = false;
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + Default_VectorField_Alignment_Height);
			Label(_label.c_str());
			ImGui::SameLine();
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - Default_VectorField_Alignment_Height);
			if (ImGui::DragFloat(("###DragFloat" + _label).c_str(), _outputFloat, _dragSpeed, _min, _max))
			{
				if (gContextComdHND && !gContextComdHND->IsRecording() && ImGui::IsMouseDown(0))
				{
					gContextComdHND->StartRecording(_outputFloat);
				}
				changed = true;
			}
			if (gContextComdHND && gContextComdHND->IsRecording() && !ImGui::IsMouseDown(0))
			{
				gContextComdHND->EndRecording();
			}
			return changed;
		}
	
		bool DragInt(const std::string& _label, int* _outputInt, float _dragSpeed, int _min, int _max)
		{
			bool changed = false;
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + Default_VectorField_Alignment_Height);
			Label(_label.c_str());
			ImGui::SameLine();
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - Default_VectorField_Alignment_Height);
			if (ImGui::DragInt(("###DragInt" + _label).c_str(), _outputInt, _dragSpeed, _min, _max))
			{
				if (gContextComdHND && !gContextComdHND->IsRecording() && ImGui::IsMouseDown(0))
				{
					gContextComdHND->StartRecording(_outputInt);
				}
				changed = true;
			}
			if (gContextComdHND && gContextComdHND->IsRecording() && !ImGui::IsMouseDown(0))
			{
				gContextComdHND->EndRecording();
			}
			return changed;
		}
	
		bool VectorFields(const std::string& _label, Math::Vector4 *_outputVec, float _dragSpeed, float _min, float _max, float _width)
		{
			bool changed = false;
			std::string field1 = "##VecFieldX", field2 = "##VecFieldY", field3 = "##VecFieldZ";
			float x, y, z;
			x = _outputVec->x;
			y = _outputVec->y;
			z = _outputVec->z;
			field1 += _label;
			field2 += _label;
			field3 += _label;

			ImGui::PushItemWidth(_width);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + Default_VectorField_Alignment_Height);
			Label(_label.c_str());
			ImGui::SameLine(Default_VectorField_Alignment_Left);
			Label("X:");
			ImGui::SameLine();
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - Default_VectorField_Alignment_Height);
			if (ImGui::DragFloat(field1.c_str(), &x, _dragSpeed, _min, _max))
			{
				if (gContextComdHND && !gContextComdHND->IsRecording() && ImGui::IsMouseDown(0))
				{
					gContextComdHND->StartRecording(&_outputVec->x);
				}
				_outputVec->x = x;
				changed = true;
			}
			ImGui::SameLine();
			Label("Y:");
			ImGui::SameLine();
			if (ImGui::DragFloat(field2.c_str(), &y, _dragSpeed, _min, _max))
			{
				if (gContextComdHND && !gContextComdHND->IsRecording() && ImGui::IsMouseDown(0))
				{
					gContextComdHND->StartRecording(&_outputVec->y);
				}
				_outputVec->y = y;
				changed = true;
			}
			ImGui::SameLine();
			Label("Z:");
			ImGui::SameLine();
			if (ImGui::DragFloat(field3.c_str(), &z, _dragSpeed, _min, _max))
			{
				if (gContextComdHND && !gContextComdHND->IsRecording() && ImGui::IsMouseDown(0))
				{
					gContextComdHND->StartRecording(&_outputVec->z);
				}
				changed = true;
				_outputVec->z = z;
			}
			ImGui::PopItemWidth();
			if (gContextComdHND && gContextComdHND->IsRecording() && !ImGui::IsMouseDown(0))
			{
				gContextComdHND->EndRecording();
			}

			return changed;
		}
	
		bool CollapsingHeader(const std::string& _label)
		{
			return ImGui::CollapsingHeader(_label.c_str(), ImGuiTreeNodeFlags_DefaultOpen);
		}

		bool SelectableTxt(const std::string& _label, bool _outputBool)
		{
			return (ImGui::Selectable(_label.c_str(), _outputBool));
		}

		bool SelectableTxt(const std::string& _label, bool* _outputBool)
		{
			return (ImGui::Selectable(_label.c_str(), _outputBool));
		}

		bool SelectableTxtDouble(const std::string& _label, bool _outputBool)
		{
			return (ImGui::Selectable(_label.c_str(), _outputBool, ImGuiSelectableFlags_AllowDoubleClick)) && ImGui::IsMouseDoubleClicked(0);
		}

		bool SelectableTxtDouble(const std::string& _label, bool* _outputBool)
		{
			if ((ImGui::Selectable(_label.c_str(), _outputBool, ImGuiSelectableFlags_AllowDoubleClick)) && ImGui::IsMouseDoubleClicked(0))
				return true;
			else
				*_outputBool = false;
			return false;
		}

		bool StartTreeNode(const std::string&_label)
		{
			return ImGui::TreeNode(_label.c_str());
		}

		void OpenTreeNode(const std::string&_label, bool _collapseMe)
		{
			ImGui::GetStateStorage()->SetInt(ImGui::GetID(_label.c_str()), static_cast<int>(_collapseMe));
		}

		void EndTreeNode()
		{
			ImGui::TreePop();
		}

	}
}	// NAMESPACE DYSTOPIA::EGUI::DISPLAY
#endif // EDITOR

