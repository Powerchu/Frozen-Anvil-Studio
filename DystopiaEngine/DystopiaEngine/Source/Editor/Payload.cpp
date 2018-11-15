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

namespace Dystopia
{
	char my_tolower(char const ch)
	{
		return static_cast<unsigned char>(::tolower(static_cast<unsigned char>(ch)));
	}

	CrawlItem::CrawlItem(const HashString& _name, const HashString& _path)
		: mName{ _name }, mPath{ _path }, mLowerCaseName{ mName }
	{
		//std::transform(mLowerCaseName.cbegin(), mLowerCaseName.cend(), mLowerCaseName.begin(), my_tolower);
		auto start = mLowerCaseName.begin();
		for(auto const & elem : mLowerCaseName)
		{
			*start = my_tolower(elem);
			start++;
		}
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
		return mName<(rhs.mName);
	}

	EGUI::ePayloadTags File::DetermineTag(const HashString& _name)
	{
		if (_name.find(g_PayloadPngEx.c_str()) == _name.length() - 4)
			return EGUI::ePayloadTags::PNG;
		else if (_name.find(g_PayloadBmpEx.c_str()) == _name.length() - 4)
			return EGUI::ePayloadTags::BMP;
		else if (_name.find(g_PayloadCppEx.c_str()) == _name.length() - 4)
			return EGUI::ePayloadTags::FILE;
		else if (_name.find(g_PayloadPrefabEx.c_str()) == _name.length() - 5)
			return EGUI::ePayloadTags::PREFAB;
		else if (_name.find(g_PayloadSceneEx.c_str()) == _name.length() - 7)
			return EGUI::ePayloadTags::SCENE;
		else if (_name.find(g_PayloadMp3Ex.c_str()) == _name.length() - 4)
			return EGUI::ePayloadTags::MP3;
		else if (_name.find(g_PayloadDDSEx.c_str()) == _name.length() - 4)
			return EGUI::ePayloadTags::DDS;
		else if (_name.find(g_PayloadWavEx.c_str()) == _name.length() - 4)
			return EGUI::ePayloadTags::WAV;
		else if (_name.find(g_PayloadTTFEx.c_str()) == _name.length() - 4)
			return EGUI::ePayloadTags::TTF;
		return EGUI::ePayloadTags::UNKNOWN;
	}
}

#endif //EDITOR
