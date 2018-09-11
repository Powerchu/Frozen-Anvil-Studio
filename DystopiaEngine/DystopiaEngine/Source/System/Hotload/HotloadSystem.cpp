#include "System/Hotload/HotloadSystem.h"




#define TESTING 1
#define HOME_WORK 2
#define SCHOOL_WORK 3
#define LAPTOP 4
#define CURRENT_WORK_PLACE HOME_WORK
namespace Dystopia
{
	/*To Do: Change so that the absolute file path is not hard coded*/
#if CURRENT_WORK_PLACE == SCHOOL_WORK
	LPCSTR HotloadSystem::HEADER_DEFAULT_PATH = "\DystopiaEngine/Include/Behaviour";
	LPCSTR HotloadSystem::SOURCE_DEFAULT_PATH = "C:\\Users\\keith.goh\\source\\repos\\Frozen-Anvil-Studio\\DystopiaEngine\\DystopiaEngine\\Source\\Behaviour\\"; /*Absolute file path must be in this format*/
#elif CURRENT_WORK_PLACE == HOME_WORK
	LPCSTR HotloadSystem::HEADER_DEFAULT_PATH = "C:/Users/Owner/Source/Repos/Frozen-Anvil-Studio/DystopiaEngine/DystopiaEngine/Include/Behaviour";
	LPCSTR HotloadSystem::SOURCE_DEFAULT_PATH = "C:/Users/Owner/Source/Repos/Frozen-Anvil-Studio/DystopiaEngine/DystopiaEngine/Source/Behaviour";
#elif CURRENT_WORK_PLACE == LAPTOP
	LPCSTR HotloadSystem::HEADER_DEFAULT_PATH = "C:/Users/Keith/source/repos/Frozen-Anvil-Studio/DystopiaEngine/DystopiaEngine/Include/Behaviour";
	LPCSTR HotloadSystem::SOURCE_DEFAULT_PATH = "C:/Users/Keith/source/repos/Frozen-Anvil-Studio/DystopiaEngine/DystopiaEngine/Source/Behaviour";
#endif


	HotloadSystem::HotloadSystem()
	{
		for (auto & elem : mFileHandles)
			elem = INVALID_HANDLE_VALUE;
		for (auto & elem : marrOverlapped)
			elem.hEvent = INVALID_HANDLE_VALUE;
	}

	bool HotloadSystem::Init(void)
	{

		/*
		Use SHGetFolderPathA to get Path to %APPDATA%
		and assign it to mDll_Folder_Name
		Remember to ask jacky if he has the HWND
		*/
		HANDLE File_Hand_Source   = CreateFileA(mSource_Folder_Name.c_str(),
			                                    FILE_LIST_DIRECTORY,
			                                    FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			                                    NULL,
			                                    OPEN_EXISTING,
			                                    FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
			                                    NULL);

		HANDLE File_Hand_Includes = CreateFileA(mHeader_Folder_Name.c_str(),
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

		marrOverlapped[eSource_Index].hEvent  = CreateEventA(NULL, true, false,  mSource_Folder_Name.c_str());
		marrOverlapped[eInclude_Index].hEvent = CreateEventA(NULL, true, false,  mHeader_Folder_Name.c_str());
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

		LocateAndLoadCompiler();

		return true;
	}

	void Dystopia::HotloadSystem::Update(float)
	{
		/*Keep track of newly created/edited .h/.cpp files for behaviour*/
		//static constexpr unsigned NumOfFileInfo = 256;
		DWORD bytes_read;
		if (mFileHandles[eSource_Index] != INVALID_HANDLE_VALUE && marrOverlapped[eSource_Index].hEvent != INVALID_HANDLE_VALUE)
		{
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
		}

		if (mFileHandles[eInclude_Index] != INVALID_HANDLE_VALUE && marrOverlapped[eInclude_Index].hEvent != INVALID_HANDLE_VALUE)
		{
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
		}
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
	bool HotloadSystem::isDllModified(DLLWrapper ** _pDLLWrapper, unsigned size)
	{
		DWORD bytes_read;
		unsigned count = 0;

		if (GetOverlappedResult(mFileHandles[eDll_Index], &marrOverlapped[eDll_Index], &bytes_read, false))
		{
			ResetEvent(marrOverlapped[eDll_Index].hEvent);

			for (FILE_NOTIFY_INFORMATION * pstart = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(&marrFileInfo[eDll_Index].front());
				pstart != nullptr;
				pstart = pstart->NextEntryOffset ? reinterpret_cast<FILE_NOTIFY_INFORMATION*>(reinterpret_cast<char *>(pstart) + (pstart->NextEntryOffset)) : nullptr)
			{
				
				std::wstring DllDir { mDll_Folder_Name.begin(), mDll_Folder_Name.end()};
				std::wstring DllName{ pstart->FileName, pstart->FileName + (pstart->FileNameLength / sizeof(*(pstart->FileName))) };

				//if (CheckFileExtension(DllName) != eDll)
					//continue;


				switch (pstart->Action)
				{
				case FILE_ACTION_ADDED:
				{
					DllDir += DllName;
					HMODULE dllModule = LoadLibrary(DllDir.c_str());
					if (dllModule != NULL)
					{
						auto pointer  = mvDLL.Emplace(DllDir, DllName, dllModule);

						if(count < size)
							*(_pDLLWrapper + count++) = pointer;
					}
				}
				break;

				case FILE_ACTION_MODIFIED:
				{
					DllDir += DllName;
					if (mvDLL.IsEmpty())
					{
						HMODULE dllModule = LoadLibrary(DllDir.c_str());
						if (dllModule != NULL)
						{
							auto pDllWrapper = mvDLL.Emplace(DllDir,DllName, dllModule);
							if (count < size)
								*(_pDLLWrapper + count++) = pDllWrapper;
						}
					}
					else
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
								{
									auto pDllWrapper = mvDLL.Emplace(DllDir, DllName, dllModule);
									if (count < size)
										*(_pDLLWrapper + count++) = pDllWrapper;
								}
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

			}

			ReadDirectoryChangesW(mFileHandles[eDll_Index],
				                   &marrFileInfo[eDll_Index].front(),
				                   marrFileInfo[eDll_Index].size(),
				                   true,
				                   FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_CREATION | FILE_NOTIFY_CHANGE_LAST_WRITE,
				                   &bytes_read,
				                   &marrOverlapped[eDll_Index],
				                   NULL);

			*(_pDLLWrapper + count) = nullptr;
			return true;
		}
		return false;
	}
	void HotloadSystem::SetDllFolder(std::string _mDllFolderPath)
	{
		mDll_Folder_Name = _mDllFolderPath;
	}
	void HotloadSystem::SetHeaderFolder(std::string _mHeaderFolderPath)
	{
		mHeader_Folder_Name = _mHeaderFolderPath;
	}
	void HotloadSystem::SetCourceFolder(std::string _mSourceFolderPath)
	{
		mSource_Folder_Name = _mSourceFolderPath;
	}
	void HotloadSystem::Recompile(HANDLE const & _File_Handle, FILE_NOTIFY_INFORMATION * pFileInfo)
	{
	
		if (mVcvarPath == L"" || mCmdPath == L"" )
			return;
		/*
		Checklist.
		- Execute a process									   (DONE)
		- Make the process be a compiler					   (NOT DONE)
		- Make the compiler recompile the file that was edited (NOT DONE) 
		- Generate the dll									   (NOT DONE)
		*/
		//STARTUPINFO			si;
		//PROCESS_INFORMATION pi;

		//ZeroMemory(&si, sizeof(si));
		//ZeroMemory(&pi, sizeof(pi));

		/*To Do: Change so that the absolute file path is not hard coded
		         Locate msvc on computer and have it run cl.exe*/
		/*Testing that multi process works. This is not the msvc compiler*/

#if CURRENT_WORK_PLACE == SCHOOL_WORK
		LPCWSTR msvc_location = L"C:/Program Files (x86)/Microsoft Visual Studio 14.0/VC/bin/cl.exe";
		LPWSTR  command_line  = L"/WX /W4 /EHsc /DLL /nologo C:/Users/keith.goh/source/repos/Frozen-Anvil-Studio/DystopiaEngine/DystopiaEngine/Resource/Behaviours/Whatever.cpp";
#else
	    
		LPCWSTR msvc_location = L"C:/Program Files(x86)/Microsoft Visual Studio/2017/Community/VC/Tools/MSVC/14.15.26726/bin/Hostx64/x64/cl.exe";
			//L"C:/Program Files (x86)/Microsoft Visual Studio 14.0/VC/bin/cl.exe";
		LPWSTR  command_line = L"/WX /W4 /EHsc /DLL /nologo C:/Users/keith.goh/source/repos/Frozen-Anvil-Studio/DystopiaEngine/DystopiaEngine/Resource/Behaviours/Whatever.cpp";
#endif

		
		std::wstring wstrDll_Folder_Name   { mDll_Folder_Name.begin(),mDll_Folder_Name.end() };
		std::wstring wstrSource_Folder_Name{ mSource_Folder_Name.begin(), mSource_Folder_Name.end() };
		std::wstring FileName              { pFileInfo->FileName,pFileInfo->FileNameLength / sizeof(*pFileInfo->FileName) };

		std::wstring CmdArgument           = L"/k cd ";
		std::wstring Flags                 = mCompilerFlags;
		std::wstring OutputCommand;


		CmdArgument += mVcvarPath + L" && " + mVcvarName + L" " + mVcvarBuildEnv + L" && cd \"" + wstrDll_Folder_Name.c_str() + L"\" && ";

		OutputCommand += wstrSource_Folder_Name + L"/";
		OutputCommand += FileName;


		std::wstring Final_Command = CmdArgument + Flags + L' ' + OutputCommand + L" && exit";

		std::string cFinal_Command{ Final_Command.begin(),Final_Command.end() };
		std::string cCmdPath{ mCmdPath.begin(), mCmdPath.end() };

		//si.cb = sizeof(si);

		SHELLEXECUTEINFO ExecInfo{ 0 };
		ExecInfo.cbSize       = sizeof(SHELLEXECUTEINFO);
		ExecInfo.fMask        = SEE_MASK_NOCLOSEPROCESS;
		ExecInfo.hwnd         = NULL;
		ExecInfo.lpVerb       = NULL;
		ExecInfo.lpFile       = mCmdPath.c_str();
		ExecInfo.lpParameters = Final_Command.c_str();
		ExecInfo.lpDirectory  = NULL;
		ExecInfo.nShow        = SW_SHOW;
		ExecInfo.hInstApp     = NULL;

		if ( ShellExecuteEx(&ExecInfo) == false)
		{
			std::cout << "ShellExecuteA Failed" << std::endl;
		}

		WaitForSingleObject(ExecInfo.hProcess, INFINITE);

		CloseHandle(ExecInfo.hProcess);
		//CloseHandle(pi.hThread);

		/*
if (!CreateProcess(mCmdPath.c_str(),
	pFlags,
	NULL,
	NULL,
	true,
	CREATE_NEW_CONSOLE | CREATE_PRESERVE_CODE_AUTHZ_LEVEL | INHERIT_PARENT_AFFINITY,
	"SystemRoot=C:/Windows",
	NULL,
	&si,
	&pi)
	)
{
	throw("CreateProcess failed");
	return;
}
*/
	}


	void HotloadSystem::CheckReloadDll()
	{
		/*Check for latest added or modified DLL in %APPDATA%*/
		/*If the DLL is modified, update the current entry in mvDLL*/
		/*If the DLL is newly created, add it to mvDLL*/
	}
	bool HotloadSystem::LocateAndLoadCompiler()
	{
		std::filesystem::path p{ "C:/" };
		std::error_code error;
		std::filesystem::recursive_directory_iterator i{ p,std::filesystem::directory_options::skip_permission_denied,error };
		std::wstring name;

		DWORD BinaryTypeStatus;
		bool cl_status     = false;
		bool cmd_status    = false;
		bool vcvars_status = false;

		for (auto & elem : i)
		{
			name = (elem).path().filename().wstring();
			if( name == L"cmd.exe" && !cmd_status)
			{
				mCmdPath = elem.path().wstring();

				if (GetBinaryTypeA(elem.path().string().c_str(), &BinaryTypeStatus))
				{
					if (BinaryTypeStatus == SCS_64BIT_BINARY)
					{
						mVcvarBuildEnv = BIT32_ENV;
					}
					else if (BinaryTypeStatus == SCS_32BIT_BINARY)
					{
						mVcvarBuildEnv = BIT32_ENV;
					}
				}

				cmd_status |= true;
			}
			else if (name == L"vcvarsall.bat" && !vcvars_status)
			{
				mVcvarName = elem.path().filename().wstring();
				mVcvarPath = L'\"' + elem.path().parent_path().wstring() + L'\"';

				vcvars_status |= true;
			}

			if (cmd_status & vcvars_status)
				return true;
		}

		return cl_status & cmd_status & vcvars_status;
	}
	eFileExtension HotloadSystem::CheckFileExtension(std::wstring const & _FileName)
	{
		using ExtensionTable = std::map<std::wstring, eFileExtension>;
		
		static ExtensionTable Table
		{
			std::make_pair(L".cpp", eCpp),
			std::make_pair(L".h"  , eHeader),
			std::make_pair(L".hpp", eHeader),
			std::make_pair(L".dll", eDll),
			std::make_pair(L".lib", eLib)
		};
		for (auto & elem : Table)
		{
			size_t pos = _FileName.find_last_of(elem.first);
			if (pos != std::wstring::npos)
				return elem.second;
		}
		
		return eNotValid;
	}
}

