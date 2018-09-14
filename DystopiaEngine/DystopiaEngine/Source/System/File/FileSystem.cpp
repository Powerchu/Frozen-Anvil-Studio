#include "System/File/FileSystem.h"

#include <filesystem>
#include <Windows.h>

namespace Dystopia
{
	FileSystem::PathTable FileSystem::mPathTable
	{
		std::make_pair(eFileDir::eHeader,    "Include"),
		std::make_pair(eFileDir::eSource,    "Source"),
		std::make_pair(eFileDir::eResource,  "Resource"),
		std::make_pair(eFileDir::eRoot    ,  "C:/"),
		std::make_pair(eFileDir::eAppData ,  "")
	};


	FileSystem::FileSystem()
	{
		//char const * appDataPath = getenv("APPDATA");
		//mPathTable[eFileDir::eAppData] = appDataPath ? std::string{ appDataPath } : "";
	}

	std::string FileSystem::GetFullPath(std::string const & _FileName, eFileDir _ParentDirectory)
	{
		std::filesystem::path DirPath{ mPathTable[_ParentDirectory] };
		std::error_code error; 
		std::filesystem::recursive_directory_iterator DirIter { DirPath, std::filesystem::directory_options::skip_permission_denied, error };

		for (auto const & elem : DirIter)
		{
			if (elem.path().filename().string() == _FileName)
				return elem.path().string();
		}

		return std::string{};
	}

}

