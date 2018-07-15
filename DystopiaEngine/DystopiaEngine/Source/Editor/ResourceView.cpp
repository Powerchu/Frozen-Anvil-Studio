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

constexpr float DEFAULT_WIDTH = 300;
constexpr float DEFAULT_HEIGHT = 300;

namespace Dystopia
{
	ResourceView::ResourceView()
		: mpFocusData{ nullptr }, mpRootFolder{ nullptr }, mLastSelected{ -1 }, mRefreshCrawl{ false },
		mLabel{ "Resource" }, mpCurrentFolder{ nullptr }
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
		static constexpr float smallY = 20;
		const float maxSizeY = Size().y - 55;
		const float maxSizeX = Size().x - 7;
		{
			EGUI::StartChild("ResourceChild1", Math::Vec2{ maxSizeX, smallY });

			EGUI::EndChild();
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
					ImGui::PushID(i);
					FileInterface(mpCurrentFolder->mArrFiles[i]);
					ImGui::PopID();
				}
			}
			EGUI::EndChild();
		}
	}

	void ResourceView::StartCrawl()
	{
		mpRootFolder = new CrawlFolder{ GLOBAL_DEFAULT_PROJECT_NAME, GLOBAL_DEFAULT_PROJECT_PATH };
		mpRootFolder->Crawl();
	}

	void ResourceView::FolderInterface(CrawlFolder *_pFolder)
	{
		if (_pFolder->mRefreshMe) ReCrawl(_pFolder);

		if (_pFolder->mArrChildFolders.size())
		{
			if (EGUI::Display::StartTreeNode(_pFolder->mFolderName.c_str()))
			{
				if (_pFolder->mToggle)
				{
					mpCurrentFolder = _pFolder;
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
			if (EGUI::Display::SelectableTxt(_pFolder->mFolderName.c_str()))
			{
				mpCurrentFolder = _pFolder;
			}
		}
	}

	void ResourceView::FileInterface(CrawlFile& _file)
	{
		ImGui::Indent(10);	
		ImGui::Button(_file.mFileName.c_str(), ImVec2{ 200, 20 } );
		if (EGUI::Display::StartPayload(EGUI::ePAY_LOAD_1, &_file, sizeof(CrawlFile), _file.mFileName.c_str()))
		{
			EGUI::Display::EndPayload();
		}
		ImGui::Unindent(10);
	}

	void ResourceView::ReCrawl(CrawlFolder *_pFolder)
	{
		for (auto e : _pFolder->mArrChildFolders)
			EGUI::Display::OpenTreeNode(e->mFolderName.c_str(), false);
		for (auto e : _pFolder->mArrChildFolders)
			delete e;
		_pFolder->mArrChildFolders.clear();
		_pFolder->mArrFiles.clear();
		_pFolder->Crawl();
		_pFolder->mRefreshMe = false;
	}

	ResourceView::CrawlFile::CrawlFile(const char* _name, const char* _path)
		: mFileName{ _name }, mFilePath{ _path }
	{}

	ResourceView::CrawlFile::CrawlFile(const std::string& _name, const std::string& _path)
		: mFileName{ _name }, mFilePath{ _path }
	{}

	ResourceView::CrawlFolder::CrawlFolder(const char* _name, const char* _path)
		: mFolderName{ _name }, mFolderPath{ _path }, mpParentFolder{ nullptr }, mRefreshMe{ true }, mToggle{ true }
	{}

	ResourceView::CrawlFolder::CrawlFolder(const std::string &_name, const std::string &_path)
		: mFolderName{ _name }, mFolderPath{ _path }, mpParentFolder{ nullptr }, mRefreshMe{ true }, mToggle{ true }
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

	void ResourceView::CrawlFolder::AddFile(const std::string& _name, const std::string& _path)
	{
		mArrFiles.EmplaceBack(CrawlFile{ _name, _path });
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
			std::cout << "   " << e.mFileName << std::endl;
		std::cout << "\n======End of Folder Data======\n";
		
	}
}




#endif // EDITOR





