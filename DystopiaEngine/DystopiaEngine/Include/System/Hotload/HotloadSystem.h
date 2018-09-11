#pragma once
#ifndef HOTLOADSYSTEM_H
#define HOTLOADSYSTEM_H

#define BIT32_ENV L"x86_amd64"
#define BIT64_ENV L"amd64"

#include "System/Base/Systems.h"
#include "DataStructure/AutoArray.h"
#include "DataStructure/MagicArray.h"

#include <windows.h>
#include <map>
#include <functional>
#include <utility>
#include <array>
#include <filesystem>
#include <iostream>

namespace Dystopia
{
	enum eFileHandle
	{
		eInclude_Index = 0,
		eSource_Index,
		eDll_Index,
		eTotalFileHandles
	};
	enum eFileExtension
	{
		eCpp = 0,
		eHeader,
		eDll,
		eLib,
		eNotValid

	};
	struct DLLWrapper
	{
		DLLWrapper(std::wstring _DllPathName, std::wstring _DllFileName, HMODULE _DllModule)
			:mDllPathName{ _DllPathName },
			mDllFileName{ _DllFileName },
			mDllModule{ _DllModule }
		{

		}
		bool operator==(DLLWrapper const & _rhs)
		{
			return _rhs.mDllPathName == mDllPathName && _rhs.mDllFileName == mDllFileName;
		}

		bool operator==(std::wstring _rhsName)
		{
			return _rhsName == mDllFileName;
		}

		HMODULE ReloadDll()
		{
			FreeLibrary(mDllModule);
			mDllModule = LoadLibrary((mDllPathName + mDllFileName).c_str());
			return mDllModule;
		}

		bool isValid()
		{
			return mDllModule != NULL;
		}

		~DLLWrapper()
		{
			if (mDllModule)
				FreeLibrary(mDllModule);
		}

		HMODULE GetDllModule()
		{
			return mDllModule;
		}

		template<typename ReturnType, typename ... ParamType>
		ReturnType(*GetDllFunc(std::string _FuncName)) (ParamType ...)
		{
			FARPROC dllFunc = GetProcAddress(mDllModule, _FuncName.c_str());
			if (dllFunc)
				return (ReturnType(*) (ParamType...)) (dllFunc);

			return nullptr;
		}

		template<typename ReturnType, typename ... ParamType>
		ReturnType operator()(std::string _ExportedFuncName, ParamType && ... params)
		{
			using fpType = ReturnType(*) (ParamType...);
			FARPROC dllFunc = GetProcAddress(mDllModule, "Clone");
			_ExportedFuncName += '\0';

			if (mDllModule != INVALID_HANDLE_VALUE)

				if (dllFunc == NULL)
				{
					DWORD error = GetLastError();
					std::cout << error << std::endl;
				}
			return (reinterpret_cast<fpType>(*dllFunc))();
		}
	private:

		std::wstring mDllPathName;
		std::wstring mDllFileName;
		HMODULE		 mDllModule;

	};

	struct HotloadSystem : public Systems
	{
		HotloadSystem();

		virtual void PreInit(void) { };
		virtual bool Init(void);
		virtual void PostInit(void) { };

		virtual void FixedUpdate(float) { };
		virtual void Update(float);
		virtual void PostUpdate(void) { };
		virtual void Shutdown(void);

		virtual void LoadDefaults(void) { };
		virtual void LoadSettings(TextSerialiser&) { };

		virtual ~HotloadSystem(void) = default;

		//ReferenceFunc GetDllFunc(LPCWSTR _dllFileName, LPCSTR _dllFuncName);
		FARPROC GetDllFuncTest(LPCWSTR _dllFileName, LPCSTR _dllFuncName);

		bool isDllModified(DLLWrapper ** _pDLLWrapperBuffer = nullptr, unsigned size = 0);

		void SetDllFolder(std::string _mDllFolderPath);
		void SetHeaderFolder(std::string _mHeaderFolderPath);
		void SetCourceFolder(std::string _mSourceFolderPath);

	private:

		static constexpr unsigned NumOfFileInfo = 256;
		using  ArrayFileInfo = std::array<char, NumOfFileInfo * MAX_PATH * sizeof(WCHAR)>;

		static LPCSTR HEADER_DEFAULT_PATH;
		static LPCSTR SOURCE_DEFAULT_PATH;


		std::string   mDll_Folder_Name = "C:/Users/Keith/AppData/Roaming/Dystopia/DLL/";//"C:/Users/Owner/AppData/Roaming/Dystopia/DLL/";
		std::string   mSource_Folder_Name = SOURCE_DEFAULT_PATH;
		std::string   mHeader_Folder_Name = HEADER_DEFAULT_PATH;

		std::wstring  mCmdPath;

		std::wstring  mVcvarPath;
		std::wstring  mVcvarName;
		std::wstring  mVcvarBuildEnv;

		std::wstring  mCompilerPath;
		std::wstring  mMSVCPath;
		std::wstring  mCompilerFlags = L"cl /W4 /EHsc /nologo /std:c++latest /DLL /Za /LD /IC:/Users/Keith/source/repos/Frozen-Anvil-Studio/DystopiaEngine/DystopiaEngine/Include";
		/*
		L" /I\"C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Tools\MSVC\14.15.26726\include\" "
		L" /I\"C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Tools\MSVC\14.15.26726\lib\" "
		L" /Fe\"C:/Users/Owner/AppData/Roaming/Dystopia/DLL\" "
		L"C:/Users/Owner/source/repos/Frozen-Anvil-Studio/DystopiaEngine/DystopiaEngine/Source/Behaviour/TestClass.cpp";
		*/

		MagicArray<DLLWrapper> mvDLL;

		HANDLE        mFileHandles[eTotalFileHandles];
		ArrayFileInfo marrFileInfo[eTotalFileHandles];
		_OVERLAPPED   marrOverlapped[eTotalFileHandles];
		std::string   marrFolderNames[eTotalFileHandles];

		void Recompile(HANDLE const & _File_Handle, FILE_NOTIFY_INFORMATION * pFileInfo = nullptr);
		void CheckReloadDll();
		bool LocateAndLoadCompiler();

		eFileExtension CheckFileExtension(std::wstring const & _FileName);
	};



	template <unsigned TOTAL_FILE_DIRECTORIES>
	struct Hotloader
	{


		template<unsigned FILE_DIRECTORY>
		void SetFileDirectoryPath(std::string const & _FileName)
		{
			marrFilePath[FILE_DIRECTORY] = _FileName;
		}

		void SetFileDirectoryPath(unsigned Index, std::string const & _FileName)
		{
			marrFilePath[Index] = _FileName;
		}

		bool InitFileDirectory(unsigned _Index)
		{
			HANDLE hand = CreateFileA(marrFilePath[_Index].c_str(),
				FILE_LIST_DIRECTORY,
				FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
				NULL,
				OPEN_EXISTING,
				FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
				NULL);

			if (hand == INVALID_HANDLE_VALUE)
				return false;

			marrOverlapped[_Index].hEvent = CreateEventA(NULL, true, false, marrFilePath[_Index].c_str());

			if (marrOverlapped[_Index].hEvent == INVALID_HANDLE_VALUE)
				return false;

			return true;
		}

		bool InitReadFileDirectory(unsigned _Index)
		{
			DWORD bytes_read;

			/*Start Reading Directory*/
			return
				ReadDirectoryChangesW(mFileHandles[_Index],
					&marrFileInfo[_Index].front(),
					marrFileInfo[_Index].size(),
					false,
					FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_CREATION | FILE_NOTIFY_CHANGE_LAST_WRITE,
					&bytes_read,
					&marrOverlapped[eInclude_Index],
					NULL);
		}

		void CompileFiles(unsigned _Index, std::wstring const & _FileName)
		{
			std::wstring wstrDll_Folder_Name{ mDll_Folder_Name.begin()    ,mDll_Folder_Name.end() };
			std::wstring wstrFolder_Name{ marrFilePath[_Index].begin(), marrFilePath[_Index].end() };

			std::wstring FileName{ _FileName };

			std::wstring CmdArgument{ L"/k cd " };

			std::wstring OutputCommand;


			CmdArgument += mVcvarPath + L" && " + mVcvarName + L" " + mVcvarBuildEnv + L" && cd \"" + wstrDll_Folder_Name.c_str() + L"\" && ";

			OutputCommand += wstrFolder_Name + L"/";
			OutputCommand += FileName;


			std::wstring Final_Command = CmdArgument + mCompilerFlags + L' ' + OutputCommand + L" && exit";

			std::string cFinal_Command{ Final_Command.begin(),Final_Command.end() };
			std::string cCmdPath{ mCmdPath.begin(), mCmdPath.end() };

			SHELLEXECUTEINFO ExecInfo{ 0 };
			ExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
			ExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
			ExecInfo.hwnd = NULL;
			ExecInfo.lpVerb = NULL;
			ExecInfo.lpFile = mCmdPath.c_str();
			ExecInfo.lpParameters = Final_Command.c_str();
			ExecInfo.lpDirectory = NULL;
			ExecInfo.nShow = SW_SHOW;
			ExecInfo.hInstApp = NULL;

			if (ShellExecuteEx(&ExecInfo) == false)
			{
				std::cout << "ShellExecuteA Failed" << std::endl;
			}

			WaitForSingleObject(ExecInfo.hProcess, INFINITE);

			CloseHandle(ExecInfo.hProcess);
		}

		void Init()
		{
			for (unsigned i = 0; i < TOTAL_FILE_DIRECTORIES; ++i)
				InitReadFileDirectory(i);
			
			for (unsigned i = 0; i < TOTAL_FILE_DIRECTORIES; ++i)
			{
				std::filesystem::path p{ marrFilePath[i] };
				std::filesystem::filesystem_error error;
				std::filesystem::recursive_directory_iterator iter{ p, std::filesystem::directory_options::skip_permission_denied, error };

				for (auto & elem : iter)
				{
					if(CheckFileExtension(elem.path().filename().wstring()) == eCpp)
						CompileFiles(i, elem.path().filename().wstring())
				}
			}
		}

		void Update()
		{
			for (unsigned i = 0; i < TOTAL_FILE_DIRECTORIES; ++i)
			{
				if (marrFileHandles[i] != INVALID_HANDLE_VALUE && marraOverlapped[i].hEvent != INVALID_HANDLE_VALUE)
				{
					if (GetOverlappedResult(mFileHandles[i], &marrOverlapped[i], &bytes_read, false))
					{
						ResetEvent(marrOverlapped[i].hEvent);
						auto pFileInfo = reinterpret_cast<FILE_NOTIFY_INFORMATION *>(&marrFileInfo[i].front());
						for(auto pFileIterator = pFileInfo;
							pFileIterator != nullptr;
							pFileIterator = 
							static_cast<FILE_NOTIFY_INFORMATION *>(pFileIterator->NextEntryOffset ? reinterpret_cast<char *>(pFileIterator) + pFileIterator->NextEntryOffset : nullptr))
						{
							std::wstring wstrFileName{ pFileIterator->FileName, pFileIterator->FileNameLength / sizeof(*(pFileIterator->FileName)) };
							CompileFiles(i, wstrFileName);
						}
						InitReadFileDirectory(i);
					}
				}
			}

		}

	private:

		static constexpr unsigned NumOfFileInfo = 256;
		using  FileInfo = std::array<char, NumOfFileInfo * MAX_PATH * sizeof(WCHAR)>;


		std::string   mDll_Folder_Name = "C:/Users/Owner/AppData/Roaming/Dystopia/DLL/";

		std::wstring  mCmdPath;

		std::wstring  mVcvarPath;
		std::wstring  mVcvarName;
		std::wstring  mVcvarBuildEnv;

		std::wstring  mCompilerFlags = L"cl /W4 /EHsc /nologo /std:c++latest /DLL /Za /LD /IC:/Users/Owner/source/repos/Frozen-Anvil-Studio/DystopiaEngine/DystopiaEngine/Include";


		std::array<HANDLE, TOTAL_FILE_DIRECTORIES> marrFileHandles;
		std::array<FileInfo, TOTAL_FILE_DIRECTORIES> marrFileInfo;
		std::array<_OVERLAPPED, TOTAL_FILE_DIRECTORIES> marraOverlapped;
		std::array<std::string, TOTAL_FILE_DIRECTORIES> marrFilePath;


		MagicArray<DLLWrapper> mvDLL;

		void Recompile(HANDLE const & _File_Handle, FILE_NOTIFY_INFORMATION * pFileInfo = nullptr);

		void CheckReloadDll();

		bool LocateAndLoadCompiler()
		{
			std::filesystem::path p{ "C:/" };
			std::error_code error;
			std::filesystem::recursive_directory_iterator i{ p,std::filesystem::directory_options::skip_permission_denied,error };
			std::wstring name;

			DWORD BinaryTypeStatus;
			bool cl_status = false;
			bool cmd_status = false;
			bool vcvars_status = false;

			for (auto & elem : i)
			{
				name = (elem).path().filename().wstring();
				if (name == L"cmd.exe" && !cmd_status)
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

		eFileExtension CheckFileExtension(std::wstring const & _FileName)
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

	};

}
#endif
