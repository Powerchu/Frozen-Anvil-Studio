#pragma once
#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H


#include <map>
#include <string>
#include <filesystem>

#include "System/Base/Systems.h"
#include "DataStructure/HashString.h"
#include "DataStructure/MagicArray.h"

#include "Globals.h"

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
		_DLL_EXPORT_ONLY HashString GetFromResource(const char *_str);

	private:

		using PathTable = std::map<eFileDir, std::string>;

		/*Static Members*/
		static PathTable            mPathTable;
		MagicArray<DetectionInfo *> mDetectionFiles;
		/*Private Members*/
		eFileSystemError            mLastKnownError;



		/*Private Function*/
		unsigned GetChangesInfo(DetectionInfo & _DetectionInfo, std::string * _ChangesBuffer, size_t _size);
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
					retString = std::filesystem::current_path().string() + '/' + mPathTable[_ParentDirectory];
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
}

#endif