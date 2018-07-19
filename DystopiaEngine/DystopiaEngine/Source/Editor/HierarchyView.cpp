/* HEADER *********************************************************************************/
/*!
\file	HierarchyView.cpp
\author Digipen (100%)
\par    email: t.shannon\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#include "Editor\EGUI.h"
#include "Editor\HierarchyView.h"
#include "Object\GameObject.h"

constexpr float DEFAULT_WIDTH = 300;
constexpr float DEFAULT_HEIGHT = 300;

namespace Dystopia
{
	static HierarchyView* gpInstance = 0;
	HierarchyView* HierarchyView::GetInstance()
	{
		if (gpInstance) return gpInstance;

		gpInstance = new HierarchyView{};
		return gpInstance;
	}

	HierarchyView::HierarchyView()
		: EditorTab{ true }, 
		mLabel{ "Hierarchy" }, mpFocusGameObj{ nullptr }, mpCurrentScene{ nullptr }
	{
	}

	HierarchyView::~HierarchyView()
	{
	}

	void HierarchyView::Init()
	{
	}

	void HierarchyView::Update(const float& _dt)
	{
		_dt;
	}

	void HierarchyView::Shutdown()
	{
	}

	std::string HierarchyView::GetLabel() const
	{
		return mLabel;
	}

	void HierarchyView::Window()
	{
		ImDrawList* draw_list = ImGui::GetWindowDrawList();

		// Primitives
		static float thickness = 2.0f;
		const ImU32 col32 = ImColor(ImVec4(1.0f, 1.0f, 0.4f, 1.0f));
		ImVec2 pos = ImGui::GetCursorScreenPos();
		pos.x += 4;
		pos.y += 4;
		draw_list->AddBezierCurve(ImVec2{ pos.x, pos.y + 50.f },
								  ImVec2{ pos.x + 30.f, pos.y + 150.f },
								  ImVec2{ pos.x + 60, pos.y + 100.f},
								  ImVec2{ pos.x + 100.f, pos.y}, col32, thickness);

		if (!mpCurrentScene) return;

		// Do for all objects in the scene
		// Dystopia::EGUI::Display::Label(mpFocusGameObj->GetName().c_str()); 
	}

}


#endif // EDITOR


