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

	bool FileSystem::DetectFileChanges(std::string _FilePath, std::string * _ChangesBuffer, size_t _size)
	{
		DetectionInfo * pDetectionInfo = nullptr;
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
				//mDetectionFiles.Insert(new DetectionInfo{}
				_OVERLAPPED _overlapObj;
				DWORD       byte_read;
				_overlapObj.hEvent = CreateEventA(NULL, true, false, _FilePath.c_str());

				if (_overlapObj.hEvent == INVALID_HANDLE_VALUE)
				{
					CloseHandle(hand);
					return false;
				}

				DetectionInfo ** ptr = mDetectionFiles.Emplace(new DetectionInfo{ _FilePath, hand, _overlapObj });

				if (ReadDirectoryChangesW((*ptr)->mFileHandle,
					&(*ptr)->mFileInfo.front(),
					static_cast<DWORD>((*ptr)->mFileInfo.size()),
					false,
					FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_CREATION | FILE_NOTIFY_CHANGE_LAST_WRITE,
					&byte_read,
					&(*ptr)->mOverlappedInfo,
					NULL))

				{

				}
				if (GetLastError() == ERROR_NOTIFY_ENUM_DIR || byte_read == 0)
				{
					CloseHandle((*ptr)->mFileHandle);
					CloseHandle((*ptr)->mOverlappedInfo.hEvent);
					(*ptr)->mOverlappedInfo.hEvent = (*ptr)->mFileHandle = INVALID_HANDLE_VALUE;

					return false; 
				}

				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			unsigned count = 0;
			DWORD byte_read;
			if (GetOverlappedResult(pDetectionInfo->mFileHandle, &pDetectionInfo->mOverlappedInfo, &byte_read, false))
			{
				for (FILE_NOTIFY_INFORMATION * pstart = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(&pDetectionInfo->mFileInfo.front());
					pstart != nullptr;
					pstart = pstart->NextEntryOffset ? reinterpret_cast<FILE_NOTIFY_INFORMATION*>(reinterpret_cast<char *>(pstart) + (pstart->NextEntryOffset)) : nullptr)
				{
					std::wstring FileName{ pstart->FileName, pstart->FileName + (pstart->FileNameLength / sizeof(*(pstart->FileName))) };

					switch (pstart->Action)
					{
					case FILE_ACTION_ADDED:
					case FILE_ACTION_MODIFIED:
					{
						_ChangesBuffer[count++] = std::string{ FileName.begin() , FileName.end() };
					}
					break;

					case FILE_ACTION_REMOVED:
					break;

					case FILE_ACTION_RENAMED_NEW_NAME:
					break;

					case FILE_ACTION_RENAMED_OLD_NAME:
					break;
					}
				}

				/*Do ReadFirst Function again*/
			}
		}


		return false;
	}

}

