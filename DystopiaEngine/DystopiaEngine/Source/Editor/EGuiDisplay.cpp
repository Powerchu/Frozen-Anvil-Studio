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
constexpr float Default_VectorField_Alignment_Height = 3.0f;
constexpr float Default_Alightnment_Labels_And_Items = 150.f;
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

	void Indent(float _spacing)
	{
		ImGui::Indent(_spacing);
	}

	void UnIndent(float _spacing)
	{
		ImGui::Unindent(_spacing);
	}

	bool StartChild(const std::string& _label, const Math::Vec2& _size, bool _showBorder, const Math::Vec4& _colour)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 5, 5 });
		ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 1);
		ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4{ _colour.x, _colour.y, _colour.z, _colour.w });
		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4{ 0, 0, 0, 0 });
		return ImGui::BeginChild(_label.c_str(), ImVec2{ _size.x, _size.y }, _showBorder);
	}

	bool StartChild(const std::string& _label, const float& _x, const float& _y, bool _showBorder, const Math::Vec4& _colour)
	{
		return StartChild(_label, Math::Vec2{ _x, _y }, _showBorder, _colour);
	}

	void EndChild()
	{
		ImGui::EndChild();
		ImGui::PopStyleColor(2);
		ImGui::PopStyleVar(2);
	}

	void SameLine(float _customOffset)
	{
		ImGui::SameLine(_customOffset);
	}

	void PushID(int id)
	{
		ImGui::PushID(id);
	}

	void PopID()
	{
		ImGui::PopID();
	}

	namespace Display
	{
		void HorizontalSeparator()
		{
			ImGui::Separator();
		}

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
			SameLine(Default_Alightnment_Labels_And_Items);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - Default_VectorField_Alignment_Height);
			ImGui::InputText(("###TextField" + _label).c_str(), _outputbuffer, _size, flags);
		}

		bool EmptyBox(const std::string& _label, float _width, const std::string& _anythingToShowInside)
		{
			bool clicked = false;
			ImVec4 greyColor = ImGui::GetStyleColorVec4(ImGuiCol_FrameBg);
			ImVec4 btnHoveredColor = ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered);
			ImVec4 btnActiveColor = ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive);
			bool show = _anythingToShowInside.length() ? true : false;

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + Default_VectorField_Alignment_Height);
			Label(_label.c_str());
			SameLine(Default_Alightnment_Labels_And_Items);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - Default_VectorField_Alignment_Height);
			
			ImGui::PushItemWidth(_width);
			ImGui::PushStyleColor(ImGuiCol_Button, greyColor);
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, show ? btnHoveredColor : greyColor);
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, show ? btnActiveColor : greyColor);
			if (ImGui::Button(show ? (_anythingToShowInside + "###" + _label).c_str() : ("###EmptyBoxBtn" + _label).c_str(), 
				ImVec2{ _width, (ImGui::GetStyle().FramePadding.y * 2.f) + GImGui->FontSize }))
			{
				if (show) clicked = true;
			}
			ImGui::PopStyleColor(3);
			ImGui::PopItemWidth();
			return clicked;
		}
	
		bool CheckBox(const std::string& _label, bool* _outputBool)
		{
			bool changed = false;
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + Default_VectorField_Alignment_Height);
			Label(_label.c_str());
			SameLine(Default_Alightnment_Labels_And_Items);
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
			SameLine(Default_Alightnment_Labels_And_Items);
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
			SameLine(Default_Alightnment_Labels_And_Items);
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
			SameLine(Default_Alightnment_Labels_And_Items);
			Label("X:");
			SameLine();
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
			SameLine();
			Label("Y:");
			SameLine();
			if (ImGui::DragFloat(field2.c_str(), &y, _dragSpeed, _min, _max))
			{
				if (gContextComdHND && !gContextComdHND->IsRecording() && ImGui::IsMouseDown(0))
				{
					gContextComdHND->StartRecording(&_outputVec->y);
				}
				_outputVec->y = y;
				changed = true;
			}
			SameLine();
			Label("Z:");
			SameLine();
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

		bool StartTreeNode(const std::string&_label, bool* _outClicked)
		{
			return ImGui::TreeNode(_label.c_str(), _outClicked);
		}

		void OpenTreeNode(const std::string&_label, bool _open)
		{
			ImGuiStorage* pStorage = ImGui::GetStateStorage();
			ImGuiID key = ImGui::GetID(_label.c_str());
			if (!_open && pStorage->GetInt(key))
				pStorage->SetInt(key, 0);
			else if (_open && !pStorage->GetInt(key))
				pStorage->SetInt(key, 1);
		}

		void EndTreeNode()
		{
			ImGui::TreePop();
		}

		bool StartPayload(ePayloadTags _tagLoad, void* _pData, size_t _dataSize, const std::string& _toolTip)
		{
			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
			{
				ImGui::SetDragDropPayload(EGUI::ToString(_tagLoad), _pData, _dataSize);
				ImGui::Text("%s", _toolTip.c_str());
				return true;
			}
			return false;
		}

		void EndPayload()
		{
			ImGui::EndDragDropSource();
		}

		void EndPayloadReceiver()
		{
			ImGui::EndDragDropTarget();
		}

		bool DropDownSelection(const std::string& _label, int& _currentIndex, AutoArray<std::string>& _arrOfItems)
		{
			AutoArray<const char*> arrCharPtr;
			for (const auto &e : _arrOfItems)
				arrCharPtr.push_back(e.c_str());

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + Default_VectorField_Alignment_Height);
			Label(_label.c_str());
			SameLine(Default_Alightnment_Labels_And_Items);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - Default_VectorField_Alignment_Height);
			return ImGui::Combo(("##DropDownList" + _label).c_str(), &_currentIndex, arrCharPtr.begin(), arrCharPtr.size());
		}
		
		bool Button(const std::string& _label, const Math::Vec2& _size)
		{
			return ImGui::Button(_label.c_str(), ImVec2{ _size.x, _size.y });
		}

	}
}	// NAMESPACE DYSTOPIA::EGUI::DISPLAY
#endif // EDITOR

