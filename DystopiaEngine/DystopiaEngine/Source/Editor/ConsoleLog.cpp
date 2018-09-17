/* HEADER *********************************************************************************/
/*!
\file	ConsoleLog.cpp
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
#include "Editor\ConsoleLog.h"
#include "Editor\EGUI.h"
#include "Utility\DebugAssert.h"
#include <algorithm>

namespace Dystopia
{
	static ConsoleLog* gpInstance = 0;
	ConsoleLog* ConsoleLog::GetInstance()
	{
		if (gpInstance) return gpInstance;

		gpInstance = new ConsoleLog{};
		return gpInstance;
	}

	void PrintToConsoleLog(const std::string& _text)
	{
		gpInstance->Debug(_text);
	}

	ConsoleLog::ConsoleLog()
		: EditorTab{ false },
		mLabel{ "Console" },
		mArrDebugTexts{ "" },
		mLoggingIndex{ 0 },
		mRecordIndex{ 0 }
	{}

	ConsoleLog::~ConsoleLog()
	{
		gpInstance = nullptr;
	}
	
	void ConsoleLog::Init()
	{
	}

	void ConsoleLog::Update(const float&)
	{

	}

	void ConsoleLog::EditorUI()
	{
		EGUI::Indent(5);
		if (EGUI::Display::Button("Clear")) Clear();
		EGUI::UnIndent(5);
		EGUI::Display::HorizontalSeparator();
		PrintLogs();
	}

	void ConsoleLog::Shutdown()
	{

	}

	std::string ConsoleLog::GetLabel() const
	{
		return mLabel;
	}

	void ConsoleLog::PrintLogs()
	{
		static constexpr float bottomOffset = -55.f;
		Math::Vec2 size{ Size() };
		size.y = (size.y > 100.f) ? size.y + bottomOffset : size.y;
		if (EGUI::StartChild("PrintingArea", size))
		{
			for (unsigned int i = 0; i < mLoggingIndex; ++i)
				EGUI::Display::Label(mArrDebugTexts[i].c_str());
		}
		EGUI::EndChild();
	}

	void ConsoleLog::Debug(const std::string& _text)
	{
		if (mLoggingIndex == maxLog)
		{
			std::rotate(mArrDebugTexts.begin(), mArrDebugTexts.begin() + 1, mArrDebugTexts.end());
			mArrDebugTexts[maxLog - 1] = _text;
		}
		else mArrDebugTexts[mLoggingIndex++] = _text;
		mRecordIndex++;
	}
	
	void ConsoleLog::Clear()
	{
		for (auto& e : mArrDebugTexts)
			e.clear();
		mLoggingIndex = 0;
		mRecordIndex = 0;
	}
}

#endif // EDITOR

