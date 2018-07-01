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
#include "../../Dependancies/ImGui/imgui.h"

constexpr float DEFAULT_WIDTH = 300;
constexpr float DEFAULT_HEIGHT = 300;

namespace Dystopia
{
	ResourceView::ResourceView()
		: mWidth{ DEFAULT_WIDTH }, mHeight{ DEFAULT_HEIGHT }, mpFocusData{ nullptr }, mpCrawlData{ nullptr }, 
		mLastSelected{ -1 }, mRefreshCrawl{ false }
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
		delete mpCrawlData;
	}

	void ResourceView::SetWidth(const float& _width)
	{
		mWidth = _width;
	}

	void ResourceView::SetHeight(const float& _height)
	{
		mHeight = _height;
	}

	void ResourceView::SetSize(const Math::Vec4& _size)
	{
		SetSize(_size.x, _size.y);
	}

	void ResourceView::SetSize(const float& _x, const float& _y)
	{
		SetWidth(_x);
		SetHeight(_y);
	}

	void ResourceView::Window()
	{
		if (EGUI::Display::CollapsingHeader(mpCrawlData->mFolderName.c_str()))
		{
			if (mRefreshCrawl)
			{
				for (auto e : mpCrawlData->mArrChildFolders)
					delete e;
				mpCrawlData->mArrChildFolders.clear();
				mpCrawlData->mArrFiles.clear();
				mpCrawlData->Crawl();
				mRefreshCrawl = false;
				for (auto e : mpCrawlData->mArrChildFolders)
					EGUI::Display::OpenTreeNode(e->mFolderName.c_str(), false);
			}

			for (auto e : mpCrawlData->mArrChildFolders)
				FolderInterface(e);
			for (auto &e : mpCrawlData->mArrFiles)
				FileInterface(e);
		}
		else
		{
			if (!mRefreshCrawl)
				mRefreshCrawl = true;
		}
	}

	void ResourceView::StartCrawl()
	{
		mpCrawlData = new CrawlFolder{ GLOBAL_DEFAULT_PROJECT_PATH.c_str() };
		mpCrawlData->Crawl();
	}

	void ResourceView::FolderInterface(CrawlFolder *_pFolder)
	{
		if (EGUI::Display::StartTreeNode(_pFolder->mFolderName.c_str()))
		{
			if (_pFolder->mRefreshMe)
			{
				for (auto e : _pFolder->mArrChildFolders)
					delete e;
				_pFolder->mArrChildFolders.clear();
				_pFolder->mArrFiles.clear();
				_pFolder->Crawl();
				_pFolder->mRefreshMe = false;
				for (auto e : _pFolder->mArrChildFolders)
					EGUI::Display::OpenTreeNode(e->mFolderName.c_str(), false);
			}

			for (auto e : _pFolder->mArrChildFolders)
				FolderInterface(e);
			for (auto &e : _pFolder->mArrFiles)
				FileInterface(e);

			EGUI::Display::EndTreeNode();
		}
		else
		{
			if (!_pFolder->mRefreshMe)
				_pFolder->mRefreshMe = true;
		}
	}

	void ResourceView::FileInterface(CrawlFile& _file)
	{
		if (EGUI::Display::SelectableTxt(_file.mFileName.c_str()))
		{

		}
	}

	ResourceView::CrawlFile::CrawlFile(const std::string& _name)
		: mFileName{ _name }
	{}

	ResourceView::CrawlFolder::CrawlFolder(const char* _myName)
		: mFolderName{ _myName }, mpParentFolder{ nullptr }, mRefreshMe{ true }
	{}

	ResourceView::CrawlFolder::CrawlFolder(const std::string &_myName)
		: mFolderName{ _myName }, mpParentFolder{ nullptr }, mRefreshMe{ true }
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
		HANDLE hfind = FindFirstFileA(pathBuffer.c_str(), &data);
		if (hfind != INVALID_HANDLE_VALUE)
		{
			do
			{
				if (strcmp(data.cFileName, ".") && strcmp(data.cFileName, "..") && data.dwFileAttributes != FILE_ATTRIBUTE_HIDDEN)
				{
					if (data.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
					{
						std::string pFolderNamePath = mFolderName + "\\" + data.cFileName;
						CrawlFolder *pfolder = new CrawlFolder{ pFolderNamePath };
						pfolder->SetParent(this);
						AddFolder(pfolder);
						dataBuffer += "<DIR>";
					}
					else
						AddFile(data.cFileName);
					dataBuffer += data.cFileName;
					dataBuffer.clear();
				}
			} while (FindNextFileA(hfind, &data));
			FindClose(hfind);
		}
	}

	ResourceView::CrawlFolder* ResourceView::CrawlFolder::GetParent()
	{
		return mpParentFolder;
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
}




#endif // EDITOR





