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
		: mpFocusData{ nullptr }, mpCrawlData{ nullptr }, mLastSelected{ -1 }, mRefreshCrawl{ false },
		mLabel{ "Resource" }, mpLastFolder{ nullptr }
	{}
	
	ResourceView::~ResourceView()
	{}
	
	void ResourceView::Init()
	{
		StartCrawl();
		mpLastFolder = mpCrawlData;
	}

	void ResourceView::Update(const float& _dt)
	{
		_dt;
	}

	void ResourceView::Shutdown()
	{
		delete mpCrawlData;
	}

	std::string ResourceView::GetLabel() const
	{
		return mLabel;
	}

	void ResourceView::Window()
	{
		const float maxSizeY = Size().y - 55;
		const float maxSizeX = Size().x - 7;
		const float leftColX = 250;
		const float smallY = 20;
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0);
		{
			{
				ImGui::BeginChild("ResourceChild1", ImVec2{ maxSizeX, smallY }, true);

				ImGui::EndChild();
			}
			{
				ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4{ 0.1f, 0.1f, 0.1f, 0.2f });
				ImGui::BeginChild("ResourceChild2", ImVec2{ leftColX, maxSizeY }, true);
				if (EGUI::Display::CollapsingHeader(mpCrawlData->mFolderName.c_str()))
				{
					if (mRefreshCrawl)
					{
						mpLastFolder = mpCrawlData;
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
				}
				else
				{
					if (!mRefreshCrawl)
					{
						mpLastFolder = nullptr;
						mRefreshCrawl = true;
					}
				}
				ImGui::EndChild();
			}
			ImGui::SameLine(leftColX + 4);
			{
				ImGui::PushStyleColor(ImGuiCol_Border, ImVec4{ 0, 0, 0, 0 });
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 10, 10 });
				ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 1);
				ImGui::BeginChild("ResourceChild3", ImVec2{ maxSizeX - leftColX - 3, maxSizeY }, true);
				if (mpLastFolder)
				{
					EGUI::Display::Label(mpLastFolder->mFolderName.c_str());
					ImGui::Separator();
					for (auto &e : mpLastFolder->mArrFiles)
						FileInterface(e);
				}
				ImGui::EndChild();
			}
		}
		ImGui::PopStyleVar(4);
		ImGui::PopStyleColor(2);
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
				mpLastFolder = _pFolder;
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

			EGUI::Display::EndTreeNode();
		}
		else
		{
			if (!_pFolder->mRefreshMe)
			{
				mpLastFolder = _pFolder->GetParent();
				_pFolder->mRefreshMe = true;
			}
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





