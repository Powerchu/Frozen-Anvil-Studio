/* HEADER *********************************************************************************/
/*!
\file	Payloads.h
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief

If you need anything specificly, lemme know and I will create a user-friendly one for you.
Else, just hack it - include imgui.h and use more functions available at your own risk.

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#ifndef _PAYLOADS_H_
#define _PAYLOADS_H_
#include "DataStructure/AutoArray.h"
#include <string>

namespace EGUI
{
	enum ePayloadTags
	{
		UNKNOWN,
		FILE,
		PNG,
		BMP,
		PREFAB,
		GAME_OBJ,
		COMPONENT,

		ePAY_LOAD_LAST
	};

	inline const char* GetPayloadString(ePayloadTags _tag)
	{
		switch (_tag)
		{
		case FILE:			return "FILE";
		case PNG:			return "PNG";
		case BMP:			return "BMP";
		case PREFAB:		return "PREFAB";
		case GAME_OBJ:		return "GAME_OBJ";
		case COMPONENT:		return "COMPONENT";
		default:			return "ePAY_LOAD_LAST";
		}
	}
}

namespace Dystopia
{
	static const std::string g_PayloadPrefabEx = ".dobj";
	static const std::string g_PayloadPngEx = ".png";
	static const std::string g_PayloadBmpEx = ".bmp";
	static const std::string g_PayloadCppEx = ".cpp";

	/********************************************************************* FILE & FOLDER *********************************************************************/
	char my_tolower(char const ch);

	struct CrawlItem
	{
		CrawlItem(const std::string& _name, const std::string& _path);
		std::string mName;
		std::string mPath;
		std::string mLowerCaseName;
	};

	struct Folder;
	struct File : CrawlItem
	{
		File(const std::string& _name, const std::string& _path, Folder * const _parent);
		~File();
		bool operator<(const File&);
		static EGUI::ePayloadTags DetermineTag(const std::string& _name);
		EGUI::ePayloadTags mTag;
		Folder*	mpParentFolder;
	};

	struct Folder : CrawlItem
	{
		Folder(const std::string& _name, const std::string& _path, Folder * const _parent);
		~Folder();

		void				Crawl();
		void				ClearFolder();

		Folder*				mpParentFolder;
		AutoArray<File*>	mArrPtrFiles;
		AutoArray<Folder*>	mArrPtrFolders;
		bool				mToggle;
	};
}


#endif	// _PAYLOADS_H_
#endif	// EDITOR

