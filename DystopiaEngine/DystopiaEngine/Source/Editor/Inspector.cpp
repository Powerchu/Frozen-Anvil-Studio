/* HEADER *********************************************************************************/
/*!
\file	Inspector.cpp
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
#include "Editor/EGUI.h"
#include "Editor/Inspector.h"
#include "Editor/ScriptFormatter.h"

#include "Editor/EditorMain.h"
#include "Editor/RuntimeMeta.h"
#include "Editor/EditorClipboard.h"
#include "Editor/EditorFactory.h"
#include "Editor/EditorCommands.h"
#include "Editor/ProjectResource.h"

#include "System/Scene/SceneSystem.h"
#include "System/Scene/Scene.h"
#include "System/Driver/Driver.h"
#include "System/Tag/TagSystem.h"
#include "Object/GameObject.h"


#include "Reflection/ReadWriteObject.h"
#include "Reflection/ReflectionTypeErasure.h"

#include <iostream>


static const std::string g_bPopup = "Behaviour List";
static const std::string g_cPopup = "Component List";
static const std::string g_nPopup = "New Behaviour Name";
static const std::string g_tPopip = "Tag List";


namespace Editor
{
	using namespace Dystopia;
	//static std::string g_arr[3] = { "item1", "item2", "item3" };
	static std::string g_arr2[3] = { "Invalid", "World", "UI" };

	Inspector::Inspector(void)
		:
		mpFocus{ nullptr }, mLabel{ "Inspector" }, mShowListOfComponents{ false },
		mpBehaviourSys{ nullptr }, mPromptNewBehaviour{ false }, mPromptCreateBehaviour{ false },
		mBufferInput{}, mBufferCreator{}, mBufferLogin{}, mnConfirmations{ 0 }
	{
		EditorPanel::SetOpened(true);
	}

	Inspector::~Inspector(void)
	{}

	void Inspector::Load(void)
	{}

	bool Inspector::Init(void)
	{
		mpBehaviourSys = Dystopia::EngineCore::GetInstance()->GetSystem<BehaviourSystem>();
		return true;
	}

	void Inspector::Update(float)
	{}

	void Inspector::EditorUI(void)
	{
		const auto& allSelectedIDs = EditorMain::GetInstance()->GetSystem<EditorClipboard>()->GetSelectedIDs();
		if (allSelectedIDs.size() == 1)
		{
			auto& scene = Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::SceneSystem>()->GetCurrentScene();
			mpFocus = scene.FindGameObject(*allSelectedIDs.begin());
		}
		else
			mpFocus = nullptr;

		if (mpFocus)
		{
			static constexpr Math::Vec2 btnSize{ 270, 20 };
			const float mid = Size().x / 2;
			const float inde = mid - (btnSize.x / 2);

			GameObjectDetails();
			GameObjectComponents();
			EGUI::Display::HorizontalSeparator();
			EGUI::Indent(inde);
			AddComponentButton(btnSize);
			AddBehaviourButton(btnSize);
			AddTagButton(btnSize);
			EGUI::UnIndent(inde);
		}
		else
		{
			//PrefabObject();
		}
	}

	void Inspector::Shutdown(void)
	{
		mpFocus = nullptr;
	}

	void Inspector::Message(eEMessage)
	{}

	void Inspector::SaveSettings(Dystopia::TextSerialiser&) const
	{}

	void Inspector::LoadSettings(Dystopia::TextSerialiser&)
	{}

	HashString Inspector::GetLabel(void) const
	{
		return mLabel;
	}

	void Inspector::GameObjectDetails()
	{
		static int i = 0;
		static int j = 0;

		static char buffer[MAX_SEARCH * 2] = "";
		std::string name = mpFocus->GetName().c_str();
		strcpy_s(buffer, name.c_str());

		EGUI::Display::Dummy(0.f, 5.f);
		EGUI::Display::IconGameObj("GameObjIcon", 50, 50);
		EGUI::SameLine(10);
		if (EGUI::StartChild("InfoArea", Math::Vec2{ Size().x - 60, 90 }, false))
		{
			auto activeState = mpFocus->IsActive();
			if (EGUI::Display::CheckBox("Active", &activeState, false))
			{
				mpFocus->SetActive(activeState);
			};

			EGUI::SameLine(5, 0);
			if (EGUI::Display::TextField("##gobjName", buffer, MAX_SEARCH, false, 190.f) && strlen(buffer))
			{
				auto cmd = EditorMain::GetInstance()->GetSystem<EditorCommands>();
				const auto oFn = cmd->MakeFnCommand(&Dystopia::GameObject::SetName, mpFocus->GetName());
				const auto nFn = cmd->MakeFnCommand(&Dystopia::GameObject::SetName, HashString{ static_cast<const char*>(buffer) });
				cmd->FunctionCommand(mpFocus->GetID(), oFn, nFn);
			}
			auto arr = mpFocus->GetAllTags_str();
			EGUI::PushLeftAlign(36.f);
			if (EGUI::Display::DropDownSelection("Tag", i, arr, 175.f))
			{

			}
			//EGUI::SameLine();
			//EGUI::ChangeAlignmentYOffset(0);
			j = (mpFocus->GetFlags() & FLAG_LAYER_WORLD) ? 1 : (mpFocus->GetFlags() & FLAG_LAYER_UI) ? 2 : 0;
			if (EGUI::Display::DropDownSelection("Layer", j, g_arr2, 175.f))
			{
				switch (j)
				{
				case 1:
					mpFocus->RemoveFlags(FLAG_LAYER_UI);
					mpFocus->SetFlag(FLAG_LAYER_WORLD);
					break;
				case 2:
					mpFocus->RemoveFlags(FLAG_LAYER_WORLD);
					mpFocus->SetFlag(FLAG_LAYER_UI);
					break;
				}
			}
			//EGUI::ChangeAlignmentYOffset();
			EGUI::PopLeftAlign();
		}
		EGUI::EndChild();
	}

	void Inspector::GameObjectComponents()
	{
		EGUI::Display::HorizontalSeparator();

		Dystopia::Transform& tempTransform = *mpFocus->GetComponent<Dystopia::Transform>();
		HashString uID{ tempTransform.GetEditorName().c_str() };
		//uID += "##";
		//uID += mpFocus->GetID();
		EGUI::Display::Dummy(0.f, 25.f);
		EGUI::SameLine(0, 0);
		EGUI::ChangeAlignmentYOffset(5);
		if (EGUI::Display::StartTreeNode(uID.c_str(), nullptr, false, false, true, true))
		{
			tempTransform.EditorUI();
			EGUI::Display::EndTreeNode();
		}
		EGUI::ChangeAlignmentYOffset();

		auto& arrComp = mpFocus->GetAllComponents();
		for (unsigned int i = 0; i < arrComp.size(); ++i)
		{
			auto activeState = arrComp[i]->IsActive();
			EGUI::PushID(i + static_cast<int>(arrComp.size()));
			EGUI::Display::Dummy(4.f, 2.f);
			EGUI::Display::HorizontalSeparator();

			if (EGUI::Display::CheckBox("comActive", &activeState, false))
			{
				arrComp[i]->SetActive(activeState);
			};

			HashString uID2{ arrComp[i]->GetEditorName().c_str() };
			//uID2 += "##";
			//uID2 += mpFocus->GetID();
			EGUI::SameLine(5, 0);
			bool open = EGUI::Display::StartTreeNode(uID2.c_str(), nullptr, false, false, true, true);
			bool show = !RemoveComponent(arrComp[i]);

			if (open)
			{
				if (show)
					arrComp[i]->EditorUI();
				EGUI::Display::EndTreeNode();
			}

			EGUI::PopID();
		}

		auto& arrBehav = mpFocus->GetAllBehaviours();
		for (unsigned int k = 0; k < arrBehav.size(); ++k)
		{
			auto c = arrBehav[k];
			EGUI::PushID(k + static_cast<int>(arrBehav.size()));
			EGUI::Display::Dummy(4.f, 2.f);
			EGUI::Display::HorizontalSeparator();

			if (!c) continue;
			auto activeState = c->IsActive();

			if (EGUI::Display::CheckBox("behavActive", &activeState, false))
			{
				c->SetActive(activeState);
			};

			HashString uID3{ c->GetBehaviourName() };
			//uID3 += "##";
			//uID3 += mpFocus->GetID();
			EGUI::SameLine(5, 0);
			bool open = EGUI::Display::StartTreeNode(uID3.c_str(), nullptr, false, false, true, true);
			bool show = !RemoveComponent(c);
			if (open)
			{
				if (show)
				{
					auto && MetaData = c->GetMetaData();
					if (MetaData)
					{
						auto Allnames = MetaData.GetAllNames();
						for (auto i : Allnames)
						{
							if (MetaData[i])
								MetaData[i].Reflect(i, c, Dystopia::SuperReflectFunctor{});
						}
					}
				}
				EGUI::Display::EndTreeNode();
			}
			EGUI::PopID();
		}
	}

	void Inspector::AddComponentButton(const Math::Vec2& _btnSize)
	{
		if (EGUI::Display::Button("Add Component", _btnSize))
		{
			EGUI::Display::OpenPopup(g_cPopup.c_str(), false);
		}
		ComponentsDropDownList();
	}

	void Inspector::ComponentsDropDownList()
	{
		static Dystopia::ComponentList availableComp;
		static constexpr size_t numComponents = Ut::SizeofList<Dystopia::UsableComponents>::value;
		static Array<std::string, numComponents> arr;
		Dystopia::MakeArrayOfNames<std::make_index_sequence<numComponents>, Dystopia::UsableComponents>::Make(arr);

		if (EGUI::Display::StartPopup(g_cPopup.c_str()))
		{
			EGUI::Display::Dummy(235, 2);
			for (unsigned int i = 0; i < numComponents; ++i)
			{
				// If Renderer, skip
				if (i == 1) continue;

				const auto& e = arr[i];
				if (EGUI::Display::SelectableTxt(e.c_str(), false))
				{
					//Dystopia::Component* pComp = availableComp.GetComponent(i, mpFocus);
					//mpFocus->AddComponent(pComp, typename Dystopia::Component::TAG{});
					availableComp.GetComponent(i, mpFocus);
					//EditorMain::GetInstance()->GetSystem<EditorCommands>()->AddComponent(mpFocus->GetID(), availableComp.GetComponent(i, mpFocus));
				}
			}
			EGUI::Display::EndPopup();
		}
	}

	void Inspector::AddBehaviourButton(const Math::Vec2& _btnSize)
	{
		if (EGUI::Display::Button("Add Behaviour", _btnSize))
		{
			EGUI::Display::OpenPopup(g_bPopup.c_str(), false);
		}
		BehaviourDropDownList();
	}

	void Inspector::BehaviourDropDownList()
	{
		if (EGUI::Display::StartPopup(g_bPopup.c_str()))
		{
			EGUI::Display::Dummy(235, 2);

			auto& list = mpBehaviourSys->GetAllBehaviour();
			for (auto& elem : list)
			{
				if (EGUI::Display::SelectableTxt(elem.mName.c_str()))
				{
					auto ptr = mpBehaviourSys->RequestBehaviour(mpFocus->GetID(), elem.mName);
					if (ptr)
					{
						mpFocus->AddComponent(ptr, Dystopia::BehaviourTag{});
						ptr->SetActive(mpFocus->IsActive());
					}
				}
			}

			if (EGUI::Display::SelectableTxt("New Behaviour"))
				mPromptNewBehaviour = true;

			EGUI::Display::EndPopup();
		}
		PromptCreateBehaviour();
	}

	void Inspector::PromptCreateBehaviour()
	{
		if (mPromptNewBehaviour)
		{
			EGUI::Display::OpenPopup(g_nPopup.c_str());
			mPromptNewBehaviour = false;
		}
		if (EGUI::Display::StartPopupModal(g_nPopup.c_str(), "Creating New Behaviour"))
		{
			EGUI::Display::TextField("File Name :", mBufferInput, MAX_BUFFER_SIZE);
			EGUI::Display::TextField("Creator   :", mBufferCreator, MAX_BUFFER_SIZE);
			EGUI::Display::TextField("Login ID  :", mBufferLogin, MAX_BUFFER_SIZE);

			mPromptCreateBehaviour = ((mBufferInput[0] != '\0') && 
									  (mBufferCreator[0] != '\0') && 
									  (mBufferLogin[0] != '\0')) ? true : false;

			if (EGUI::Display::Button("Close"))
			{
				EGUI::Display::CloseCurrentPopup();
				ResetBehaviourCreation();
			}

			if (mPromptCreateBehaviour) ConfirmCreateBehaviour();

			EGUI::Display::EndPopup();
		}
	}

	void Inspector::ConfirmCreateBehaviour()
	{
		EGUI::SameLine(230);
		if (EGUI::Display::Button("Create"))
		{
			EGUI::Display::CloseCurrentPopup();
			GenerateScript(std::string{ mBufferInput }, 
						   std::string{ mBufferCreator }, 
						   std::string{ mBufferLogin });
			ResetBehaviourCreation();
		}
	}

	void Inspector::ResetBehaviourCreation()
	{
		mPromptCreateBehaviour = false;
		mBufferInput[0] = mBufferCreator[0] = mBufferLogin[0] = '\0';
	}

	bool Inspector::RemoveComponent(Dystopia::Component* _pCom)
	{
		bool ret = false;
		static Dystopia::ComponentList availableComp;
		if (ImGui::BeginPopupContextItem())
		{
			if (EGUI::Display::SelectableTxt("Remove"))
			{
				//mpFocus->RemoveComponent(_pCom);
				//EditorMain::GetInstance()->GetSystem<EditorCommands>()->RemoveComponent(mpFocus->GetID(), _pCom);
				availableComp.RemoveComponentCommand(_pCom->GetRealComponentType(), mpFocus);
				ret = true;
			}
			ImGui::EndPopup();
		}
		return ret;
	}

	bool Inspector::RemoveComponent(Dystopia::Behaviour* _pCom)
	{
		bool ret = false;
		if (ImGui::BeginPopupContextItem())
		{
			if (EGUI::Display::SelectableTxt("Remove"))
			{
				auto owner = _pCom->GetOwner();
				owner->RemoveComponent(_pCom);
				ret = true;
			}
			ImGui::EndPopup();
		}
		return ret;
	}

	void Inspector::AddTagButton(const Math::Vec2& _btnSize)
	{
		if (EGUI::Display::Button("Add Tag", _btnSize))
		{
			EGUI::Display::OpenPopup(g_tPopip.c_str(), false);
		}
		TagsDropDownList();
	}

	void Inspector::TagsDropDownList()
	{
		if (EGUI::Display::StartPopup(g_tPopip.c_str()))
		{
			EGUI::Display::Dummy(235, 2);

			auto && list = Dystopia::EngineCore::GetInstance()->Get<Dystopia::TagSystem>()->GetAllTagsName();
			for (auto& elem : list)
			{
				if (EGUI::Display::SelectableTxt(elem.c_str()))
				{
					mpFocus->AddTag(elem);
				}
			}

			EGUI::Display::EndPopup();
		}
	}

	void Inspector::PrefabObject(void)
	{
		auto clip = EditorMain::GetInstance()->GetSystem<EditorClipboard>();
		auto pPrefab = clip->GetPrefab();
		if (pPrefab)
		{
			auto& objects = pPrefab->mArrObjects;
			auto& allInstances = pPrefab->mArrInstanced;
			auto& curScene = Dystopia::EngineCore::Get<Dystopia::SceneSystem>()->GetCurrentScene();

			EGUI::StartChild("##Archetype Editor", Math::Vec2{ Size().x - 60, 100 }, false);
			EGUI::Display::Label("Has childrens:   %s", objects.size() > 1 ? "True" : "False");
			EGUI::Display::Label("No. of Instance: %d", allInstances.size());
			for (size_t i = 0; i < allInstances.size(); i++)
			{
				auto& instArr = allInstances[i];
				for (auto& inst : instArr)
				{
					auto instObj = curScene.FindGameObject(inst);
					if (instObj)
						EGUI::Display::Label("%d] %s", i, instObj->GetName().c_str());
				}
			}
			EGUI::EndChild();

			if (allInstances.size())
			{
				EGUI::Display::HorizontalSeparator();
				if (EGUI::Display::Button("Apply to all", Math::Vec2{ 100, 24 }))
				{
					mnConfirmations = 2;
				}
				EGUI::SameLine();
				if (EGUI::Display::Button("Detach all", Math::Vec2{ 100, 24 }))
				{
					mnConfirmations = 1;
				}
			}

			EGUI::Display::HorizontalSeparator();

			EGUI::Display::IconGameObj("pGameObjIcon", 50, 50);
			EGUI::SameLine(10);
			if (EGUI::StartChild("pInfoArea", Math::Vec2{ Size().x - 60, 60 }, false))
			{
				EGUI::SameLine();
				EGUI::Display::Label("PREFAB %s", pPrefab->mPrefabFile.c_str());
				auto arr = objects[0]->GetAllTags_str();
				int selected = 0;
				EGUI::Display::DropDownSelection("Tag", selected, arr, 50);

				EGUI::SameLine();
				EGUI::ChangeAlignmentYOffset(0);
				int j = (objects[0]->GetFlags() & FLAG_LAYER_WORLD) ? 1 : (objects[0]->GetFlags() & FLAG_LAYER_UI) ? 2 : 0;
				if (EGUI::Display::DropDownSelection("pLayer", j, g_arr2, 80))
				{
					switch (j)
					{
					case 1:
						objects[0]->RemoveFlags(FLAG_LAYER_UI);
						objects[0]->SetFlag(FLAG_LAYER_WORLD);
						break;
					case 2:
						objects[0]->RemoveFlags(FLAG_LAYER_WORLD);
						objects[0]->SetFlag(FLAG_LAYER_UI);
					}
				}
				EGUI::ChangeAlignmentYOffset();
			}
			EGUI::EndChild();

			EGUI::Display::HorizontalSeparator();

			Dystopia::Transform& tempTransform = *objects[0]->GetComponent<Dystopia::Transform>();
			if (EGUI::Display::StartTreeNode(tempTransform.GetEditorName().c_str(), nullptr, false, false, true, true))
			{
				tempTransform.EditorUI();
				EGUI::Display::EndTreeNode();
			}

			auto& arrComp = objects[0]->GetAllComponents();
			for (unsigned int i = 0; i < arrComp.size(); ++i)
			{
				EGUI::PushID(i);
				EGUI::Display::HorizontalSeparator();
				bool open = EGUI::Display::StartTreeNode(arrComp[i]->GetEditorName().c_str(), nullptr, false, false, true, true);
				bool show = true;//!RemoveComponent(arrComp[i]);
				if (open)
				{
					if (show)
						arrComp[i]->EditorUI();
					EGUI::Display::EndTreeNode();
				}
				EGUI::PopID();
			}

			auto& arrBehav = objects[0]->GetAllBehaviours();
			for (auto & c : arrBehav)
			{
				EGUI::Display::HorizontalSeparator();
				if (!c)
					continue;
				bool open = EGUI::Display::StartTreeNode(c->GetBehaviourName(), nullptr, false, false, true, true);
				bool show = true;//!RemoveComponent(c);
				if (open)
				{
					if (show)
					{
						auto && MetaData = c->GetMetaData();
						if (MetaData)
						{
							auto Allnames = MetaData.GetAllNames();
							for (auto i : Allnames)
							{
								if (MetaData[i])
									MetaData[i].Reflect(i, c, Dystopia::SuperReflectFunctor{});
							}
						}
					}
					EGUI::Display::EndTreeNode();
				}
			}

			Confirmations(static_cast<void*>(pPrefab));
		}
	}

	void Inspector::Confirmations(void* _gPtr)
	{
		switch (mnConfirmations)
		{
		case 1:
			EGUI::Display::OpenPopup("Confirm Detach");
			if (EGUI::Display::StartPopupModal("Confirm Detach", "WARNING!"))
			{
				EGUI::Display::Label("CHANGES ARE PERMANENT!");
				if (ImGui::Button("DETACH", ImVec2{ 100, 40 }))
				{
					static_cast<EditorFactory::PrefabData*>(_gPtr)->mArrInstanced.clear();
					EGUI::Display::CloseCurrentPopup();
					mnConfirmations = 0;
				}
				ImGui::SameLine();
				if (ImGui::Button("Cancel", ImVec2{ 100, 40 }))
				{
					EGUI::Display::CloseCurrentPopup();
					mnConfirmations = 0;
				}
				EGUI::Display::EndPopup();
			}
			break;

		case 2:
			EGUI::Display::OpenPopup("Confirm Apply");
			if (EGUI::Display::StartPopupModal("Confirm Apply", "WARNING!"))
			{
				EGUI::Display::Label("CHANGES ARE PERMANENT!");
				if (ImGui::Button("APPLY", ImVec2{ 100, 40 }))
				{
					EditorMain::GetInstance()->GetSystem<EditorFactory>()->ApplyChanges(static_cast<EditorFactory::PrefabData*>(_gPtr));
					EGUI::Display::CloseCurrentPopup();
					mnConfirmations = 0;
				}
				ImGui::SameLine();
				if (ImGui::Button("Cancel", ImVec2{ 100, 40 }))
				{
					EGUI::Display::CloseCurrentPopup();
					mnConfirmations = 0;
				}
				EGUI::Display::EndPopup();
			}
			break;
		default:
			break;
		}
	}
}

#endif // EDITOR 





