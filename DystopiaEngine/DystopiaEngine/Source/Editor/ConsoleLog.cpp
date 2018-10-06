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
#include "Utility/DebugAssert.h"

#include "Editor/ConsoleLog.h"
#include "Editor/EGUI.h"
#include "Editor/AdminCalls.h"

#include "System/Driver/Driver.h"
#include "System/Scene/Scene.h"
#include "System/Scene/SceneSystem.h"
#include "System/Logger/LoggerSystem.h"

#include "Behaviour/Behaviour.h"
#include "Component/Component.h"
#include "Object/GameObject.h"
#include <algorithm>

namespace Dystopia
{
	static ConsoleLog* gpConsoleInst = 0;
	ConsoleLog* ConsoleLog::GetInstance()
	{
		if (gpConsoleInst) return gpConsoleInst;

		gpConsoleInst = new ConsoleLog{};
		return gpConsoleInst;
	}

	void _DLL_EXPORT PrintToConsoleLog(const std::string& _text)
	{
		ConsoleLog::GetInstance()->Debug(_text);
	}

	void _DLL_EXPORT PrintToConsoleLog(const char* _text)
	{
		ConsoleLog::GetInstance()->Debug(_text);
	}

	ConsoleLog::ConsoleLog()
		: EditorTab{ false },
		mLabel{ "Console" },
		mArrDebugTexts{ "" },
		mLoggingIndex{ 0 },
		mRecordIndex{ 0 },
		mAdminCommands{ "" }
	{}

	ConsoleLog::~ConsoleLog()
	{
		gpConsoleInst = nullptr;
	}
	
	void ConsoleLog::Init()
	{
		EngineCore::GetInstance()->GetSubSystem<LoggerSystem>()->RedirectOutput(PrintToConsoleLog);
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
		AdminInput();
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
		if (EGUI::StartChild("##DetailLog", Math::Vec2{Size().x - 2.f, Size().y - 85.f}, false))
		{
			for (unsigned int i = 0; i < mLoggingIndex; ++i)
				EGUI::Display::Label(mArrDebugTexts[i].c_str());
			EGUI::EndChild();
		}
		EGUI::Display::HorizontalSeparator();
	}

	void ConsoleLog::AdminInput()
	{
		if (EGUI::Display::TextField("AdminText", mAdminCommands, MAX_SEARCH, false, Size().x - 6.f))
		{
			int count	= 0;
			std::string var = "";
			std::string fnName = "";
			PrintToConsoleLog(std::string{ mAdminCommands });
			if (!Admin::ValidCommand(mAdminCommands, count, var, fnName))
				PrintToConsoleLog("Invalid Command");
			else
			{
				for (int i = 0; i < count; ++i)
				{
					for (int j = 0; j < count; ++j)
					{
						auto *p = (*(Admin::g_AdminFuncs[fnName]))(var + std::to_string(i));
						p->GetComponent<Transform>()->SetPosition(Math::Vec3D{ 20.f * i , 20.f * j, 0});
						GetCurrentScene()->GetAllGameObjects().EmplaceBack(Utility::Move(*p));
						GetCurrentScene()->GetAllGameObjects().back().Init();
						delete p;
					}
					
				}
			}

			int i = MAX_SEARCH - 1;
			while (i > -1)
			{
				mAdminCommands[i] = '\0';
				--i;
			}
		}
	}

	void ConsoleLog::Debug(const std::string& _text)
	{
		if (mLoggingIndex == maxLog)
		{
			std::rotate(mArrDebugTexts.begin(), mArrDebugTexts.begin() + 1, mArrDebugTexts.end());
			mArrDebugTexts[maxLog - 1] = _text.c_str();
		}
		else mArrDebugTexts[mLoggingIndex++] = _text.c_str();
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

