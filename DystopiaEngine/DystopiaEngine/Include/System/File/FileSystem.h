/* HEADER *********************************************************************************/
/*!
\file	FileSystem.h
\author Goh Kai Jie Keith (100%)
\par    email: keith.goh\@digipen.edu
\brief


All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#pragma once
#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H


#include <map>
#include <string>
#include <filesystem>

#include "System/Base/Systems.h"
#include "DataStructure/HashString.h"
#include "DataStructure/MagicArray.h"
#include "System/Events/Event.h"
#include "DataStructure/AutoArray.h"
#include "Utility/GUID.h"

#include "Globals.h"

struct _OVERLAPPED;

namespace Dystopia
{
	/*Predeclaration*/
	struct DetectionInfo;

	typedef enum class eFileDirectories : unsigned
	{
		eHeader,
		eSource,
		eResource,
		eCurrent,
		eSolution,
		eRoot,
		eAppData,
		eTotalFilePath

	} eFileDir;

	typedef enum class eFileSystemError : unsigned
	{
		NONE,
		SUCCESS,
		CREATE_HANDLE_ERROR,
		CREATE_OVERLAP_ERROR,
		READ_DIRECTORY_ERROR,

	} FileErrorCode;

	typedef HashID FileTrackInfoID_t;
	typedef GUID_t FileEventID_t;

	class FileSystem
	{
	public:

		FileSystem();
		~FileSystem();
		std::string  GetFullPath (std::string const & _FileName, eFileDir _ParentDirectory);
		std::wstring GetFullPath_w(std::wstring const & _FileName, eFileDir _ParentDirectory);
		HashString    ConvertToRelative(HashString const & _FullPath, eFileDir _ParentDirectory = eFileDir::eResource) const; 
		std::string   ConvertToRelative(std::string const & _FullPath, eFileDir _ParentDirectory = eFileDir::eResource) const;
		bool CreateFiles(std::string const & _FileName, eFileDir _Directory);

		template<typename T, typename U = std::enable_if_t< std::is_same_v< T, std::string> || std::is_same_v<T, std::wstring>>>
		T GetProjectFolders(eFileDir _ParentDirectory);

		template<typename T, typename U = std::enable_if_t< std::is_same_v< T, std::string> || std::is_same_v<T, std::wstring>>>
		T RemoveFileExtension(T const & _File);

		bool CheckFileExist(std::string const & _FileName, eFileDir _Directory = eFileDir::eRoot);

		bool IsSameFile(std::string const& _lhs, std::string const& _rhs);

		/*
		  Detect changes in the file path provided. 
		  The FilesNames return is relative to the _FilePath provided.
		  DetectFileChanges will return 0 when
		  1. There is no changes detected
		  2. There is a error in the function

		  DetectFileChanges will return a number when 
		  1. There is files changes
		  The number will reflect the number of changes detected
		*/
		unsigned DetectDirectoryChanges(const std::string& _FilePath, std::string * _ChangesBuffer, size_t _size);
		bool     DetectFileChanges(const std::string& _FilePath, eFileDir _ParentDirectory = eFileDir::eResource);
		FileErrorCode GetLastKnownError() const;

		bool CheckFolderExist(const HashString& _folderName, const HashString& _path) const;
		bool CheckPathExist(const HashString& _path) const;

		bool CreateFolder(const HashString& _name, const HashString& _path);

		void ChangeDirPath(eFileDir _dirToChange, const HashString& _newPath);
		HashString FindFilePath(const HashString& _file, eFileDir _parentDir);
		_DLL_EXPORT HashString GetFromResource(const char *_str);

		/*EXPERIMENTAL FUNCTIONS*/
		void RegisterFileTrackEvent (const HashString & _FileName, CallBackEvent const & _Event, eFileDir _ParentDirectory = eFileDir::eResource);

		/*Create a Track File Event, The EventName should be the filepath*/
		FileTrackInfoID_t TrackFile(const HashString & _FileName);

		/*Bind a Function to the event created*/
		template<class Caller, typename Ret_t, typename ...params_t>
		FileEventID_t RegisterFileTrackEvent(const HashString & _FileName, Ret_t(Caller::*_fn)(params_t...), Caller* const _user, eFileDir _ParentDirectory = eFileDir::eResource)
		{
			/*Check for existing EventInfo who has a filepath that is equilivent*/
			for (auto & elem : mArrayOfTrackInfo)
			{
				if (IsSameFile(GetFullPath(_FileName.c_str(), _ParentDirectory), elem.first.c_str()))
				{
					auto temp = GUIDGenerator::GetUniqueID();
					elem.second.push_back(EventInfo{temp , CallBackEvent{_user, _fn } });
					return temp;
				}
			}
			auto id = GUIDGenerator::GetUniqueID();
			EventInfo tempInfo = { id, CallBackEvent{ _user, _fn } };
			AutoArray<EventInfo> temp;
			temp.EmplaceBack(tempInfo);
			mArrayOfTrackInfo.push_back(std::make_pair(_FileName, Ut::Move(temp)));
			return id;
		}

		///*Bind a Function to the event created*/
		//template<class Caller, typename Ret_t, typename ...params_t>
		//FileSystemEventID RegisterFileTrackEvent(const HashString & _FileName, Ret_t(Caller::*_fn)(params_t...), eFileDir _ParentDirectory = eFileDir::eResource)
		//{
		//	/*Check for existing EventInfo who has a filepath that is equilivent*/
		//	for (auto & elem : mArrayOfEventInfo)
		//	{
		//		if (IsSameFile(GetFullPath(_FileName, _ParentDirectory), elem.first))
		//		{
		//			elem.second.push_back(CallBackEvent{ _user, _fn });
		//			return elem.first.id();
		//		}
		//	}

		//	AutoArray<CallBackEvent> temp;
		//	temp.EmplaceBack(_fn);
		//	mArrayOfEventInfo.push_back(std::make_pair(_FileName, Ut::Move(temp)));
		//	return _FileName.id();
		//}
		
	private:

		using PathTable     = std::map<eFileDir, std::string>;
		using EventInfo     = std::pair<GUID_t, CallBackEvent>;
		using FileTrackInfo = std::pair<HashString, AutoArray<EventInfo>>;

		/*Static Members*/
		static PathTable            mPathTable;
		MagicArray<DetectionInfo *> mDetectionFiles;
		/*Private Members*/
		eFileSystemError            mLastKnownError;
		/*EXPERIMENTAL MEMBERS*/
		AutoArray<FileTrackInfo>    mArrayOfTrackInfo;


		/*Private Function*/
		unsigned GetChangesInfo(DetectionInfo & _DetectionInfo, std::string * _ChangesBuffer, size_t _size);

		/*EXPERIMENTAL STUFF*/

		/*Callback Function for ReadDirectoryChangesExW*/
		void FileTrackCallBack (unsigned long dwErrorCode, unsigned long dwNumOfBytes, _OVERLAPPED * lpOverlapped);
		friend void FileTrackCallBack(unsigned long dwErrorCode, unsigned long dwNumOfBytes, _OVERLAPPED * lpOverlapped);
	};


	template<typename T, typename U>
	inline T FileSystem::GetProjectFolders(eFileDir _ParentDirectory)
	{
		std::error_code error;

		std::filesystem::recursive_directory_iterator iter{ std::filesystem::current_path(),
			                                                std::filesystem::directory_options::skip_permission_denied,
			                                                error };

		T retString;

		switch (_ParentDirectory)
		{
		case eFileDir::eHeader:
		case eFileDir::eResource:
		case eFileDir::eSource:
		case eFileDir::eCurrent:
		{
			if constexpr (std::is_same_v<T, std::string>)
			{
				std::filesystem::path DirPath{ mPathTable[_ParentDirectory] };
				if (DirPath.has_root_path())
					retString = mPathTable[_ParentDirectory];
				else
					retString = std::filesystem::current_path().string() + '/' + mPathTable[_ParentDirectory] + '/';
			}

			else if constexpr (std::is_same_v<T, std::wstring>)
			{
				std::filesystem::path DirPath{ mPathTable[_ParentDirectory] };
				if (DirPath.has_root_path())
					retString = std::wstring{ mPathTable[_ParentDirectory].begin(), mPathTable[_ParentDirectory].end() };
				else
					retString = std::filesystem::current_path().wstring() + L'/' + std::wstring{ mPathTable[_ParentDirectory].begin(), mPathTable[_ParentDirectory].end() };
			}

		}
			break;
		case eFileDir::eSolution:
		{
			if constexpr (std::is_same_v<T, std::string>)
				retString = std::filesystem::current_path().string();
			else
			if constexpr (std::is_same_v<T, std::wstring>)
				retString = std::filesystem::current_path().wstring();

			break;
		}
		default:
		{
			if constexpr (std::is_same_v<T, std::string>)
				retString = mPathTable[_ParentDirectory];
			else
			if constexpr (std::is_same_v<T, std::wstring>)
				retString = std::wstring{ mPathTable[_ParentDirectory].begin(), mPathTable[_ParentDirectory].end() };
		}
			break;
		};


		return retString;
		/*
		if constexpr (std::is_same_v<T, std::string>)
			return DirPath.parent_path().string() + DirPath.filename().string();
		else 
		if constexpr (std::is_same_v<T, std::wstring>)
			return DirPath.parent_path().wstring() + DirPath.filename().wstring();
		else
			static_assert(std::is_same_v<T, std::string> || std::is_same_v<T, std::wstring>);
		*/
	}

	template<typename T, typename U>
	inline T FileSystem::RemoveFileExtension(T const & _File)
	{
		std::filesystem::path File{ _File };
		if (File.has_extension())
		{
			if constexpr (std::is_same_v<T, std::string>)
				return File.replace_extension("").filename().string();
			else
			if constexpr (std::is_same_v<T, std::wstring>)
				return File.replace_extension("").filename().wstring();
		}

		return _File;

	}

	void FileTrackCallBack(unsigned long dwErrorCode, unsigned long dwNumOfBytes, _OVERLAPPED * lpOverlapped);


}

#endif