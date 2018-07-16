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
#include <stdlib.h>
#include <tchar.h>
#include "../../Dependancies/ImGui/imgui.h"
#include "../../Dependancies/ImGui/imgui_internal.h"

static const std::string GLOBAL_DEFAULT_PROJECT_PATH = "..\\DystopiaEngine\\Resource";
static const std::string GLOBAL_DEFAULT_PROJECT_NAME = "Resource";

namespace Dystopia
{
	ResourceView::ResourceView()
		: mpRootFolder{ nullptr }, mLabel{ "Resource" }, mpCurrentFolder{ nullptr }, 
		mSearchBarText{ "" }, mArrFileSearchResultPtrs{}
	{}
	
	ResourceView::~ResourceView()
	{}
	
	void ResourceView::Init()
	{
		StartCrawl();
		mpCurrentFolder = mpRootFolder;
	}

	void ResourceView::Update(const float& _dt)
	{
		_dt;
	}

	void ResourceView::Shutdown()
	{
		delete mpRootFolder;
	}

	std::string ResourceView::GetLabel() const
	{
		return mLabel;
	}

	void ResourceView::Window()
	{
		static constexpr float leftColX = 250;
		static constexpr float smallY = 50;
		const float maxSizeY = Size().y - 55;
		const float maxSizeX = Size().x - 7;
		{
			EGUI::Indent(5);
			EGUI::Display::TextField("Search", mSearchBarText, MAX_SEARCH_SIZE);
			EGUI::UnIndent(5);
			EGUI::Display::HorizontalSeparator();
		}
		{
			EGUI::StartChild("ResourceChild2", Math::Vec2{ leftColX, maxSizeY });
			FolderInterface(mpRootFolder);
			EGUI::EndChild();
		}
		EGUI::SameLine(leftColX + 4);
		{
			EGUI::StartChild("ResourceChild3", Math::Vec2{ maxSizeX - leftColX - 3, maxSizeY });
			if (mpCurrentFolder)
			{
				EGUI::Display::Label(mpCurrentFolder->mFolderName.c_str());
				EGUI::Display::HorizontalSeparator();
				for (unsigned int i = 0; i < mpCurrentFolder->mArrFiles.size(); ++i)
				{
					EGUI::Indent(10);
					EGUI::PushID(i);
					FileInterface(*mpCurrentFolder->mArrFiles[i]);
					EGUI::PopID();
					EGUI::UnIndent(10);
				}
			}
			EGUI::EndChild();
		}
	}

	void ResourceView::CrawlAll(CrawlFolder* _pFolder)
	{
		for (auto e : _pFolder->mArrChildFolders)
		{
			e->Crawl();
			CrawlAll(e);
		}
	}

	void ResourceView::StartCrawl()
	{
		mpRootFolder = new CrawlFolder{ GLOBAL_DEFAULT_PROJECT_NAME, GLOBAL_DEFAULT_PROJECT_PATH };
		mpRootFolder->Crawl();
	}

	void ResourceView::FolderInterface(CrawlFolder *_pFolder)
	{
		bool temp = false;
		if (_pFolder->mRefreshMe) ReCrawl(_pFolder);

		if (_pFolder->mArrChildFolders.size())
		{
			if (EGUI::Display::StartTreeNode(_pFolder->mFolderName.c_str(), &temp))
			{
				if (_pFolder->mToggle)
				{
					if (temp) mpCurrentFolder = _pFolder;
					_pFolder->mToggle = false;
				}
				for (auto e : _pFolder->mArrChildFolders)
					FolderInterface(e);
				EGUI::Display::EndTreeNode();
			}
			else
			{
				if (!_pFolder->mToggle)
				{
					mpCurrentFolder = _pFolder;
					_pFolder->mToggle = true;
					_pFolder->mRefreshMe = true;
				}
			}
		}
		else
		{
			EGUI::Indent(20);
			if (EGUI::Display::SelectableTxt(_pFolder->mFolderName.c_str()))
			{
				mpCurrentFolder = _pFolder;
			}
			EGUI::UnIndent(20);
		}
	}

	void ResourceView::FileInterface(CrawlFile& _file)
	{
		EGUI::Display::Button(_file.mFileName.c_str(), Math::Vec2{ 200, 20 } );
		if (EGUI::Display::StartPayload(EGUI::FILE, &_file, sizeof(CrawlFile), _file.mFileName.c_str()))
		{
			EGUI::Display::EndPayload();
		}
	}

	void ResourceView::ReCrawl(CrawlFolder *_pFolder)
	{
		for (auto e : _pFolder->mArrChildFolders)
			delete e;
		_pFolder->mArrChildFolders.clear();
		for (auto e : _pFolder->mArrFiles)
			delete e;
		_pFolder->mArrFiles.clear();
		_pFolder->Crawl();
		_pFolder->mRefreshMe = false;
		for (auto e : _pFolder->mArrChildFolders)
			EGUI::Display::OpenTreeNode(e->mFolderName.c_str(), false);
	}

	void ResourceView::FindFile(CrawlFolder *_pFolder, const std::string& _item)
	{
		if (!_pFolder) return;

		for (auto e : _pFolder->mArrFiles)
		{
			if (!e->mFileName.find(_item)) mArrFileSearchResultPtrs.push_back(e);
		}

		for (auto e : _pFolder->mArrChildFolders)
		{
			FindFile(e, _item);
		}
	}

	ResourceView::CrawlFile::CrawlFile(const char* _name, const char* _path, CrawlFolder* _pParent)
		: mFileName{ _name }, mFilePath{ _path }, mpParentFolder{ _pParent }
	{}

	ResourceView::CrawlFile::CrawlFile(const std::string& _name, const std::string& _path, CrawlFolder* _pParent)
		: mFileName{ _name }, mFilePath{ _path }, mpParentFolder{ _pParent }
	{}

	ResourceView::CrawlFolder::CrawlFolder(const char* _name, const char* _path)
		: mFolderName{ _name }, mFolderPath{ _path }, mpParentFolder{ nullptr }, mRefreshMe{ true }, mToggle{ true }
	{}

	ResourceView::CrawlFolder::CrawlFolder(const std::string &_name, const std::string &_path)
		: mFolderName{ _name }, mFolderPath{ _path }, mpParentFolder{ nullptr }, mRefreshMe{ true }, mToggle{ true }
	{}

	ResourceView::CrawlFolder::~CrawlFolder()
	{
		this->mpParentFolder = nullptr;
		for (auto e : mArrChildFolders)
			delete e;
		for (auto e : mArrFiles)
			delete e;
	}

	void ResourceView::CrawlFolder::AddFolder(CrawlFolder *_subFolder)
	{
		mArrChildFolders.EmplaceBack(_subFolder);
	}

	void ResourceView::CrawlFolder::AddFile(const std::string& _name, const std::string& _path)
	{
		mArrFiles.EmplaceBack(new CrawlFile{ _name, _path, this });
	}

	void ResourceView::CrawlFolder::Crawl()
	{
		WIN32_FIND_DATAA data;
		std::string dataBuffer, pathBuffer{ mFolderPath };
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
						std::string pFolderNamePath = mFolderPath + "\\" + data.cFileName;
						CrawlFolder *pfolder = new CrawlFolder{ data.cFileName ,pFolderNamePath };
						pfolder->SetParent(this);
						AddFolder(pfolder);
						dataBuffer += "<DIR>";
					}
					else
						AddFile(data.cFileName, mFolderPath + "\\" + data.cFileName);
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
			std::cout << "   " << e->mFileName << std::endl;
		std::cout << "\n======End of Folder Data======\n";
		
	}
}




#endif // EDITOR





