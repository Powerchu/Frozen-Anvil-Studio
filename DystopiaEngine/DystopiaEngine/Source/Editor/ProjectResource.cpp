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
#include <iostream>
#include <Windows.h>
#include <stdlib.h>
#include <tchar.h>

static const std::string DEFAULT_PATH = "..\\DystopiaEngine";
static const std::string DEFAULT_NAME = "DystopiaEngine";

static float delay = 5;
namespace Dystopia
{
	/********************************************************************* FILE & FOLDER *********************************************************************/
	CrawlItem::CrawlItem(const std::string& _name, const std::string& _path)
		: mName{ _name }, mPath{ _path }
	{}

	Folder::Folder(const std::string& _name, const std::string& _path, Folder * const _parent)
		: CrawlItem{ _name, _path }, mpParentFolder{ _parent }, mArrPtrFiles{}, mArrPtrFolders{}
	{}

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
		for (auto e : mArrPtrFiles)
			delete e;
		for (auto e : mArrPtrFolders)
			delete e;
		mArrPtrFiles.clear();
		mArrPtrFolders.clear();
	}

	File::File(const std::string& _name, const std::string& _path, Folder * const _parent)
		: CrawlItem{ _name, _path }, mpParentFolder{ _parent }
	{}

	File::~File()
	{
		mpParentFolder = nullptr;
	}
	
	bool File::operator<(const File& rhs)
	{
		 return mName.compare(rhs.mName) <= 0;
	}

	/****************************************************************** PROJECT RESOURCE ********************************************************************/

	ProjectResource::ProjectResource()
		: mLabel{ "Resource View" }, mSearchText{}, mpRootFolder{ nullptr }, mpCurrentFolder{ nullptr }, 
		mArrAllFiles{}, mSearchResultFiles{}, mSearchTextLastFrame{}
	{}

	ProjectResource::~ProjectResource()
	{}

	void ProjectResource::Init()
	{
		mpRootFolder = new Folder{ DEFAULT_NAME , DEFAULT_PATH, nullptr };
		FullCrawl(mpRootFolder);
		mArrAllFiles.clear();
		GetAllFiles(mArrAllFiles, mpRootFolder);
		SortAllFiles(mArrAllFiles);
	}

	void ProjectResource::Update(const float& _dt)
	{
		_dt;
		std::string searchString = mSearchText;
		if (searchString.compare(mSearchTextLastFrame))
		{
			mSearchResultFiles.clear();
			FindFile(mSearchResultFiles, mSearchText);
			strcpy_s(mSearchTextLastFrame, mSearchText);
		}
	}

	void ProjectResource::Window()
	{
		SearchWindow();
		FolderWindow();
		EGUI::SameLine(254);
		if (!strlen(mSearchText))
			FileWindow();
		else
			SearchResultWindow();
	}

	void ProjectResource::SearchWindow()
	{
		EGUI::Indent(5);
		EGUI::Display::TextField("SearchWindow", mSearchText, MAX_SEARCH);
		EGUI::UnIndent(5);
		EGUI::Display::HorizontalSeparator();
	}

	void ProjectResource::FolderWindow()
	{
		EGUI::StartChild("FolderWindow", Math::Vec2{ 250, Size().y - 55 });
		FolderUI(mpRootFolder);
		EGUI::EndChild();
	}

	void ProjectResource::FileWindow()
	{
		EGUI::StartChild("FileWindow", Math::Vec2{ Size().x - 260, Size().y - 55 });
		if (mpCurrentFolder)
		{
			EGUI::Display::Label(mpCurrentFolder->mPath.c_str());
			EGUI::Display::HorizontalSeparator();
			for (unsigned int i = 0; i < mpCurrentFolder->mArrPtrFiles.size(); ++i)
			{
				EGUI::Indent(10);
				EGUI::PushID(i);
				FileUI(mpCurrentFolder->mArrPtrFiles[i]);
				EGUI::PopID();
				EGUI::UnIndent(10);
			}
		}
		EGUI::EndChild();
	}
	
	void ProjectResource::SearchResultWindow()
	{
		EGUI::StartChild("SearchResultWindow", Math::Vec2{ Size().x - 260, Size().y - 55 });

		EGUI::Display::Label("Searching: %s", mSearchText);
		EGUI::Display::HorizontalSeparator();
		for (unsigned int i = 0; i < mSearchResultFiles.size(); ++i)
		{
			EGUI::Indent(10);
			EGUI::PushID(i);
			FileUI(mSearchResultFiles[i]);
			EGUI::PopID();
			EGUI::UnIndent(10);
		}
		EGUI::EndChild();
	}

	void ProjectResource::FocusOnFile(const std::string& _fileName)
	{
		strcpy_s(mSearchText, "");
		mSearchResultFiles.clear();
		FindFile(mSearchResultFiles, _fileName);

		if (mSearchResultFiles.size() == 1) mpCurrentFolder = mSearchResultFiles[0]->mpParentFolder;
	}

	void ProjectResource::Shutdown()
	{
		mSearchResultFiles.clear();
		mArrAllFiles.clear();
		delete mpRootFolder;
		mpRootFolder = nullptr;
		mpCurrentFolder = nullptr;
	}

	std::string ProjectResource::GetLabel() const
	{
		return mLabel;
	}

	void ProjectResource::FindFile(AutoArray<File*>& _outResult, const std::string& _item)
	{
		for (auto e : mArrAllFiles)
		{
			if (!e->mName.find(_item)) _outResult.push_back(e);
		}
	}

	void ProjectResource::GetAllFiles(AutoArray<File*>& _outResult, Folder* _folder)
	{
		for (auto e : _folder->mArrPtrFiles)
			_outResult.push_back(e);
		for (auto e : _folder->mArrPtrFolders)
			GetAllFiles(_outResult, e);
	}

	void ProjectResource::SortAllFiles(AutoArray<File*>& _outResult)
	{
		_outResult.Sort([](File* lhs, File* rhs)->bool { return *lhs < *rhs; });
	}

	void ProjectResource::FolderUI(Folder* _folder)
	{
		bool clickedThisFrame = false;
		if (_folder->mArrPtrFolders.size())
		{
			if (EGUI::Display::StartTreeNode(_folder->mName.c_str(), &clickedThisFrame))
			{
				for (auto e : _folder->mArrPtrFolders)
					FolderUI(e);
				EGUI::Display::EndTreeNode();
			}
			else
			{
				if (clickedThisFrame && !strlen(mSearchText))
				{
					_folder->ClearFolder();
					FullCrawl(_folder);
					mArrAllFiles.clear();
					GetAllFiles(mArrAllFiles, mpRootFolder);
					SortAllFiles(mArrAllFiles);
				}
			}
			mpCurrentFolder = (clickedThisFrame) ? _folder : mpCurrentFolder;
		}
		else
		{
			EGUI::Indent(20);
			if (EGUI::Display::SelectableTxt(_folder->mName.c_str()))
			{
				mpCurrentFolder = _folder;
			}
			EGUI::UnIndent(20);
		}
	}

	void ProjectResource::FileUI(File* _file)
	{
		EGUI::Display::Button(_file->mName.c_str(), Math::Vec2{ 200, 20 });
		if (EGUI::Display::StartPayload(EGUI::FILE, &(*_file), sizeof(File), _file->mName.c_str()))
		{
			EGUI::Display::EndPayload();
		}
	}

	void ProjectResource::FullCrawl(Folder* _folder)
	{
		_folder->Crawl();
		for (auto e : _folder->mArrPtrFolders)
			FullCrawl(e);
	}

}

#endif //EDITOR


