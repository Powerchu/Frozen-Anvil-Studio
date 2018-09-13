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
#include "Editor\EditorEvents.h"
#include "Editor\Editor.h"
#include "Object\GameObject.h"
#include "System\Scene\Scene.h"
#include "System\Camera\CameraSystem.h"
#include "System\Driver\Driver.h"
#include "Utility\GUID.h"
#include "Component\Camera.h"
#include <algorithm>
#include <cctype>
#include <iostream>

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
		mLabel{ "Hierarchy" }, mpFocus{ nullptr }, mSearchText{ "" }, 
		mPopupID{ "CreateGameObjFromHierarchy" }, 
		mSearchTextPrevFrame{ "" }, mArrSearchID{}
	{
	}

	HierarchyView::~HierarchyView()
	{
		gpInstance = nullptr;
	}

	void HierarchyView::Init()
	{
		mArrSearchID.reserve(200);
	}

	void HierarchyView::Update(const float& _dt)
	{
		_dt;

		UpdateSearch();
	}

	void HierarchyView::Shutdown()
	{
		mArrSearchID.clear();
	}

	std::string HierarchyView::GetLabel() const
	{
		return mLabel;
	}

	void HierarchyView::EditorUI()
	{
		CreateButton();
		EGUI::ChangeAlignmentYOffset(0);
		EGUI::SameLine();
		EGUI::ChangeAlignmentYOffset();
		SearchBar();

		if (EGUI::StartChild("ItemsInScene", Math::Vec2{ Size().x - 5, Size().y - 55 }))
		{
			auto& arrayOfGameObjects = GetCurrentScene()->GetAllGameObjects();
			for (auto& obj : arrayOfGameObjects)
			{
				if (strlen(mSearchText))
				{
					for (auto& i : mArrSearchID)
					{
						if (!(obj.GetID() == i)) 
							continue;
				
						GameObjectName(obj);
						break;
					}
				}
				else
					GameObjectName(obj);
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

	void HierarchyView::UpdateSearch()
	{
		std::string toBeSearched{ mSearchText };
		if (toBeSearched != std::string{ mSearchTextPrevFrame })
		{
			mArrSearchID.clear();
			for (auto& e : GetCurrentScene()->GetAllGameObjects())
			{
				std::string item{ e.GetName() };
				auto it = std::search(item.begin(), item.end(), toBeSearched.begin(), toBeSearched.end(),
									  [](char c1, char c2) { return std::toupper(c1) == std::toupper(c2); });
				if (it == item.begin())
				{
					mArrSearchID.push_back(e.GetID());
				}
			}
			strcpy_s(mSearchTextPrevFrame, mSearchText);
		}
		else if (!strlen(mSearchText))
		{
			mArrSearchID.clear();
		}
	}

	void HierarchyView::CreatePopup()
	{
		if (EGUI::Display::StartPopup(mPopupID))
		{
			if (EGUI::Display::SelectableTxt("New GameObject"))
			{
				strcpy_s(mSearchTextPrevFrame, "");
				GameObject *pObject = GetCurrentScene()->InsertGameObject(GUIDGenerator::GetUniqueID());
				pObject->SetName("GameObject");
				pObject->SetActive(true);
				pObject->Init();
			}

			if (EGUI::Display::SelectableTxt("New Camera"))
			{
				strcpy_s(mSearchTextPrevFrame, "");
				GameObject *pObject = GetCurrentScene()->InsertGameObject(GUIDGenerator::GetUniqueID());
				pObject->SetName("Camera");
				auto p = EngineCore::GetInstance()->GetSystem<CameraSystem>()->RequestComponent();
				p->SetOwner(pObject);
				pObject->AddComponent(p, typename Camera::TAG{});
				pObject->SetActive(true);
				pObject->Init();
			}
			EGUI::Display::EndPopup();
		}
	}

	void HierarchyView::GameObjectName(GameObject& _obj)
	{
		std::string uniqueifyName = _obj.GetName() + "##" + std::to_string(_obj.GetID());
		bool highlighted = mpFocus && (mpFocus->GetID() == _obj.GetID());
		if (EGUI::Display::SelectableTxt(uniqueifyName, highlighted))
		{
			GetMainEditor().RemoveFocus();
			GetMainEditor().SetFocus(_obj);
		}
	}

	void HierarchyView::SetFocus(GameObject& _rObj)
	{
		mpFocus = &_rObj;
	}

	void HierarchyView::RemoveFocus()
	{
		mpFocus = nullptr;
	}
}


#endif // EDITOR


