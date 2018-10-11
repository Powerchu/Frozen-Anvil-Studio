/* HEADER *********************************************************************************/
/*!
\file	Gizmo.cpp
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#include "Editor/Gizmo.h"
#include "../../ImGui/imgui.h"
#include "../../ImGui/imgui_internal.h"

static ImVec4 yellow{ 1.f, 1.f,0.f ,1.f };
static constexpr int sizeGizA = 7;

EGUI::eDragStatus EGUI::Gizmo2D::ArrowLeft(const std::string& _uniqueID, float& _outputValX, const Math::Vec2& _origin, float _vSpeed, const Math::Vec4& _col, float _len, float _thickness, bool _negateDir)
{
	EGUI::eDragStatus status = EGUI::eNO_CHANGE;
	auto initialOrigin = ImGui::GetCursorPos();
	ImGui::SetCursorPos(_origin);
	float halfThick = 0.5f*_thickness;

	ImDrawList*	pCanvas		= ImGui::GetWindowDrawList();
	ImVec2		pos			= ImGui::GetCursorScreenPos();
	ImVec2 gizmoArrowPoints[sizeGizA] =
	{
		ImVec2{ pos.x + _len		, pos.y + halfThick - 1.f },
		ImVec2{ pos.x + _len		, pos.y					  },
		ImVec2{ pos.x + _len*1.25f	, pos.y + halfThick		  },
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
	const ImRect bb(window->DC.CursorPos, ImVec2{ window->DC.CursorPos.x + size.x, window->DC.CursorPos.y + size.y });
	ImGui::ItemSize(bb);
	if (!ImGui::ItemAdd(bb, id))
		return status;
	bool hovered, held;
	bool released = ImGui::ButtonBehavior(bb, id, &hovered, &held);
	bool amActive = g.ActiveId == window->DC.LastItemId;
	if (released)
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
				adjust = g.IO.MouseDelta.x * _vSpeed;
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

#endif // EDITOR
