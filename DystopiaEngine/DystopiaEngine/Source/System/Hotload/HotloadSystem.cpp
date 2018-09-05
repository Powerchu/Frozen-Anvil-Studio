#include "System/Hotload/HotloadSystem.h"

#include <filesystem>
#include <array>

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
		HANDLE File_Hand_Source   = CreateFileA(SOURCE_DEFAULT_PATH,
			                                    FILE_LIST_DIRECTORY,
			                                    FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			                                    NULL,
			                                    OPEN_EXISTING,
			                                    FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
			                                    NULL);
		//FindFirstChangeNotificationA(SOURCE_DEFAULT_PATH, true, FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_FILE_NAME);
		HANDLE File_Hand_Includes = CreateFileA(HEADER_DEFAULT_PATH,
			                                    FILE_LIST_DIRECTORY, 
			                                    FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			                                    NULL,
			                                    OPEN_EXISTING,
			                                    FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
			                                    NULL);

		if (File_Hand_Source == INVALID_HANDLE_VALUE)
			return false;

		if (File_Hand_Includes == INVALID_HANDLE_VALUE)
		{
			CloseHandle(File_Hand_Source);
			return false;
		}

		mFileHandles[eSource_Index]  = File_Hand_Source;
		mFileHandles[eInclude_Index] = File_Hand_Includes;
		return true;
	}

	void Dystopia::HotloadSystem::Update(float)
	{
		/*Keep track of newly created/edited .h/.cpp files for behaviour*/

		static constexpr unsigned NumOfFileInfo = 256;
		using ArrayFileInfo  = std::array<char, NumOfFileInfo * MAX_PATH * sizeof(WCHAR)>;
		static ArrayFileInfo buffer;
		DWORD bytes_read;

		if (mFileHandles[eSource_Index] == INVALID_HANDLE_VALUE || mFileHandles[eInclude_Index] == INVALID_HANDLE_VALUE)
		{
			return;
		}

		if (ReadDirectoryChangesW(mFileHandles[eSource_Index],
			                      &buffer.front(),
								  buffer.size(),
			                      true,
			                      FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_CREATION | FILE_NOTIFY_CHANGE_LAST_WRITE,
			                      &bytes_read,
			                      NULL,
			                      NULL))
		{
			Recompile(mFileHandles[eSource_Index], reinterpret_cast<FILE_NOTIFY_INFORMATION *>(&buffer.front()));
		}

		if (ReadDirectoryChangesW(mFileHandles[eInclude_Index],
			                      &buffer.front(),
			                      buffer.size(),
			                      true,
			                      FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_CREATION | FILE_NOTIFY_CHANGE_LAST_WRITE,
			                      &bytes_read,
			                      NULL,
			                      NULL))
		{
			Recompile(mFileHandles[eInclude_Index], reinterpret_cast<FILE_NOTIFY_INFORMATION *>(&buffer.front()));
		}
	}

	void Dystopia::HotloadSystem::Shutdown(void)
	{
		if (mFileHandles[eSource_Index] != INVALID_HANDLE_VALUE)
			CloseHandle(mFileHandles[eSource_Index]);

		if (mFileHandles[eInclude_Index] != INVALID_HANDLE_VALUE)
			CloseHandle(mFileHandles[eInclude_Index]);
	}

	FARPROC HotloadSystem::GetDllFuncTest(LPCWSTR _dllFileName, LPCSTR _dllFuncName)
	{
		HMODULE module_handle = LoadLibrary(_dllFileName);

		if (module_handle == NULL)
			return FARPROC{};

		FARPROC f = GetProcAddress(module_handle, _dllFuncName);
		return f;
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

