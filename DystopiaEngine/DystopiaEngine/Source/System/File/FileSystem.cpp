/* HEADER *********************************************************************************/
/*!
\file	FileSystem.cpp
\author keith.goh (100%)
\par    email: keith.goh\@digipen.edu
\brief

	Implementation for file system

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "System/File/FileSystem.h"
#include "System/Driver/Driver.h"

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

		DetectionInfo(std::string const & _name, HANDLE _FileHandle, _OVERLAPPED _OverlapObj, std::string _ParentDirectory = "")
			:mFileName{ _name }, mFileHandle{ _FileHandle }, mOverlappedInfo{ _OverlapObj }, mParentDirectory{_ParentDirectory}
		{}
		~DetectionInfo()
		{
			if (mFileHandle != INVALID_HANDLE_VALUE)
				CloseHandle(mFileHandle);
			if (mOverlappedInfo.hEvent != INVALID_HANDLE_VALUE)
				CloseHandle(mOverlappedInfo.hEvent);

			mFileHandle = mOverlappedInfo.hEvent = INVALID_HANDLE_VALUE;
		}

		std::string mParentDirectory;
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
		std::make_pair(eFileDir::eSolution ,  ""),
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
		mPathTable[eFileDir::eSolution] = GetProjectFolders<std::string>(eFileDir::eSolution);
		mPathTable[eFileDir::eHeader] = GetProjectFolders<std::string>(eFileDir::eHeader);
		mPathTable[eFileDir::eSource] = GetProjectFolders<std::string>(eFileDir::eSource);
		mPathTable[eFileDir::eResource] = GetProjectFolders<std::string>(eFileDir::eResource);
	}

	FileSystem::~FileSystem()
	{
		for (auto & elem : mDetectionFiles)
		{
			delete &*elem;
			elem = nullptr;
		}

	}

	bool FileSystem::Init(void)
	{
		return true;
	}

	void FileSystem::Update(float)
	{
		/*Loop through all File Tracking Jobs and poll for updates*/
		std::string ListOfFileNames[20];
		for (auto & TrackJob : mDetectionFiles)
		{
			/*Get the list of file changes in the directory*/
			unsigned num  = 0;
			unsigned prev = 0;

			if ((num = GetChangesInfo(*TrackJob, ListOfFileNames, 20)))
			{
				prev = num;
			}
			for (unsigned i=0;i< prev;++i)
			{
				auto & names = ListOfFileNames[i];// .first;

				//names = Normalize(names);

				names = Ut::Move(Normalize(TrackJob->mParentDirectory + "/" + names));
				if ((names  == TrackJob->mFileName) || names == TrackJob->mParentDirectory + "/" + TrackJob->mFileName)
				{
					/*Find the Arrays of EventCallBack for the current Job and invoke them*/
					auto & ref = mMapOfTrackInfo[TrackJob->mFileHandle];
					for (auto & f : ref)
					{
						//f.second(TrackJob->mFileName.c_str());
						f.second();
						//f.second(names.c_str(), ListOfFileNames[i].second);
					}
					break;
				}
				names = "";
			}
		}
	}

	void FileSystem::Shutdown(void)
	{
	}

	void FileSystem::PostUpdate()
	{
	}

	void FileSystem::LoadDefaults(void)
	{
	}

	void FileSystem::LoadSettings(DysSerialiser_t &)
	{
	}

	void FileSystem::SaveSettings(DysSerialiser_t &)
	{
	}

	std::string FileSystem::GetFullPath(std::string const & _FileName, eFileDir _ParentDirectory)
	{
		std::filesystem::path DirPath{ mPathTable[_ParentDirectory] };
		std::error_code error; 
		std::filesystem::recursive_directory_iterator DirIter { DirPath, std::filesystem::directory_options::skip_permission_denied, error };
		std::string fullpath = Normalize(DirPath.string() + "/" + _FileName);
		for (auto const & elem : DirIter )
		{
			std::string FullName = Normalize(elem.path().string());
			if (elem.path().filename().string() == _FileName || FullName == fullpath)
			{
				//|| std::filesystem::equivalent(DirPath.wstring + "/"+ _FileName,elem,error))
				return elem.path().string();
			}

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

	HashString FileSystem::ConvertToRelative(HashString const &, eFileDir _ParentDirectory) const
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
		std::filesystem::path ParentPath{ _FullPath };
		std::string Path;
	
		auto begin = ParentPath.begin();
		auto end   = ParentPath.end();
		Path       = (*begin).string();
		while (begin != end)
		{
			if (std::filesystem::equivalent(Path, DirPath))
			{
				std::string toRet;
				while (++begin != end)
				{

					toRet += "/" + (begin)->string();
				}
				return toRet;
			}

			if(++begin != end)
				Path += '/' + (*(begin)).string();
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
		std::string name = Normalize(_FileName);
		std::string Full = Normalize(mPathTable[_Directory] + "/" + _FileName);
		std::filesystem::recursive_directory_iterator DirIter{ DirPath, std::filesystem::directory_options::skip_permission_denied, error };
		std::wstring wstrFileName{ name.begin(), name.end() };
		for (auto const & elem : DirIter)
		{
			std::wstring filename = elem.path().filename().wstring();
			std::string  full     = Normalize(elem.path().string());
			if (filename == wstrFileName || (full == name) || (full == Full))
				return true;
		}

		return false;
	}

	bool FileSystem::IsSameFile(std::string const & _lhs, std::string const & _rhs)
	{
		std::error_code err;
		return std::filesystem::equivalent(_lhs, _rhs, err);
	}

	std::string FileSystem::Normalize(std::string _FileName) const
	{
		size_t pos;
		while ((pos = _FileName.find("//")) != OString::nPos)
		{
			_FileName.replace(pos, strlen("//"), "/");
		}
		while ((pos = _FileName.find("\\\\")) != OString::nPos)
		{
			_FileName.replace(pos, strlen("\\\\"), "/");
		}
		while ((pos = _FileName.find("\\")) != OString::nPos)
		{
			_FileName.replace(pos, strlen("\\"), "/");
		}
		return _FileName;
	}

	bool FileSystem::DetectFileChanges(const std::string& _FilePath, eFileDir _ParentDirectory)
	{
		static std::string _ChangesBuffer[100];
		size_t _size = 100;
		DetectionInfo * pDetectionInfo = nullptr;

		std::filesystem::path DirPath{ _FilePath };
		std::error_code error;

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
			HANDLE hand = CreateFileA(mPathTable[_ParentDirectory].c_str(),
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
				if (auto count = GetChangesInfo(**ptr, _ChangesBuffer, _size))
				{
					
					for (unsigned i = 0; i < count; ++i)
					{
						//std::filesystem::path DirPath2{ _ChangesBuffer[i] };
						if (IsSameFile(_FilePath, GetFullPath(mPathTable[_ParentDirectory] + "/" + _ChangesBuffer[i], _ParentDirectory)))
						{
							return true;
						}
					}
				}
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
			if (auto count = GetChangesInfo(*pDetectionInfo, _ChangesBuffer, _size))
			{

				for (unsigned i = 0; i < count; ++i)
				{
					//std::filesystem::path DirPath2{ _ChangesBuffer[i] };
					if (IsSameFile(_FilePath, GetFullPath( mPathTable[_ParentDirectory] + "/" + _ChangesBuffer[i], _ParentDirectory)))
					{
						return true;
					}
				}
			}
		}

		return 0;
	}

	unsigned FileSystem::DetectDirectoryChanges(const std::string& _FilePath, std::string * _ChangesBuffer, size_t _size)
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

		//return 0;
	}

	FileErrorCode FileSystem::GetLastKnownError() const
	{
		return mLastKnownError;
	}

	unsigned FileSystem::GetChangesInfo(DetectionInfo & _DetectionInfo, std::string * _ChangesBuffer, size_t _size)
	{
		unsigned count = 0;
		DWORD byte_read;

		if (_DetectionInfo.mFileHandle == INVALID_HANDLE_VALUE || _DetectionInfo.mOverlappedInfo.hEvent == INVALID_HANDLE_VALUE)
			return count;

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
						if ( IsSameFile(name, _ChangesBuffer[i]))
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
						if (IsSameFile(name, _ChangesBuffer[i]))
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

	unsigned FileSystem::GetChangesInfo(DetectionInfo & _DetectionInfo, CallBackTrackInfo * _ChangesBuffer, size_t _size)
	{
		unsigned count = 0;
		DWORD byte_read;

		if (_DetectionInfo.mFileHandle == INVALID_HANDLE_VALUE || _DetectionInfo.mOverlappedInfo.hEvent == INVALID_HANDLE_VALUE)
			return count;

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
						_ChangesBuffer[count++] = std::make_pair(std::string{ FileName.begin() , FileName.end() }, eFileTrackStatus::ADDED);
				}
				break;
				case FILE_ACTION_MODIFIED:
				{
					bool found = false;
					std::string name{ FileName.begin(),FileName.end() };
					for (unsigned i = 0; i < count; ++i)
					{
						if (IsSameFile(name, _ChangesBuffer[i].first))
						{
							_ChangesBuffer[i] = std::make_pair(std::string{ FileName.begin(),FileName.end() }, eFileTrackStatus::MODIFIED);
							found = true;
							break;
						}
					}
					if (count < _size && !found)
						_ChangesBuffer[count++].first = name;
				}
				break;

				case FILE_ACTION_REMOVED:
				{
					if (count < _size)
						_ChangesBuffer[count++] = std::make_pair(std::string{ FileName.begin() , FileName.end() }, eFileTrackStatus::REMOVE);
				}
				break;

				case FILE_ACTION_RENAMED_NEW_NAME:
				{
					for (unsigned i = 0; i < count; ++i)
					{
						if (_ChangesBuffer[i].first == "")
						{
							_ChangesBuffer[i].first = std::string{ FileName.begin(),FileName.end() };
							_ChangesBuffer[i].second = eFileTrackStatus::MODIFIED;
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
						if (IsSameFile(name, _ChangesBuffer[i].first))
						{
							_ChangesBuffer[i].first = "";
							_ChangesBuffer[i].second = eFileTrackStatus::MODIFIED;
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

	void FileSystem::FileTrackCallBack(unsigned long dwErrorCode, unsigned long /*dwNumOfBytes*/, _OVERLAPPED * lpOverlapped)
	{
		/*Extract following info from OVERLAPPED object
		- Name of Detected File
		- Type of action
		*/
		switch (dwErrorCode)
		{
		case 0:{
			/*Success*/
		}
		break;

		default:{
#if __DEBUG | DEBUG
		/*Throw a error*/
#else
		/*Warning*/
#endif
			return;
		}
		break;
		}

		DWORD byte_read;

		/*Loop through list of registered events tied to a event which the same file path as the detected file*/
		/*Fire the event. The event registered should not require any arguement to be invoked*/
		for (auto & elem : mDetectionFiles)
		{
			if (elem->mOverlappedInfo.hEvent == lpOverlapped->hEvent)
			{
				static std::string _temp[100];
				unsigned int && results = GetChangesInfo(*elem, _temp, 100);

				/*If there is no changes detected*/
				if (!results)
					return;

				for (auto  const & FileNames : _temp)
				{
					if (FileNames == elem->mFileName)
					{
						/*Registered File Path of event is same as detected change*/
						/*Fire the registered events*/
						for (auto & TrackElem : mMapOfTrackInfo)
						{
							if (TrackElem.first == FileNames.c_str())
							{
								for (auto & EventElem : TrackElem.second)
									EventElem.second();
							}
						}

						/*Once the event is fired, restart the tracking*/
						if (ReadDirectoryChangesW((elem)->mFileHandle,
							&(elem)->mFileInfo.front(),
							static_cast<DWORD>((elem)->mFileInfo.size()),
							false,
							FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_CREATION | FILE_NOTIFY_CHANGE_LAST_WRITE,
							&byte_read,
							&(elem)->mOverlappedInfo,
							&Dystopia::FileTrackCallBack))

						{
							/*Success*/
						}

						return;
					}
				}
			}

		}
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

	HashString FileSystem::GetFromResource(const char *_str)
	{
		std::filesystem::path DirPath{ GetProjectFolders<std::string>(eFileDir::eResource) };
		std::error_code error;
		std::filesystem::recursive_directory_iterator DirIter{ DirPath, std::filesystem::directory_options::skip_permission_denied, error };

		for (auto const & elem : DirIter)
		{
			if (elem.path().filename().string() == _str || std::filesystem::equivalent(mPathTable[eFileDir::eResource] + _str, elem, error))
				return HashString{ elem.path().string().c_str() };
		}

		return HashString{};
	}

	FileTrackInfoID_t FileSystem::TrackFile(HashString _EventName, eFileDir _ParentDirectory)
	{
		_EventName = Normalize(_EventName.c_str()).c_str();
		//while ((pos = _EventName.find_first_of("//")) != OString::nPos)
		//{
		//	_EventName[pos] = '/';
		//	_EventName.erase(pos+1, 1);
		//}
		//while ((pos = _EventName.find_first_of("\\\\")) != OString::nPos)
		//{
		//	_EventName[pos] = '/';
		//	_EventName.erase(pos + 1, 1);
		//}
		//while ((pos = _EventName.find_first_of("\\")) != OString::nPos)
		//{
		//	_EventName[pos] = '/';
		//}


		DetectionInfo * pDetectionInfo = nullptr;

		/*Find for existing Files*/
		for (auto const & elem : mDetectionFiles)
		{
			if (elem->mFileName == GetFullPath(_EventName.c_str(), _ParentDirectory))
			{
				pDetectionInfo = elem;

				bool found = false;
				for (auto const & elem2 : mMapOfTrackInfo)
				{
					if (elem2.first == elem->mFileHandle)
					{
						found = true;
						return elem2.first;
					}

				}
				if (!found)
				{
					//FileTrackInfo temp = std::make_pair(elem->mFileHandle, AutoArray<EventInfo>{});
					mMapOfTrackInfo[elem->mFileHandle];
					return elem->mFileHandle;
				}
			}
		}

		if (!pDetectionInfo)
		{
			HANDLE hand = CreateFileA(mPathTable[_ParentDirectory].c_str(),
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
				_overlapObj.hEvent = CreateEventA(NULL, false, true, NULL);

				if (_overlapObj.hEvent == INVALID_HANDLE_VALUE || !_overlapObj.hEvent)
				{
					/*Failed to create a Event/_OVERLAPPED*/
					CloseHandle(hand);
					mLastKnownError = eFileSystemError::CREATE_OVERLAP_ERROR;
					return 0;
				}

				DetectionInfo ** ptr = mDetectionFiles.Emplace(new DetectionInfo{ _EventName.c_str(), hand, _overlapObj,mPathTable[_ParentDirectory] });
				if (ReadDirectoryChangesW((*ptr)->mFileHandle,
					&(*ptr)->mFileInfo[0],
					static_cast<DWORD>((*ptr)->mFileInfo.size()),
					true,
					FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_CREATION | FILE_NOTIFY_CHANGE_LAST_WRITE,
					&byte_read,
					&(*ptr)->mOverlappedInfo,
					&Dystopia::FileTrackCallBack
				   ))

				{
					/*Success*/
					mLastKnownError = eFileSystemError::NONE;
					mMapOfTrackInfo[(*ptr)->mFileHandle];
					//FileTrackInfo temp = std::make_pair((*ptr)->mFileHandle, AutoArray<EventInfo>{});
					//mMapOfTrackInfo.push_back(Ut::Move(temp));
					return (*ptr)->mFileHandle;
				}

				if (GetLastError() == ERROR_NOTIFY_ENUM_DIR || byte_read == 0)
				{
					/*Failed to read directory successfully*/
					mLastKnownError = eFileSystemError::READ_DIRECTORY_ERROR;
					CloseHandle((*ptr)->mFileHandle);
					CloseHandle((*ptr)->mOverlappedInfo.hEvent);
					(*ptr)->mOverlappedInfo.hEvent = (*ptr)->mFileHandle = INVALID_HANDLE_VALUE;
				}
			}
		}
		return 0;
	}

	void FileSystem::UnbindFileTrackEvent(FileEventCallBack_t _EventID, FileTrackInfoID_t _TrackID)
	{
		mMapOfTrackInfo[_TrackID].erase(_EventID);
	}

	void FileSystem::UnbindAllTrackEvent(FileTrackInfoID_t _TrackID)
	{
		mMapOfTrackInfo[_TrackID].clear();
	}

	void FileSystem::StopTrackFile(FileTrackInfoID_t _TrackID)
	{
		mMapOfTrackInfo.erase(_TrackID);
	}

	void FileTrackCallBack(unsigned long dwErrorCode, unsigned long dwNumOfBytes, _OVERLAPPED * lpOverlapped)
	{
		EngineCore::Get<FileSystem>()->FileTrackCallBack(dwErrorCode, dwNumOfBytes, lpOverlapped);
	}

}




