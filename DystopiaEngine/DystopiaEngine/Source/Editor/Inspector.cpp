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
#include "Editor/Commands.h"
#include "Editor/EditorEvents.h"
#include "Editor/EditorMetaHelpers.h"

#include "Component/Camera.h"
#include "Component/Collider.h"
#include "Component/Renderer.h"
#include "Component/RigidBody.h"
#include "Component/CharacterController.h"

#include "System/Physics/PhysicsSystem.h"
#include "System/Graphics/GraphicsSystem.h"
#include "System/Camera/CameraSystem.h"
#include "System/Behaviour/BehaviourSystem.h"
#include "System/Collision/CollisionSystem.h"
#include "System/Input/InputSystem.h"

#include "Utility/ComponentGUID.h"
#include "Object/ObjectFlags.h"
#include "Object/GameObject.h"

#include <iostream>


static const std::string g_bPopup = "Behaviour List";
static const std::string g_cPopup = "Component List";
static const std::string g_nPopup = "New Behaviour Name";

namespace Dystopia
{
	static std::string g_arr[3] = { "item1", "item2", "item3" };
	static std::string g_arr2[3] = { "item4", "item5", "item6" };

	static Inspector* gpInstance = 0;
	Inspector* Inspector::GetInstance()
	{
		if (gpInstance) return gpInstance;

		gpInstance = new Inspector{};
		return gpInstance;
	}

	Inspector::Inspector()
		: EditorTab{ false },
		mpFocus{ nullptr }, mLabel{ "Inspector" }, mShowListOfComponents{ false },
		mpBehaviourSys{ nullptr }, mPromptNewBehaviour{ false }, mPromptCreateBehaviour{ false },
		mBufferInput{}, mBufferCreator{}, mBufferLogin{}
	{
	}

	Inspector::~Inspector()
	{
		gpInstance = nullptr;
	}

	void Inspector::Init()
	{
		mpBehaviourSys = EngineCore::GetInstance()->GetSystem<BehaviourSystem>();
	}

	void Inspector::Update(const float& _dt)
	{
		_dt;
	}

	void Inspector::EditorUI()
	{
		auto& allObj = GetMainEditor()->GetSelectionObjects();
		mpFocus = (allObj.size() == 1) ? *allObj.begin() : nullptr;
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
		EGUI::UnIndent(inde);
	}

	void Inspector::Shutdown()
	{
		mpFocus = nullptr;
	}

	void Inspector::GameObjectDetails()
	{
		static int i = 0;
		static int j = 0;

		char buffer[MAX_SEARCH * 2] = "";
		std::string name = mpFocus->GetName();
		strcpy_s(buffer, name.c_str());

		EGUI::Display::IconGameObj("GameObjIcon", 50, 50);
		EGUI::SameLine(10);
		if (EGUI::StartChild("InfoArea", Math::Vec2{ Size().x - 60, 50 }, false, Math::Vec4{ 0,0,0,0 }))
		{
			EGUI::SameLine();
			if (EGUI::Display::TextField("Name", buffer, MAX_SEARCH, false, 223.f) && strlen(buffer))
			{
				auto f_Old = GetCommandHND()->Make_FunctionModWrapper(&GameObject::SetName, mpFocus->GetName());
				auto f_New = GetCommandHND()->Make_FunctionModWrapper(&GameObject::SetName, std::string{ buffer });
				GetCommandHND()->InvokeCommand(mpFocus->GetID(), f_Old, f_New);
			}
			if (EGUI::Display::DropDownSelection("Tag", i, g_arr, 80))
			{

			}
			EGUI::SameLine();
			EGUI::ChangeAlignmentYOffset(0);
			if (EGUI::Display::DropDownSelection("Layer", j, g_arr2, 80))
			{

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
			std::to_string(mpFocus->GetID())))
		{
			tempTransform.EditorUI();
			EGUI::Display::EndTreeNode();
		}

		auto& arrComp = mpFocus->GetAllComponents();
		for (unsigned int i = 0; i < arrComp.size(); ++i)
		{
			EGUI::PushID(i);
			EGUI::Display::HorizontalSeparator();
			bool open = EGUI::Display::StartTreeNode(arrComp[i]->GetEditorName() + "##" +
				std::to_string(mpFocus->GetID()));
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
		for (const auto& c : arrBehav)
		{
			EGUI::Display::HorizontalSeparator();
			if (EGUI::Display::StartTreeNode(std::string{ c->GetBehaviourName() } + "##" +
				std::to_string(mpFocus->GetID())))
			{
				c->EditorUI();
				EGUI::Display::EndTreeNode();
			}
		}
	}

	std::string Inspector::GetLabel() const
	{
		return mLabel;
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
		static ListOfComponents availableComp;
		static constexpr size_t numComponents = Ut::SizeofList<UsableComponents>::value;
		Array<std::string, numComponents> arr;
		ListOfComponentsName<std::make_index_sequence<numComponents>, UsableComponents>::Extract(arr);

		if (EGUI::Display::StartPopup(g_cPopup))
		{
			EGUI::Display::Dummy(235, 2);
			for (unsigned int i = 0; i < numComponents; ++i)
			{
				const auto& e = arr[i];
				if (EGUI::Display::SelectableTxt(e, false))
				{
					Component* pComp = availableComp.Get(i, mpFocus);
					mpFocus->AddComponent(pComp, typename Component::TAG{});
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
					if (ptr) mpFocus->AddComponent(ptr, BehaviourTag{});
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

	bool Inspector::RemoveComponent(Component* _pCom)
	{
		bool ret = false;
		if (ImGui::BeginPopupContextItem())
		{
			if (EGUI::Display::SelectableTxt("Remove"))
			{
				mpFocus->RemoveComponent(_pCom);
				ret = true;
			}
			ImGui::EndPopup();
		}
		return ret;
	}
}

#endif // EDITOR 


