/* HEADER *********************************************************************************/
/*!
\file	ConsoleLog.cpp
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
#include "Editor\ConsoleLog.h"
#include "Editor\EGUI.h"

namespace Dystopia
{
	static ConsoleLog* gpInstance = 0;
	ConsoleLog* ConsoleLog::GetInstance()
	{
		if (gpInstance) return gpInstance;

		gpInstance = new ConsoleLog{};
		return gpInstance;
	}

	ConsoleLog::ConsoleLog()
		: EditorTab{ false },
		mLabel{ "Console" }
	{}

	ConsoleLog::~ConsoleLog()
	{}
	
	void ConsoleLog::Init()
	{
	
	}

	void ConsoleLog::Update(const float&)
	{

	}

	void ConsoleLog::Window()
	{
		EGUI::Display::Button("Clear");
		EGUI::Display::HorizontalSeparator();
	}

	void ConsoleLog::Shutdown()
	{

	}

	std::string ConsoleLog::GetLabel() const
	{
		return mLabel;
	}
}

#endif // EDITOR

