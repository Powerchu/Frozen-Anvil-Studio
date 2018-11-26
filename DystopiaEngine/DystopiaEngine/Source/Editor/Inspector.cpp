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
#include "Editor/EditorCommands.h"

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
	static std::string g_arr[3] = { "item1", "item2", "item3" };
	static std::string g_arr2[3] = { "Invalid", "World", "UI" };

	Inspector::Inspector(void)
		: 
		mpFocus{ nullptr }, mLabel{ "Inspector" }, mShowListOfComponents{ false },
		mpBehaviourSys{ nullptr }, mPromptNewBehaviour{ false }, mPromptCreateBehaviour{ false },
		mBufferInput{}, mBufferCreator{}, mBufferLogin{}
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
		{
			mpFocus = nullptr;
			return;
		}
		if (!mpFocus) return;

		static constexpr Math::Vec2 btnSize{ 270, 20 };
		const float mid = Size().x / 2;
		float inde = mid - (btnSize.x / 2);
		
		GameObjectDetails();
		GameObjectComponents();
		EGUI::Display::HorizontalSeparator();
		EGUI::Indent(inde);
		AddComponentButton(btnSize);
		AddBehaviourButton(btnSize);
		AddTagButton(btnSize);
		EGUI::UnIndent(inde);
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

		char buffer[MAX_SEARCH * 2] = "";
		std::string name = mpFocus->GetName().c_str();
		strcpy_s(buffer, name.c_str());

		EGUI::Display::IconGameObj("GameObjIcon", 50, 50);
		EGUI::SameLine(10);
		if (EGUI::StartChild("InfoArea", Math::Vec2{ Size().x - 60, 60 }, false, Math::Vec4{ 0,0,0,0 }))
		{
			EGUI::SameLine();
			if (EGUI::Display::TextField("Name", buffer, MAX_SEARCH, false, 223.f) && strlen(buffer))
			{
				auto cmd = EditorMain::GetInstance()->GetSystem<EditorCommands>();
				auto oFn = cmd->MakeFnCommand(&Dystopia::GameObject::SetName, mpFocus->GetName());
				auto nFn = cmd->MakeFnCommand(&Dystopia::GameObject::SetName, HashString{ buffer });
				cmd->FunctionCommand(mpFocus->GetID(), oFn, nFn);
			}

			//TODO:
			if (EGUI::Display::DropDownSelection("Tag", i, mpFocus->GetAllTags_str(), 32))
			{

			}

			EGUI::SameLine();
			EGUI::ChangeAlignmentYOffset(0);
			j = (mpFocus->GetFlags() & FLAG_LAYER_WORLD) ? 1 : (mpFocus->GetFlags() & FLAG_LAYER_UI) ? 2 : 0;
			if (EGUI::Display::DropDownSelection("Layer", j, g_arr2, 80))
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
			EGUI::ChangeAlignmentYOffset();
		}
		EGUI::EndChild();
	}

	void Inspector::GameObjectComponents()
	{
		EGUI::Display::HorizontalSeparator();

		Transform& tempTransform = *mpFocus->GetComponent<Transform>();
		if (EGUI::Display::StartTreeNode(tempTransform.GetEditorName() + "##" +
			std::to_string(mpFocus->GetID()), nullptr, false, false, true, true))
		{
			tempTransform.EditorUI();
			EGUI::Display::EndTreeNode();
		}

		auto& arrComp = mpFocus->GetAllComponents();
		for (unsigned int i = 0; i < arrComp.size(); ++i)
		{
			EGUI::PushID(i);
			EGUI::Display::HorizontalSeparator();
			bool open = EGUI::Display::StartTreeNode(arrComp[i]->GetEditorName() + "##" + std::to_string(mpFocus->GetID()), nullptr, false, false, true, true);
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
		for (auto & c : arrBehav)
		{
			EGUI::Display::HorizontalSeparator();
			bool open = EGUI::Display::StartTreeNode(std::string{ c->GetBehaviourName() } +"##" + std::to_string(mpFocus->GetID()), nullptr, false, false, true, true);
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
		}
	}

	void Inspector::AddComponentButton(const Math::Vec2& _btnSize)
	{
		if (EGUI::Display::Button("Add Component", _btnSize))
		{
			EGUI::Display::OpenPopup(g_cPopup, false);
		}
		ComponentsDropDownList();
	}

	void Inspector::ComponentsDropDownList()
	{
		static Dystopia::ComponentList availableComp;
		static constexpr size_t numComponents = Ut::SizeofList<Dystopia::UsableComponents>::value;
		static Array<std::string, numComponents> arr;
		Dystopia::MakeArrayOfNames<std::make_index_sequence<numComponents>, Dystopia::UsableComponents>::Make(arr);

		if (EGUI::Display::StartPopup(g_cPopup))
		{
			EGUI::Display::Dummy(235, 2);
			for (unsigned int i = 0; i < numComponents; ++i)
			{
				const auto& e = arr[i];
				if (EGUI::Display::SelectableTxt(e, false))
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
			EGUI::Display::OpenPopup(g_bPopup, false);
		}
		BehaviourDropDownList();
	}

	void Inspector::BehaviourDropDownList()
	{
		if (EGUI::Display::StartPopup(g_bPopup))
		{
			EGUI::Display::Dummy(235, 2);

			auto& list = mpBehaviourSys->GetAllBehaviour();
			for (auto& elem : list)
			{
				if (EGUI::Display::SelectableTxt(elem.mName))
				{
					auto ptr = mpBehaviourSys->RequestBehaviour(mpFocus->GetID(), elem.mName);
					if (ptr) mpFocus->AddComponent(ptr, Dystopia::BehaviourTag{});
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
			EGUI::Display::OpenPopup(g_nPopup);
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

	void Inspector::AddTagButton(const Math::Vec2& _btnSize)
	{
		if (EGUI::Display::Button("Add Tag", _btnSize))
		{
			EGUI::Display::OpenPopup(g_tPopip, false);
		}
		TagsDropDownList();
	}

	void Inspector::TagsDropDownList()
	{
		if (EGUI::Display::StartPopup(g_tPopip))
		{
			EGUI::Display::Dummy(235, 2);

			auto && list = Dystopia::EngineCore::GetInstance()->Get<Dystopia::TagSystem>()->GetAllTagsName();
			for (auto& elem : list)
			{
				if (EGUI::Display::SelectableTxt(elem))
				{
					mpFocus->AddTag(elem);
				}
			}

			EGUI::Display::EndPopup();
		}
	}
}

#endif // EDITOR 


