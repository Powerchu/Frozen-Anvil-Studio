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
		mLabel{ "Hierarchy" }, mpFocusGameObj{ nullptr }, mpCurrentScene{ nullptr }, mSearchText{ "" }
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
		SearchBar();

		if (EGUI::StartChild("ItemsInScene", Math::Vec2{ Size().x - 5, Size().y - 55 }))
		{

		}
		EGUI::EndChild();

		if (!mpCurrentScene) return;

		// Do for all objects in the scene
		// Dystopia::EGUI::Display::Label(mpFocusGameObj->GetName().c_str()); 
	}

	void HierarchyView::CreateButton()
	{
		if (EGUI::Display::Button("Create", Math::Vec2{ 50, 18 }))
		{

		}
	}

	void HierarchyView::SearchBar()
	{
		EGUI::Indent(5);
		EGUI::ChangeLabelSpacing(10);
		EGUI::Display::TextField("Search", mSearchText, MAX_SEARCH, true);
		EGUI::ChangeLabelSpacing();
		EGUI::UnIndent(5);
		EGUI::Display::HorizontalSeparator();
	}
}


#endif // EDITOR


