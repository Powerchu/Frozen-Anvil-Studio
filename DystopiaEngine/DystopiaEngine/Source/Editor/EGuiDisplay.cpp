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
#include "Editor/Commands.h"
#include "Editor/CommandList.h"
#include "DataStructure/Stack.h"

Dystopia::CommandHandler *gContextComdHND = nullptr;
Stack<float> g_StackLeftAlign{ 100 };

namespace EGUI
{
	static bool IsItemActiveLastFrame()
	{
		ImGuiContext g = *GImGui;
		if (g.ActiveIdPreviousFrame) return g.ActiveIdPreviousFrame == g.CurrentWindow->DC.LastItemId;
		return false;
	}

	void SetContext(Dystopia::CommandHandler *_pContext)
	{
		gContextComdHND = _pContext;
	}

	Dystopia::CommandHandler* GetCommandHND()
	{
		return gContextComdHND;
	}

	void RemoveContext()
	{
		gContextComdHND = nullptr;
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

	bool StartMenuHeader(const std::string& _label, bool _enabled)
	{
		return ImGui::BeginMenu(_label.c_str(), _enabled);
	}

	bool StartMenuBody(const std::string& _label, const std::string& _shortcut, bool _enabled)
	{
		return ImGui::MenuItem(_label.c_str(), _shortcut.c_str(), false, _enabled);
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

	bool StartChild(const std::string& _label, const Math::Vec2& _size, bool _showBorder, const Math::Vec4& /*_colour*/)
	{
		return ImGui::BeginChild(_label.c_str(), ImVec2{ _size.x, _size.y }, _showBorder);
	}

	bool StartChild(const std::string& _label, const float& _x, const float& _y, bool _showBorder, const Math::Vec4& _colour)
	{
		return StartChild(_label, Math::Vec2{ _x, _y }, _showBorder, _colour);
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

		bool TextField(const std::string& _label, char* _outputbuffer, size_t _size, bool _showLabel, float _width)
		{
			ImGuiInputTextFlags flags = ImGuiInputTextFlags_AutoSelectAll | 
										ImGuiInputTextFlags_EnterReturnsTrue;
			ImGui::PushItemWidth(_width);
			if (_showLabel)
			{
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + DefaultAlighnmentOffsetY);
				Label(_label.c_str());
				SameLine(DefaultAlighnmentSpacing, g_StackLeftAlign.IsEmpty() ? DefaultAlignLeft : g_StackLeftAlign.Peek());
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() - DefaultAlighnmentOffsetY);
			}
			bool b = ImGui::InputText(("###TextField" + _label).c_str(), _outputbuffer, _size, flags);
			ImGui::PopItemWidth();
			return b;
		}

		bool EmptyBox(const std::string& _label, float _width, const std::string& _anythingToShowInside, bool _iteractive, bool _showLabel)
		{
			bool clicked = false;
			bool show = _iteractive ? true : false;

			if (_showLabel)
			{
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + DefaultAlighnmentOffsetY);
				Label(_label.c_str());
				SameLine(DefaultAlighnmentSpacing, g_StackLeftAlign.IsEmpty() ? DefaultAlignLeft : g_StackLeftAlign.Peek());
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() - DefaultAlighnmentOffsetY);
			}

			ImGui::PushItemWidth(_width);
			if (ImGui::Button((_anythingToShowInside + "###" + _label).c_str(),
				ImVec2{ _width, (ImGui::GetStyle().FramePadding.y * 2.f) + GImGui->FontSize }))
			{
				if (show) clicked = true;
			}
			ImGui::PopItemWidth();
			return clicked;
		}

		bool CheckBox(const std::string& _label, bool* _outputBool, bool _showLabel)
		{
			if (_showLabel)
			{
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + DefaultAlighnmentOffsetY);
				Label(_label.c_str());
				SameLine(DefaultAlighnmentSpacing, g_StackLeftAlign.IsEmpty() ? DefaultAlignLeft : g_StackLeftAlign.Peek());
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() - DefaultAlighnmentOffsetY);
			}
			return ImGui::Checkbox(("##CheckBox" + _label).c_str(), _outputBool);
		}

		eDragStatus DragFloat(const std::string& _label, float* _outputFloat, float _dragSpeed, float _min, float _max, bool _hideText, float _width)
		{
			static POINT p;
			static const int xPos = (GetSystemMetrics(SM_CXSCREEN)) / 2;
			static const int yPos = (GetSystemMetrics(SM_CYSCREEN)) / 2;

			if (!_hideText)
			{
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + DefaultAlighnmentOffsetY);
				Label(_label.c_str());
				SameLine(DefaultAlighnmentSpacing, g_StackLeftAlign.IsEmpty() ? DefaultAlignLeft : g_StackLeftAlign.Peek());
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() - DefaultAlighnmentOffsetY);
			}
			bool changing = false;
			ImGui::PushItemWidth(_width);
			changing = ImGui::DragFloat(("###DragFloat" + _label).c_str(), _outputFloat, _dragSpeed, _min, _max, "%.3f");
			ImGui::PopItemWidth();

			if (!IsItemActiveLastFrame() && ImGui::IsItemActive())
			{
				//GetCursorPos(&p);
				//SetCursorPos(xPos, yPos);
				//ShowCursor(false);
				return eSTART_DRAG;
			}

			if (changing) return eDRAGGING;


			if (ImGui::IsItemDeactivatedAfterChange())
			{
				//SetCursorPos(p.x, p.y);
				//ShowCursor(true);
				return (ImGui::IsMouseReleased(0)) ? eEND_DRAG : eENTER;
			}

			if (ImGui::IsItemDeactivated()) {
				//SetCursorPos(p.x, p.y);
				//ShowCursor(true);
				return eDEACTIVATED;
			}
			return eNO_CHANGE;
		}

		eDragStatus SliderFloat(const std::string& _label, float *_pOutFloat, float _min, float _max, bool _hideText, float _width)
		{
			if (!_hideText)
			{
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + DefaultAlighnmentOffsetY);
				Label(_label.c_str());
				SameLine(DefaultAlighnmentSpacing, g_StackLeftAlign.IsEmpty() ? DefaultAlignLeft : g_StackLeftAlign.Peek());
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() - DefaultAlighnmentOffsetY);
			}
			bool changing = false;
			ImGui::PushItemWidth(_width);
			changing = ImGui::SliderFloat(("###SlideFloat" + _label).c_str(), _pOutFloat, _min, _max, "%.3f");
			ImGui::PopItemWidth();

			if (!IsItemActiveLastFrame() && ImGui::IsItemActive())
				return changing ? eINSTANT_CHANGE : eSTART_DRAG;
			else if (ImGui::IsItemDeactivated())
				return ImGui::IsMouseReleased(0) ? eEND_DRAG : eDEACTIVATED;
			else if (ImGui::IsItemActive())
				return changing ? eDRAGGING : eNO_CHANGE;
			return eNO_CHANGE;
		}

		eDragStatus DragInt(const std::string& _label, int* _outputInt, float _dragSpeed, int _min, int _max, bool _hideText, float _width)
		{
			if (!_hideText)
			{
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + DefaultAlighnmentOffsetY);
				Label(_label.c_str());
				SameLine(DefaultAlighnmentSpacing, g_StackLeftAlign.IsEmpty() ? DefaultAlignLeft : g_StackLeftAlign.Peek());
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() - DefaultAlighnmentOffsetY);
			}
			bool changing = false;
			ImGui::PushItemWidth(_width);
			changing = ImGui::DragInt(("###DragInt" + _label).c_str(), _outputInt, _dragSpeed, _min, _max);
			ImGui::PopItemWidth();

			if (!IsItemActiveLastFrame() && ImGui::IsItemActive()) return eSTART_DRAG;
			if (changing) return eDRAGGING;
			if (ImGui::IsItemDeactivatedAfterChange())
				return (ImGui::IsMouseReleased(0)) ? eEND_DRAG : eENTER;
			if (ImGui::IsItemDeactivated()) return eDEACTIVATED;
			return eNO_CHANGE;
		}

		Array<eDragStatus, 3> VectorFields(const std::string& _label, Math::Vector4 *_outputVec, float _dragSpeed, float _min, float _max, float _width)
		{
			std::string field1 = "##VecFieldX", field2 = "##VecFieldY", field3 = "##VecFieldZ";
			float x, y, z;
			x = _outputVec->x;
			y = _outputVec->y;
			z = _outputVec->z;
			field1 += _label;
			field2 += _label;
			field3 += _label;

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + DefaultAlighnmentOffsetY);
			Label(_label.c_str());
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
	
		Array<eDragStatus, 2> VectorFields(const std::string& _label, Math::Vector2 *_outputVec, float _dragSpeed, float _min, float _max, float _width)
		{
			std::string field1 = "##VecFieldX", field2 = "##VecFieldY";
			float x, y;
			x = _outputVec->x;
			y = _outputVec->y;
			field1 += _label;
			field2 += _label;

			ImGui::PushItemWidth(_width);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + DefaultAlighnmentOffsetY);
			Label(_label.c_str());
			SameLine(DefaultAlighnmentSpacing, g_StackLeftAlign.IsEmpty() ? DefaultAlignLeft : g_StackLeftAlign.Peek());

			Label("X:"); SameLine();
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - DefaultAlighnmentOffsetY);
			eDragStatus statX = EGUI::Display::DragFloat(field1.c_str(), &x, _dragSpeed, _min, _max, true);
			if (statX != eDragStatus::eNO_CHANGE) _outputVec->x = x;

			SameLine(); Label("Y:"); SameLine();
			eDragStatus statY = EGUI::Display::DragFloat(field2.c_str(), &y, _dragSpeed, _min, _max, true);
			if (statY != eDragStatus::eNO_CHANGE) _outputVec->y = y;

			ImGui::PopItemWidth();

			return Array<eDragStatus, 2>{statX, statY};
		}

		bool CollapsingHeader(const std::string& _label)
		{
			return ImGui::CollapsingHeader(_label.c_str(), ImGuiTreeNodeFlags_DefaultOpen);
		}

		bool SelectableTxt(const std::string& _label, bool _highlight)
		{
			if (_highlight) ImGui::PushStyleColor(ImGuiCol_Header, ImGui::GetColorU32(ImGuiCol_HeaderHovered));
			bool ret = ImGui::Selectable(_label.c_str(), _highlight);
			if (_highlight) ImGui::PopStyleColor();
			return ret;
		}

		bool SelectableTxt(const std::string& _label, bool* _outputBool)
		{
			return (ImGui::Selectable(_label.c_str(), _outputBool));
		}

		bool SelectableTxtDouble(const std::string& _label, bool _highlight)
		{
			return (ImGui::Selectable(_label.c_str(), _highlight, ImGuiSelectableFlags_AllowDoubleClick)) && ImGui::IsMouseDoubleClicked(0);
		}

		bool SelectableTxtDouble(const std::string& _label, bool* _outputBool)
		{
			if ((ImGui::Selectable(_label.c_str(), _outputBool, ImGuiSelectableFlags_AllowDoubleClick)) && ImGui::IsMouseDoubleClicked(0))
				return true;
			else
				*_outputBool = false;
			return false;
		}

		bool StartTreeNode(const std::string&_label, bool* _outClicked, bool _highlighted, bool _noArrow, bool _defaultOpen)
		{
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
			flags = _highlighted ? flags | ImGuiTreeNodeFlags_Selected : flags;
			flags = _noArrow ? flags | ImGuiTreeNodeFlags_Leaf : flags;

			if (_highlighted)
				ImGui::PushStyleColor(ImGuiCol_Header, ImGui::GetColorU32(ImGuiCol_HeaderHovered));
			if (_defaultOpen)
				flags |= ImGuiTreeNodeFlags_DefaultOpen;
			bool ret = ImGui::TreeNode(_label.c_str(), _outClicked, flags);

			if (_highlighted)
				ImGui::PopStyleColor();

			return ret;
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

		void OpenTreeNode(bool _open)
		{
			ImGui::SetNextTreeNodeOpen(_open, ImGuiCond_FirstUseEver);
		}

		void EndTreeNode()
		{
			ImGui::TreePop();
		}

		bool StartPayload(ePayloadTags _tagLoad, void* _pData, size_t _dataSize, const std::string& _toolTip)
		{
			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
			{
				ImGui::SetDragDropPayload(EGUI::GetPayloadString(_tagLoad), _pData, _dataSize);
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

		bool CustomPayload(const std::string& _uniqueId, const std::string& _label, const std::string& _tooltip,
						   const Math::Vec2& _displaytSize, ePayloadTags _tagLoad, void* _pData, size_t _dataSize)
		{
			ImVec2 pos = ImGui::GetCursorScreenPos();
			ImVec2 size{ _displaytSize.x, _displaytSize.y };
			const float iconWidth = size.x / 2;
			const float iconHeight = size.y / 2;
			const float offsetX = iconWidth / 2;
			const float offsetY = iconHeight / 4;
			ImVec2 posIcon{ pos.x + offsetX, pos.y + offsetY };
			ImVec2 posText{ pos.x + 1, pos.y + iconHeight + (2* offsetY) };

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0,0,0,0 });
			bool btn = ImGui::Button(("###CustomPayload" + _uniqueId).c_str(), size);
			bool payload = StartPayload(_tagLoad, _pData, _dataSize, _tooltip);
			ImGui::PopStyleColor();
			if (payload) EndPayload();
			ImGui::SetCursorScreenPos(posIcon);
			IconFile(_uniqueId.c_str(), size.x, size.y);
			ImGui::SetCursorScreenPos(posText);
			ImGui::TextWrapped(_label.c_str());
			return btn;
		}

		void OpenPopup(const std::string& _thePopupID, bool _toOpenAtMousePos)
		{
			ImGui::OpenPopup(_thePopupID.c_str());
			if (!_toOpenAtMousePos)
			{
				ImGuiContext& g = *ImGui::GetCurrentContext();
				ImGuiWindow* pGuiWin = g.CurrentWindow;
				ImRect callerRect = pGuiWin->DC.LastItemRect;
				ImGuiPopupRef& popup_ref = g.OpenPopupStack[g.CurrentPopupStack.Size];
				popup_ref.OpenPopupPos = callerRect.GetBL();
			}
		}

		bool StartPopup(const std::string& _uniqueID)
		{
			return ImGui::BeginPopup(_uniqueID.c_str());
		}

		bool StartPopupModal(const std::string& _uniqueID, const std::string& _label)
		{
			ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize |
									 ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;

			bool ret = ImGui::BeginPopupModal(_uniqueID.c_str(), NULL, flags);
			if (ret)
			{
				EGUI::Display::Label(_label.c_str());
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

		bool DropDownSelection(const std::string& _label, int& _currentIndex, AutoArray<std::string>& _arrOfItems, float _width)
		{
			AutoArray<const char*> arrCharPtr;
			for (const auto &e : _arrOfItems)
				arrCharPtr.push_back(e.c_str());

			ImGui::PushItemWidth(_width);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + DefaultAlighnmentOffsetY);
			Label(_label.c_str());
			SameLine(DefaultAlighnmentSpacing, g_StackLeftAlign.IsEmpty() ? DefaultAlignLeft : g_StackLeftAlign.Peek());
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - DefaultAlighnmentOffsetY);
			bool ret = ImGui::Combo(("##DropDownList" + _label).c_str(), &_currentIndex, arrCharPtr.begin(), static_cast<int>(arrCharPtr.size()));
			ImGui::PopItemWidth();
			return ret;
		}

		bool Button(const std::string& _label, const Math::Vec2& _size)
		{
			return ImGui::Button(_label.c_str(), ImVec2{ _size.x, _size.y });
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

		bool IconFolder(const std::string& _uniqueId, float width, float height, bool _open, const Math::Vec4& _colour)
		{
			ImDrawList*		pCanvas		= ImGui::GetWindowDrawList();
			ImVec2			pos			= ImGui::GetCursorScreenPos();
			const ImU32		col32		= static_cast<ImColor>(ImVec4{ _colour.x , _colour.y, _colour.z, _colour.w });
			const ImU32		col32Dull	= static_cast<ImColor>(ImVec4{ _colour.x - 0.2f, _colour.y - 0.2f, _colour.z - 0.2f, _colour.w });
			const float iconWidth = (width * 0.8f);
			const float offset = (height * 0.2f);
			ImVec2 topLeft{ pos.x, pos.y + offset };
			ImVec2 botLeft{ pos.x, topLeft.y + height - 1 };
			ImVec2 topRight{ pos.x + iconWidth, topLeft.y + (height / 6.f) };
			ImVec2 botRight{ topRight.x, botLeft.y };
			ImVec2 tabTop{ pos.x + (width * 0.3f), topLeft.y };
			ImVec2 tabBot{ pos.x + (width * 0.4f), topRight.y };

			if (!_open)
			{
				ImVec2 inside{ topLeft.x, topRight.y + (height * 0.1f) };
				pCanvas->PathClear();
				pCanvas->PathLineTo(topLeft);
				pCanvas->PathLineTo(inside);
				pCanvas->PathLineTo(ImVec2{ inside.x + iconWidth, inside.y });
				pCanvas->PathLineTo(topRight);
				pCanvas->PathLineTo(tabBot);
				pCanvas->PathLineTo(tabTop);
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
				ImVec2 inside{ topLeft.x + (0.2f * width), topLeft.y + (height * 0.4f) };
				pCanvas->PathClear();
				pCanvas->PathLineTo(botLeft);
				pCanvas->PathLineTo(topLeft);
				pCanvas->PathLineTo(tabTop);
				pCanvas->PathLineTo(tabBot);
				pCanvas->PathLineTo(topRight);
				pCanvas->PathLineTo(ImVec2{ topRight.x, topLeft.y + (height * 0.4f) });
				pCanvas->PathStroke(col32, true);

				pCanvas->PathClear();
				pCanvas->PathLineTo(ImVec2{ inside.x + iconWidth, inside.y });
				pCanvas->PathLineTo(ImVec2{ botRight.x, botRight.y + 1 });
				pCanvas->PathLineTo(ImVec2{ botLeft.x, botLeft.y + 1 });
				pCanvas->PathLineTo(inside);
				pCanvas->PathFillConvex(col32);
			}

			//ImGui::Dummy(ImVec2{ width, height + offset });
			return ImGui::InvisibleButton(("##iconCircle" + _uniqueId).c_str(), ImVec2{ width, height + offset });
		}

		bool IconCircle(const std::string& _uniqueId, float radius, float offsetX, float offsetY, const Math::Vec4& _colour)
		{
			ImDrawList*		pCanvas = ImGui::GetWindowDrawList();
			ImVec2			pos		= ImGui::GetCursorScreenPos();
			const ImU32		col32	= static_cast<ImColor>(ImVec4{ _colour.x , _colour.y, _colour.z, _colour.w });

			ImVec2 centre{ pos.x + radius + offsetX, pos.y + radius + offsetY };
			pCanvas->AddCircle(centre, radius, col32);
			pCanvas->AddCircleFilled(centre, radius / 4, col32);
			return ImGui::InvisibleButton(("##iconCircle" + _uniqueId).c_str(), 
				   ImVec2{ (2 * radius) + (2 * offsetX), (2 * radius) + (2 * offsetY) });
		}

		bool IconCross(const std::string& _uniqueId, float radius, float offsetX, float offsetY, const Math::Vec4& _colour)
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
			return ImGui::InvisibleButton(("##iconCross" + _uniqueId).c_str(),
					ImVec2{ (2 * radius) + (2 * offsetX), (2 * radius) + (2 * offsetY) });
		}

		bool IconGameObj(const std::string& _uniqueId, float _width, float _height)
		{
			const ImU32		col32R = static_cast<ImColor>(ImVec4{ 1,0,0,1 });
			const ImU32		col32G = static_cast<ImColor>(ImVec4{ 0,1,0,1 });
			const ImU32		col32B = static_cast<ImColor>(ImVec4{ 0,0,1,1 });
			ImDrawList*		pCanvas = ImGui::GetWindowDrawList();
			ImVec2			pos		= ImGui::GetCursorScreenPos();
			ImVec2 midTop{ pos.x + (_width/2), pos.y };
			ImVec2 mid{ midTop.x, midTop.y + (_width /3) };
			ImVec2 midBot{ mid.x, midTop.y + (_height * 0.8f) };
			ImVec2 midLeft{ pos.x + (_width * 0.1f), (midTop.y + mid.y) / 2 };
			ImVec2 midRight{ pos.x + (_width * 0.9f), midLeft.y };
			ImVec2 botLeft{ midLeft.x + (_width / 8), midTop.y + (3 * (_height /5))};
			ImVec2 botRight{ midRight.x - (_width / 8), botLeft.y };

			pCanvas->PathClear();
			pCanvas->PathLineTo(midTop);
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

			return ImGui::InvisibleButton(("##iconGameObj" + _uniqueId).c_str(), ImVec2{ _width, _height });
		}

		bool IconFile(const std::string& _uniqueId, float _width, float _height, const Math::Vec4& _colour)
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
			return ImGui::InvisibleButton(("###IconFile" + _uniqueId).c_str(), size);;
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
}	// NAMESPACE DYSTOPIA::EGUI::DISPLAY
#endif // EDITOR

