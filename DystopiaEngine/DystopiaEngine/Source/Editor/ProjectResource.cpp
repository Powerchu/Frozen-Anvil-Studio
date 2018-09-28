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
#include "Editor\ProjectResource.h"
#include "Editor\EGUI.h"
#include "Editor\EditorEvents.h"
#include <algorithm>
#include <iostream>
#include <Windows.h>
#include <stdlib.h>
#include <tchar.h>

static const std::string DEFAULT_PATH = "..\\DystopiaEngine\\Resource";
static const std::string DEFAULT_NAME = "Resource";
static float delay = 5;

namespace Dystopia
{
	/********************************************************************* FILE & FOLDER *********************************************************************/

	CrawlItem::CrawlItem(const std::string& _name, const std::string& _path)
		: mName{ _name }, mPath{ _path }, mLowerCaseName{ mName }
	{
		std::transform(mLowerCaseName.begin(), mLowerCaseName.end(), mLowerCaseName.begin(), ::tolower);
	}

	Folder::Folder(const std::string& _name, const std::string& _path, Folder * const _parent)
		: CrawlItem{ _name, _path }, mpParentFolder{ _parent }, mArrPtrFiles{}, mArrPtrFolders{}
	{
	}

	Folder::~Folder()
	{
		ClearFolder();
		mpParentFolder = nullptr;
	}

	void Folder::Crawl()
	{
		WIN32_FIND_DATAA data;
		std::string pathBuffer = mPath + "\\*";
		HANDLE hfind = FindFirstFileA(pathBuffer.c_str(), &data);
		if (hfind != INVALID_HANDLE_VALUE)
		{
			do
			{
				if (strcmp(data.cFileName, ".") && strcmp(data.cFileName, "..") && data.dwFileAttributes != FILE_ATTRIBUTE_HIDDEN)
				{
					if (data.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
						mArrPtrFolders.push_back(new Folder{ data.cFileName , mPath + "\\" + data.cFileName, this });
					else
						mArrPtrFiles.push_back(new File{ data.cFileName, mPath + "\\" + data.cFileName, this });
				}
			} while (FindNextFileA(hfind, &data));
			FindClose(hfind);
		}
	}

	void Folder::ClearFolder()
	{
		for (auto& e : mArrPtrFiles)
		{
			delete e;
			e = nullptr;
		}
		for (auto& e : mArrPtrFolders)
		{
			delete e;
			e = nullptr;
		}
		mArrPtrFiles.clear();
		mArrPtrFolders.clear();
	}

	File::File(const std::string& _name, const std::string& _path, Folder * const _parent)
		: CrawlItem{ _name, _path }, mpParentFolder{ _parent }, mTag{ DetermineTag(_name) }
	{}

	File::~File()
	{
		mpParentFolder = nullptr;
	}
	
	bool File::operator<(const File& rhs)
	{
		 return mName.compare(rhs.mName) <= 0;
	}
	
	EGUI::ePayloadTags File::DetermineTag(const std::string& _name)
	{
		if (_name.find(".png") == _name.length() - 4)
			return EGUI::ePayloadTags::PNG;
		else if (_name.find(".bmp") == _name.length() - 4)
			return EGUI::ePayloadTags::BMP;
		else if (_name.find(".cpp") == _name.length() - 4)
			return EGUI::ePayloadTags::FILE;

		return EGUI::ePayloadTags::UNKNOWN;
	}

	/****************************************************************** PROJECT RESOURCE ********************************************************************/
	
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
		mpCurrentFolder{ nullptr }, mArrAllFiles{}, mArrFilesSearchedThisFrame{}, mArrFilesSearchedLastFrame{},
		mChangeHandle{}, mWaitStatus{}, mWaitFlags{}, mFocusedFile{ nullptr }, mPayloadRect{ 70, 90 }
	{}

	ProjectResource::~ProjectResource()
	{
		GetEditorEventHND()->GetEvent(eEditorEvents::EDITOR_LCLICK)->Unbind(this);
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

		std::wstring wPath{ DEFAULT_PATH.begin(), DEFAULT_PATH.end() };
		mChangeHandle[0] = FindFirstChangeNotification(wPath.c_str(), true, mWaitFlags);
		if (mChangeHandle[0] == INVALID_HANDLE_VALUE || !mChangeHandle[0])
		{
			ExitProcess(GetLastError());
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
		Math::Vec2 fileWindowSize = Math::Vec2{ Size().x - 210, Size().y - 55 };
		SearchWindow();
		EGUI::Display::OpenTreeNode();
		FolderWindow();
		EGUI::SameLine(2);
		ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, ImVec4{ 0,0,0,0 });
		EGUI::StartChild("FileWindow", fileWindowSize);
		if (!strlen(mSearchText))
			FileWindow(fileWindowSize);
		else
			SearchResultWindow(fileWindowSize);
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
	}

	void ProjectResource::UpdateSearch()
	{
		std::string currentSearch = mSearchText;
		std::string previousSearch = mSearchTextLastFrame;
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

		if (mpCurrentFolder)
		{ 
			EGUI::Display::Label(mpCurrentFolder->mPath.c_str());
			EGUI::Display::HorizontalSeparator();
			for (unsigned int i = 0; i < mpCurrentFolder->mArrPtrFiles.size(); ++i)
			{
				File* pFile = mpCurrentFolder->mArrPtrFiles[i];
				std::string id = "ProjectResourceFileWindow" + pFile->mName + std::to_string(i);
				if (i % columns) EGUI::SameLine();
				if (EGUI::StartChild(id.c_str(), buffedSize, false, Math::Vec4{0,0,0,0}))
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
				const std::string id = "ProjectResourceSearchResultWindow" + pFile->mName + std::to_string(i);
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
		if (strlen(mSearchText)) strcpy_s(mSearchText, "");
		std::string currentSelectionName = mpCurrentFolder ? mpCurrentFolder->mName : "";

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

	void ProjectResource::FocusOnFile(const std::string& _fileName)
	{
		strcpy_s(mSearchText, "");
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

	Folder* ProjectResource::FindFolder(const std::string& _name)
	{
		if (_name.length())
		{
			for (auto& e : mArrAllFiles)
			{
				if (e->mpParentFolder->mName == _name)
					return e->mpParentFolder;
			}
		}
		return mpRootFolder;
	}

	void ProjectResource::FindFile(AutoArray<File*>& _outResult, std::string& _item, const AutoArray<File*>& _fromArr)
	{
		MakeStringLower(_item);
		for (auto& e : _fromArr)
		{
			if (!e->mLowerCaseName.find(_item))
				_outResult.push_back(e);
		}
	}

	bool ProjectResource::FindFirstOne(AutoArray<File*>& _outResult, const std::string& _item)
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
		if (EGUI::Display::IconFolder(_folder->mLowerCaseName, 15, 9, flagIt))
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

		if (EGUI::Display::CustomPayload(("###ProjectView" + _file->mName).c_str(), _file->mName.c_str(), 
			_file->mName.c_str(), mPayloadRect, _file->mTag, &(*_file), sizeof(File)))
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

	void ProjectResource::MakeStringLower(std::string& _transformMe)
	{
		std::transform(_transformMe.begin(), _transformMe.end(), _transformMe.begin(), ::tolower);
	}

}

#endif //EDITOR

