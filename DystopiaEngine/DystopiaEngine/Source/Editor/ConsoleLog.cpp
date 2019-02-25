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

#include "Editor/EditorMain.h"

#include "Editor/ConsoleLog.h"
#include "Editor/EGUI.h"
//#include "Editor/AdminCalls.h"

#include "System/Driver/Driver.h"
#include "System/Scene/Scene.h"
#include "System/Scene/SceneSystem.h"
#include "System/Logger/LoggerSystem.h"

#include "Behaviour/Behaviour.h"
#include "Component/Component.h"
#include "Object/GameObject.h"
#include <algorithm>


void _DLL_EXPORT PrintToConsoleLog(const std::string& _text)
{
	PrintToConsoleLog(_text.c_str());
}

void _DLL_EXPORT PrintToConsoleLog(const HashString& _text)
{
	//Editor::ConsoleLog::GetInstance()->Debug(_text);
	Editor::EditorMain::GetInstance()->GetPanel<Editor::ConsoleLog>()->Debug(_text);
}

void _DLL_EXPORT PrintToConsoleLog(const char* _text)
{
	//Editor::ConsoleLog::GetInstance()->Debug(_text);
	Editor::EditorMain::GetInstance()->GetPanel<Editor::ConsoleLog>()->Debug(_text);
}

namespace Editor //Dystopia
{
	/*
	static ConsoleLog* gpConsoleInst = 0;
	ConsoleLog* ConsoleLog::GetInstance()
	{
		if (gpConsoleInst) return gpConsoleInst;

		gpConsoleInst = new ConsoleLog{};
		return gpConsoleInst;
	}
	*/


	ConsoleLog::ConsoleLog(void)
		: //EditorTab{ false },
		mLabel{ "Console" },
		mArrDebugTexts{ "" },
		mnLoggingIndex{ 0 },
		mnRecordIndex{ 0 },
		mAdminCommands{ "" }
	{
		EditorPanel::SetOpened(true);
	}

	ConsoleLog::~ConsoleLog()
	{
		//gpConsoleInst = nullptr;
	}

	void ConsoleLog::Load(void)
	{}

	bool ConsoleLog::Init(void)
	{
		Dystopia::EngineCore::GetInstance()->GetSubSystem<Dystopia::LoggerSystem>()->RedirectOutput(PrintToConsoleLog);
		return true;
	}

	void ConsoleLog::Update(float)
	{}

	void ConsoleLog::EditorUI(void)
	{
		EGUI::Indent(5);
		if (EGUI::Display::Button("Clear")) Clear();
		EGUI::UnIndent(5);
		EGUI::Display::HorizontalSeparator();
		PrintLogs();
		AdminInput();
	}

	void ConsoleLog::Shutdown(void)
	{}

	void ConsoleLog::Message(eEMessage)
	{}
	
	void ConsoleLog::SaveSettings(Dystopia::TextSerialiser&) const
	{}

	void ConsoleLog::LoadSettings(Dystopia::TextSerialiser&)
	{}

	const HashString& ConsoleLog::GetLabel(void) const
	{
		return mLabel;
	}

	void ConsoleLog::PrintLogs(void)
	{
		if (EGUI::StartChild("##DetailLog", Math::Vec2{Size().x - 2.f, Size().y - 85.f}, false))
		{
			for (unsigned int i = 0; i < mnLoggingIndex; ++i)
				ImGui::TextWrapped(mArrDebugTexts[i].c_str());
		}
		EGUI::EndChild();
		EGUI::Display::HorizontalSeparator();
	}

	void ConsoleLog::AdminInput(void)
	{
		//if (EGUI::Display::TextField("AdminText", mAdminCommands, MAX_SEARCH, false, Size().x - 6.f))
		//{
		//	int count	= 0;
		//	std::string var = "";
		//	std::string fnName = "";
		//	PrintToConsoleLog(std::string{ mAdminCommands });
		//	if (!Dystopia::Admin::ValidCommand(mAdminCommands, count, var, fnName))
		//		PrintToConsoleLog("Invalid Command");
		//	else
		//	{
		//		for (int i = 0; i < count; ++i)
		//		{
		//			for (int j = 0; i < count; ++j)
		//			{
		//				auto *p = (*(Dystopia::Admin::g_AdminFuncs[fnName]))(var + std::to_string(i));
		//				p->GetComponent<Dystopia::Transform>()->SetPosition(Math::Vec3D{ 20.f * i , 20.f * j, 0});
		//				auto obj = GetCurrentScene()->InsertGameObject(Ut::Move(*p));
		//				obj->Identify();
		//				obj->Awake();
		//				obj->Init();
		//				delete p;
		//			}
		//		}
		//	}
		//
		//	int i = MAX_SEARCH - 1;
		//	while (i > -1)
		//	{
		//		mAdminCommands[i] = '\0';
		//		--i;
		//	}
		//}
	}

	void ConsoleLog::Debug(const HashString& _text)
	{
		if (mnLoggingIndex == MAX_CLOG)
		{
			std::rotate(mArrDebugTexts.begin(), mArrDebugTexts.begin() + 1, mArrDebugTexts.end());
			mArrDebugTexts[MAX_CLOG - 1] = _text.c_str();
		}
		else mArrDebugTexts[mnLoggingIndex++] = _text.c_str();
		mnRecordIndex++;
	}
	
	void ConsoleLog::Clear(void)
	{
		for (auto& e : mArrDebugTexts)
			e.clear();
		mnLoggingIndex = 0;
		mnRecordIndex = 0;
	}
}

#endif // EDITOR

