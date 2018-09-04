#include "System/Hotload/HotloadSystem.h"

#define TESTING 1

namespace Dystopia
{
	/*To Do: Change so that the absolute file path is not hard coded*/
	LPCSTR HotloadSystem::HEADER_DEFAULT_PATH = "\DystopiaEngine/Include/Behaviour";
	LPCSTR HotloadSystem::SOURCE_DEFAULT_PATH = "C:\\Users\\Owner\\source\\repos\\Frozen-Anvil-Studio\\DystopiaEngine\\DystopiaEngine\\Source\\Behaviour\\"; /*Absolute file path must be in this format*/

	void Dystopia::HotloadSystem::Update(float)
	{
		/*Keep track of newly created/edited .h/.cpp files for behaviour*/

		/*Use FindFirstChangeNotificationA function*/
		HANDLE File_Hand = FindFirstChangeNotificationA(SOURCE_DEFAULT_PATH, true, FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_FILE_NAME);

#if TESTING
		while (File_Hand == INVALID_HANDLE_VALUE)
			File_Hand = FindFirstChangeNotificationA(SOURCE_DEFAULT_PATH, true, FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_FILE_NAME);;
#else
		if (File_Hand == INVALID_HANDLE_VALUE)
			return;
#endif

		Recompile(File_Hand);
	}

	void Dystopia::HotloadSystem::Shutdown(void)
	{
	}
	ReferenceFunc HotloadSystem::GetDllFunc(LPCWSTR _dllFileName, LPCSTR _dllFuncName)
	{
		HMODULE module_handle = LoadLibrary(_dllFileName);

		if (module_handle == NULL)
			return ReferenceFunc{};
		/*Keep track of the opened dlls*/
		/*
		-Insert into vectors of dlls
		*/
		/*Load the function*/
		return ReferenceFunc();
	}
	void HotloadSystem::Recompile(HANDLE const & _File_Handle)
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
		LPCWSTR msvc_location = L"C:\\Windows\\System32\\cmd.exe";
		LPWSTR  command_line  = L"";
		ZeroMemory(&si, sizeof(si));
		ZeroMemory(&pi, sizeof(pi));

		si.cb = sizeof(si);

		if (!CreateProcess(msvc_location,
			command_line,
			NULL,
			NULL,
			FALSE,
			CREATE_NEW_CONSOLE,
			NULL,
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
}

