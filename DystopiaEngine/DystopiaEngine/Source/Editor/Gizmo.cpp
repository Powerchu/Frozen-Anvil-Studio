/* HEADER *********************************************************************************/
/*!
\file	Gizmo.cpp
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include <string>
#include "Editor/EGUI.h"
#if EDITOR
#include "Editor/Gizmo.h"
#include "../../ImGui/imgui.h"
#include "../../ImGui/imgui_internal.h"

static ImVec4 yellow{ 1.f, 1.f,0.f ,0.9f };
static constexpr int sizeGizA = 7;
static constexpr int sizeGizS = 8;
static constexpr float speedMult = 1.375f;

EGUI::eDragStatus EGUI::Gizmo2D::ArrowLeft(const std::string& _uniqueID, float& _outputValX, const Math::Vec2& _origin, float _vSpeed, const Math::Vec4& _col, bool* _pHovered, float _len, float _thickness)
{
	EGUI::eDragStatus status = EGUI::eNO_CHANGE;
	auto initialOrigin = ImGui::GetCursorPos();
	ImGui::SetCursorPos(_origin);
	float halfThick = 0.5f*_thickness;

	ImDrawList*	pCanvas		= ImGui::GetWindowDrawList();
	ImVec2		pos			= ImGui::GetCursorScreenPos();
	pos.y -= halfThick;
	ImVec2 gizmoArrowPoints[sizeGizA] =
	{
		ImVec2{ pos.x + _len		, pos.y + halfThick - 1.f },
		ImVec2{ pos.x + _len		, pos.y					  },
		ImVec2{ pos.x + _len * 1.25f, pos.y + halfThick		  },
		ImVec2{ pos.x + _len		, pos.y + _thickness	  },
		ImVec2{ pos.x + _len		, pos.y + halfThick + 1.f },
		ImVec2{ pos.x				, pos.y + halfThick + 1.f },
		ImVec2{ pos.x				, pos.y + halfThick - 1.f }
	};
	ImGui::SetItemAllowOverlap();

	/* Start Button Behaviours ------------------------------------------------------- */
	ImGuiContext& g = *GImGui;
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return status;
	const ImGuiID id = window->GetID(_uniqueID.c_str());
	ImVec2 size = ImGui::CalcItemSize(ImVec2{ _len*1.25f, _thickness * 0.8f }, 0.0f, 0.0f);
	const ImRect bb(ImVec2{ window->DC.CursorPos.x, window->DC.CursorPos.y - halfThick }, ImVec2{ window->DC.CursorPos.x + size.x, window->DC.CursorPos.y + size.y - halfThick });
	ImGui::ItemSize(bb);
	if (!ImGui::ItemAdd(bb, id))
		return status;
	bool hovered, held;
	bool released = ImGui::ButtonBehavior(bb, id, &hovered, &held);
	bool amActive = g.ActiveId == window->DC.LastItemId;
	if (hovered &&_pHovered) *_pHovered = true;
	if (released || (g.ActiveIdPreviousFrame == window->DC.LastItemId && g.IO.MouseReleased[0]))
		status = EGUI::eEND_DRAG;
	else if (amActive)
	{
		if (g.ActiveIdPreviousFrame != window->DC.LastItemId)
			status = EGUI::eSTART_DRAG;
		else
		{
			float adjust = 0;
			status = EGUI::eDRAGGING;
			if (g.ActiveIdSource == ImGuiInputSource_Mouse && ImGui::IsMousePosValid() && g.IO.MouseDragMaxDistanceSqr[0] > 1.0f*1.0f)
				adjust = g.IO.MouseDelta.x * _vSpeed * speedMult;
			_outputValX += adjust;
		}
	}
	
	ImU32 color = (held) ? ImGui::ColorConvertFloat4ToU32(yellow) : 
						   ImGui::ColorConvertFloat4ToU32(_col);
	/* End Button Behaviours --------------------------------------------------------- */

	ImGui::SetCursorPos(_origin);
	pCanvas->AddConvexPolyFilled(gizmoArrowPoints, sizeGizA, color);
	ImGui::SetCursorPos(initialOrigin);
	return status;
}

EGUI::eDragStatus EGUI::Gizmo2D::ArrowUp(const std::string& _uniqueID, float& _outputValY, const Math::Vec2& _origin, float _vSpeed, const Math::Vec4& _col, bool* _pHovered, float _len, float _thickness)
{
	EGUI::eDragStatus status = EGUI::eNO_CHANGE;
	auto initialOrigin = ImGui::GetCursorPos();
	ImGui::SetCursorPos(_origin);
	float halfThick = 0.5f*_thickness;

	ImDrawList*	pCanvas = ImGui::GetWindowDrawList();
	ImVec2		pos		= ImGui::GetCursorScreenPos();
	pos.x -= halfThick;
	ImVec2 gizmoArrowPoints[sizeGizA] =
	{
		ImVec2{ pos.x + halfThick - 1.f , pos.y - _len			 },
		ImVec2{ pos.x					, pos.y - _len			 },
		ImVec2{ pos.x + halfThick		, pos.y - _len * 1.25f	 },
		ImVec2{ pos.x + _thickness 		, pos.y - _len			 },
		ImVec2{ pos.x + halfThick + 1.f , pos.y - _len			 },
		ImVec2{ pos.x + halfThick + 1.f , pos.y					 },
		ImVec2{ pos.x + halfThick - 1.f , pos.y					 }
	};
	ImGui::SetItemAllowOverlap();

	/* Start Button Behaviours ------------------------------------------------------- */
	ImGuiContext& g = *GImGui;
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return status;
	const ImGuiID id = window->GetID(_uniqueID.c_str());
	ImVec2 size = ImGui::CalcItemSize(ImVec2{ _thickness * 0.8f, _len*1.25f }, 0.0f, 0.0f);
	const ImRect bb(ImVec2{ window->DC.CursorPos.x - halfThick, window->DC.CursorPos.y - size.y }, ImVec2{ window->DC.CursorPos.x + size.x - halfThick, window->DC.CursorPos.y});
	ImGui::ItemSize(bb);
	if (!ImGui::ItemAdd(bb, id))
		return status;
	bool hovered, held;
	bool released = ImGui::ButtonBehavior(bb, id, &hovered, &held);
	bool amActive = g.ActiveId == window->DC.LastItemId;
	if (hovered &&_pHovered) *_pHovered = true;
	if (released || (g.ActiveIdPreviousFrame == window->DC.LastItemId && g.IO.MouseReleased[0]))
		status = EGUI::eEND_DRAG;
	else if (amActive)
	{
		if (g.ActiveIdPreviousFrame != window->DC.LastItemId)
			status = EGUI::eSTART_DRAG;
		else
		{
			float adjust = 0;
			status = EGUI::eDRAGGING;
			if (g.ActiveIdSource == ImGuiInputSource_Mouse && ImGui::IsMousePosValid() && g.IO.MouseDragMaxDistanceSqr[0] > 1.0f*1.0f)
				adjust = g.IO.MouseDelta.y * _vSpeed * speedMult;
			_outputValY -= adjust;
		}
	}

	ImU32 color = (held) ? ImGui::ColorConvertFloat4ToU32(yellow) :
						   ImGui::ColorConvertFloat4ToU32(_col);
	/* End Button Behaviours --------------------------------------------------------- */

	ImGui::SetCursorPos(_origin);
	pCanvas->AddConvexPolyFilled(gizmoArrowPoints, sizeGizA, color);
	ImGui::SetCursorPos(initialOrigin);
	return status;
}

EGUI::eDragStatus EGUI::Gizmo2D::ScalerLeft(const std::string& _uniqueID, float& _outputValX, const Math::Vec2& _origin, float _vSpeed, const Math::Vec4& _col, bool* _pHovered, float _len, float _thickness)
{
	EGUI::eDragStatus status = EGUI::eNO_CHANGE;
	auto initialOrigin = ImGui::GetCursorPos();
	ImGui::SetCursorPos(_origin);
	float halfThick = 0.5f*_thickness;

	ImDrawList*	pCanvas = ImGui::GetWindowDrawList();
	ImVec2		pos = ImGui::GetCursorScreenPos();
	pos.y -= halfThick;
	_len *= 0.9f;
	ImVec2 gizmoArrowPoints[sizeGizS] =
	{
		ImVec2{ pos.x + _len		, pos.y + halfThick - 1.f },
		ImVec2{ pos.x + _len		, pos.y					  },
		ImVec2{ pos.x + _len * 1.3f , pos.y 				  },
		ImVec2{ pos.x + _len * 1.3f , pos.y + _thickness	  },
		ImVec2{ pos.x + _len		, pos.y + _thickness	  },
		ImVec2{ pos.x + _len		, pos.y + halfThick + 1.f },
		ImVec2{ pos.x				, pos.y + halfThick + 1.f }, 
		ImVec2{ pos.x				, pos.y + halfThick - 1.f }
	};
	ImGui::SetItemAllowOverlap();

	/* Start Button Behaviours ------------------------------------------------------- */
	ImGuiContext& g = *GImGui;
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return status;
	const ImGuiID id = window->GetID(_uniqueID.c_str());
	ImVec2 size = ImGui::CalcItemSize(ImVec2{ _len*1.25f, _thickness * 0.8f }, 0.0f, 0.0f);
	const ImRect bb(ImVec2{ window->DC.CursorPos.x, window->DC.CursorPos.y - halfThick }, ImVec2{ window->DC.CursorPos.x + size.x, window->DC.CursorPos.y + size.y - halfThick });
	ImGui::ItemSize(bb);
	if (!ImGui::ItemAdd(bb, id))
		return status;
	bool hovered, held;
	bool released = ImGui::ButtonBehavior(bb, id, &hovered, &held);
	bool amActive = g.ActiveId == window->DC.LastItemId;
	if (hovered &&_pHovered) *_pHovered = true;
	if (released || (g.ActiveIdPreviousFrame == window->DC.LastItemId && g.IO.MouseReleased[0]))
		status = EGUI::eEND_DRAG;
	else if (amActive)
	{
		if (g.ActiveIdPreviousFrame != window->DC.LastItemId)
			status = EGUI::eSTART_DRAG;
		else
		{
			float adjust = 0;
			status = EGUI::eDRAGGING;
			if (g.ActiveIdSource == ImGuiInputSource_Mouse && ImGui::IsMousePosValid() && g.IO.MouseDragMaxDistanceSqr[0] > 1.0f*1.0f)
				adjust = g.IO.MouseDelta.x * _vSpeed * speedMult;
			_outputValX += adjust;
		}
	}

	ImU32 color = (held) ? ImGui::ColorConvertFloat4ToU32(yellow) :
		ImGui::ColorConvertFloat4ToU32(_col);
	/* End Button Behaviours --------------------------------------------------------- */

	ImGui::SetCursorPos(_origin);
	pCanvas->AddConvexPolyFilled(gizmoArrowPoints, sizeGizS, color);
	ImGui::SetCursorPos(initialOrigin);
	return status;
}

EGUI::eDragStatus EGUI::Gizmo2D::ScalerUp(const std::string& _uniqueID, float& _outputValY, const Math::Vec2& _origin, float _vSpeed, const Math::Vec4& _col, bool* _pHovered, float _len, float _thickness)
{
	EGUI::eDragStatus status = EGUI::eNO_CHANGE;
	auto initialOrigin = ImGui::GetCursorPos();
	ImGui::SetCursorPos(_origin);
	float halfThick = 0.5f*_thickness;
	_len *= 0.9f;
	ImDrawList*	pCanvas = ImGui::GetWindowDrawList();
	ImVec2		pos = ImGui::GetCursorScreenPos();
	pos.x -= halfThick;
	ImVec2 gizmoArrowPoints[sizeGizS] =
	{
		ImVec2{ pos.x + halfThick - 1.f , pos.y - _len			 },
		ImVec2{ pos.x					, pos.y - _len			 },
		ImVec2{ pos.x 					, pos.y - _len * 1.3f	 },
		ImVec2{ pos.x + _thickness		, pos.y - _len * 1.3f    },
		ImVec2{ pos.x + _thickness 		, pos.y - _len			 },
		ImVec2{ pos.x + halfThick + 1.f , pos.y - _len			 },
		ImVec2{ pos.x + halfThick + 1.f , pos.y					 },
		ImVec2{ pos.x + halfThick - 1.f , pos.y					 }
	};
	ImGui::SetItemAllowOverlap();

	/* Start Button Behaviours ------------------------------------------------------- */
	ImGuiContext& g = *GImGui;
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return status;
	const ImGuiID id = window->GetID(_uniqueID.c_str());
	ImVec2 size = ImGui::CalcItemSize(ImVec2{ _thickness * 0.8f, _len*1.25f }, 0.0f, 0.0f);
	const ImRect bb(ImVec2{ window->DC.CursorPos.x - halfThick, window->DC.CursorPos.y - size.y }, ImVec2{ window->DC.CursorPos.x + size.x - halfThick, window->DC.CursorPos.y });
	ImGui::ItemSize(bb);
	if (!ImGui::ItemAdd(bb, id))
		return status;
	bool hovered, held;
	bool released = ImGui::ButtonBehavior(bb, id, &hovered, &held);
	bool amActive = g.ActiveId == window->DC.LastItemId;
	if (hovered &&_pHovered) *_pHovered = true;
	if (released || (g.ActiveIdPreviousFrame == window->DC.LastItemId && g.IO.MouseReleased[0]))
		status = EGUI::eEND_DRAG;
	else if (amActive)
	{
		if (g.ActiveIdPreviousFrame != window->DC.LastItemId)
			status = EGUI::eSTART_DRAG;
		else
		{
			float adjust = 0;
			status = EGUI::eDRAGGING;
			if (g.ActiveIdSource == ImGuiInputSource_Mouse && ImGui::IsMousePosValid() && g.IO.MouseDragMaxDistanceSqr[0] > 1.0f*1.0f)
				adjust = g.IO.MouseDelta.y * _vSpeed * speedMult;
			_outputValY -= adjust;
		}
	}

	ImU32 color = (held) ? ImGui::ColorConvertFloat4ToU32(yellow) :
						   ImGui::ColorConvertFloat4ToU32(_col);
	/* End Button Behaviours --------------------------------------------------------- */

	ImGui::SetCursorPos(_origin);
	pCanvas->AddConvexPolyFilled(gizmoArrowPoints, sizeGizS, color);
	ImGui::SetCursorPos(initialOrigin);
	return status;
}

EGUI::eDragStatus EGUI::Gizmo2D::Box(const std::string& _uniqueID, float& _outputValX, float& _outputValY, const Math::Vec2& _origin, float _vSpeed, const Math::Vec4& _col, bool* _pHovered, float _side)
{
	EGUI::eDragStatus status = EGUI::eNO_CHANGE;
	float offset = (_side / 3);
	auto initialOrigin = ImGui::GetCursorPos();
	ImGui::SetCursorPos(_origin);

	ImDrawList*	pCanvas = ImGui::GetWindowDrawList();
	ImVec2		pos = ImGui::GetCursorScreenPos();
	ImGui::SetItemAllowOverlap();
	/* Start Button Behaviours ------------------------------------------------------- */
	ImGuiContext& g = *GImGui;
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return status;
	const ImGuiID id = window->GetID(_uniqueID.c_str());
	ImVec2 size = ImGui::CalcItemSize(ImVec2{ _side, _side }, 0.0f, 0.0f);
	const ImRect bb(ImVec2{ window->DC.CursorPos.x + offset, window->DC.CursorPos.y - offset - size.y },
					ImVec2{ window->DC.CursorPos.x + size.x + offset, window->DC.CursorPos.y - offset });
	ImGui::ItemSize(bb);
	if (!ImGui::ItemAdd(bb, id))
		return status;
	bool hovered, held;
	bool released = ImGui::ButtonBehavior(bb, id, &hovered, &held);
	bool amActive = g.ActiveId == window->DC.LastItemId;
	if (hovered &&_pHovered) *_pHovered = true;
	if (released || (g.ActiveIdPreviousFrame == window->DC.LastItemId && g.IO.MouseReleased[0]))
		status = EGUI::eEND_DRAG;
	else if (amActive)
	{
		if (g.ActiveIdPreviousFrame != window->DC.LastItemId)
			status = EGUI::eSTART_DRAG;
		else
		{
			float adjustX = 0;
			float adjustY = 0;
			status = EGUI::eDRAGGING;
			if (g.ActiveIdSource == ImGuiInputSource_Mouse && ImGui::IsMousePosValid() && g.IO.MouseDragMaxDistanceSqr[0] > 1.0f*1.0f)
			{
				adjustX = g.IO.MouseDelta.x * _vSpeed * speedMult;
				adjustY = g.IO.MouseDelta.y * _vSpeed * speedMult;
			}
			_outputValY -= adjustY;
			_outputValX += adjustX;
		}
	}

	ImU32 color = (held) ? ImGui::ColorConvertFloat4ToU32(yellow) :
						   ImGui::ColorConvertFloat4ToU32(_col);
	/* End Button Behaviours --------------------------------------------------------- */

	ImGui::SetCursorPos(_origin);
	pCanvas->AddRectFilled(ImVec2{ pos.x + offset, pos.y - offset - _side }, ImVec2{ pos.x + offset + _side, pos.y - offset}, color);
	ImGui::SetCursorPos(initialOrigin);
	return status;
}

#endif // EDITOR
