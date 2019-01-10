/* HEADER *********************************************************************************/
/*!
\file	FlowChart.cpp
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#include "Editor/FlowChart.h"
#include "Editor/EGUI.h"
#include "Editor/Payloads.h"
#include "Editor/EditorMain.h"

#include <fstream>
#include <iostream>
#include <string>

Editor::FlowChart::FlowChart(void)
	: mLabel{ "FlowChart" }, mCurrentDataFile{}
{}

Editor::FlowChart::~FlowChart(void)
{}

void Editor::FlowChart::Load(void)
{}

bool Editor::FlowChart::Init(void)
{
	return true;
}

void Editor::FlowChart::Update(float)
{}

void Editor::FlowChart::EditorUI(void)
{
	EGUI::Display::EmptyBox("Data File", 150, mCurrentDataFile.size() ? mCurrentDataFile.c_str() : "-empty-");
	if (auto t = EGUI::Display::StartPayloadReceiver<Editor::File>(EGUI::ePayloadTags::TXT))
	{
		ProcessData(t->mPath);
		EGUI::Display::EndPayloadReceiver();
	}
}

void Editor::FlowChart::Shutdown(void)
{}

void Editor::FlowChart::Message(eEMessage)
{}

void Editor::FlowChart::SaveSettings(Dystopia::TextSerialiser&) const
{}

void Editor::FlowChart::LoadSettings(Dystopia::TextSerialiser&)
{}

HashString Editor::FlowChart::GetLabel(void) const
{
	return mLabel;
}

void Editor::FlowChart::ProcessData(const HashString& _filepath)
{
	static constexpr size_t MAX_BUFFER = 100;
	char buffer[MAX_BUFFER];
	FILE* file;
	auto e = fopen_s(&file, _filepath.c_str(), "r");

	if (file)
	{
		while (fgets(buffer, MAX_BUFFER, file))
		{
			std::string word{ buffer };
			size_t pos = word.find('\n');
			if (pos != std::string::npos)
				word = std::string{ word.begin(), word.begin() + pos };

			if (word.front() == '<' && word.back() == '>')
			{
				std::string mode{ word .begin() + 1, word.end() - 1};
				if (mode.length())
				{
				}
			}
		}
		fclose(file);
	}
}

#endif // EDITOR



