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
#include "Editor\Inspector.h"
#include "Editor\EGUI.h"
#include "Object\GameObject.h"
#include "Component\Component.h"

namespace Dystopia
{
	Inspector::Inspector()
		: mpFocusGameObj{ nullptr }, mLabel{ "Inspector" }
	{}

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
		Math::Vec4 field{ 0,0,0,0 };
		if (EGUI::Display::VectorFields("Vec Me", &field))
		{

		}

		if (!mpFocusGameObj) return;

		AutoArray<Component*> pAllComponents = mpFocusGameObj->GetComponents<Component>();
		for (auto e : pAllComponents)
		{
			// e->Editor() or e->Window()
			e;
		}
	}
}

#endif // EDITOR 


