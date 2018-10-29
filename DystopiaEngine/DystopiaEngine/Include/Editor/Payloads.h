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
#include "Globals.h"
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
		SCENE,
		MP3,
		DDS,

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
		case SCENE:			return "SCENE";
		case MP3:			return "MP3";
		case DDS:			return "DDS";
		default:			return "ePAY_LOAD_LAST";
		}
	}
}

namespace Dystopia
{
	static const std::string g_PayloadSceneEx = Gbl::SCENE_EXT;
	static const std::string g_PayloadPrefabEx = Gbl::PREFAB_EXT;
	static const std::string g_PayloadPngEx = Gbl::PNG_EXT;
	static const std::string g_PayloadBmpEx = Gbl::BMP_EXT;
	static const std::string g_PayloadCppEx = Gbl::CPP_EXT;
	static const std::string g_PayloadMp3Ex = Gbl::MP3_EXT;
	static const std::string g_PayloadDDSEx = Gbl::DDS_EXT;

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
		Folder*				FindFolder(const std::string& _name);

		Folder*				mpParentFolder;
		AutoArray<File*>	mArrPtrFiles;
		AutoArray<Folder*>	mArrPtrFolders;
		bool				mToggle;
	};
}


#endif	// _PAYLOADS_H_
#endif	// EDITOR

