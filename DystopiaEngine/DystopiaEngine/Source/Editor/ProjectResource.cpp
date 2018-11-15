/* HEADER *********************************************************************************/
/*!
\file	ProjectResource.h
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
#include "Editor/ProjectResource.h"
#include "Editor/EGUI.h"
#include "Editor/EditorEvents.h"
#include "Editor/DefaultFactory.h"
#include "Editor/Payloads.h"

#include "System/Scene/Scene.h"
#include "System/Scene/SceneSystem.h"
#include "System/Graphics/GraphicsSystem.h"
#include "System/Graphics/Texture2D.h"

#include "Object/GameObject.h"

#include <algorithm>
#include <iostream>
#include <Windows.h>
#include <cstdlib>
#include <tchar.h>

static const HashString DEFAULT_PATH = "..\\DystopiaEngine\\Resource";
static const HashString DEFAULT_NAME = "Resource";
static float delay = 5;

namespace Dystopia
{
	static ProjectResource* gpInstance = 0;
	ProjectResource* ProjectResource::GetInstance()
	{
		if (gpInstance) return gpInstance;

		gpInstance = new ProjectResource{};
		return gpInstance;
	}

	ProjectResource::ProjectResource()
		: EditorTab{ false },
		mLabel{ "Project" }, mSearchText{ "" }, mSearchTextLastFrame{ "" }, mpRootFolder{ nullptr },
		mpCurrentFolder{ nullptr }, mArrAllFiles{100}, mArrFilesSearchedThisFrame{}, mArrFilesSearchedLastFrame{},
		mChangeHandle{}, mWaitStatus{}, mWaitFlags{}, mFocusedFile{ nullptr }, mPayloadRect{ 70, 90 },
		mResetToFile{}
	{}

	ProjectResource::~ProjectResource()
	{
		gpInstance = nullptr;
	}

	void ProjectResource::Init()
	{
		mWaitFlags = FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | 
					 FILE_NOTIFY_CHANGE_ATTRIBUTES | FILE_NOTIFY_CHANGE_SIZE | 
					 FILE_NOTIFY_CHANGE_LAST_WRITE;

		mpRootFolder = new Folder{ DEFAULT_NAME , DEFAULT_PATH, nullptr };
		mpCurrentFolder = mpRootFolder;
		FullCrawl(mpRootFolder);

		mArrAllFiles.clear();
		GetAllFiles(mArrAllFiles, mpRootFolder);
		SortAllFiles(mArrAllFiles);

		GetEditorEventHND()->GetEvent(eEditorEvents::EDITOR_LCLICK)->Bind(&ProjectResource::RemoveFocusOnFile, this);
		std::string path{ DEFAULT_PATH.c_str() };
		std::wstring wPath{ path.begin(), path.end() };
		mChangeHandle[0] = FindFirstChangeNotification(wPath.c_str(), true, mWaitFlags);
		if (mChangeHandle[0] == INVALID_HANDLE_VALUE || !mChangeHandle[0])
		{
			ExitProcess(GetLastError());
		}

		for (auto& f : mArrAllFiles)
		{
			if (f->mTag == EGUI::ePayloadTags::PNG || f->mTag == EGUI::ePayloadTags::BMP || f->mTag == EGUI::ePayloadTags::DDS)
			{
				EngineCore::GetInstance()->GetSystem<GraphicsSystem>()->LoadTexture(f->mPath.c_str());
			}
		}
	}

	void ProjectResource::Update(const float& _dt)
	{
		_dt;
		mWaitStatus = WaitForMultipleObjects(1, mChangeHandle, false, 0);
		switch (mWaitStatus)
		{
		case WAIT_OBJECT_0:
			RefreshResourceFolder();
			if (!FindNextChangeNotification(mChangeHandle[0]))
			{
				ExitProcess(GetLastError());
			}
			break;
		case WAIT_FAILED:
			ExitProcess(GetLastError());
			break;
		}
		UpdateSearch();
	}

	void ProjectResource::EditorUI()
	{
		if (mResetToFile.length())
		{
			FocusOnFile(mResetToFile.c_str());
			mResetToFile.clear();
		}

		Math::Vec2 fileWindowSize = Math::Vec2{ Size().x - 210, Size().y - 55 };
		SearchWindow();
		EGUI::Display::OpenTreeNode();
		FolderWindow();
		EGUI::SameLine(2);
		ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, ImVec4{ 0,0,0,0 });
		EGUI::StartChild("FileWindow", fileWindowSize);
		if (mpCurrentFolder)
		{
			auto origin = ImGui::GetCursorPos();
			EGUI::Display::Dummy(fileWindowSize.x, fileWindowSize.y);
			if (uint64_t *id = EGUI::Display::StartPayloadReceiver<uint64_t>(EGUI::GAME_OBJ))
			{
				GameObject *t = GetCurrentScene()->FindGameObject(*id);
				mResetToFile = Factory::SaveAsPrefab(*t).c_str();
				EGUI::Display::EndPayloadReceiver();
			}
			ImGui::SetCursorPos(origin);
			if (!strlen(mSearchText))
				FileWindow(fileWindowSize);
			else
				SearchResultWindow(fileWindowSize);
		}
		EGUI::EndChild();
		ImGui::PopStyleColor();
	}

	void ProjectResource::Shutdown()
	{
		mArrFilesSearchedThisFrame.clear();
		mArrFilesSearchedLastFrame.clear();
		mArrAllFiles.clear();
		delete mpRootFolder;
		mFocusedFile = nullptr;
		mpRootFolder = nullptr;
		mpCurrentFolder = nullptr;

		GetEditorEventHND()->GetEvent(eEditorEvents::EDITOR_LCLICK)->Unbind(this);
	}

	void ProjectResource::UpdateSearch()
	{
		HashString currentSearch = mSearchText;
		HashString previousSearch = mSearchTextLastFrame;
		if (currentSearch.length() && currentSearch != previousSearch)
		{
			mArrFilesSearchedThisFrame.clear();
			MakeStringLower(currentSearch);
			MakeStringLower(previousSearch);
			if ((currentSearch.find(previousSearch) != std::string::npos) && mArrFilesSearchedLastFrame.size())
			{
				FindFile(mArrFilesSearchedThisFrame, currentSearch, mArrFilesSearchedLastFrame);
			}
			else
			{
				FindFile(mArrFilesSearchedThisFrame, currentSearch, mArrAllFiles);
			}
			mArrFilesSearchedLastFrame.clear();
			mArrFilesSearchedLastFrame = mArrFilesSearchedThisFrame;
			strcpy_s(mSearchTextLastFrame, mSearchText);
		}
		else if (!currentSearch.length() && previousSearch.length())
		{
			mArrFilesSearchedLastFrame.clear();
		}
	}

	void ProjectResource::SearchWindow()
	{
		float width = Size().x - 20;
		width = (width < 20) ? 20 : width;
		EGUI::Indent(4);
		EGUI::ChangeLabelSpacing(10);
		EGUI::Display::TextField("Search", mSearchText, MAX_SEARCH, false, width);
		EGUI::ChangeLabelSpacing();
		EGUI::UnIndent(4);
		EGUI::Display::HorizontalSeparator();
	}

	void ProjectResource::FolderWindow()
	{
		EGUI::StartChild("FolderWindow", Math::Vec2{ 200, Size().y - 55 });
		FolderUI(mpRootFolder);
		EGUI::EndChild();
	}

	void ProjectResource::FileWindow(const Math::Vec2& _mySize)
	{
		const Math::Vec2 buffedSize{ mPayloadRect.x * 1.25f, mPayloadRect.y * 1.5f };
		unsigned int columns = static_cast<unsigned int>(_mySize.x / buffedSize.x);
		columns = columns ? columns : 1 ;

		EGUI::Display::Label(mpCurrentFolder->mPath.c_str());
		EGUI::Display::HorizontalSeparator();
		for (unsigned int i = 0; i < mpCurrentFolder->mArrPtrFiles.size(); ++i)
		{
			File* pFile = mpCurrentFolder->mArrPtrFiles[i];
			HashString id = "ProjectResourceFileWindow" + pFile->mName + HashString{ std::to_string(i).c_str() };
			if (i % columns) EGUI::SameLine();
			if (EGUI::StartChild(id.c_str(), buffedSize, false, Math::Vec4{ 0,0,0,0 }))
			{
				EGUI::Indent(10);
				EGUI::PushID(i);
				FileUI(pFile);
				EGUI::PopID();
				EGUI::UnIndent(10);
			}
			EGUI::EndChild();
		}
	}
	
	void ProjectResource::SearchResultWindow(const Math::Vec2& _mySize)
	{
		const Math::Vec2 buffedSize{ mPayloadRect.x * 1.25f, mPayloadRect.y * 1.5f };
		unsigned int columns = static_cast<unsigned int>(_mySize.x / buffedSize.x);
		columns = columns ? columns : 1;

		EGUI::Display::Label("Searching: %s", mSearchText);
		EGUI::Display::HorizontalSeparator();
		size_t size = mArrFilesSearchedThisFrame.size();
		if (size)
		{
			for (unsigned int i = 0; i < size; ++i)
			{
				File* pFile = mArrFilesSearchedThisFrame[i];
				const HashString id = "ProjectResourceSearchResultWindow" + pFile->mName + HashString{ std::to_string(i).c_str() };
				if (i % columns) EGUI::SameLine();
				if (EGUI::StartChild(id.c_str(), buffedSize, false, Math::Vec4{ 0,0,0,0 }))
				{
					EGUI::Indent(10);
					EGUI::PushID(i);
					FileUI(pFile);
					EGUI::PopID();
					EGUI::UnIndent(10);
				}
				EGUI::EndChild();
			}
		}
		else
		{
			EGUI::Display::Label("%s cannot be found", mSearchText);
		}
	}

	void ProjectResource::RefreshResourceFolder()
	{
		if (strlen(mSearchText))
		{
			strcpy_s(mSearchText, "");
			strcpy_s(mSearchTextLastFrame, "");
		}
		HashString currentSelectionName = mpCurrentFolder ? mpCurrentFolder->mName : "";

		mArrAllFiles.clear();
		mArrFilesSearchedThisFrame.clear();
		mArrFilesSearchedLastFrame.clear();

		delete mpRootFolder;
		mpRootFolder = new Folder{ DEFAULT_NAME , DEFAULT_PATH, nullptr };;

		FullCrawl(mpRootFolder);
		GetAllFiles(mArrAllFiles, mpRootFolder);
		SortAllFiles(mArrAllFiles);
		mpCurrentFolder = FindFolder(currentSelectionName);
	}

	void ProjectResource::FocusOnFile(const HashString& _fileName)
	{
		strcpy_s(mSearchText, ""); 
		strcpy_s(mSearchTextLastFrame, "");
		mArrFilesSearchedLastFrame.clear();
		mArrFilesSearchedThisFrame.clear(); 
		if (FindFirstOne(mArrFilesSearchedThisFrame, _fileName))
		{
			mpCurrentFolder = mArrFilesSearchedThisFrame[0]->mpParentFolder;
			mFocusedFile = mArrFilesSearchedThisFrame[0];
		}
		else
		{
			mpCurrentFolder = mpRootFolder;
			mFocusedFile = nullptr;
		}
	}

	void ProjectResource::RemoveFocusOnFile()
	{
		mFocusedFile = nullptr;
	}

	std::string ProjectResource::GetLabel() const
	{
		return mLabel;
	}

	Folder* ProjectResource::FindFolder(const HashString& _name)
	{
		if (_name.length())
		{
			Folder* pFound = mpRootFolder->FindFolder(_name);
			if (pFound) 
				return pFound;
		}	
		return mpRootFolder;
	}

	void ProjectResource::FindFile(AutoArray<File*>& _outResult, HashString& _item, const AutoArray<File*>& _fromArr)
	{
		MakeStringLower(_item);
		for (auto& e : _fromArr)
		{
			if (!e->mLowerCaseName.find(_item))
				_outResult.push_back(e);
		}
	}

	bool ProjectResource::FindFirstOne(AutoArray<File*>& _outResult, const HashString& _item)
	{
		for (auto& e : mArrAllFiles)
		{
			if (!e->mName.find(_item))
			{
				_outResult.push_back(e);
				return true;
			}
		}
		return false;
	}

	void ProjectResource::GetAllFiles(AutoArray<File*>& _outResult, Folder* _folder)
	{
		for (auto& e : _folder->mArrPtrFiles)
			_outResult.push_back(e);
		for (auto& e : _folder->mArrPtrFolders)
			GetAllFiles(_outResult, e);
	}

	void ProjectResource::SortAllFiles(AutoArray<File*>& _outResult)
	{
		_outResult.Sort([](File* lhs, File* rhs)->bool { return *lhs < *rhs; });
	}

	void ProjectResource::FolderUI(Folder* _folder)
	{
		bool clickedThisFrame = false;
		bool clickedFolderIcon = false;
		bool flagIt = (mpCurrentFolder == _folder) ? true : false;
		bool hideArrow = _folder->mArrPtrFolders.size() ? false : true;
		if (EGUI::Display::IconFolder(_folder->mLowerCaseName.c_str(), 15, 9, flagIt))
		{
			clickedFolderIcon = true;
			EGUI::Display::OpenTreeNode();
		}
		EGUI::SameLine();
		if (EGUI::Display::StartTreeNode(_folder->mName.c_str(), &clickedThisFrame, flagIt, hideArrow))
		{
			for (auto& e : _folder->mArrPtrFolders)
				FolderUI(e);
			EGUI::Display::EndTreeNode();
		}
		if (clickedThisFrame || clickedFolderIcon)
		{
			mFocusedFile = nullptr;
			mpCurrentFolder = _folder;
		}
	}

	void ProjectResource::FileUI(File* _file)
	{
		if (_file == mFocusedFile) EGUI::Display::Outline(mPayloadRect.x, mPayloadRect.y);

		int id = -1;
		if (_file->mTag == EGUI::ePayloadTags::PNG || _file->mTag == EGUI::ePayloadTags::BMP || _file->mTag == EGUI::ePayloadTags::DDS)
		{
			id = static_cast<int>(EngineCore::GetInstance()->GetSystem<GraphicsSystem>()->LoadTexture(_file->mPath.c_str())->GetID());
		}

		if (EGUI::Display::CustomPayload(("###ProjectView" + _file->mName).c_str(), _file->mName.c_str(), 
			_file->mName.c_str(), mPayloadRect, _file->mTag, &(*_file), sizeof(File), id))
		{
			mFocusedFile = _file;
		}
	}

	void ProjectResource::FullCrawl(Folder* _folder)
	{
		_folder->Crawl();
		for (auto& e : _folder->mArrPtrFolders)
			FullCrawl(e);
	}

	void ProjectResource::MakeStringLower(HashString& _transformMe)
	{
		//std::transform(_transformMe.begin(), _transformMe.end(), _transformMe.begin(), my_tolower);
		auto start = _transformMe.begin();
		for (auto const & elem : _transformMe)
		{
			*start = my_tolower(elem);
			start++;
		}
	}

}

#endif //EDITOR

