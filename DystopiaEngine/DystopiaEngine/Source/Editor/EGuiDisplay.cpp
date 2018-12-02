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
#include <Windows.h>
#include <consoleapi2.h>
#include <winuser.h>

#include "Editor/EGUI.h"
#include "Editor/EditorCommands.h"
#include "Editor/CommandTypes.h"
#include "Editor/EditorMain.h"
#include "Editor/EditorClipboard.h"
#include "Editor/Payloads.h"

#include "System/Graphics/Texture.h"
#include "System/Graphics/GraphicsSystem.h"

#include "System/Time/ScopedTimer.h"
#include "System/Profiler/Profiler.h"
#include "System/Profiler/ProfilerAction.h"

#include "../../Dependancies/ImGui/imgui_internal.h"

namespace EGUI
{
	Stack<float> g_StackLeftAlign{ 100 };

	static bool IsItemActiveLastFrame()
	{
		ImGuiContext g = *GImGui;
		if (g.ActiveIdPreviousFrame) return g.ActiveIdPreviousFrame == g.CurrentWindow->DC.LastItemId;
		return false;
	}

	Stack<float>& GetLeftAlignStack(void)
	{
		return g_StackLeftAlign;
	}

	void SetContext(DysPeekia::CommandHandler*)
	{
		//gContextComdHND = _pContext;
	}

	DysPeekia::CommandHandler* GetCommandHND()
	{
		//return gContextComdHND;
		return nullptr;
	}

	void RemoveContext()
	{
		//gContextComdHND = nullptr;
	}

	void ChangeLabelSpacing(float _amount)
	{
		DefaultAlighnmentSpacing = _amount;
	}

	void ChangeLabelSpacing()
	{
		DefaultAlighnmentSpacing = 10.f;
	}

	void ChangeAlignmentYOffset(float _amount)
	{
		DefaultAlighnmentOffsetY = _amount;
	}

	void ChangeAlignmentYOffset()
	{
		DefaultAlighnmentOffsetY = 3.f;
	}

	float GetAlignmentYOffset()
	{
		return DefaultAlighnmentOffsetY;
	}

	float GetLabelSpacing()
	{
		return DefaultAlighnmentSpacing;
	}

	bool StartTab(const char* _label, bool *_pOpen, ImGuiWindowFlags _flags)
	{
		return EGUI::Docking::BeginTabs(_label, _pOpen, _flags);
	}

	void EndTab()
	{
		EGUI::Docking::EndTabs();
	}

	bool StartMainMenuBar()
	{
		return ImGui::BeginMainMenuBar();
	}

	bool StartMenuHeader(const char * _label, bool _enabled)
	{
		return ImGui::BeginMenu(_label, _enabled);
	}

	bool StartMenuBody(const char * _label, const char * _shortcut, bool _enabled)
	{
		return ImGui::MenuItem(_label, _shortcut, false, _enabled);
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

	bool StartChild(const char* _label, const Math::Vec2& _size, bool _showBorder)
	{
		return ImGui::BeginChild(_label, ImVec2{ _size.x, _size.y }, _showBorder);
	}

	bool StartChild(const char* _label, const float& _x, const float& _y, bool _showBorder)
	{
		return StartChild(_label, Math::Vec2{ _x, _y }, _showBorder);
	}

	void EndChild()
	{
		ImGui::EndChild();
	}

	void SameLine(float _customOffset, float _leftOff)
	{
		ImGui::SameLine(_leftOff, _customOffset);
	}

	void PushID(int id)
	{
		ImGui::PushID(id);
	}

	void PopID()
	{
		ImGui::PopID();
	}

	void PushLeftAlign(float _f)
	{
		g_StackLeftAlign.Push(_f);
	}

	void PopLeftAlign()
	{
		if (!g_StackLeftAlign.IsEmpty())
			g_StackLeftAlign.Pop();
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

		void LabelWrapped(const char *_formatLabel, ...)
		{
			va_list args;
			va_start(args, _formatLabel);
			ImGui::TextWrappedV(_formatLabel, args);
			va_end(args);
		}

		bool TextField(const char * _label, char* _outputbuffer, size_t _size, bool _showLabel, float _width, bool _onlyEnterReturnsTrue)
		{
			ImGuiInputTextFlags flags = ImGuiInputTextFlags_AutoSelectAll;
			if (_onlyEnterReturnsTrue)
				flags |= ImGuiInputTextFlags_EnterReturnsTrue;
			ImGui::PushItemWidth(_width);
			if (_showLabel)
			{
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + DefaultAlighnmentOffsetY);
				Label(_label);
				SameLine(DefaultAlighnmentSpacing, g_StackLeftAlign.IsEmpty() ? DefaultAlignLeft : g_StackLeftAlign.Peek());
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() - DefaultAlighnmentOffsetY);
			}
			//HashString invi{ "##" };
			//invi += _label;
			bool b = ImGui::InputText(_label, _outputbuffer, _size, flags);
			ImGui::PopItemWidth();
			return b;
		}

		bool TextField(const char * _label, HashString& _out, bool _showLabel, float _width, bool _onlyEnterReturnsTrue)
		{
			ImGuiInputTextFlags flags = ImGuiInputTextFlags_AutoSelectAll;
			if (_onlyEnterReturnsTrue)
				flags |= ImGuiInputTextFlags_EnterReturnsTrue;
			ImGui::PushItemWidth(_width);
			if (_showLabel)
			{
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + DefaultAlighnmentOffsetY);
				Label(_label);
				SameLine(DefaultAlighnmentSpacing, g_StackLeftAlign.IsEmpty() ? DefaultAlignLeft : g_StackLeftAlign.Peek());
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() - DefaultAlighnmentOffsetY);
			}
			//HashString inviLabel{ "##" };
			//inviLabel += _label;
			bool b = ImGui::InputText(_label, _out.access_c(), _out.size(), flags);
			ImGui::PopItemWidth();
			return b;
		}

		// Helper operator for ImVec2 - Splitter
		ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs)
		{
			return ImVec2{ lhs.x + rhs.x, lhs.y + rhs.y };
		}

		bool Splitter(bool split_vertically, float thickness, float* size1, float* size2, float min_size1, float min_size2, float splitter_long_axis_size)
		{
			using namespace ImGui;
			ImGuiContext& g = *GImGui;
			ImGuiWindow* window = g.CurrentWindow;
			const ImGuiID id = window->GetID("##Splitter");
			ImRect bb;
			bb.Min = window->DC.CursorPos + (split_vertically ? ImVec2(*size1, 0.0f) : ImVec2(0.0f, *size1));
			bb.Max = bb.Min + CalcItemSize(split_vertically ? ImVec2(thickness, splitter_long_axis_size) : ImVec2(splitter_long_axis_size, thickness), 0.0f, 0.0f);
			return SplitterBehavior(id, bb, split_vertically ? ImGuiAxis_X : ImGuiAxis_Y, size1, size2, min_size1, min_size2, 0.0f);
		}

		bool EmptyBox(const char * _label, float _width, const char *, bool _iteractive, bool _showLabel)
		{
			bool clicked = false;
			const bool show = _iteractive;

			if (_showLabel)
			{
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + DefaultAlighnmentOffsetY);
				Label(_label);
				SameLine(DefaultAlighnmentSpacing, g_StackLeftAlign.IsEmpty() ? DefaultAlignLeft : g_StackLeftAlign.Peek());
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() - DefaultAlighnmentOffsetY);
			}

			//HashString inviLabel{ _anythingToShowInside };
			//inviLabel += "##";
			//inviLabel += _label;

			ImGui::PushItemWidth(_width);
			if (ImGui::Button(_label,
				ImVec2{ _width, (ImGui::GetStyle().FramePadding.y * 2.f) + GImGui->FontSize }))
			{
				if (show) clicked = true;
			}
			ImGui::PopItemWidth();
			return clicked;
		}

		bool CheckBox(const char * _label, bool* _outputBool, bool _showLabel)
		{
			if (_showLabel)
			{
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + DefaultAlighnmentOffsetY);
				Label(_label);
				SameLine(DefaultAlighnmentSpacing, g_StackLeftAlign.IsEmpty() ? DefaultAlignLeft : g_StackLeftAlign.Peek());
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() - DefaultAlighnmentOffsetY);
			}
		    //HashString invi{ "##checkBox" };
			//invi += _label;
			return ImGui::Checkbox(_label, _outputBool);
		}

		bool RadioBtn(const char * _label, int* _pValueStorage, int _btnValue, bool _showLabel)
		{
			if (_showLabel)
			{
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + DefaultAlighnmentOffsetY);
				Label(_label);
				SameLine(DefaultAlighnmentSpacing, g_StackLeftAlign.IsEmpty() ? DefaultAlignLeft : g_StackLeftAlign.Peek());
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() - DefaultAlighnmentOffsetY);
			}
			//HashString invi{ "##checkBox" };
			//invi += _label;
			return ImGui::RadioButton(_label, _pValueStorage, _btnValue);
		}

		eDragStatus DragFloat(const char * _label, float* _outputFloat, float _dragSpeed, float _min, float _max, bool _hideText, float _width)
		{
			static POINT p;
			static const int xPos = (GetSystemMetrics(SM_CXSCREEN)) / 2;
			static const int yPos = (GetSystemMetrics(SM_CYSCREEN)) / 2;

			if (!_hideText)
			{
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + DefaultAlighnmentOffsetY);
				Label(_label);
				SameLine(DefaultAlighnmentSpacing, g_StackLeftAlign.IsEmpty() ? DefaultAlignLeft : g_StackLeftAlign.Peek());
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() - DefaultAlighnmentOffsetY);
			}
			bool changing = false;
			ImGui::PushItemWidth(_width);
			//HashString invi{ "##DragF" };
			//invi += _label;
			changing = ImGui::DragFloat(_label, _outputFloat, _dragSpeed, _min, _max, "%.3f");
			ImGui::PopItemWidth();

			if (!IsItemActiveLastFrame() && ImGui::IsItemActive())
				return eSTART_DRAG;
			if (changing)
				return eDRAGGING;
			if (ImGui::IsItemDeactivatedAfterChange())
				return (ImGui::IsMouseReleased(0)) ? eEND_DRAG : eENTER;
			if (ImGui::IsItemDeactivated())
				return eDEACTIVATED;
			return eNO_CHANGE;
		}

		eDragStatus SliderFloat(const char * _label, float *_pOutFloat, float _min, float _max, bool _hideText, float _width)
		{
			if (!_hideText)
			{
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + DefaultAlighnmentOffsetY);
				Label(_label);
				SameLine(DefaultAlighnmentSpacing, g_StackLeftAlign.IsEmpty() ? DefaultAlignLeft : g_StackLeftAlign.Peek());
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() - DefaultAlighnmentOffsetY);
			}
			bool changing = false;
			ImGui::PushItemWidth(_width);
			//HashString invi{ "##SliderF" };
			//invi += _label;
			changing = ImGui::SliderFloat(_label, _pOutFloat, _min, _max, "%.3f");
			ImGui::PopItemWidth();

			if (!IsItemActiveLastFrame() && ImGui::IsItemActive())
				return changing ? eINSTANT_CHANGE : eSTART_DRAG;
			else if (ImGui::IsItemDeactivated())
				return ImGui::IsMouseReleased(0) ? eEND_DRAG : eDEACTIVATED;
			else if (ImGui::IsItemActive())
				return changing ? eDRAGGING : eNO_CHANGE;
			return eNO_CHANGE;
		}

		eDragStatus SliderInt(const char * _label, int *_pOutInt, int _min, int _max, bool _hideText, float _width)
		{
			if (!_hideText)
			{
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + DefaultAlighnmentOffsetY);
				Label(_label);
				SameLine(DefaultAlighnmentSpacing, g_StackLeftAlign.IsEmpty() ? DefaultAlignLeft : g_StackLeftAlign.Peek());
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() - DefaultAlighnmentOffsetY);
			}
			bool changing = false;
			ImGui::PushItemWidth(_width);
			//HashString inviLabel{ "##" };
			//inviLabel += _label;
			changing = ImGui::SliderInt(_label, _pOutInt, _min, _max);
			ImGui::PopItemWidth();

			if (!IsItemActiveLastFrame() && ImGui::IsItemActive())
				return changing ? eINSTANT_CHANGE : eSTART_DRAG;
			else if (ImGui::IsItemDeactivated())
				return ImGui::IsMouseReleased(0) ? eEND_DRAG : eDEACTIVATED;
			else if (ImGui::IsItemActive())
				return changing ? eDRAGGING : eNO_CHANGE;
			return eNO_CHANGE;
		}

		eDragStatus DragInt(const char * _label, int* _outputInt, float _dragSpeed, int _min, int _max, bool _hideText, float _width)
		{
			if (!_hideText)
			{
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + DefaultAlighnmentOffsetY);
				Label(_label);
				SameLine(DefaultAlighnmentSpacing, g_StackLeftAlign.IsEmpty() ? DefaultAlignLeft : g_StackLeftAlign.Peek());
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() - DefaultAlighnmentOffsetY);
			}
			bool changing = false;
			ImGui::PushItemWidth(_width);
			//HashString invi{ "##DragI" };
			//invi += _label;
			changing = ImGui::DragInt(_label, _outputInt, _dragSpeed, _min, _max);
			ImGui::PopItemWidth();

			if (!IsItemActiveLastFrame() && ImGui::IsItemActive()) return eSTART_DRAG;
			if (changing) return eDRAGGING;
			if (ImGui::IsItemDeactivatedAfterChange())
				return (ImGui::IsMouseReleased(0)) ? eEND_DRAG : eENTER;
			if (ImGui::IsItemDeactivated()) return eDEACTIVATED;
			return eNO_CHANGE;
		}

		Array<eDragStatus, 3> VectorFields(const char * _label, Math::Vector4 *_outputVec, float _dragSpeed, float _min, float _max, float _width)
		{
			std::string field1 = "##VecX", field2 = "##VecY", field3 = "##VecZ";
			float x, y, z;
			x = _outputVec->x;
			y = _outputVec->y;
			z = _outputVec->z;
			field1 += _label;
			field2 += _label;
			field3 += _label;

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + DefaultAlighnmentOffsetY);
			Label(_label);
			SameLine(DefaultAlighnmentSpacing, g_StackLeftAlign.IsEmpty() ? DefaultAlignLeft : g_StackLeftAlign.Peek());

			Label("X:"); SameLine();
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - DefaultAlighnmentOffsetY);
			eDragStatus statX = EGUI::Display::DragFloat(field1.c_str(), &x, _dragSpeed, _min, _max, true, _width);
			if (statX != eDragStatus::eNO_CHANGE) _outputVec->x = x;

			SameLine(); Label("Y:"); SameLine();
			eDragStatus statY = EGUI::Display::DragFloat(field2.c_str(), &y, _dragSpeed, _min, _max, true, _width);
			if (statY != eDragStatus::eNO_CHANGE) _outputVec->y = y;

			SameLine(); Label("Z:"); SameLine();
			eDragStatus statZ = EGUI::Display::DragFloat(field3.c_str(), &z, _dragSpeed, _min, _max, true, _width);
			if (statZ != eDragStatus::eNO_CHANGE) _outputVec->z = z;


			return Array<eDragStatus, 3>{statX, statY, statZ};
		}

		Array<eDragStatus, 2> VectorFields(const char * _label, Math::Vector2 *_outputVec, float _dragSpeed, float _min, float _max, float _width)
		{
			std::string field1 = "##VecX", field2 = "##VecY";
			float x, y;
			x = _outputVec->x;
			y = _outputVec->y;
			field1 += _label;
			field2 += _label;

			ImGui::PushItemWidth(_width);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + DefaultAlighnmentOffsetY);
			Label(_label);
			SameLine(DefaultAlighnmentSpacing, g_StackLeftAlign.IsEmpty() ? DefaultAlignLeft : g_StackLeftAlign.Peek());

			Label("X:"); SameLine();
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - DefaultAlighnmentOffsetY);
			eDragStatus statX = EGUI::Display::DragFloat(field1.c_str(), &x, _dragSpeed, _min, _max, true, _width);
			if (statX != eDragStatus::eNO_CHANGE) _outputVec->x = x;

			SameLine(); Label("Y:"); SameLine();
			eDragStatus statY = EGUI::Display::DragFloat(field2.c_str(), &y, _dragSpeed, _min, _max, true, _width);
			if (statY != eDragStatus::eNO_CHANGE) _outputVec->y = y;

			ImGui::PopItemWidth();

			return Array<eDragStatus, 2>{statX, statY};
		}

		Array<eDragStatus, 2> VectorFieldsInt(const char *_label, Math::Vector2 *_outputVec, int _dragSpeed, int _min, int _max, float _width)
		{
			std::string field1 = "##VecFieldX", field2 = "##VecFieldY";
			int x, y;
			x = static_cast<int>(_outputVec->x);
			y = static_cast<int>(_outputVec->y);
			field1 += _label;
			field2 += _label;

			ImGui::PushItemWidth(_width);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + DefaultAlighnmentOffsetY);
			Label(_label);
			SameLine(DefaultAlighnmentSpacing, g_StackLeftAlign.IsEmpty() ? DefaultAlignLeft : g_StackLeftAlign.Peek());

			Label("X:"); SameLine();
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - DefaultAlighnmentOffsetY);
			eDragStatus statX = EGUI::Display::DragInt(field1.c_str(), &x, static_cast<float>(_dragSpeed), _min, _max, true, _width);
			if (statX != eDragStatus::eNO_CHANGE) _outputVec->x = static_cast<float>(x);

			SameLine(); Label("Y:"); SameLine();
			eDragStatus statY = EGUI::Display::DragInt(field2.c_str(), &y, static_cast<float>(_dragSpeed), _min, _max, true, _width);
			if (statY != eDragStatus::eNO_CHANGE) _outputVec->y = static_cast<float>(y);

			ImGui::PopItemWidth();

			return Array<eDragStatus, 2>{statX, statY};
		}

		bool CollapsingHeader(const char * _label, bool defaultOpen)
		{
			if (defaultOpen)
				return ImGui::CollapsingHeader(_label, ImGuiTreeNodeFlags_DefaultOpen);
			//else
			return ImGui::CollapsingHeader(_label, ImGuiTreeNodeFlags_Bullet);
		}

		bool SelectableTxt(const char* _label, bool _highlight)
		{
			if (_highlight)
			{
				ImGui::PushStyleColor(ImGuiCol_Header, ImGui::GetColorU32(ImGuiCol_HeaderActive));
				ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImGui::GetColorU32(ImGuiCol_HeaderActive));
			}
			bool ret = ImGui::Selectable(_label, _highlight);
			if (_highlight) ImGui::PopStyleColor(2);
			return ret;
		}

		bool SelectableTxt(const char*  _label, bool* _outputBool)
		{
			if (*_outputBool)
			{
				ImGui::PushStyleColor(ImGuiCol_Header, ImGui::GetColorU32(ImGuiCol_HeaderActive));
				ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImGui::GetColorU32(ImGuiCol_HeaderActive));
			}
			bool ret = (ImGui::Selectable(_label, _outputBool));
			if (*_outputBool) ImGui::PopStyleColor(2);
			return ret;
		}

		bool SelectableTxtDouble(const char*  _label, bool _highlight)
		{
			if (_highlight)
			{
				ImGui::PushStyleColor(ImGuiCol_Header, ImGui::GetColorU32(ImGuiCol_HeaderActive));
				ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImGui::GetColorU32(ImGuiCol_HeaderActive));
			}
			bool ret = (ImGui::Selectable(_label, _highlight, ImGuiSelectableFlags_AllowDoubleClick)) && ImGui::IsMouseDoubleClicked(0);
			if (_highlight) ImGui::PopStyleColor(2);

			return ret;
		}

		bool SelectableTxtDouble(const char*  _label, bool* _outputBool)
		{
			if (*_outputBool)
			{
				ImGui::PushStyleColor(ImGuiCol_Header, ImGui::GetColorU32(ImGuiCol_HeaderActive));
				ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImGui::GetColorU32(ImGuiCol_HeaderActive));
			}
			bool ret = (ImGui::Selectable(_label, _outputBool, ImGuiSelectableFlags_AllowDoubleClick)) && ImGui::IsMouseDoubleClicked(0);
			if (*_outputBool) ImGui::PopStyleColor(2);
			*_outputBool = ret;
			return ret;
		}

		bool StartTreeNode(const char* _label, bool* _outClicked, bool _highlighted, bool _noArrow, bool _defaulPeeken, bool _singleClickOpen)
		{
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_AllowItemOverlap;
			flags = _highlighted ? flags | ImGuiTreeNodeFlags_Selected : flags;
			flags = _noArrow ? flags | ImGuiTreeNodeFlags_Leaf : flags;
			flags = _singleClickOpen ? flags : flags | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow;

			if (_highlighted)
			{
				ImGui::PushStyleColor(ImGuiCol_Header, ImGui::GetColorU32(ImGuiCol_HeaderActive));
				ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImGui::GetColorU32(ImGuiCol_HeaderActive));
			}

			if (_defaulPeeken)
				flags |= ImGuiTreeNodeFlags_DefaultOpen;

			const bool ret = ImGui::TreeNode(_label, _outClicked, flags);

			if (_highlighted)
				ImGui::PopStyleColor(2);

			return ret;
		}

		void OpenTreeNode(const char* _label, bool _open)
		{
			ImGuiStorage* pStorage = ImGui::GetStateStorage();
			ImGuiID key = ImGui::GetID(_label);
			if (!_open && pStorage->GetInt(key))
				pStorage->SetInt(key, 0);
			else if (_open && !pStorage->GetInt(key))
				pStorage->SetInt(key, 1);
		}

		void OpenTreeNode(bool _open)
		{
			ImGui::SetNextTreeNodeOpen(_open, ImGuiCond_FirstUseEver);
		}

		void EndTreeNode()
		{
			ImGui::TreePop();
		}

		bool StartPayload(ePayloadTags _tagLoad, void* _pData, size_t _dataSize, const char* _toolTip)
		{
			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
			{
				std::string show = _toolTip;
				if (_tagLoad == ePayloadTags::GAME_OBJ)
				{
					bool exist = false;
					auto& selection = ::Editor::EditorMain::GetInstance()->GetSystem<::Editor::EditorClipboard>()->GetSelectedIDs();
					uint64_t id = *static_cast<uint64_t*>(_pData);
					for (const auto& i : selection)
					{
						if (id == i)
						{
							exist = true;
							break;
						}
					}
					if (!exist)
						::Editor::EditorMain::GetInstance()->GetSystem<::Editor::EditorClipboard>()->AddGameObject(id);

					if (selection.size() > 1)
						show = "multiple ...";
				}

				ImGui::SetDragDropPayload(EGUI::GetPayloadString(_tagLoad), _pData, _dataSize);
				ImGui::Text("%s", show.c_str());
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

		bool PrefabPayload(const char * _uniqueId, const char * _label, const char * _tooltip,
			const Math::Vec2& _displaySize, ePayloadTags _tagLoad, void* _pData, size_t _dataSize)
		{
			auto originalPos = ImGui::GetCursorPos();
			ImVec2 size{ _displaySize.x * 0.9f, _displaySize.y * 0.75f };

			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0,0,0,0 });
			
			//HashString invi{ "##CPLBtnP" };
			//invi += _uniqueId;
			
			bool btn = ImGui::Button(_uniqueId, _displaySize, true);
			bool payload = StartPayload(_tagLoad, _pData, _dataSize, _tooltip);
			ImGui::PopStyleColor();
			ImGui::PopStyleVar();
			if (payload) EndPayload();

			ImGui::SetCursorPos(ImVec2{ originalPos.x + _displaySize.x * 0.05f, originalPos.y + size.y*0.1f});
			IconGameObj(_uniqueId, size.x, size.y);
			ImGui::SetCursorPos(ImVec2{ originalPos.x + 1, originalPos.y + size.y});
			ImGui::TextWrapped(_label);
			return btn;
		}

		bool ImagePayload(const char * _uniqueId, const char * _label, const char * _tooltip,
			const Math::Vec2& _displaySize, ePayloadTags _tagLoad, void* _pData, size_t _dataSize)
		{
			auto originalPos = ImGui::GetCursorPos();

			//Dystopia::ScopedTimer<Dystopia::ProfilerAction> scope{ "FULL ", "0" };

			auto pFile = static_cast<::Editor::File*>(_pData);
			if (!pFile->mpImgData)
				pFile->mpImgData = static_cast<void*>(Dystopia::EngineCore::Get<Dystopia::GraphicsSystem>()->LoadTexture(pFile->mPath.c_str()));
			auto img = static_cast<Dystopia::Texture*>(pFile->mpImgData);

			size_t id = static_cast<size_t>(img->GetID());
			float imageMaxHeight = _displaySize.y * 0.75f;
			float imageMaxWidth = _displaySize.x * 0.9f;
			float aspect = imageMaxWidth / imageMaxHeight;
			float ix = static_cast<float>(aspect * img->GetWidth());
			float iy = static_cast<float>(img->GetHeight());
			float sx = imageMaxWidth;
			float sy = imageMaxHeight;
			auto mImgSize = (sx / sy) > (ix / iy) ? Math::Vec2{ ix * (sy / iy), sy } :
				Math::Vec2{ sx, iy * (sx / ix) };
			auto posImage = Math::Vec2{ (sx - mImgSize.x) / 2, (sy - mImgSize.y) / 2 };

			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0,0,0,0 });

			//Dystopia::ScopedTimer<Dystopia::ProfilerAction> scope1{ "FIRST ", "1" };

			//HashString invi{ "##CPLBtnI" };

			

			//Dystopia::ScopedTimer<Dystopia::ProfilerAction> scope3{ "THIRD", "3" };
			bool btn = ImGui::Button(_uniqueId, _displaySize, true);
			bool payload = StartPayload(_tagLoad, _pData, _dataSize, _tooltip);
			ImGui::PopStyleColor();
			ImGui::PopStyleVar();
			if (payload) EndPayload();

			ImGui::SetCursorPos(ImVec2{ originalPos.x + posImage.x + _displaySize.x * 0.05f, originalPos.y + posImage.y });
			ImGui::Image(reinterpret_cast<void*>(id), ImVec2{ mImgSize.x, mImgSize.y });
			ImGui::SetCursorPos(ImVec2{ originalPos.x + 1, originalPos.y + imageMaxHeight });
			ImGui::TextWrapped(_label);
			return btn;
		}

		bool CustomPayload(const char * _uniqueId, const char * _label, const char * _tooltip,
			const Math::Vec2& _displaySize, ePayloadTags _tagLoad, void* _pData, size_t _dataSize)
		{
			switch (_tagLoad)
			{
				//	FILE,
				//	COMPONENT,
				//case SCENE:
				//	return false;
				//case MP3:
				//case WAV:
				//	return false;
			case PREFAB:
				return PrefabPayload(_uniqueId, _label, _tooltip, _displaySize, _tagLoad, _pData, _dataSize);
			case BMP:
			case PNG:
			case DDS:
				return ImagePayload(_uniqueId, _label, _tooltip, _displaySize, _tagLoad, _pData, _dataSize);
			}

			ImVec2 pos = ImGui::GetCursorScreenPos();
			ImVec2 size{ _displaySize.x, _displaySize.y };
			const float iconWidth = size.x / 2;
			const float iconHeight = size.y / 2;
			const float offsetX = iconWidth / 2;
			const float offsetY = iconHeight / 4;
			ImVec2 posIcon{ pos.x + offsetX, pos.y + offsetY };
			ImVec2 posText{ pos.x + 1, pos.y + iconHeight + (2* offsetY) };

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0,0,0,0 });
			//HashString invi{ "##CPLBtn" };
			//invi += _uniqueId;
			bool btn = ImGui::Button(_uniqueId, size, true);
			bool payload = StartPayload(_tagLoad, _pData, _dataSize, _tooltip);
			ImGui::PopStyleColor();
			if (payload) EndPayload();

			ImGui::SetCursorScreenPos(posIcon);
			IconFile(_uniqueId, size.x, size.y);
			ImGui::SetCursorScreenPos(posText);
			ImGui::TextWrapped(_label);
			return btn;
		}

		void OpenPopup(const char* _thePopupID, bool _toOpenAtMousePos)
		{
			ImGui::OpenPopup(_thePopupID);
			if (!_toOpenAtMousePos)
			{
				ImGuiContext& g = *ImGui::GetCurrentContext();
				ImGuiWindow* pGuiWin = g.CurrentWindow;
				ImRect callerRect = pGuiWin->DC.LastItemRect;
				ImGuiPopupRef& popup_ref = g.OpenPopupStack[g.CurrentPopupStack.Size];
				popup_ref.OpenPopupPos = callerRect.GetBL();
			}
		}

		bool StartPopup(const char* _uniqueID)
		{
			return ImGui::BeginPopup(_uniqueID);
		}

		bool StartPopupModal(const char* _uniqueID, const char* _label)
		{
			ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize |
				ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;

			bool ret = ImGui::BeginPopupModal(_uniqueID, NULL, flags);
			if (ret)
			{
				EGUI::Display::Label(_label);
				EGUI::Display::HorizontalSeparator();
			}
			return ret;
		}

		void CloseCurrentPopup()
		{
			ImGui::CloseCurrentPopup();
		}

		void EndPopup()
		{
			ImGui::EndPopup();
		}

		bool DropDownSelection(const char* _label, int& _currentIndex, AutoArray<std::string>& _arrOfItems, float _width)
		{
			AutoArray<const char*> arrCharPtr;
			for (const auto &e : _arrOfItems)
				arrCharPtr.push_back(e.c_str());

			ImGui::PushItemWidth(_width);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + DefaultAlighnmentOffsetY);
			Label(_label);
			SameLine(DefaultAlighnmentSpacing, g_StackLeftAlign.IsEmpty() ? DefaultAlignLeft : g_StackLeftAlign.Peek());
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - DefaultAlighnmentOffsetY);
			std::string invi{ "##DDSel" };
			invi += _label;
			bool ret = ImGui::Combo(invi.c_str(), &_currentIndex, arrCharPtr.begin(), static_cast<int>(arrCharPtr.size()));
			ImGui::PopItemWidth();
			return ret;
		}

		bool Button(const char* _label, const Math::Vec2& _size)
		{
			return ImGui::Button(_label, ImVec2{ _size.x, _size.y });
		}

		void Dummy(float width, float height)
		{
			ImGui::Dummy(ImVec2{ width, height });
		}

		void IconTick(float width, float height, float _thickness, const Math::Vec4& _colour)
		{
			ImVec2 pos = ImGui::GetCursorScreenPos();
			ImDrawList* pCanvas = ImGui::GetWindowDrawList();
			const ImU32 col32 = static_cast<ImColor>(ImVec4{ _colour.x , _colour.y, _colour.z, _colour.w });

			pCanvas->AddLine(ImVec2{ pos.x, pos.y + height * 0.5f },
				ImVec2{ pos.x + width * 0.25f, pos.y + height }, col32, _thickness);
			pCanvas->AddLine(ImVec2{ pos.x + width * 0.25f, pos.y + height },
				ImVec2{ pos.x + width, pos.y }, col32, _thickness);

			ImGui::Dummy(ImVec2{ width, height });
		}

		bool IconFolder(const char* _uniqueId, float width, float height, bool _open, const Math::Vec4& _colour)
		{
			ImDrawList*		pCanvas		= ImGui::GetWindowDrawList();
			ImVec2			pos			= ImGui::GetCursorScreenPos();
			const ImU32		col32		= static_cast<ImColor>(ImVec4{ _colour.x , _colour.y, _colour.z, _colour.w });
			//const ImU32		col32Dull	= static_cast<ImColor>(ImVec4{ _colour.x - 0.2f, _colour.y - 0.2f, _colour.z - 0.2f, _colour.w });
			const float iconWidth = (width * 0.8f);
			const float offset = (height * 0.2f);
			const ImVec2 PeekLeft{ pos.x, pos.y + offset };
			const ImVec2 botLeft{ pos.x, PeekLeft.y + height - 1 };
			const ImVec2 PeekRight{ pos.x + iconWidth, PeekLeft.y + (height / 6.f) };
			const ImVec2 botRight{ PeekRight.x, botLeft.y };
			const ImVec2 tabPeek{ pos.x + (width * 0.3f), PeekLeft.y };
			const ImVec2 tabBot{ pos.x + (width * 0.4f), PeekRight.y };

			if (!_open)
			{
				ImVec2 inside{ PeekLeft.x, PeekRight.y + (height * 0.1f) };
				pCanvas->PathClear();
				pCanvas->PathLineTo(PeekLeft);
				pCanvas->PathLineTo(inside);
				pCanvas->PathLineTo(ImVec2{ inside.x + iconWidth, inside.y });
				pCanvas->PathLineTo(PeekRight);
				pCanvas->PathLineTo(tabBot);
				pCanvas->PathLineTo(tabPeek);
				pCanvas->PathStroke(col32, true);

				pCanvas->PathClear();
				pCanvas->PathLineTo(inside);
				pCanvas->PathLineTo(botLeft);
				pCanvas->PathLineTo(botRight);
				pCanvas->PathLineTo(ImVec2{ inside.x + iconWidth, inside.y });
				pCanvas->PathFillConvex(col32);
			}
			else
			{
				ImVec2 inside{ PeekLeft.x + (0.2f * width), PeekLeft.y + (height * 0.4f) };
				pCanvas->PathClear();
				pCanvas->PathLineTo(botLeft);
				pCanvas->PathLineTo(PeekLeft);
				pCanvas->PathLineTo(tabPeek);
				pCanvas->PathLineTo(tabBot);
				pCanvas->PathLineTo(PeekRight);
				pCanvas->PathLineTo(ImVec2{ PeekRight.x, PeekLeft.y + (height * 0.4f) });
				pCanvas->PathStroke(col32, true);

				pCanvas->PathClear();
				pCanvas->PathLineTo(ImVec2{ inside.x + iconWidth, inside.y });
				pCanvas->PathLineTo(ImVec2{ botRight.x, botRight.y + 1 });
				pCanvas->PathLineTo(ImVec2{ botLeft.x, botLeft.y + 1 });
				pCanvas->PathLineTo(inside);
				pCanvas->PathFillConvex(col32);
			}

			//ImGui::Dummy(ImVec2{ width, height + offset });
			//HashString invi{ "##IconF" };
			//invi += _uniqueId;
			return ImGui::InvisibleButton(_uniqueId, ImVec2{ width, height + offset });
		}

		bool IconCircle(const char* _uniqueId, float radius, float offsetX, float offsetY, const Math::Vec4& _colour)
		{
			ImDrawList*		pCanvas = ImGui::GetWindowDrawList();
			ImVec2			pos		= ImGui::GetCursorScreenPos();
			const ImU32		col32	 = static_cast<ImColor>(ImVec4{ _colour.x , _colour.y, _colour.z, _colour.w });

			ImVec2 centre{ pos.x + radius + offsetX, pos.y + radius + offsetY };
			pCanvas->AddCircle(centre, radius, col32);
			pCanvas->AddCircleFilled(centre, radius / 4, col32);
			//HashString invi{ "##IconC" };
			//invi += _uniqueId;
			return ImGui::InvisibleButton(_uniqueId,
				ImVec2{ (2 * radius) + (2 * offsetX), (2 * radius) + (2 * offsetY) });
		}

		bool IconCross(const char * _uniqueId, float radius, float offsetX, float offsetY, const Math::Vec4& _colour)
		{
			ImDrawList*		pCanvas = ImGui::GetWindowDrawList();
			ImVec2			pos = ImGui::GetCursorScreenPos();
			const ImU32		col32 = static_cast<ImColor>(ImVec4{ _colour.x , _colour.y, _colour.z, _colour.w });

			ImVec2 centre{ pos.x + radius + offsetX, pos.y + radius + offsetY };
			pCanvas->AddCircle(centre, radius, col32);
			float r = radius / 3.f;
			pCanvas->AddLine(ImVec2{ centre.x - r, centre.y - r },
				ImVec2{ centre.x + r, centre.y + r },
				col32);
			pCanvas->AddLine(ImVec2{ centre.x - r, centre.y + r },
				ImVec2{ centre.x + r, centre.y - r },
				col32);
			//HashString invi{ "##IconX" };
			//invi += _uniqueId;
			return ImGui::InvisibleButton(_uniqueId,
				ImVec2{ (2 * radius) + (2 * offsetX), (2 * radius) + (2 * offsetY) });
		}

		bool IconGameObj(const char* _uniqueId, float _width, float _height)
		{
			const ImU32		col32R = static_cast<ImColor>(ImVec4{ 1,0,0,1 });
			const ImU32		col32G = static_cast<ImColor>(ImVec4{ 0,1,0,1 });
			const ImU32		col32B = static_cast<ImColor>(ImVec4{ 0.2f,0.93f,1,1 });
			ImDrawList*		pCanvas = ImGui::GetWindowDrawList();
			ImVec2			pos		= ImGui::GetCursorScreenPos();
			ImVec2 midPeek{ pos.x + (_width/2), pos.y };
			ImVec2 mid{ midPeek.x, midPeek.y + (_width /3) };
			ImVec2 midBot{ mid.x, midPeek.y + (_height * 0.8f) };
			ImVec2 midLeft{ pos.x + (_width * 0.1f), (midPeek.y + mid.y) / 2 };
			ImVec2 midRight{ pos.x + (_width * 0.9f), midLeft.y };
			ImVec2 botLeft{ midLeft.x + (_width / 8), midPeek.y + (3 * (_height /5))};
			ImVec2 botRight{ midRight.x - (_width / 8), botLeft.y };

			pCanvas->PathClear();
			pCanvas->PathLineTo(midPeek);
			pCanvas->PathLineTo(midLeft);
			pCanvas->PathLineTo(mid);
			pCanvas->PathLineTo(midRight);
			pCanvas->PathStroke(col32G, true);
			pCanvas->PathClear();
			pCanvas->PathLineTo(midLeft);
			pCanvas->PathLineTo(botLeft);
			pCanvas->PathLineTo(midBot);
			pCanvas->PathStroke(col32B, false);
			pCanvas->PathClear();
			pCanvas->PathLineTo(mid);
			pCanvas->PathLineTo(midBot);
			pCanvas->PathLineTo(botRight);
			pCanvas->PathLineTo(midRight);
			pCanvas->PathStroke(col32R, false);

			//HashString invi{ "##IconG" };
			//invi += _uniqueId;
			return ImGui::InvisibleButton(_uniqueId, ImVec2{ _width, _height });
		}

		bool IconFile(const char* _uniqueId, float _width, float _height, const Math::Vec4& _colour)
		{
			const ImU32 col32 = static_cast<ImColor>(ImVec4{ _colour.x , _colour.y, _colour.z, _colour.w});
			ImVec2 pos = ImGui::GetCursorScreenPos();
			ImVec2 size{ _width, _height };
			const float iconWidth = size.x / 2;
			const float iconHeight = size.y / 2;
			ImDrawList* pCanvas = ImGui::GetWindowDrawList();

			pCanvas->PathClear();
			pCanvas->PathLineTo(pos);
			pCanvas->PathLineTo(ImVec2{ pos.x + (iconWidth * 0.7f), pos.y });
			pCanvas->PathLineTo(ImVec2{ pos.x + iconWidth, pos.y + (iconWidth * 0.3f) });
			pCanvas->PathLineTo(ImVec2{ pos.x + iconWidth, pos.y + iconHeight });
			pCanvas->PathLineTo(ImVec2{ pos.x, pos.y + iconHeight });
			pCanvas->PathStroke(col32, true);
			ImGui::SetCursorScreenPos(pos);

			//HashString invi{ "##IconFI" };
			//invi += _uniqueId;
			return ImGui::InvisibleButton(_uniqueId, size);;
		}

		void Outline(float _x, float _y)
		{
			ImGuiContext& g = *GImGui;
			ImGuiWindow* window = g.CurrentWindow;
			ImVec2 pos = ImGui::GetCursorScreenPos();
			ImRect r{ pos, ImVec2{ pos.x + _x, pos.y + _y } };

			r.Expand(2.f);
			bool push_clip_rect = !window->ClipRect.Contains(r);
			if (push_clip_rect) window->DrawList->PushClipRectFullScreen();
			window->DrawList->AddRect(r.Min, r.Max, ImGui::GetColorU32(ImGuiCol_DragDropTarget), 0.0f, ~0, 2.0f);
			if (push_clip_rect) window->DrawList->PopClipRect();

			ImGui::SetCursorScreenPos(pos);
		}

		bool Image(const size_t& _imgID, const Math::Vec2& _imgSize, bool _interactive, bool _outlineBG)
		{
			if (!_interactive)
			{
				ImGui::Image(reinterpret_cast<void*>(_imgID), _imgSize,
					ImVec2{ 0,0 }, ImVec2{ 1,1 }, ImVec4{ 1,1,1,1 },
					(_outlineBG) ? ImGui::GetStyleColorVec4(ImGuiCol_Border) : ImVec4{ 0,0,0,0 });
				return false;
			}
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0,0,0,0 });
			bool ret = ImGui::ImageButton(reinterpret_cast<void*>(_imgID), _imgSize,
				ImVec2{ 0,0 }, ImVec2{ 1,1 }, 0,
				(_outlineBG) ? ImGui::GetStyleColorVec4(ImGuiCol_BorderShadow) : ImVec4{ 0,0,0,0 },
				ImVec4{ 1,1,1,1 });
			ImGui::PopStyleColor();
			return ret;
		}
	}
}	// NAMESPACE DYSPeekIA::EGUI::DISPLAY
#endif // EDITOR

