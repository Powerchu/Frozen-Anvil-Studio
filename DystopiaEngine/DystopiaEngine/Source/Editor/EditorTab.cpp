/* HEADER *********************************************************************************/
/*!
\file	EditorTab.cpp
\author Digipen (100%)
\par    email: t.shannon\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "Editor\EditorTab.h"
#include "Editor\Editor.h"


namespace Dystopia
{
	AutoArray<EditorTab*> EditorTab::mAllTabs;

	EditorTab::EditorTab()
		: mSize{ Math::Vec4{ 0,0,0,0 } }
	{
		mAllTabs.push_back(this);
	}

	EditorTab::~EditorTab()
	{}

	void EditorTab::SetSize(const Math::Vec4& _size)
	{
		mSize = _size;
	}

	void EditorTab::SetSize(const float& _x, const float& _y)
	{
		mSize = Math::Vec4{ _x, _y, 0, 0 };
	}

}