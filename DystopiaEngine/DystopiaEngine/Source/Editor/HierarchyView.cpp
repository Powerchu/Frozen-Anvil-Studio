/* HEADER *********************************************************************************/
/*!
\file	HierarchyView.cpp
\author Digipen (100%)
\par    email: t.shannon\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#include "Editor/EGUI.h"
#include "Editor/HierarchyView.h"
#include "Editor/EditorEvents.h"
#include "Editor/Editor.h"
#include "Editor/DefaultFactory.h"
#include "Editor/Payloads.h"

#include "Object/GameObject.h"
#include "Component/Camera.h"
#include "Component/Transform.h"

#include "System/Scene/Scene.h"
#include "System/Camera/CameraSystem.h"
#include "System/Driver/Driver.h"

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
		: EditorTab{ false },
		mLabel{ "Hierarchy" }, 
		mSearchText{ "" }, 
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
			ShowGameObjects();
			//auto& arrayOfGameObjects = GetCurrentScene()->GetAllGameObjects();
			//for (auto& obj : arrayOfGameObjects)
			//{
			//	const auto& selections = GetMainEditor()->GetSelectionObjects();
			//	bool highlight = false;
			//	for (auto& elem : selections)
			//	{
			//		if (elem == &obj)
			//		{
			//			highlight = true;
			//			break;
			//		}
			//	}
			//	if (strlen(mSearchText))
			//	{
			//		for (auto& i : mArrSearchID)
			//		{
			//			if (!(obj.GetID() == i)) 
			//				continue;
			//			GameObjectName(obj, highlight);
			//			break;
			//		}
			//	}
			//	else
			//		GameObjectName(obj, highlight);
			//}
		}
		EGUI::EndChild();
	}

	void HierarchyView::CreateButton()
	{
		EGUI::Indent(5);
		if (EGUI::Display::Button("Create", Math::Vec2{ 50, 22 }))
		{
			EGUI::Display::OpenPopup(mPopupID);
		}
		EGUI::UnIndent(5);
		CreatePopup();
	}

	void HierarchyView::SearchBar()
	{
		float width = Size().x - 70;
		width = (width < 20) ? 20 : width;
		EGUI::ChangeLabelSpacing(10);
		EGUI::Display::TextField("Search", mSearchText, MAX_SEARCH, false, width);
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
			if (EGUI::Display::SelectableTxt("GameObject"))
			{
				strcpy_s(mSearchTextPrevFrame, "");
				GameObject* p = Factory::CreateGameObj("GameObject");
				p->SetFlag(eObjFlag::FLAG_LAYER_WORLD);
				GetCommandHND()->InvokeCommandInsert(*p, *GetCurrentScene());
			}

			if (EGUI::Display::SelectableTxt("Camera"))
			{
				//strcpy_s(mSearchTextPrevFrame, "");
				//GameObject* p = Factory::CreateCamera("Camera");
				//GetCommandHND()->InvokeCommand(new ComdInsertObject{ p, GetCurrentScene() });
			}
			EGUI::Display::EndPopup();
		}
	}

	void HierarchyView::SelectedObj(GameObject& _obj)
	{
		auto ed = GetMainEditor();
		bool exist = false;
		for (const auto& id : ed->GetSelectionObjects())
		{
			if (id->GetID() == _obj.GetID())
			{
				ed->RemoveSelection(_obj.GetID());
				exist = true;
				break;
			}
		}
		if (!exist)
			ed->AddSelection(_obj.GetID());
	}

	void HierarchyView::GameObjectPopups(GameObject& _obj)
	{
		if (ImGui::BeginPopupContextItem())
		{
			GetMainEditor()->NewSelection(_obj.GetID());
			if (EGUI::Display::SelectableTxt("Duplicate"))
			{
				GetMainEditor()->EditorCopy();
				GetMainEditor()->EditorPaste();
			}
			if (EGUI::Display::SelectableTxt("Delete"))
			{
				GetCommandHND()->InvokeCommandDelete(_obj, *GetCurrentScene());
			}
			ImGui::EndPopup();
		}
		uint64_t id = _obj.GetID();
		if (EGUI::Display::StartPayload(EGUI::ePayloadTags::GAME_OBJ, &id, sizeof(uint64_t), _obj.GetName()))
		{
			EGUI::Display::EndPayload();
		}
	}
	
	void HierarchyView::ShowGameObjects(void)
	{
		auto& arrayOfGameObjects = GetCurrentScene()->GetAllGameObjects();
		const auto& selections = GetMainEditor()->GetSelectionObjects();
		for (auto& obj : arrayOfGameObjects)
		{
			if (obj.GetComponent<Transform>()->GetParent())
				continue;

			if (obj.GetComponent<Transform>()->GetAllChild().size())
				ShowAsParent(obj, selections);
			else
				ShowAsChild(obj, selections);
		}

		EGUI::Display::Dummy(Size().x, 50.f);
		if (uint64_t *id = EGUI::Display::StartPayloadReceiver<uint64_t>(EGUI::GAME_OBJ))
		{
			GameObject *t = GetCurrentScene()->FindGameObject(*id);

			auto fOld = EGUI::GetCommandHND()->Make_FunctionModWrapper(&Dystopia::Transform::SetParent, t->GetComponent<Transform>()->GetParent());
			auto fNew = EGUI::GetCommandHND()->Make_FunctionModWrapper(&Dystopia::Transform::SetParent, nullptr);
			EGUI::GetCommandHND()->InvokeCommand(t->GetID(), fOld, fNew);

			EGUI::Display::EndPayloadReceiver();
		}
	}

	void HierarchyView::ShowAsParent(GameObject& _obj, const AutoArray<GameObject*>& _arr)
	{
		bool selected = false;
		bool clicked = false;
		for (auto& o : _arr)
		{
			if (_obj.GetID() == o->GetID())
			{
				selected = true;
				break;
			}
		}
		std::string uniqueifyName = _obj.GetName() + "##" + std::to_string(_obj.GetID());
		bool tree = EGUI::Display::StartTreeNode(uniqueifyName, &clicked, selected);

		uint64_t id = _obj.GetID();
		if (EGUI::Display::StartPayload(EGUI::ePayloadTags::GAME_OBJ, &id, sizeof(uint64_t), _obj.GetName()))
		{
			EGUI::Display::EndPayload();
		}
		if (uint64_t *id2 = EGUI::Display::StartPayloadReceiver<uint64_t>(EGUI::GAME_OBJ))
		{
			GameObject *t = GetCurrentScene()->FindGameObject(*id2);

			auto fOld = EGUI::GetCommandHND()->Make_FunctionModWrapper(&Dystopia::Transform::SetParent, t->GetComponent<Transform>()->GetParent());
			auto fNew = EGUI::GetCommandHND()->Make_FunctionModWrapper(&Dystopia::Transform::SetParent, _obj.GetComponent<Transform>());
			EGUI::GetCommandHND()->InvokeCommand(t->GetID(), fOld, fNew);

			EGUI::Display::EndPayloadReceiver();
		}
		GameObjectPopups(_obj);

		if (tree)
		{
			if (clicked) 
				SelectedObj(_obj);

			auto& arrChild = _obj.GetComponent<Transform>()->GetAllChild();
			for (auto& c : arrChild)
			{
				GameObject *childObj = c->GetOwner();
				if (childObj)
				{
					EGUI::Indent(20);
					if (c->GetAllChild().size())
						ShowAsParent(*childObj, _arr);
					else
						ShowAsChild(*childObj, _arr);
					EGUI::UnIndent(20);
				}
			}
			EGUI::Display::EndTreeNode();
		}
		else if (clicked)
			SelectedObj(_obj);
	}

	void HierarchyView::ShowAsChild(GameObject& _obj, const AutoArray<GameObject*>& _arr)
	{
		bool selected = false;
		for (auto& o : _arr)
		{
			if (_obj.GetID() == o->GetID())
			{
				selected = true;
				break;
			}
		}
		std::string uniqueifyName = _obj.GetName() + "##" + std::to_string(_obj.GetID());
		if (EGUI::Display::SelectableTxt(uniqueifyName, selected))
		{
			SelectedObj(_obj);
		}
		uint64_t id = _obj.GetID();
		if (EGUI::Display::StartPayload(EGUI::ePayloadTags::GAME_OBJ, &id, sizeof(uint64_t), _obj.GetName()))
		{
			EGUI::Display::EndPayload();
		}
		if (uint64_t *id2 = EGUI::Display::StartPayloadReceiver<uint64_t>(EGUI::GAME_OBJ))
		{
			GameObject *t = GetCurrentScene()->FindGameObject(*id2);

			auto fOld = EGUI::GetCommandHND()->Make_FunctionModWrapper(&Dystopia::Transform::SetParent, t->GetComponent<Transform>()->GetParent());
			auto fNew = EGUI::GetCommandHND()->Make_FunctionModWrapper(&Dystopia::Transform::SetParent, _obj.GetComponent<Transform>());
			EGUI::GetCommandHND()->InvokeCommand(t->GetID(), fOld, fNew);

			EGUI::Display::EndPayloadReceiver();
		}
		GameObjectPopups(_obj);
	}
}


#endif // EDITOR


