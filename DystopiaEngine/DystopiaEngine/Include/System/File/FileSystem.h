#pragma once
#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H


#include <map>
#include <string>

#include "System/Base/Systems.h"
#include "DataStructure/HashString.h"


namespace Dystopia
{

	typedef enum class eFileDirectories : unsigned
	{
		eHeader,
		eSource,
		eResource,
		eCurrent,
		eRoot,
		eAppData,
		eTotalFilePath

	} eFileDir;

	class FileSystem
	{
	public:

		FileSystem();

		std::string GetFullPath(std::string const & _FileName, eFileDir _ParentDirectory);

		bool CreateFiles(std::string const & _FileName, eFileDir _Directory);

		template<typename T, typename U = std::enable_if_t< std::is_same_v< T, std::string> || std::is_same_v<T, std::wstring>>>
		T GetProjectFolders(eFileDir _ParentDirectory);

		template<typename T, typename U = std::enable_if_t< std::is_same_v< T, std::string> || std::is_same_v<T, std::wstring>>>
		T RemoveFileExtension(T const & _File);

		bool CheckFileExist(std::string const & _FileName, eFileDir _Directory = eFileDir::eRoot);

		bool CheckFolderExist(const HashString& _folderName, const HashString& _path) const;
		bool CheckPathExist(const HashString& _path) const;

		bool CreateFolder(const HashString& _name, const HashString& _path);

	private:

		using PathTable = std::map<eFileDir, std::string>;

		/*Static Members*/
		static PathTable        mPathTable;
		
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
		{
			if constexpr (std::is_same_v<T, std::string>)
				retString = std::filesystem::current_path().string() + '/' + mPathTable[_ParentDirectory];
			else
			if constexpr (std::is_same_v<T, std::wstring>)
				retString = std::filesystem::current_path().wstring() + L'/' + std::wstring{ mPathTable[_ParentDirectory].begin(), mPathTable[_ParentDirectory].end() };
		}
			break;
		case eFileDir::eCurrent:
		{
			if constexpr (std::is_same_v<T, std::string>)
				retString = std::filesystem::current_path().string();
			else
			if constexpr (std::is_same_v<T, std::wstring>)
				retString = std::filesystem::current_path().wstring();
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