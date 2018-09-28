#include "System/File/FileSystem.h"

#include <filesystem>
#include <Windows.h>
#include <ShlObj.h>

namespace Dystopia
{
	FileSystem::PathTable FileSystem::mPathTable
	{
		std::make_pair(eFileDir::eHeader,    "Include"),
		std::make_pair(eFileDir::eSource,    "Source"),
		std::make_pair(eFileDir::eResource,  "Resource"),
		std::make_pair(eFileDir::eRoot    ,  "C:/"),
		std::make_pair(eFileDir::eAppData ,  ""),
		std::make_pair(eFileDir::eCurrent,   "")
	};


	FileSystem::FileSystem()
	{
		PWSTR * _AppDataPath = static_cast<PWSTR*>(::operator new (sizeof(PWSTR)));
		/*Get the Path for %APPDATA%*/
		HRESULT result;
		result = SHGetKnownFolderPath(FOLDERID_RoamingAppData,
			                          NULL,
			                          NULL,
			                          _AppDataPath);
		if (result == S_OK)
		{
			/*Store the path for APPDATA*/
			std::wstring path{ *_AppDataPath };
			mPathTable[eFileDir::eAppData] = std::string{ path.begin(), path.end() };
			/*Free the Resource created by SHGetFunc*/
			CoTaskMemFree(*_AppDataPath);
			/*Free the allocated Memory*/
			::operator delete(_AppDataPath);

		}
		else
		{
			switch (result)
			{
			case E_FAIL:
				break;

			case E_INVALIDARG:
				//throw("INVALID ARGUEMENT");
				break;
			}
		}
		
	}

	std::string FileSystem::GetFullPath(std::string const & _FileName, eFileDir _ParentDirectory)
	{
		std::filesystem::path DirPath{ GetProjectFolders<std::string>(_ParentDirectory) };
		std::error_code error; 
		std::filesystem::recursive_directory_iterator DirIter { DirPath, std::filesystem::directory_options::skip_permission_denied, error };

		for (auto const & elem : DirIter)
		{
			if (elem.path().filename().string() == _FileName)
				return elem.path().string();
		}

		return std::string{};
	}

	bool FileSystem::CreateFiles(std::string const & _FileName, eFileDir _Directory)
	{
		std::filesystem::path _FilePath{ mPathTable[_Directory] + '/' + _FileName };
		return std::filesystem::create_directories(_FilePath);
	}

	bool FileSystem::CheckFileExist(std::string const & _FileName, eFileDir _Directory)
	{
		std::filesystem::path DirPath{ mPathTable[_Directory] };
		std::error_code error;
		std::filesystem::recursive_directory_iterator DirIter{ DirPath, std::filesystem::directory_options::skip_permission_denied, error };

		for (auto const & elem : DirIter)
		{
			if (elem.path().filename().string() == _FileName)
				return true;
		}

		return false;
	}

}

