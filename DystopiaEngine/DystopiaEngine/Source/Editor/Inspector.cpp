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
#include "Editor\Inspector.h"
#include "Editor\ProjectResource.h"
#include "Editor\EGUI.h"
#include "Object\GameObject.h"
#include "Component\Component.h"
#include "../../Dependancies/ImGui/imgui.h"
#include <iostream>

namespace Dystopia
{
	Inspector::Inspector()
		: mpFocusGameObj{ nullptr }, mLabel{ "Inspector" }, mDemoVec{ Math::Vec4{0,0,0,0} },
		mDemoText{ "hello" }, mDemoName{ "" }
	{
	}

	Inspector::~Inspector()
	{}

	void Inspector::Init()
	{
	}

	void Inspector::Update(const float& _dt)
	{
		_dt;
	}

	void Inspector::Shutdown()
	{
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
		EGUI::Display::EmptyBox("Box to accept payload", 150, mDemoName);
		if (File *t = EGUI::Display::StartPayloadReceiver<File>(EGUI::FILE))
		{
			mDemoName = (*t).mName;
			EGUI::Display::EndPayloadReceiver();
		}
		EGUI::Display::DropDownSelection("TestDropDown", i, arr);

		if (!mpFocusGameObj) 
			return;

		AutoArray<Component*> pAllComponents = mpFocusGameObj->GetComponents<Component>();
		for (auto e : pAllComponents)
		{
			// e->Editor() or e->Window()
			e;
		}
	}
}

#endif // EDITOR 


