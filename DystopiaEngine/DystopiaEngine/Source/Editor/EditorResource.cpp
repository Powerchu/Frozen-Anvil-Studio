/* HEADER *********************************************************************************/
/*!
\file	EditorResource.cpp
\author Digipen (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#include "Editor/EditorResource.h"
#include "Editor/EditorMain.h"
#include "Editor/EditorFactory.h"
#include "Editor/EditorClipboard.h"
#include "Editor/Payloads.h"
#include "Editor/ConsoleLog.h"

#include "Component/Component.h"

#include "System/Driver/Driver.h"
#include "System/File/FileSystem.h"

#include <string>
#include <Windows.h>

Editor::EditorResource::EditorResource(void)
	: mCurrProjectPath{ "" }, mArrComponentResource{}
{
}

Editor::EditorResource::~EditorResource(void)
{
}

void Editor::EditorResource::Load(void)
{
}

bool Editor::EditorResource::Init(void)
{
	return true;
}

void Editor::EditorResource::StartFrame(void)
{
}

void Editor::EditorResource::Update(float)
{
}

void Editor::EditorResource::EndFrame(void)
{
	for (size_t i = 0; i < mArrComponentResource.size(); ++i)
	{
		if (mArrComponentResource[i]->GetFlags() & Dystopia::eObjFlag::FLAG_REMOVE)
		{
			mArrComponentResource.FastRemove(i);
			--i;
		}
	}
}

void Editor::EditorResource::Shutdown(void)
{
}

void Editor::EditorResource::Message(eEMessage)
{
	//if (_msg == eEMessage::SCENE_ABOUT_TO_CHANGE)
	//	mArrComponentResource.clear();
}

void Editor::EditorResource::SaveSettings(Dystopia::TextSerialiser& _out) const
{
	_out.InsertStartBlock("Editor Resource");
	//_out << mCurrProjectPath.c_str();
	_out.InsertEndBlock("Editor Resource");
}

void Editor::EditorResource::LoadSettings(Dystopia::TextSerialiser& _in)
{
	_in.ConsumeStartBlock();
	//_in >> mCurrProjectPath;
	_in.ConsumeEndBlock();
}

void Editor::EditorResource::AddComponent(Dystopia::Component* _p, const uint64_t& _nID, bool _amComponent)
{
	mArrComponentResource.Insert(_p);
	EditorMain::GetInstance()->GetSystem<EditorFactory>()->ReattachToPrefab(_p, _nID, _amComponent);
}

void Editor::EditorResource::RemoveComponent(const uint64_t& _cID)
{
	for (size_t i = 0; i < mArrComponentResource.size(); ++i)
	{
		if (mArrComponentResource[i]->GetID() == _cID)
		{
			mArrComponentResource.FastRemove(i);
			return;
		}
	}
}
void Editor::EditorResource::DestroyDelegate(const uint64_t& _cID)
{
	for (size_t i = 0; i < mArrComponentResource.size(); ++i)
	{
		if (mArrComponentResource[i]->GetID() == _cID)
		{
			mArrComponentResource[i]->RemoveFlags(Dystopia::eObjFlag::FLAG_EDITOR_OBJ);
			mArrComponentResource[i]->DestroyComponent();
			mArrComponentResource.FastRemove(i);
			return;
		}
	}
}

Dystopia::Component* Editor::EditorResource::GetComponent(const uint64_t& _cID)
{
	for (auto c : mArrComponentResource)
		if (c->GetID() == _cID)
			return c;

	return nullptr;
}

HashString Editor::EditorResource::GetCurrProjectPath(void) const
{
	return mCurrProjectPath;
}

void Editor::EditorResource::ExternalFileHandling(const char* _file)
{
	HashString folder, fileName{ _file };
	size_t pos = fileName.rfind("\\");
	if (pos != HashString::nPos)
		fileName.erase(0, pos + 1);
	auto fs = Dystopia::EngineCore::Get<Dystopia::FileSystem>();

	switch (DeterminePathType(_file))
	{
	case EGUI::ePayloadTags::PNG:
	case EGUI::ePayloadTags::BMP:
	case EGUI::ePayloadTags::DDS:
		folder = fs->GetProjectFolders<std::string>(Dystopia::eFileDir::eResource).c_str();
		folder += "/Asset/";
		break;
	case EGUI::ePayloadTags::PREFAB:
		folder = fs->GetProjectFolders<std::string>(Dystopia::eFileDir::eResource).c_str();
		folder += "/Prefab/";
		break;
	case EGUI::ePayloadTags::SCENE:
		folder = fs->GetProjectFolders<std::string>(Dystopia::eFileDir::eResource).c_str();
		folder += "/Scene/";
		break;
	case EGUI::ePayloadTags::MP3:
	case EGUI::ePayloadTags::WAV:
		folder = fs->GetProjectFolders<std::string>(Dystopia::eFileDir::eResource).c_str();
		folder += "/Audio/";
		break;
	default:
		return;
	}
	
	if (folder.length())
	{
		std::string file{ _file };
		folder += fileName;
		std::wstring owfile{ file.begin(), file.end() };
		std::wstring nwfile{ folder.begin(), folder.end() };
		auto res = CopyFile(owfile.c_str(), nwfile.c_str(), false);
		HashString result{ "Importing from " };
		result += _file;
		if (res)
		{
			result += " to ";
			result += folder;
			result += " was Successful";
			PrintToConsoleLog(result);
		}
		else
		{
			result += " has Failed!";
			PrintToConsoleLog(result);
		}
	}
}

#endif 











