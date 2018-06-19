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
		: mWidth{ DEFAULT_WIDTH }, mHeight{ DEFAULT_HEIGHT }, mpFocusData{ nullptr }, mpCrawlData{ nullptr }, mpCurrentFolder{ nullptr }, 
		mLastSelected{ -1 }, mTrySelect{ 0 }, mRefreshCrawl{ false }
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
		mTrySelect = 0;
		if (EGUI::Display::CollapseHeader(mpCurrentFolder->mFolderName.c_str()))
		{
			mTrySelect = mpCurrentFolder->mpParentFolder ? 0 : 1;
			if (!mTrySelect)
				FolderInterface(mpCurrentFolder->mpParentFolder);
			for (auto e : mpCurrentFolder->mArrChildFolders)
				FolderInterface(e);
			for (auto& e : mpCurrentFolder->mArrFiles)
				FileInterface(e);
			mRefreshCrawl = true;
		}
		else
		{
			if (mRefreshCrawl)
			{
				mpCurrentFolder->Crawl();
				mRefreshCrawl = false;
			}
		}
	}

	void ResourceView::StartCrawl()
	{
		mpCrawlData = new CrawlFolder{ GLOBAL_DEFAULT_PROJECT_PATH.c_str() };
		mpCrawlData->Crawl();
		mpCurrentFolder = mpCrawlData;
	}

	void ResourceView::FolderInterface(CrawlFolder *_pFolder)
	{
		std::string showName = _pFolder->mFolderName;

		if (ImGui::TreeNode(showName.c_str()))
		{
			_pFolder->Crawl();
			for (auto e : _pFolder->mArrChildFolders)
				FolderInterface(e);
			ImGui::TreePop();
		}
		return;

		if (_pFolder == mpCurrentFolder->mpParentFolder)
			showName = "[Back to previous Folder]";

		bool highlighted = (mTrySelect == mLastSelected) ? true : false;
		if (EGUI::Display::SelectableTxtDouble(showName.c_str(), highlighted))
		{
			mLastSelected = mTrySelect;
			if (!mLastSelected)
			{
				mpCurrentFolder = mpCurrentFolder->GetParent();
				mpCurrentFolder->Crawl();
				mLastSelected = -1;
			}
			else
			{
				for (unsigned int j = 0; j < mpCurrentFolder->mArrChildFolders.size(); ++j)
				{
					if (j == static_cast<unsigned int>(mLastSelected - 1))
					{
						mpCurrentFolder = mpCurrentFolder->mArrChildFolders[j];
						mpCurrentFolder->Crawl();
						mLastSelected = -1;
						break;
					}
				}
			}
		}
		mTrySelect++;
	}

	void ResourceView::FileInterface(CrawlFile& _file)
	{
		bool highlighted = (mTrySelect == mLastSelected) ? true : false;
		if (EGUI::Display::SelectableTxt((_file.mFileName).c_str(), highlighted))
			mLastSelected = mTrySelect;
		mTrySelect++;
	}

	ResourceView::CrawlFile::CrawlFile(const std::string& _name)
		: mFileName{ _name }
	{}

	ResourceView::CrawlFolder::CrawlFolder(const char* _myName)
		: mFolderName{ _myName }, mpParentFolder{ nullptr }
	{}

	ResourceView::CrawlFolder::CrawlFolder(const std::string &_myName)
		: mFolderName{ _myName }, mpParentFolder{ nullptr }
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
						if (!FolderExist(pFolderNamePath))
						{
							CrawlFolder *pfolder = new CrawlFolder{ pFolderNamePath };
							pfolder->SetParent(this);
							AddFolder(pfolder);
							dataBuffer += "<DIR>";
						}
					}
					else
					{
						if (!FileExist(data.cFileName))
							AddFile(data.cFileName);
					}
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





