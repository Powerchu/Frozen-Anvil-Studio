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
#include "Editor\EGUI.h"
#include "Editor\Inspector.h"
#include "Editor\ProjectResource.h"
#include "Editor\Commands.h"
#include "Editor\CommandList.h"
#include "Object\GameObject.h"
#include "Component\Component.h"
#include <iostream>

namespace Dystopia
{
	static Inspector* gpInstance = 0;

	Inspector* Inspector::GetInstance()
	{
		if (gpInstance) return gpInstance;

		gpInstance = new Inspector{};
		return gpInstance;
	}

	Inspector::Inspector()
		: EditorTab{ true }, 
		mpFocusGameObj{ nullptr }, mLabel{ "Inspector" }, mDemoVec{ Math::Vec4{0,0,0,0} },
		mDemoText{ "hello" }, mDemoName{ "" }
	{
	}

	Inspector::~Inspector()
	{
	
	}

	void Inspector::Init()
	{
	}

	void Inspector::Update(const float& _dt)
	{
		_dt;
	}

	void Inspector::Window()
	{
		GameObjectDetails();

		float x = mDemoVec.x;
		float y = mDemoVec.y;
		float z = mDemoVec.z;
		EGUI::Display::HorizontalSeparator();
		if (EGUI::Display::StartTreeNode("Temporary Transform"))	// replace with for loop of all components name
		{
			EGUI::Display::TextField("mDemoText", mDemoText, 32);

			if (EGUI::Display::VectorFields("Demo Vec", &mDemoVec, 0.1f, 0.f, 10.f))
			{
				x = mDemoVec.x;
				y = mDemoVec.y;
				z = mDemoVec.z;
			}
			EGUI::Display::Label("Variable mDemoVec x : [%.3f] y: [%.3f] z:[%.3f]", x, y, z);
			EGUI::Display::EndTreeNode();
		}
		EGUI::Display::HorizontalSeparator();
		if (EGUI::Display::StartTreeNode("Temporary Renderer"))	// replace with for loop of all components name
		{
			EGUI::Display::TextField("mDemoText", mDemoText, 32);

			if (EGUI::Display::VectorFields("Demo Vec", &mDemoVec, 0.1f, 0.f, 10.f))
			{
				x = mDemoVec.x;
				y = mDemoVec.y;
				z = mDemoVec.z;
			}
			EGUI::Display::Label("Variable mDemoVec x : [%.3f] y: [%.3f] z:[%.3f]", x, y, z);
			EGUI::Display::EndTreeNode();
		}
		if (EGUI::Display::EmptyBox("Payload", 150, mDemoName, true) && mDemoName.length())
		{
			ProjectResource::GetInstance()->FocusOnFile(mDemoName);
		}
		if (File *t = EGUI::Display::StartPayloadReceiver<File>(EGUI::FILE))
		{
			mpComdHandler->InvokeCommand(new ComdModifyValue<std::string>{ &mDemoName, (*t).mName });
			EGUI::Display::EndPayloadReceiver();
		}
		EGUI::SameLine(); 
		if (EGUI::Display::IconCircle("payload1box", 6, 0, 3) && mDemoName.length())
		{
			ProjectResource::GetInstance()->FocusOnFile(mDemoName);
		}



		AddComponentButton();
	}

	void Inspector::Shutdown()
	{
		mpFocusGameObj = nullptr;
	}

	void Inspector::GameObjectDetails()
	{
		static int i = 0;
		static int j = 0;
		AutoArray<std::string> arr;
		arr.push_back("");
		arr.push_back("item1");
		arr.push_back("item2");
		AutoArray<std::string> arr2;
		arr2.push_back("item3");
		arr2.push_back("item4");
		arr2.push_back("item5");
		char arr3[MAX_SEARCH] = "This is where the object name goes";
		static bool checked = true;

		EGUI::Display::IconGameObj("GameObjIcon", 50, 50);
		EGUI::SameLine(10);
		if (EGUI::StartChild("InfoArea", Math::Vec2{ Size().x - 60, 50 }, false, Math::Vec4{ 0,0,0,0 }))
		{
			EGUI::Display::CheckBox("GameObjActive", &checked, false);
			EGUI::SameLine();
			EGUI::Display::TextField("GameObjName", arr3, MAX_SEARCH, false, 350.f);
			EGUI::Display::DropDownSelection("Tag", i, arr, 100);
			EGUI::SameLine();
			EGUI::ChangeAlignmentYOffset(0);
			EGUI::Display::DropDownSelection("Layer", j, arr2, 100);
			EGUI::ChangeAlignmentYOffset();
		}
		EGUI::EndChild();
	}

	std::string Inspector::GetLabel() const
	{
		return mLabel;
	}

	void Inspector::SetFocusObj(GameObject* _target)
	{
		mpFocusGameObj = _target;
	}

	void Inspector::RemoveFocus()
	{
		mpFocusGameObj = nullptr;
	}

	void Inspector::AddComponentButton()
	{
		ImGui::Separator();
		static const ImVec2 btnSize{250, 20};
		float mid = Size().x / 2;
		float inde = mid - (btnSize.x / 2);
		inde = (inde < 20) ? 20 : inde;
		EGUI::Indent(inde);
		ImVec2 pos = ImGui::GetCursorScreenPos();
		pos.y += btnSize.y;
		pos.x -= 1;
		if (ImGui::Button("Add Component", btnSize))
		{
			ImGui::OpenPopup("Inspector Component List");
			ImGuiContext& g = *ImGui::GetCurrentContext();
			ImGuiPopupRef& popup_ref = g.OpenPopupStack[g.CurrentPopupStack.Size];
			popup_ref.OpenPopupPos = pos;
		}

		EGUI::UnIndent(inde);
		ComponentsDropDownList();
	}

	void Inspector::ComponentsDropDownList()
	{
		std::string components[5] = { "Com1", "Com2", "Com3", "Com4", "Com5" };
		if (ImGui::BeginPopup("Inspector Component List"))
		{
			ImGui::Dummy(ImVec2{ 250 - 15, 5 });
			for (const auto& e : components)
				EGUI::Display::SelectableTxt(e, false);

			ImGui::EndPopup();
		}
	}
}

#endif // EDITOR 


