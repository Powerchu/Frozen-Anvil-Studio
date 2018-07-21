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
		float x = mDemoVec.x;
		float y = mDemoVec.y;
		float z = mDemoVec.z;
		if (EGUI::Display::CollapsingHeader("Temporary Transform"))	// replace with for loop of all components name
		{
			EGUI::Indent();
			EGUI::Display::TextField("mDemoText", mDemoText, 32);

			if (EGUI::Display::VectorFields("Demo Vec", &mDemoVec, 0.1f, 0.f, 10.f))
			{
				x = mDemoVec.x;
				y = mDemoVec.y;
				z = mDemoVec.z;
			}
			EGUI::Display::Label("Variable mDemoVec x : [%.3f] y: [%.3f] z:[%.3f]", x, y, z);
			EGUI::UnIndent();
		}

		static int i = 0;
		AutoArray<std::string> arr;
		arr.push_back("");
		arr.push_back("item1");
		arr.push_back("item2");

		if (EGUI::Display::EmptyBox("Box to accept payload", 150, mDemoName, true) && mDemoName.length())
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
		EGUI::Display::DropDownSelection("TestDropDown", i, arr);

		AddComponentButton();
	}

	void Inspector::Shutdown()
	{
		mpFocusGameObj = nullptr;
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
		static const ImVec2 btnSize{250, 20};
		float mid = Size().x / 2;

		EGUI::Indent(mid - (btnSize.x / 2));
		if (ImGui::Button("Add Component", btnSize))
		{
			ImGui::OpenPopup("Inspector Component List");
		}
		EGUI::UnIndent(mid - (btnSize.x / 2));
		ComponentsDropDownList();
	}

	void Inspector::ComponentsDropDownList()
	{
		if (ImGui::BeginPopup("Inspector Component List"))
		{

			ImGui::EndPopup();
		}
	}
}

#endif // EDITOR 


