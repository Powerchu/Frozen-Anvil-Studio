/* HEADER *********************************************************************************/
/*!
\file	Payloads.h
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
#ifndef _PAYLOADS_H_
#define _PAYLOADS_H_
#include "Globals.h"
#include "DataStructure/AutoArray.h"
#include "DataStructure/HashString.h"
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
		WAV,
		TTF,
		TXT,
		DATA_SHEET,

		ALL_IMG,
		ALL_AUDIO,

		SHADER,
		SHADER_VERT,
		SHADER_FRAG,
		SHADER_GEO,

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
		case WAV:			return "WAV";
		case TTF:			return "TTF";
		case TXT:			return "TXT";
		case DATA_SHEET: 	return "DATA_SHEET";
		case ALL_IMG:		return "A_IMG";
		case ALL_AUDIO:		return "A_AUDIO";
		case SHADER:		return "SHADER";
		case SHADER_VERT:	return "SHADER_VERT";
		case SHADER_FRAG:	return "SHADER_FRAG";
		case SHADER_GEO:	return "SHADER_GEO";
		default:			return "UNKNOWN";
		}
	}
}

namespace Editor //Dystopia
{
	static const std::string g_PayloadSceneEx = Gbl::SCENE_EXT;
	static const std::string g_PayloadPrefabEx = Gbl::PREFAB_EXT;
	static const std::string g_PayloadPngEx = Gbl::PNG_EXT;
	static const std::string g_PayloadBmpEx = Gbl::BMP_EXT;
	static const std::string g_PayloadCppEx = Gbl::CPP_EXT;
	static const std::string g_PayloadMp3Ex = Gbl::MP3_EXT;
	static const std::string g_PayloadDDSEx = Gbl::DDS_EXT;
	static const std::string g_PayloadWavEx = Gbl::WAV_EXT;
	static const std::string g_PayloadTTFEx = ".ttf";
	static const std::string g_PayloadTxtEx = ".txt";
	static const std::string g_PayloadSheetEx = Gbl::DATASHEET_EXT;

	/********************************************************************* FILE & FOLDER *********************************************************************/
	char my_tolower(char const ch);

	EGUI::ePayloadTags DeterminePathType(const HashString&);

	struct CrawlItem
	{
		CrawlItem(const HashString& _name, const HashString& _path);
		HashString mName;
		HashString mPath;
		HashString mLowerCaseName;
		void* mpImgData;
	};

	struct Folder;
	struct File : CrawlItem
	{
		File(const HashString& _name, const HashString& _path, Folder * const _parent);
		~File();
		bool operator<(const File&);
		static EGUI::ePayloadTags DetermineTag(const HashString& _name);
		EGUI::ePayloadTags mTag;
		Folder*	mpParentFolder;
	};

	struct Folder : CrawlItem
	{
		Folder(const HashString& _name, const HashString& _path, Folder * const _parent);
		~Folder();

		void				Crawl();
		void				ClearFolder();
		Folder*				FindFolder(const HashString& _name);

		Folder*				mpParentFolder;
		AutoArray<File*>	mArrPtrFiles;
		AutoArray<Folder*>	mArrPtrFolders;
		bool				mToggle;
	};
}


#endif	// _PAYLOADS_H_
#endif	// EDITOR

