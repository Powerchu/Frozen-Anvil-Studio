#include "System/Hotload/HotloadSystem.h"

#include <filesystem>


#define TESTING 1
#define HOME_WORK 2
#define SCHOOL_WORK 3
#define CURRENT_WORK_PLACE HOME_WORK
namespace Dystopia
{
	/*To Do: Change so that the absolute file path is not hard coded*/
#if CURRENT_WORK_PLACE == SCHOOL_WORK
	LPCSTR HotloadSystem::HEADER_DEFAULT_PATH = "\DystopiaEngine/Include/Behaviour";
	LPCSTR HotloadSystem::SOURCE_DEFAULT_PATH = "C:\\Users\\keith.goh\\source\\repos\\Frozen-Anvil-Studio\\DystopiaEngine\\DystopiaEngine\\Source\\Behaviour\\"; /*Absolute file path must be in this format*/
#else
	LPCSTR HotloadSystem::HEADER_DEFAULT_PATH = "C:/Users/Owner/Source/Repos/Frozen-Anvil-Studio/DystopiaEngine/DystopiaEngine/Include/Behaviour/";
	LPCSTR HotloadSystem::SOURCE_DEFAULT_PATH = "C:/Users/Owner/Source/Repos/Frozen-Anvil-Studio/DystopiaEngine/DystopiaEngine/Source/Behaviour/";
#endif


	bool HotloadSystem::Init(void)
	{
		/*
		Use SHGetFolderPathA to get Path to %APPDATA%
		and assign it to mDll_Folder_Name
		Remember to ask jacky if he has the HWND
		*/
		HANDLE File_Hand_Source   = CreateFileA(SOURCE_DEFAULT_PATH,
			                                    FILE_LIST_DIRECTORY,
			                                    FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			                                    NULL,
			                                    OPEN_EXISTING,
			                                    FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
			                                    NULL);

		HANDLE File_Hand_Includes = CreateFileA(HEADER_DEFAULT_PATH,
			                                    FILE_LIST_DIRECTORY | FILE_ALL_ACCESS,
			                                    FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			                                    NULL,
			                                    OPEN_EXISTING,
			                                    FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
			                                    NULL);


		HANDLE File_Hand_Dll      = CreateFileA(mDll_Folder_Name.c_str(),
			                                    FILE_LIST_DIRECTORY,
			                                    FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			                                    NULL,
			                                    OPEN_EXISTING,
			                                    FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
			                                    NULL);
		DWORD bytes_read;
		if (File_Hand_Source == INVALID_HANDLE_VALUE)
			return false;

		if (File_Hand_Includes == INVALID_HANDLE_VALUE)
		{
			CloseHandle(File_Hand_Source);
			return false;
		}
		if (File_Hand_Dll == INVALID_HANDLE_VALUE)
		{
			CloseHandle(File_Hand_Source);
			CloseHandle(File_Hand_Includes);
			return false;
		}

		marrOverlapped[eSource_Index].hEvent  = CreateEventA(NULL, true, false,  SOURCE_DEFAULT_PATH);
		marrOverlapped[eInclude_Index].hEvent = CreateEventA(NULL, true, false,  HEADER_DEFAULT_PATH);
		marrOverlapped[eDll_Index].hEvent     = CreateEventA(NULL, true, false,  mDll_Folder_Name.c_str());

		mFileHandles[eDll_Index]     = File_Hand_Dll;
		mFileHandles[eInclude_Index] = File_Hand_Includes;
		mFileHandles[eSource_Index]  = File_Hand_Source;

		if (mFileHandles[eSource_Index]  == INVALID_HANDLE_VALUE||
			mFileHandles[eInclude_Index] == INVALID_HANDLE_VALUE||
			mFileHandles[eDll_Index]     == INVALID_HANDLE_VALUE)
		{
			return false;
		}

		(ReadDirectoryChangesW(mFileHandles[eSource_Index],
			&marrFileInfo[eSource_Index].front(),
			marrFileInfo[eSource_Index].size(),
			true,
			FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_CREATION | FILE_NOTIFY_CHANGE_LAST_WRITE,
			&bytes_read,
			&marrOverlapped[eSource_Index],
			NULL));

		ReadDirectoryChangesW(mFileHandles[eInclude_Index],
			&marrFileInfo[eInclude_Index].front(),
			marrFileInfo[eInclude_Index].size(),
			true,
			FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_CREATION | FILE_NOTIFY_CHANGE_LAST_WRITE,
			&bytes_read,
			&marrOverlapped[eInclude_Index],
			NULL);

		ReadDirectoryChangesW(mFileHandles[eDll_Index],
			&marrFileInfo[eDll_Index].front(),
			marrFileInfo[eDll_Index].size(),
			true,
			FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_CREATION | FILE_NOTIFY_CHANGE_LAST_WRITE,
			&bytes_read,
			&marrOverlapped[eDll_Index],
			NULL);

		return true;
	}

	void Dystopia::HotloadSystem::Update(float)
	{
		/*Keep track of newly created/edited .h/.cpp files for behaviour*/
		//static constexpr unsigned NumOfFileInfo = 256;
		DWORD bytes_read;
		if (GetOverlappedResult(mFileHandles[eSource_Index], &marrOverlapped[eSource_Index], &bytes_read, false))
		{
			ResetEvent(marrOverlapped[eSource_Index].hEvent);
			Recompile(mFileHandles[eSource_Index], reinterpret_cast<FILE_NOTIFY_INFORMATION *>(&marrFileInfo[eSource_Index].front()));
			
			/*Not sure if this will cause resource leaks, but ResetEvent is not working for the mFileHandles*/
			(ReadDirectoryChangesW(mFileHandles[eSource_Index],
				                   &marrFileInfo[eSource_Index].front(),
				                   marrFileInfo[eSource_Index].size(),
				                   true,
				                   FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_CREATION | FILE_NOTIFY_CHANGE_LAST_WRITE,
				                   &bytes_read,
				                   &marrOverlapped[eSource_Index],
				                   NULL));
		}

		if (GetOverlappedResult(mFileHandles[eInclude_Index], &marrOverlapped[eInclude_Index], &bytes_read, false))
		{
			Recompile(mFileHandles[eInclude_Index], reinterpret_cast<FILE_NOTIFY_INFORMATION *>(&marrFileInfo[eInclude_Index].front()));
			ResetEvent(marrOverlapped[eInclude_Index].hEvent);
			(ReadDirectoryChangesW(mFileHandles[eInclude_Index],
								   &marrFileInfo[eInclude_Index].front(),
				                   marrFileInfo[eInclude_Index].size(),
				                   true,
				                   FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_CREATION | FILE_NOTIFY_CHANGE_LAST_WRITE,
				                   &bytes_read,
				                   &marrOverlapped[eInclude_Index],
				                   NULL));
		}
		isDllModified();
	}

	void Dystopia::HotloadSystem::Shutdown(void)
	{
		for (auto & elem : mFileHandles)
		{
			if (elem != INVALID_HANDLE_VALUE)
			{
				CancelIo(elem);
				CloseHandle(elem);
			}
		}
		for (auto & elem : marrOverlapped)
		{
			if (elem.hEvent != INVALID_HANDLE_VALUE)
			{
				CancelIo(elem.hEvent);
				CloseHandle(elem.hEvent);
			}
		}
	}

	FARPROC HotloadSystem::GetDllFuncTest(LPCWSTR _dllFileName, LPCSTR _dllFuncName)
	{
		HMODULE module_handle = LoadLibrary(_dllFileName);

		if (module_handle == NULL)
			return FARPROC{};

		FARPROC f = GetProcAddress(module_handle, _dllFuncName);
		return f;
	}
	bool HotloadSystem::isDllModified(DLLWrapper * _pDLLWrapper, unsigned size)
	{
		DWORD bytes_read;

		if (GetOverlappedResult(mFileHandles[eDll_Index], &marrOverlapped[eDll_Index], &bytes_read, false))
		{
			ResetEvent(marrOverlapped[eDll_Index].hEvent);

			for (FILE_NOTIFY_INFORMATION * pstart = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(&marrFileInfo[eDll_Index].front());
				pstart != nullptr; )
			{
				switch (pstart->Action)
				{
				case FILE_ACTION_ADDED:
				{
					std::wstring DllDir{ mDll_Folder_Name.begin(), mDll_Folder_Name.end() };
					std::wstring DllName{ pstart->FileName, pstart->FileName + (pstart->FileNameLength/sizeof(*(pstart->FileName))) };
					DllDir += DllName;
					HMODULE dllModule = LoadLibrary(DllDir.c_str());
					if (dllModule != NULL)
					{
						mvDLL.Insert(DLLWrapper{DllName, dllModule});
					}
				}
				break;
				case FILE_ACTION_MODIFIED:
				{
					std::wstring DllDir{ mDll_Folder_Name.begin(), mDll_Folder_Name.end() };
					std::wstring DllName{ pstart->FileName, pstart->FileName + (pstart->FileNameLength / sizeof(*(pstart->FileName))) };
					DllDir += DllName;
					/*
					if (mvDLL.IsEmpty())
					{
						HMODULE dllModule = LoadLibrary(DllDir.c_str());
						if (dllModule != NULL)
							mvDLL.Insert(DLLWrapper{ DllName, dllModule });
					}
					else
					*/
					{
						for (auto & elem : mvDLL)
						{
							if (elem == DllName)
							{
								elem.ReloadDll();
							}
							else
							{
								HMODULE dllModule = LoadLibrary(DllDir.c_str());
								if (dllModule != NULL)
									mvDLL.Insert(DLLWrapper{ DllName, dllModule });
							}
						}
					}

				}
				break;
				case FILE_ACTION_REMOVED:
				{
					std::wstring DllName{ pstart->FileName, pstart->FileName + (pstart->FileNameLength / sizeof(*(pstart->FileName))) };
					std::wstring DllDir{ mDll_Folder_Name.begin(), mDll_Folder_Name.end() };
					for (auto & elem : mvDLL)
					{
						if (elem == DllName)
						{
							mvDLL.Remove(&elem);
						}
					}
				}
				break;
				}
				pstart = pstart->NextEntryOffset?reinterpret_cast<FILE_NOTIFY_INFORMATION*>(reinterpret_cast<char *>(pstart) + (pstart->NextEntryOffset)):nullptr;
			}

			ReadDirectoryChangesW(mFileHandles[eDll_Index],
				                   &marrFileInfo[eDll_Index].front(),
				                   marrFileInfo[eDll_Index].size(),
				                   true,
				                   FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_CREATION | FILE_NOTIFY_CHANGE_LAST_WRITE,
				                   &bytes_read,
				                   &marrOverlapped[eDll_Index],
				                   NULL);

			return true;
		}
		return false;
	}
	void HotloadSystem::Recompile(HANDLE const & _File_Handle, FILE_NOTIFY_INFORMATION * pFileInfo)
	{
		/*
		Checklist.
		- Execute a process									   (DONE)
		- Make the process be a compiler					   (NOT DONE)
		- Make the compiler recompile the file that was edited (NOT DONE) 
		- Generate the dll									   (NOT DONE)
		*/
		STARTUPINFO			si;
		PROCESS_INFORMATION pi;

		/*To Do: Change so that the absolute file path is not hard coded
		         Locate msvc on computer and have it run cl.exe*/
		/*Testing that multi process works. This is not the msvc compiler*/

#if CURRENT_WORK_PLACE == SCHOOL_WORK
		LPCWSTR msvc_location = L"C:/Program Files (x86)/Microsoft Visual Studio 14.0/VC/bin/cl.exe";
		LPWSTR  command_line  = L"/WX /W4 /EHsc /DLL /nologo C:/Users/keith.goh/source/repos/Frozen-Anvil-Studio/DystopiaEngine/DystopiaEngine/Resource/Behaviours/Whatever.cpp";
#else
		LPCWSTR msvc_location = L"C:/Program Files (x86)/Microsoft Visual Studio 14.0/VC/bin/cl.exe";
		LPWSTR  command_line = L"/WX /W4 /EHsc /DLL /nologo C:/Users/keith.goh/source/repos/Frozen-Anvil-Studio/DystopiaEngine/DystopiaEngine/Resource/Behaviours/Whatever.cpp";
#endif
								//"/Fe\"C:\\Users\\keith.goh\\source\\repos\\Frozen-Anvil-Studio\\DystopiaEngine\\Whatever.dll\"";
		ZeroMemory(&si, sizeof(si));
		ZeroMemory(&pi, sizeof(pi));

		si.cb = sizeof(si);

		if (!CreateProcess(msvc_location,
			command_line,
			NULL,
			NULL,
			FALSE,
			CREATE_NEW_CONSOLE,
			"SystemRoot=C:\\Windows",
			NULL,
			&si,
			&pi)
			)
		{
			/*No such file found*/
			return;
		}

		WaitForSingleObject(pi.hProcess, INFINITE);

		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}


	void HotloadSystem::CheckReloadDll()
	{
		/*Check for latest added or modified DLL in %APPDATA%*/
		/*If the DLL is modified, update the current entry in mvDLL*/
		/*If the DLL is newly created, add it to mvDLL*/
	}
}

