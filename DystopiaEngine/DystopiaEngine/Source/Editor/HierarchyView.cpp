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
#include "Object\GameObject.h"

constexpr float DEFAULT_WIDTH = 300;
constexpr float DEFAULT_HEIGHT = 300;

namespace Dystopia
{
	HierarchyView::HierarchyView()
		: mWidth{ DEFAULT_WIDTH }, mHeight{ DEFAULT_HEIGHT }, mpFocusGameObj{ nullptr }, mpCurrentScene{ nullptr }
	{
	}

	HierarchyView::~HierarchyView()
	{
	}

	void HierarchyView::Init()
	{
	}

	void HierarchyView::Update(const float& _dt)
	{
		_dt;
	}

	void HierarchyView::Shutdown()
	{
	}

	void HierarchyView::SetWidth(float _width)
	{
		mWidth = _width;
	}

	void HierarchyView::SetHeight(float _height)
	{
		mHeight = _height;
	}

	void HierarchyView::Window()
	{
		if (!mpCurrentScene) return;

		// Do for all objects in the scene
		// Dystopia::EGUI::Display::Label(mpFocusGameObj->GetName().c_str()); 
	}

}


#endif // EDITOR


