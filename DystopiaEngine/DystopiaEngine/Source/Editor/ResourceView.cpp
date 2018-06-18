/* HEADER *********************************************************************************/
/*!
\file	ResourceView.cpp
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
#include "Editor\ResourceView.h"
#include "Editor\EGUI.h"
#include <iostream>
#include <Windows.h>

constexpr float DEFAULT_WIDTH = 300;
constexpr float DEFAULT_HEIGHT = 300;

namespace Dystopia
{
	ResourceView::ResourceView()
		: mWidth{ DEFAULT_WIDTH }, mHeight{ DEFAULT_HEIGHT }, mpFocusData{ nullptr }, mpCrawlData{ nullptr }, mpCurrentFolder{ nullptr }, mSelectedID{ 0 }
	{
	}
	
	ResourceView::~ResourceView()
	{
	}
	
	void ResourceView::Init()
	{
		StartCrawl();
	}

	void ResourceView::Update(const float& _dt)
	{
		_dt;
	}

	void ResourceView::Shutdown()
	{
	}

	void ResourceView::SetWidth(float _width)
	{
		mWidth = _width;
	}

	void ResourceView::SetHeight(float _height)
	{
		mHeight = _height;
	}

	void ResourceView::Window()
	{
		int i = 0;

		if (EGUI::Display::CollapseHeader(mpCurrentFolder->mFolderName.c_str()))
		{
			for (auto e : mpCurrentFolder->mArrChildFolders)
			{
				if (EGUI::Display::SelectableTxtDouble((e->mFolderName).c_str(), false))
				{
					mSelectedID = i;
					std::cout << "Double clicked folder with current ID: " << mSelectedID << std::endl;
					for (unsigned int j = 0; j < mpCurrentFolder->mArrChildFolders.size(); ++j)
					{
						if (j == static_cast<unsigned int>(i))
						{
							mpCurrentFolder = mpCurrentFolder->mArrChildFolders[j];
							mpCurrentFolder->Crawl();
							break;
						}
					}
				}
				i++;
			}
			for (auto e : mpCurrentFolder->mArrFiles)
			{
				if (EGUI::Display::SelectableTxt((e.mFileName).c_str(), false))
				{
					mSelectedID = i;
					std::cout << "Single clicked file with current ID: " << mSelectedID << std::endl;
				}
				i++;
			}
		}
	}

	void ResourceView::StartCrawl()
	{
		mpCrawlData = new CrawlFolder{ GLOBAL_DEFAULT_PROJECT_PATH.c_str() };
		mpCrawlData->Crawl();
		mpCrawlData->PrintAll();
		mpCurrentFolder = mpCrawlData;
	}

	ResourceView::CrawlFile::CrawlFile(const std::string& _name)
		: mFileName{ _name }
	{}

	ResourceView::CrawlFolder::CrawlFolder(const char* _myName)
		: mFolderName{ _myName }
	{}

	ResourceView::CrawlFolder::CrawlFolder(const std::string &_myName)
		: mFolderName{ _myName }
	{}

	ResourceView::CrawlFolder::~CrawlFolder()
	{
		for (auto e : mArrChildFolders)
			delete e;
	}

	void ResourceView::CrawlFolder::AddFolder(CrawlFolder *_subFolder)
	{
		mArrChildFolders.EmplaceBack(_subFolder);
	}

	void ResourceView::CrawlFolder::AddFile(const std::string &_fileName)
	{
		mArrFiles.EmplaceBack(CrawlFile{ _fileName });
	}

	void ResourceView::CrawlFolder::Crawl()
	{
		WIN32_FIND_DATAA data;
		std::string dataBuffer, pathBuffer{ mFolderName };
		pathBuffer += "\\*";
		std::cout << "Crawling : " << pathBuffer << std::endl;
		HANDLE hfind = FindFirstFileA(pathBuffer.c_str(), &data);

		if (hfind != INVALID_HANDLE_VALUE)
		{
			do
			{
				if (strcmp(data.cFileName, ".") && strcmp(data.cFileName, "..") && data.dwFileAttributes != FILE_ATTRIBUTE_HIDDEN)
				{
					if (data.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY && !FolderExist(mFolderName + "\\" + data.cFileName))
					{
						CrawlFolder *pfolder = new CrawlFolder{ mFolderName + "\\" + data.cFileName };
						pfolder->SetParent(this);
						AddFolder(pfolder);
						dataBuffer += "<DIR>";
					}
					else if (!FileExist(data.cFileName))
						AddFile(data.cFileName);

					dataBuffer += data.cFileName;
					std::cout << dataBuffer << std::endl;
					dataBuffer.clear();
				}
			} while (FindNextFileA(hfind, &data));
			FindClose(hfind);
		}
	}

	void ResourceView::CrawlFolder::SetParent(CrawlFolder* _parent)
	{
		mpParentFolder = _parent;
	}

	void ResourceView::CrawlFolder::PrintAll()
	{
		std::cout << "\n======Start of Folder Data======\nIn this folder [ " << mFolderName << " ]\n";
		std::cout << "\nFolders: \n";
		for (auto e : mArrChildFolders)
			std::cout << "   " << e->mFolderName << std::endl;
		std::cout << "Files: \n";
		for (auto e : mArrFiles)
			std::cout << "   " << e.mFileName << std::endl;
		std::cout << "\n======End of Folder Data======\n";
		
	}

	bool ResourceView::CrawlFolder::FileExist(const std::string& _name)
	{
		for (auto& e : mArrFiles)
			if (e.mFileName.compare(_name) == 0) return true;
		return false;
	}

	bool ResourceView::CrawlFolder::FolderExist(const std::string& _name)
	{
		for (auto e : mArrChildFolders)
			if (e->mFolderName.compare(_name) == 0) return true;
		return false;
	}
}




#endif // EDITOR





