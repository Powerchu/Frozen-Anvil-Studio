#include "System/File/FileSystem.h"

#include <filesystem>
#include <Windows.h>
#include <ShlObj.h>
#include <utility>
#include <algorithm>
#include <array>

namespace Dystopia
{

	struct DetectionInfo
	{
		static constexpr unsigned NumOfFileInfo = 256;
		using  FileInfo = std::array<char, NumOfFileInfo * MAX_PATH * sizeof(WCHAR)>;

		DetectionInfo(std::string const & _name, HANDLE _FileHandle, _OVERLAPPED _OverlapObj)
			:mFileName{ _name }, mFileHandle{ _FileHandle }, mOverlappedInfo{ _OverlapObj }
		{}
		~DetectionInfo()
		{
			if (mFileHandle != INVALID_HANDLE_VALUE)
				CloseHandle(mFileHandle);
			if (mOverlappedInfo.hEvent != INVALID_HANDLE_VALUE)
				CloseHandle(mOverlappedInfo.hEvent);

			mFileHandle = mOverlappedInfo.hEvent = INVALID_HANDLE_VALUE;
		}
		std::string mFileName;
		HANDLE      mFileHandle;
		_OVERLAPPED mOverlappedInfo;
		FileInfo    mFileInfo;
	};

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
		:mLastKnownError{ eFileSystemError::NONE }
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

	FileSystem::~FileSystem()
	{
		for (auto & elem : mDetectionFiles)
		{
			delete &*elem;
			elem = nullptr;
		}

	}

	std::string FileSystem::GetFullPath(std::string const & _FileName, eFileDir _ParentDirectory)
	{
		std::filesystem::path DirPath{ GetProjectFolders<std::string>(_ParentDirectory) };
		std::error_code error; 
		std::filesystem::recursive_directory_iterator DirIter { DirPath, std::filesystem::directory_options::skip_permission_denied, error };

		for (auto const & elem : DirIter )
		{
			if (elem.path().filename().string() == _FileName || std::filesystem::equivalent(mPathTable[_ParentDirectory] + _FileName,elem,error))
				return elem.path().string();
		}

		return std::string{};
	}

	std::wstring FileSystem::GetFullPath_w(std::wstring const& _FileName, eFileDir _ParentDirectory)
	{
		std::filesystem::path DirPath{ GetProjectFolders<std::string>(_ParentDirectory) };
		std::error_code error;
		std::filesystem::recursive_directory_iterator DirIter{ DirPath, std::filesystem::directory_options::skip_permission_denied, error };

		for (auto const & elem : DirIter)
		{
			if (elem.path().filename().wstring() == _FileName)
				return elem.path().wstring();
		}

		return std::wstring{};
	}

	HashString FileSystem::ConvertToRelative(HashString const & _FullPath, eFileDir _ParentDirectory) const
	{
		std::filesystem::path DirPath{ mPathTable[_ParentDirectory] };
		std::error_code error;
		std::filesystem::recursive_directory_iterator DirIter{ DirPath, std::filesystem::directory_options::skip_permission_denied, error };
		for (auto const & elem : DirIter)
		{
			if (std::filesystem::equivalent(DirPath, elem))
				return HashString{ elem.path().string().c_str() };
		}

		return HashString{};
	}

	std::string FileSystem::ConvertToRelative(std::string const & _FullPath, eFileDir _ParentDirectory) const
	{
		std::filesystem::path DirPath{ mPathTable[_ParentDirectory] };
		std::error_code error;
		std::filesystem::recursive_directory_iterator DirIter{ DirPath, std::filesystem::directory_options::skip_permission_denied, error };
		for (auto const & elem : DirIter)
		{
			if (std::filesystem::equivalent(DirPath, elem))
				return { elem.path().string() };
		}

		return std::string {};
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
			if (filename == wstrFileName || std::filesystem::equivalent(elem, _FileName))
				return true;
		}

		return false;
	}

	unsigned FileSystem::DetectFileChanges(std::string _FilePath, std::string * _ChangesBuffer, size_t _size)
	{
		DetectionInfo * pDetectionInfo = nullptr;
		/*Find for existing Files*/
		for (auto const & elem : mDetectionFiles)
		{
			if (elem->mFileName == _FilePath)
			{
				pDetectionInfo = elem;
				break;
			}

		}

		if (!pDetectionInfo)
		{
			HANDLE hand = CreateFileA(_FilePath.c_str(),
				FILE_LIST_DIRECTORY,
				FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
				NULL,
				OPEN_EXISTING,
				FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
				NULL);

			if (hand != INVALID_HANDLE_VALUE)
			{
				_OVERLAPPED _overlapObj;
				DWORD       byte_read;
				_overlapObj.hEvent = CreateEventA(NULL, true, false, _FilePath.c_str());

				if (_overlapObj.hEvent == INVALID_HANDLE_VALUE)
				{
					/*Failed to create a Event/_OVERLAPPED*/
					CloseHandle(hand);
					mLastKnownError = eFileSystemError::CREATE_OVERLAP_ERROR;
					return 0;
				}

				DetectionInfo ** ptr = mDetectionFiles.Emplace(new DetectionInfo{ _FilePath, hand, _overlapObj });

				if (ReadDirectoryChangesW((*ptr)->mFileHandle,
					&(*ptr)->mFileInfo.front(),
					static_cast<DWORD>((*ptr)->mFileInfo.size()),
					true,
					FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_CREATION | FILE_NOTIFY_CHANGE_LAST_WRITE,
					&byte_read,
					&(*ptr)->mOverlappedInfo,
					NULL))

				{
					/*Success*/
					mLastKnownError = eFileSystemError::NONE;
				}
				if (GetLastError() == ERROR_NOTIFY_ENUM_DIR || byte_read == 0)
				{
					/*Failed to read directory successfully*/
					mLastKnownError = eFileSystemError::READ_DIRECTORY_ERROR;
					CloseHandle((*ptr)->mFileHandle);
					CloseHandle((*ptr)->mOverlappedInfo.hEvent);
					(*ptr)->mOverlappedInfo.hEvent = (*ptr)->mFileHandle = INVALID_HANDLE_VALUE;
					return 0; 
				}

				/*Detect and store file changes. If false, means no changes. If true means there is changes*/
				return GetChangesInfo(**ptr, _ChangesBuffer, _size);
			}
			else
			{
				/*Failed to create a valid handle*/
				mLastKnownError = eFileSystemError::CREATE_HANDLE_ERROR;
				return 0;
			}
		}
		else
		{
			/*Detect and store file changes. If false, means no changes. If true means there is changes*/
			return GetChangesInfo(*pDetectionInfo, _ChangesBuffer, _size);
		}

		return 0;
	}

	FileErrorCode FileSystem::GetLastKnownError() const
	{
		return mLastKnownError;
	}

	unsigned FileSystem::GetChangesInfo(DetectionInfo & _DetectionInfo, std::string * _ChangesBuffer, size_t _size)
	{
		unsigned count = 0;
		DWORD byte_read;

		while (GetOverlappedResult(_DetectionInfo.mFileHandle, &_DetectionInfo.mOverlappedInfo, &byte_read, false))
		{
			for (FILE_NOTIFY_INFORMATION * pstart = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(&_DetectionInfo.mFileInfo.front());
				pstart != nullptr;
				pstart = pstart->NextEntryOffset ? reinterpret_cast<FILE_NOTIFY_INFORMATION*>(reinterpret_cast<char *>(pstart) + (pstart->NextEntryOffset)) : nullptr)
			{
				std::wstring FileName{ pstart->FileName, pstart->FileName + (pstart->FileNameLength / sizeof(*(pstart->FileName))) };

				switch (pstart->Action)
				{
				case FILE_ACTION_ADDED:
				{
					if (count < _size)
						_ChangesBuffer[count++] = std::string{ FileName.begin() , FileName.end() };
				}
				break;
				case FILE_ACTION_MODIFIED:
				{
					bool found = false;
					std::string name{ FileName.begin(),FileName.end() };
					for (unsigned i = 0; i < count; ++i)
					{
						if (_ChangesBuffer[i] == name)
						{
							_ChangesBuffer[i] = std::string{ FileName.begin(),FileName.end() };
							found = true;
							break;
						}
					}
					if(count < _size && !found)
						_ChangesBuffer[count++] = name;
				}
				break;

				case FILE_ACTION_REMOVED:
					break;

				case FILE_ACTION_RENAMED_NEW_NAME:
				{
					for (unsigned i = 0; i < count; ++i)
					{
						if (_ChangesBuffer[i] == "")
						{
							_ChangesBuffer[i] = std::string{ FileName.begin(),FileName.end() };
							break;
						}
					}
				}
					break;
				case FILE_ACTION_RENAMED_OLD_NAME:
				{
					std::string name{ FileName.begin(),FileName.end() };
					for (unsigned i = 0; i < count; ++i)
					{
						if (_ChangesBuffer[i] == name)
						{
							_ChangesBuffer[i] = "";
							break;
						}
					}
				}
					break;
				}

				mLastKnownError = eFileSystemError::NONE;
			}
			/*Do ReadFirst Function again*/
			if (ReadDirectoryChangesW((_DetectionInfo).mFileHandle,
				&(_DetectionInfo).mFileInfo.front(),
				static_cast<DWORD>((_DetectionInfo).mFileInfo.size()),
				false,
				FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_CREATION | FILE_NOTIFY_CHANGE_LAST_WRITE,
				&byte_read,
				&(_DetectionInfo).mOverlappedInfo,
				NULL))

			{
				/*Success*/
				return count;
			}
			if (GetLastError() == ERROR_NOTIFY_ENUM_DIR || byte_read == 0)
			{
				mLastKnownError = eFileSystemError::READ_DIRECTORY_ERROR;
				CloseHandle((_DetectionInfo).mFileHandle);
				CloseHandle((_DetectionInfo).mOverlappedInfo.hEvent);
				_DetectionInfo.mOverlappedInfo.hEvent = _DetectionInfo.mFileHandle = INVALID_HANDLE_VALUE;
				return 0;
			}

		}
		return 0;
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

	void FileSystem::ChangeDirPath(eFileDir _dirToChange, const HashString& _newPath)
	{
		mPathTable[_dirToChange] = _newPath.c_str();
	}

	HashString FileSystem::FindFilePath(const HashString& _file, eFileDir _parentDir)
	{
		for (auto& p : std::filesystem::recursive_directory_iterator(mPathTable[_parentDir]))
		{
			HashString temp{ p.path().filename().string().c_str() };
			if (temp == _file)
			{
				return HashString{ p.path().generic_string().c_str() };
			}
		}
		return HashString{};
	}
}


