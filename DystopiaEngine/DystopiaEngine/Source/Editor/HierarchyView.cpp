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
#include "System\Scene\Scene.h"

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
		mLabel{ "Hierarchy" }, mpFocusGameObj{ nullptr }, mSearchText{ "" },
		mPopupID{ "Create Objects From Hierarchy" }
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
		CreateButton();
		EGUI::ChangeAlignmentYOffset(0);
		EGUI::SameLine();
		SearchBar();
		EGUI::ChangeAlignmentYOffset();

		if (EGUI::StartChild("ItemsInScene", Math::Vec2{ Size().x - 5, Size().y - 55 }))
		{
			for (const auto& obj : mpCurrentScene->mGameObjs)
			{
				if (EGUI::Display::SelectableTxt(obj.GetName().c_str()))
				{

				}
			}
		}
		EGUI::EndChild();
	}

	void HierarchyView::CreateButton()
	{
		EGUI::Indent(5);
		if (EGUI::Display::Button("Create", Math::Vec2{ 50, 18 }))
		{
			EGUI::Display::OpenPopup(mPopupID);
		}
		EGUI::UnIndent(5);
		CreatePopup();
	}

	void HierarchyView::SearchBar()
	{
		float width = Size().x - 70 - 55;
		width = (width < 20) ? 20 : width;
		EGUI::ChangeLabelSpacing(10);
		EGUI::Display::TextField("Search", mSearchText, MAX_SEARCH, true, width);
		EGUI::ChangeLabelSpacing();
		EGUI::Display::HorizontalSeparator();
	}

	void HierarchyView::CreatePopup()
	{
		const static std::string creatable[5] = { "obj1", "obj2", "obj3", "obj4", "obj5" };
		if (EGUI::Display::StartPopup(mPopupID))
		{
			// for loop everything that can be created
			for (const auto& e : creatable)
			{
				if (EGUI::Display::SelectableTxt(e))
				{
					//GameObject *pObject = mpCurrentScene->InsertGameObject();
					//pObject->SetName(e);
				}
			}
			EGUI::Display::EndPopup();
		}
	}
}


#endif // EDITOR


