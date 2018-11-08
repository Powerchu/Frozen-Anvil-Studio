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
		std::make_pair(eFileDir::eCurrent ,  "")
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
		std::wstring wstrFileName{ _FileName.begin(), _FileName.end() };
		for (auto const & elem : DirIter)
		{
			std::wstring filename = elem.path().filename().wstring();
			if (filename == wstrFileName)
				return true;
		}

		return false;
	}

	bool FileSystem::CheckFolderExist(const HashString& _folderName, const HashString& _path) const
	{
		if (CheckPathExist(_path))
		{
			WIN32_FIND_DATAA data;
			HashString pathBuffer = _path + "/" + _folderName;
			HANDLE hfind = FindFirstFileA(pathBuffer.c_str(), &data);
			if (hfind != INVALID_HANDLE_VALUE)
			{
				do
				{
					if (strcmp(data.cFileName, ".") && strcmp(data.cFileName, "..") && data.dwFileAttributes != FILE_ATTRIBUTE_HIDDEN)
					{
						if (data.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
							return true;
					}
				} while (FindNextFileA(hfind, &data));
				FindClose(hfind);
			}
		}
		return false;
	}

	bool FileSystem::CheckPathExist(const HashString& _path) const
	{
		DWORD result = GetFileAttributesA(_path.c_str());
		if (result == INVALID_FILE_ATTRIBUTES)
			return false;
		else if (result & FILE_ATTRIBUTE_DIRECTORY)
			return true;
		return false;
	}

	bool FileSystem::CreateFolder(const HashString& _name, const HashString& _path)
	{
		HashString temp = _path;
		if (!(*_path.clast() == '/'))
			temp += '/';
		temp += _name;
		return CreateDirectoryA(temp.c_str(), nullptr);
	}

}

