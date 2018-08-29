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
#include "Editor\Commands.h"
#include "Editor\CommandList.h"

namespace Dystopia
{
	EditorTab::EditorTab(bool _defaultOpen)
		: mSize{ Math::Vec2{ 0, 0 } }, mPos{ Math::Vec2{ 0, 0 } }, mIsOpened{ _defaultOpen }, mpComdHandler{ nullptr }
	{}

	EditorTab::~EditorTab()
	{
		mpComdHandler = nullptr;
	}

	void EditorTab::SetSize(const Math::Vec2& _size)
	{
		mSize = _size;
	}

	void EditorTab::SetSize(const float& _x, const float& _y)
	{
		mSize = Math::Vec2{ _x, _y };
	}

	void EditorTab::SetPosition(const Math::Vec2& _pos)
	{
		mPos = _pos;
	}

	void EditorTab::SetPosition(const float& _x, const float& _y)
	{
		mPos = Math::Vec2{ _x, _y };
	}

	Math::Vec2 EditorTab::Size() const
	{
		return mSize;
	}

	Math::Vec2 EditorTab::Position() const
	{
		return mPos;
	}

	bool* EditorTab::GetOpenedBool()
	{
		return &mIsOpened;
	}
	
	void EditorTab::SetComdContext(CommandHandler * const _ctx)
	{
		mpComdHandler = _ctx;
	}

}