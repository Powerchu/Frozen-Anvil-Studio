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
#include "System/Driver/Driver.h"
#include "System/File/FileSystem.h"

#include <fstream>
#include <iostream>

#define MAX_BUFFER 100

Editor::FlowChart::FlowChart(void)
	: mLabel{ "FlowChart" }, mCurrentDataFile{}, mnWidth{ 200 }, mnHeight{ 100 }
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
	static constexpr float heightOffset = 70;
	EGUI::Display::EmptyBox("Data File", 150, mCurrentDataFile.size() ? mCurrentDataFile.c_str() : "-empty-");
	if (auto t = EGUI::Display::StartPayloadReceiver<Editor::File>(EGUI::ePayloadTags::TXT))
	{
		ProcessData(t);
		EGUI::Display::EndPayloadReceiver();
	}

	EGUI::SameLine();
	if (EGUI::Display::Button("Make Files", Math::Vec2{ 100, heightOffset * 0.5f }) && mItems.size())
		MakeFiles();

	EGUI::Display::HorizontalSeparator();

	EGUI::StartChild("GraphSheet", ImVec2{ Size().x, Size().y - heightOffset }, false, true);

	ImVec2 offset = ImGui::GetCursorScreenPos(); 
	ImDrawList* drawList = ImGui::GetWindowDrawList();
	int indexHovered = -1;
	int indexSelected = -1;
	static const Math::Vec2 itemPadding{8.f, 8.f};
	static const float cirRad = 4.f;
	drawList->ChannelsSplit(2);

	/* Draw Links */
	for (int i = 0; i < mLinks.size(); i++)
	{
		UserLinks * link = &mLinks[i];
		UserItem* from = &mItems[link->mnFromIndx];
		UserItem* to = &mItems[link->mnToIndx];
		ImVec2 p1 = offset + from->GetOutputSlotPos(link->mnFromSlot);
		ImVec2 p2 = offset + to->GetInputSlotPos(link->mnToSlot);
		drawList->AddBezierCurve(p1, p1 + Math::Vec2{ 50, 0 }, p2 + Math::Vec2{ -50,0 }, p2, IM_COL32(200, 200, 100, 255), 3.0f);
	}

	for (size_t i = 0; i < mItems.size(); i ++)
	{
		UserItem* curItem = &mItems[i];
		EGUI::PushID(static_cast<int>(curItem->GetID()));
		ImVec2 rectMin = offset + curItem->GetPos();

		/* Draw Names */
		drawList->ChannelsSetCurrent(1);
		bool anyActive = ImGui::IsAnyItemActive();
		ImGui::SetCursorScreenPos(rectMin + itemPadding);
		ImGui::BeginGroup();
		ImGui::Text("%s", curItem->GetName().c_str());
		ImGui::EndGroup();
		bool widgetActive = (!anyActive && ImGui::IsAnyItemActive());
		ImVec2 rectMax{rectMin.x + curItem->GetSize().x, rectMin.y + curItem->GetSize().y};

		/* Draw Items */
		drawList->ChannelsSetCurrent(0);
		ImGui::SetCursorScreenPos(rectMin);
		ImGui::InvisibleButton("node", ImVec2{ curItem->GetSize().x, curItem->GetSize().y });
		if (ImGui::IsItemHovered())
			indexHovered = static_cast<int>(i);
		bool movingActive = ImGui::IsItemActive();
		if (widgetActive || movingActive)
			indexSelected = static_cast<int>(i);
		if (movingActive && ImGui::IsMouseDragging(0))
		{
			auto pos = curItem->GetPos() + ImGui::GetIO().MouseDelta;
			if (pos.x > 0 && pos.x < (Size().x - ImGui::GetStyle().WindowPadding.x - curItem->GetSize().x))
				if (pos.y > 0 && pos.y < (Size().y - heightOffset - ImGui::GetStyle().WindowPadding.y - curItem->GetSize().y))
					curItem->SetPos(pos);
		}
		ImU32 col = indexHovered == i ? IM_COL32(75,75,75,255): 
										IM_COL32(60,60,60,255);
		drawList->AddRectFilled(rectMin, rectMax, col, 4.f);
		drawList->AddRect(rectMin, rectMax, IM_COL32(100,100,100,255), 4.f);
		for (int j = 0; j < curItem->GetInputCount(); j++)
			drawList->AddCircleFilled(offset + curItem->GetInputSlotPos(j), cirRad, IM_COL32(150, 150, 150, 150));
		for (int k = 0; k < curItem->GetOutputCount(); k++)
			drawList->AddCircleFilled(offset + curItem->GetOutputSlotPos(k), cirRad, IM_COL32(150, 150, 150, 150));

		EGUI::PopID();
	}

	drawList->ChannelsMerge();
	EGUI::EndChild();
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

void Editor::FlowChart::PanelClosing(void)
{
	mCurrentDataFile = "";
	mItems.clear();
	mLinks.clear();
}

HashString Editor::FlowChart::GetWord(const char * _str)
{
	std::string word = _str;
	size_t pos = word.find('\n');
	if (pos != std::string::npos)
		word = std::string{ word.begin(), word.begin() + pos };
	return word.c_str();
}

void Editor::FlowChart::ProcessData(File* _file)
{
	if (_file->mName == mCurrentDataFile)
		return;

	static constexpr char keyword[] = "<State>";

	char buffer[MAX_BUFFER];
	FILE* file;
	fopen_s(&file, _file->mPath.c_str(), "r");
	mCurrentDataFile = _file->mName;

	if (file)
	{
		while (fgets(buffer, MAX_BUFFER, file))
		{
			HashString word = GetWord(buffer);
			if (word == StringHasher(keyword))
				ParseItems(file);
		}
		ProcessLinks();
		fclose(file);
	}
}

void Editor::FlowChart::ParseItems(FILE* _file)
{
	static constexpr char exitword[] = "</State>";
	static constexpr char entryword[] = "\t<Flow>";

	int currItem = -1;
	char buffer[MAX_BUFFER];
	while (fgets(buffer, MAX_BUFFER, _file))
	{
		HashString sentence = GetWord(buffer);

		if (sentence == StringHasher(exitword))
			return;

		if (sentence == StringHasher(entryword) && currItem >= 0)
			ParseSubItem(currItem, _file);
		else
		{
			size_t pos = sentence.find("Name: ");
			if (pos != HashString::nPos)
				currItem = AddItem(HashString{ sentence.cbegin() + pos + 6, sentence.cend() });
		}
	}
}

void Editor::FlowChart::ParseSubItem(int _index, FILE* _file)
{
	static constexpr char exitword[] = "\t</Flow>";
	char buffer[MAX_BUFFER];

	while (fgets(buffer, MAX_BUFFER, _file))
	{
		HashString sentence = GetWord(buffer);

		if (sentence == StringHasher(exitword))
			return;

		size_t pos = sentence.find("Name: ");
		if (pos != HashString::nPos)
		{
			HashString word{ sentence.cbegin() + pos + 6, sentence.cend() };
			int index = AddItem(word);
			mItems[_index].AddLink(mItems[index].GetID());
		}
	}
}

int Editor::FlowChart::AddItem(const HashString& _name)
{
	int index = FindItem(_name);
	if (index < 0)
	{
		mItems.Insert(_name);
		UserItem* item = &mItems.back();
		item->SetPos(item->GetPos() + Math::Vec2{ (mItems.size() / 16) * item->GetSize().x, item->GetSize().y * (mItems.size() % 16)});
		return static_cast<int>(mItems.size() - 1);
	}
	return index;
}

int Editor::FlowChart::FindItem(const HashString& _name) const
{
	for (int i = 0; i < mItems.size(); i++)
		if (mItems[i].GetName() == _name)
			return i;
	return -1;
}

int Editor::FlowChart::FindItem(const uint64_t& _id) const
{
	for (int i = 0; i < mItems.size(); i++)
		if (mItems[i].GetID() == _id)
			return i;
	return -1;
}

void Editor::FlowChart::ProcessLinks(void)
{
	for (int i = 0; i < mItems.size(); i++)
	{
		UserItem* item = &mItems[i];
		const auto& allFlows = item->GetLinks();
		for (int j = 0; j < allFlows.size(); j++)
		{
			int foundIndex = FindItem(allFlows[j]);
			if (foundIndex >= 0)
				mLinks.Insert(UserLinks{ i, item->IncrementOutput(), foundIndex, mItems[foundIndex].IncrementInput() });
		}
	}
}

void Editor::FlowChart::MakeFiles(void)
{
	std::string line;
	AutoArray<HashString> mFormatFSHeader, mFormatFSBody;

	auto filepath = Dystopia::EngineCore::Get<Dystopia::FileSystem>()->FindFilePath("StateFormats.txt", Dystopia::eFileDir::eSolution);
	if (!filepath.length())
		return;

	std::ifstream stream(filepath.c_str(), std::ios::in);
	while (std::getline(stream, line))
	{
		if (line == "<FS_HEADER>")
			mFormatFSHeader = ExtractAllLines(static_cast<void*>(&stream), "</FS_HEADER>");
		else if (line == "<FS_BODY>")
			mFormatFSBody = ExtractAllLines(static_cast<void*>(&stream), "</FS_BODY>");
	}
	stream.close();

	ParseHeaders(mFormatFSHeader);
	ParseBodies(mFormatFSBody);
	ParseStates();
}

void Editor::FlowChart::ParseHeaders(AutoArray<HashString>& _allLines)
{
	static const std::string token = "_FS_NAME_";
	auto filepath = Dystopia::EngineCore::Get<Dystopia::FileSystem>()->FindFilePath("CharacterController.h", Dystopia::eFileDir::eCurrent);
	if (!filepath.length())
		return;

	std::vector<std::string> currFileLines = ClearBetweenCodes(filepath.c_str(), "//<AllStates>", "//</AllStates>");
	for (const auto& item : mItems)
	{
		/* convert to std vector of std string for now */
		/* need to insert at position for auto array, then will remove this */
		std::vector<std::string> mHeaderLines;
		for (const auto& elem : _allLines)
			mHeaderLines.push_back(elem.c_str());

		for (int i = 0; i < mHeaderLines.size(); i++)
		{
			size_t pos = mHeaderLines[i].find(token);
			size_t end;
			while (pos != std::string::npos)
			{
				end = pos + token.length();
				mHeaderLines[i].erase(mHeaderLines[i].begin() + pos, mHeaderLines[i].begin() + end);
				mHeaderLines[i].insert(pos, item.GetName().c_str());
				pos = mHeaderLines[i].find(token, 0);
			}
		}

		for (int i = 0; i < currFileLines.size(); i++)
		{
			size_t pos = currFileLines[i].find("//</AllStates>");
			if (pos != std::string::npos)
			{
				int repeat = 0;
				for (int j = 0; j < mHeaderLines.size(); j++)
				{
					for (int k = 0; k < pos; k++)
						mHeaderLines[j].insert(mHeaderLines[j].begin(), ' ');
					currFileLines.insert(currFileLines.begin() + i + repeat++, mHeaderLines[j]);
				}
				break;
			}
		}
	}
	std::ofstream outstream{ filepath.c_str(), std::ios::out };
	for (const auto& l : currFileLines)
		outstream << l << "\n";
	outstream.close();
}

void Editor::FlowChart::ParseBodies(AutoArray<HashString>& _allLines)
{
	static const std::string token = "_FS_NAME_";
	auto filepath = Dystopia::EngineCore::Get<Dystopia::FileSystem>()->FindFilePath("CharacterController.cpp", Dystopia::eFileDir::eCurrent);
	if (!filepath.length())
		return;

	std::vector<std::string> currFileLines = ClearBetweenCodes(filepath.c_str(), "//<AllStates>", "//</AllStates>");
	for (const auto& item : mItems)
	{
		/* convert to std vector of std string for now */
		/* need to insert at position for auto array, then will remove this */
		std::vector<std::string> mBodyLines;
		for (const auto& elem : _allLines)
			mBodyLines.push_back(elem.c_str());

		for (int i = 0; i < mBodyLines.size(); i++)
		{
			size_t pos = mBodyLines[i].find(token);
			size_t end;
			while (pos != std::string::npos)
			{
				end = pos + token.length();
				mBodyLines[i].erase(mBodyLines[i].begin() + pos, mBodyLines[i].begin() + end);
				mBodyLines[i].insert(pos, item.GetName().c_str());
				pos = mBodyLines[i].find(token, 0);
			}
		}

		for (int i = 0; i < currFileLines.size(); i++)
		{
			size_t pos = currFileLines[i].find("//</AllStates>");
			if (pos != std::string::npos)
			{
				int repeat = 0;
				for (int j = 0; j < mBodyLines.size(); j++)
				{
					for (int k = 0; k < pos; k++)
						mBodyLines[j].insert(mBodyLines[j].begin(), ' ');
					currFileLines.insert(currFileLines.begin() + i + repeat++, mBodyLines[j]);
				}
				break;
			}
		}
	}
	std::ofstream outstream{ filepath.c_str(), std::ios::out };
	for (const auto& l : currFileLines)
		outstream << l << "\n";
	outstream.close();
}

void Editor::FlowChart::ParseStates()
{
	auto filepath = Dystopia::EngineCore::Get<Dystopia::FileSystem>()->FindFilePath("CharacterController.h", Dystopia::eFileDir::eCurrent);
	if (!filepath.length())
		return;

	std::vector<std::string> currFileLines = ClearBetweenCodes(filepath.c_str(), "//<FStateTypes>", "//</FStateTypes>");
	for (int i = 0; i < currFileLines.size(); i++)
	{
		size_t pos = currFileLines[i].find("//</FStateTypes>");
		if (pos != std::string::npos)
		{
			int repeat = 0;
			for (int j = 0; j < mItems.size(); j++)
			{
				std::string word{ mItems[j].GetName().c_str() };
				word.append(",");
				for (size_t k = 0; k < pos; k++)
					word.insert(word.begin(), ' ');
				currFileLines.insert(currFileLines.begin() + i + repeat++, word);
			}
			break;
		}
	}
	
	std::ofstream outstream{ filepath.c_str(), std::ios::out };
	for (const auto& elem : currFileLines)
		outstream << elem << "\n";
	outstream.close();
}

AutoArray<HashString> Editor::FlowChart::ExtractAllLines(void* _stream, const char* _exitCode)
{
	std::ifstream& stream = *static_cast<std::ifstream*>(_stream);
	std::string line;
	AutoArray<HashString> allLines;
	while (std::getline(stream, line))
	{
		if (line == _exitCode)
			break;
		if (!line.length())
			line = "\n";
		allLines.Insert(line.c_str());
	}
	return allLines;
}

std::vector<std::string> Editor::FlowChart::ClearBetweenCodes(const char* _file, const char * _entry, const char* _exit)
{
	std::vector<std::string> currFileLines;
	std::string line;
	std::ifstream instream(_file, std::ios::in);
	while (std::getline(instream, line))
		currFileLines.push_back(line);
	instream.close();

	bool clearing = false;
	for (int i = 0; i < currFileLines.size(); i++)
	{
		if (clearing)
		{
			for (int j = i; j < currFileLines.size(); j++)
			{
				if (currFileLines[j].find(_exit) != std::string::npos)
					break;
				currFileLines.erase(currFileLines.begin() + j);
				j--;
			}
			break;
		}
		clearing = currFileLines[i].find(_entry) != std::string::npos;
	}
	return currFileLines;
}
#endif // EDITOR



