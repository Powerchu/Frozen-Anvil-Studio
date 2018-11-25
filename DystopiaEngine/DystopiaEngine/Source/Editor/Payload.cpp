/* HEADER *********************************************************************************/
/*!
\file	Payloads.cpp
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief

If you need anything specificly, lemme know and I will create a user-friendly one for you.
Else, just hack it - include imgui.h and use more functions available at your own risk.

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#include "Editor/Payloads.h"
#include <algorithm>
#include <iostream>
#include <Windows.h>
#include <cstdlib>
#include <tchar.h>
#include <string>

namespace Editor//Dystopia::
{
	EGUI::ePayloadTags DeterminePathType(const HashString& _path)
	{
		if (_path.find(g_PayloadPngEx.c_str()) == _path.length() - 3)
			return EGUI::ePayloadTags::PNG;
		else if (_path.find(g_PayloadBmpEx.c_str()) == _path.length() - 3)
			return EGUI::ePayloadTags::BMP;
		else if (_path.find(g_PayloadCppEx.c_str()) == _path.length() - 3)
			return EGUI::ePayloadTags::FILE;
		else if (_path.find(g_PayloadPrefabEx.c_str()) == _path.length() - 4)
			return EGUI::ePayloadTags::PREFAB;
		else if (_path.find(g_PayloadSceneEx.c_str()) == _path.length() - 6)
			return EGUI::ePayloadTags::SCENE;
		else if (_path.find(g_PayloadMp3Ex.c_str()) == _path.length() - 3)
			return EGUI::ePayloadTags::MP3;
		else if (_path.find(g_PayloadDDSEx.c_str()) == _path.length() - 3)
			return EGUI::ePayloadTags::DDS;
		else if (_path.find(g_PayloadWavEx.c_str()) == _path.length() - 3)
			return EGUI::ePayloadTags::WAV;
		return EGUI::ePayloadTags::UNKNOWN;
	}

	char my_tolower(char const ch)
	{
		return static_cast<unsigned char>(::tolower(static_cast<unsigned char>(ch)));
	}

	CrawlItem::CrawlItem(const HashString& _name, const HashString& _path)
		: mName{ _name }, mPath{ _path }, mLowerCaseName{ mName }
	{
		std::string temp{ mLowerCaseName.cbegin(), mLowerCaseName.cend() };
		std::string temp2 = temp;
		std::transform(temp.begin(), temp.end(), temp2.begin(), my_tolower);
		mLowerCaseName = temp2.c_str();
	}

	Folder::Folder(const HashString& _name, const HashString& _path, Folder * const _parent)
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
		HashString pathBuffer = mPath + "\\*";
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

	Folder* Folder::FindFolder(const HashString& _name)
	{
		Folder* temp = nullptr;
		if (mName == _name) 
			return this;

		for (auto& e : mArrPtrFolders)
		{
			temp = e->FindFolder(_name);

			if (temp)  
				return temp;
		}
		return nullptr;
	}

	File::File(const HashString& _name, const HashString& _path, Folder * const _parent)
		: CrawlItem{ _name, _path }, mpParentFolder{ _parent }, mTag{ DetermineTag(_name) }
	{}

	File::~File()
	{
		mpParentFolder = nullptr;
	}

	bool File::operator<(const File& rhs)
	{
		std::string temp{ mName.c_str() };
		return temp.compare(rhs.mName.c_str()) <= 0;
	}

	EGUI::ePayloadTags File::DetermineTag(const HashString& _name)
	{
		return DeterminePathType(_name);
	}
}

#endif //EDITOR
