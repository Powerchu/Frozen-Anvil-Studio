/* HEADER *********************************************************************************/
/*!
\file	Inspector.cpp
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
#include "Editor\EGUI.h"
#include "Editor\Inspector.h"
#include "Editor\ScriptFormatter.h"
#include "Editor\Commands.h"
#include "Editor\EditorEvents.h"
#include "Editor\EditorMetaHelpers.h"
#include "Utility\ComponentGUID.h"
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
		: EditorTab{ true }, mpFocus{ nullptr }, mLabel{ "Inspector" }, mShowListOfComponents{ false }
	{
	}

	Inspector::~Inspector()
	{
		gpInstance = nullptr;
	}

	void Inspector::Init()
	{
	}

	void Inspector::Update(const float& _dt)
	{
		_dt;
	}

	void Inspector::EditorUI()
	{
		if (!mpFocus) return;

		GameObjectDetails();
		GameObjectComponents();
		AddComponentButton();

		////float x = mDemoVec.x;
		////float y = mDemoVec.y;
		////float z = mDemoVec.z;
		////EGUI::Display::HorizontalSeparator();
		////if (EGUI::Display::StartTreeNode("Temporary Transform"))	// replace with for loop of all components name
		////{
		////	EGUI::Display::TextField("mDemoText", mDemoText, 32);

		////	if (EGUI::Display::VectorFields("Demo Vec", &mDemoVec, 0.1f, 0.f, 10.f))
		////	{
		////		x = mDemoVec.x;
		////		y = mDemoVec.y;
		////		z = mDemoVec.z;
		////	}
		////	EGUI::Display::Label("Variable mDemoVec x : [%.3f] y: [%.3f] z:[%.3f]", x, y, z);
		////	EGUI::Display::EndTreeNode();
		////}
		////EGUI::Display::HorizontalSeparator();
		////if (EGUI::Display::EmptyBox("Payload", 150, mDemoName, true) && mDemoName.length())
		////{
		////	ProjectResource::GetInstance()->FocusOnFile(mDemoName);
		////}
		////if (File *t = EGUI::Display::StartPayloadReceiver<File>(EGUI::FILE))
		////{
		////	mpComdHandler->InvokeCommand(&mDemoName, (*t).mName);
		////	EGUI::Display::EndPayloadReceiver();
		////}
		////EGUI::SameLine(); 
		////if (EGUI::Display::IconCircle("payload1box", 6, 0, 3) && mDemoName.length())
		////{
		////	ProjectResource::GetInstance()->FocusOnFile(mDemoName);
		////}
	}

	void Inspector::Shutdown()
	{
		mpFocus = nullptr;
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

		char buffer[MAX_SEARCH * 2] = "";
		std::string name = mpFocus->GetName();
		strcpy_s(buffer, name.c_str());

		EGUI::Display::IconGameObj("GameObjIcon", 50, 50);
		EGUI::SameLine(10);
		if (EGUI::StartChild("InfoArea", Math::Vec2{ Size().x - 60, 50 }, false, Math::Vec4{ 0,0,0,0 }))
		{
			bool prevV = mpFocus->mTestBool;
			if (EGUI::Display::CheckBox("GameObjActive", &mpFocus->mTestBool, false))
			{
				GetCommandHND()->InvokeCommand<GameObject>(mpFocus->GetID(), &mpFocus->mTestBool, prevV);
			}
			EGUI::SameLine();
			if (EGUI::Display::TextField("Name", buffer, MAX_SEARCH, false, 350.f) && strlen(buffer))
			{
				auto f_Old = GetCommandHND()->Make_FunctionModWrapper(&GameObject::SetName, mpFocus->GetName());
				auto f_New = GetCommandHND()->Make_FunctionModWrapper(&GameObject::SetName, std::string{ buffer });
				GetCommandHND()->InvokeCommand(mpFocus->GetID(), f_Old, f_New);
			}
			EGUI::Display::DropDownSelection("Tag", i, arr, 100);
			EGUI::SameLine();
			EGUI::ChangeAlignmentYOffset(0);
			EGUI::Display::DropDownSelection("Layer", j, arr2, 100);
			EGUI::ChangeAlignmentYOffset();

			switch (EGUI::Display::DragFloat("Test Float", &mpFocus->mTestFloat, 0.01f, -FLT_MAX, FLT_MAX))
			{
			case EGUI::eDragStatus::eSTART_DRAG: 
				GetCommandHND()->StartRecording<GameObject>(mpFocus->GetID(), &mpFocus->mTestFloat);
				break;
			case EGUI::eDragStatus::eEND_DRAG: 
				GetCommandHND()->EndRecording();
				break;
			}

			switch (EGUI::Display::DragInt("Test Int", &mpFocus->mTestInt, 1.f, -INT_MAX, INT_MAX))
			{
			case EGUI::eDragStatus::eSTART_DRAG:
				GetCommandHND()->StartRecording<GameObject>(mpFocus->GetID(), &mpFocus->mTestInt);
				break;
			case EGUI::eDragStatus::eEND_DRAG:
				GetCommandHND()->EndRecording();
				break;
			}
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
		for (const auto& c : arrComp)
		{
			EGUI::Display::HorizontalSeparator();
			if (EGUI::Display::StartTreeNode(c->GetEditorName() + "##" +
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

	void Inspector::SetFocus(GameObject& _rGameObj)
	{
		mpFocus = &_rGameObj;
	}

	void Inspector::RemoveFocus()
	{
		mpFocus = nullptr;
	}

	void Inspector::AddComponentButton()
	{
		static constexpr Math::Vec2 btnSize{ 250, 20 };
		float mid = Size().x / 2;
		float inde = mid - (btnSize.x / 2);
		inde = (inde < 20) ? 20 : inde;

		EGUI::Display::HorizontalSeparator();
		EGUI::Indent(inde);
		if (EGUI::Display::Button("Add Component", btnSize))
		{
			EGUI::Display::OpenPopup("Inspector Component List", false);
		}
		//if (EGUI::Display::Button("Add Behaviour", btnSize))
		//{
		//	if (GenerateScript("Whatever", "Tan Shannon", "t.shannon"))
		//		std::cout << "Script Added to the visual studio project. Please arrange the filters and code in visual then come back to test it!\n";
		//	else
		//		std::cout << "Script already Exists! Aborted!\n";
		//}
		EGUI::UnIndent(inde);
		ComponentsDropDownList();
	}

	void Inspector::ComponentsDropDownList()
	{
		static constexpr size_t numComponents = Utility::SizeofList<AllComponents>::value;
		Array<std::string, numComponents> arr;
		ListOfComponentsName<std::make_index_sequence<numComponents>, AllComponents>::Extract(arr);

		if (EGUI::Display::StartPopup("Inspector Component List"))
		{
			EGUI::Display::Dummy(235, 2);
			for (unsigned int i = 0; i < numComponents; ++i)
			{
				const auto& e = arr[i];
				if (EGUI::Display::SelectableTxt(e, false))
				{
					ListOfComponents a;
					Component* pComp = a.Get(i);
					pComp->Init();
					mpFocus->AddComponent(pComp, typename Component::TAG{});
				}
			}

			EGUI::Display::EndPopup();
		}
	}
}

#endif // EDITOR 


