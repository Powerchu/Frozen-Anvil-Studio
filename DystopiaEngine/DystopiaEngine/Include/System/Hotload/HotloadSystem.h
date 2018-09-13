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
#include <vector>

namespace Dystopia
{

	enum eFileExtension
	{
		eCpp = 0,
		eHeader,
		eDll,
		eLib,
		eObj,
		eNotValid

	};
	enum ePurpose
	{
		eCompile = 0,
		eLink,
		eExecute,
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
			mDllModule = LoadLibrary((mDllPathName + L'/'+ mDllFileName).c_str());
			return mDllModule;
		}

		bool isValid() const
		{
			return mDllModule != NULL;
		}

		~DLLWrapper()
		{
			if (mDllModule)
				FreeLibrary(mDllModule);
			mDllModule = NULL;
		}

		HMODULE GetDllModule()
		{
			return mDllModule;
		}

		void ExitLibrary()
		{
			if (mDllModule)
				FreeLibrary(mDllModule);
			mDllModule = NULL;
		}

		template<typename ReturnType, typename ... ParamType>
		ReturnType(*GetDllFunc(std::string _FuncName) const) (ParamType ...)
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

	struct InterestedFiles
	{
	

		InterestedFiles(std::wstring const & _FileName, ePurpose _purpose)
			:FileName    { _FileName },
			 found       {  false    },
			 mFilePurpose{ _purpose  }
		{

		}

		bool isFound() const
		{
			return found;
		}

		void SetFullFilePath(std::wstring const & _FullFilePath)
		{
			FullFilePath = _FullFilePath;
		}

		bool operator! ()
		{
			return !found;
		}

		void AddPossibleDirectory(std::wstring const & _DirectoryName)
		{
			mvPossibleDirectory.push_back(_DirectoryName);
		}

		std::vector<std::wstring> const & GetPossibleDirectories() const
		{
			return mvPossibleDirectory;
		}

		std::wstring GetFileName() const
		{
			return FileName;
		}

		void SetFound(bool _state)
		{
			found = _state;
		}

		ePurpose GetPurpose() const
		{
			return mFilePurpose;
		}

		std::wstring GetFullPath() const
		{
			return FullFilePath;
		}

	private:

		bool         found;
		std::wstring FileName;
		std::wstring FullFilePath;
		std::vector<std::wstring> mvPossibleDirectory;
		ePurpose     mFilePurpose;
	};


	template <unsigned TOTAL_FILE_DIRECTORIES>
	struct Hotloader
	{
		Hotloader()
			:marraOverlapped{0},
			marrFileHandles{INVALID_HANDLE_VALUE}
		{


		}

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
			marrFileHandles[_Index]        = hand;
			marraOverlapped[_Index].hEvent = CreateEventA(NULL, true, false, marrFilePath[_Index].c_str());

			if (marraOverlapped[_Index].hEvent == INVALID_HANDLE_VALUE)
				return false;

			return true;
		}

		bool InitReadFileDirectory(unsigned _Index)
		{
			DWORD bytes_read;

			/*Start Reading Directory*/
				if (ReadDirectoryChangesW(marrFileHandles[_Index],
					&marrFileInfo[_Index].front(),
					marrFileInfo[_Index].size(),
					false,
					FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_CREATION | FILE_NOTIFY_CHANGE_LAST_WRITE,
					&bytes_read,
					&marraOverlapped[_Index],
					NULL))
				{
					return true;
				}

			if (GetLastError() == ERROR_NOTIFY_ENUM_DIR || bytes_read == 0)
			{
				CloseHandle(marrFileHandles[_Index]);
				CloseHandle(marraOverlapped[_Index].hEvent);
				marraOverlapped[_Index].hEvent = marrFileHandles[_Index] = INVALID_HANDLE_VALUE;
				InitReadFileDirectory(_Index);
			}

			return false;
		}

		bool InitReadDllDirectory()
		{
			DWORD bytes_read;

			if (ReadDirectoryChangesW(mDll_Handle,
				&mDll_FileInfo.front(),
				mDll_FileInfo.size(),
				false,
				FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_CREATION | FILE_NOTIFY_CHANGE_LAST_WRITE,
				&bytes_read,
				&mDll_Overlap,
				NULL))

			{
				return true;
			}

			if (GetLastError() == ERROR_NOTIFY_ENUM_DIR || bytes_read == 0)
			{
				CloseHandle(mDll_Handle);
				CloseHandle(mDll_Overlap.hEvent);
				mDll_Overlap.hEvent = mDll_Handle = INVALID_HANDLE_VALUE;
				InitDllDirectory();
				InitReadDllDirectory();
			}

			return false;
		}

		bool ChangesInDllFolder(unsigned size = 0 , DLLWrapper ** _pDLLWrapperBuffer = nullptr)
		{
			DWORD bytes_read;
			unsigned ChangeCount = 0;
			bool status = false;

			if (GetOverlappedResult(mDll_Handle, &mDll_Overlap, &bytes_read, false))
			{
				for (FILE_NOTIFY_INFORMATION * pstart = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(&mDll_FileInfo.front());
					pstart != nullptr;
					pstart = pstart->NextEntryOffset ? reinterpret_cast<FILE_NOTIFY_INFORMATION*>(reinterpret_cast<char *>(pstart) + (pstart->NextEntryOffset)) : nullptr)
				{
					std::wstring DllDir { mDll_Folder_Name.begin(), mDll_Folder_Name.end() };
					std::wstring DllName{ pstart->FileName, pstart->FileName + (pstart->FileNameLength / sizeof(*(pstart->FileName)))};

					if (CheckFileExtension(DllName) != eDll)
					{
						continue;
					}

					switch (pstart->Action)
					{
					case FILE_ACTION_ADDED:
					{
						DllDir += DllName;
						HMODULE dllModule = LoadLibrary(DllDir.c_str());

						if (dllModule != NULL)
						{
							auto pointer = mvDLL.Emplace(DllDir, DllName, dllModule);

							if (_pDLLWrapperBuffer && ChangeCount < size)
								*(_pDLLWrapperBuffer + ChangeCount++) = pointer;

							status = true;
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
								auto pDllWrapper = mvDLL.Emplace(DllDir, DllName, dllModule);

								if (_pDLLWrapperBuffer && ChangeCount < size)
									*(_pDLLWrapperBuffer + ChangeCount++) = pDllWrapper;

								status = true;
							}
						}
						else
						{
							for (auto & elem : mvDLL)
							{
								if (elem == DllName)
								{
									elem.ReloadDll();
									if (_pDLLWrapperBuffer)
									{
										bool found = false;
										for (unsigned i = 0; i < ChangeCount; ++i)
										{
											if (elem == (**(_pDLLWrapperBuffer + i)))
											{
												*_pDLLWrapperBuffer = &elem;
												found = true;
											}

										}
										if (!found)
										{
											if (_pDLLWrapperBuffer && ChangeCount < size)
												*(_pDLLWrapperBuffer + ChangeCount++) = &elem;
										}
									}
									status = true;
								}
							}
						}
					}
					break;

					case FILE_ACTION_REMOVED:
					{

						}
					break;
					}
				}
				InitReadDllDirectory();
			}

			if (_pDLLWrapperBuffer)
				_pDLLWrapperBuffer[ChangeCount] = nullptr;

			return status;
		}

		void CompileFiles(unsigned _Index, std::wstring const & _FileName)
		{
			/*Search for exisiting opened DLL and close them to allow cl.exe to override*/
			SearchAndReplaceDll(GenerateDllName(_FileName));

			std::wstring wstrDll_Folder_Name{ mDll_Folder_Name.begin()    ,mDll_Folder_Name.end() };
			std::wstring wstrFolder_Name    { marrFilePath[_Index].begin(), marrFilePath[_Index].end() };

			std::wstring FileName           { _FileName };

			std::wstring CmdArgument{ L"/k cd " };

			std::wstring OutputCommand;


			CmdArgument += mVcvarPath + L" && " + mVcvarName + L" " + mVcvarBuildEnv + L" && cd \"" + wstrDll_Folder_Name.c_str() + L"\" && ";

			OutputCommand += L'\"' + wstrFolder_Name + L"/";
			OutputCommand += FileName + L'\"';

			/*
			OutputCommand += L" /link ";
			std::filesystem::path pp{ "C:/Users/Owner/source/repos/Frozen-Anvil-Studio/DystopiaEngine/bin" };
			std::error_code error1;

			std::filesystem::recursive_directory_iterator iter1{ pp,std::filesystem::directory_options::skip_permission_denied,error1 };
			for (auto & elem : iter1)
			{
				if (CheckFileExtension(elem.path().filename().wstring()) == eLib && elem.path().filename().wstring() != FileName)
				{
					std::string a = elem.path().string();
					OutputCommand += L" /LIBPATH\"" + elem.path().wstring() + L"\"";
				}
			}
			*/
			for (auto const & elem : mvAdditionalSourcePath)
			{
				std::filesystem::path p{ elem };
				std::error_code error;

				std::filesystem::recursive_directory_iterator iter{ p,std::filesystem::directory_options::skip_permission_denied,error };
				for (auto & elem : iter)
				{
					if (CheckFileExtension(elem.path().filename().wstring()) == eCpp && elem.path().filename().wstring() != FileName)
					{
						std::string a = elem.path().string();
						OutputCommand += L" \"" + elem.path().wstring() + L"\"";
					}
				}
			}

			for (auto const & elem : mvAddtionalFiles)
			{
				OutputCommand += L" \"" + elem + L"\"";
			}

			for (auto const & elem : mvFilesToCrawl)
			{
				if(elem.isFound() && elem.GetPurpose() == eCompile)
				OutputCommand += L" \"" + elem.GetFullPath() + L"\"";
			}
			
			std::wstring Final_Command = CmdArgument + mCompilerFlags + L" " + OutputCommand + L" && exit";



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

		bool Init()
		{
			LocateAndLoadCompiler();

			for (unsigned i = 0; i < TOTAL_FILE_DIRECTORIES; ++i)
			{
				if(InitFileDirectory(i))
					if (!InitReadFileDirectory(i))
					{
						std::cout << "Reading of Directory: " << marrFilePath[i] << " has failed \n";
					}
			}

			for (auto & elem : mvFilesToCrawl)
			{
				if (!LocateInterestedFiles(elem))
					std::cout << elem.GetFileName().c_str() << L" could not be found" << std::endl;
			}
			
			for (unsigned i = 0; i < TOTAL_FILE_DIRECTORIES; ++i)
			{
				std::filesystem::path p{ marrFilePath[i] };
				std::error_code error;
				std::filesystem::recursive_directory_iterator iter{ p, std::filesystem::directory_options::skip_permission_denied, error };

				for (auto & elem : iter)
				{
					auto result = CheckFileExtension(elem.path().filename().wstring());
					if (result == eCpp || result == eLib || result == eObj)
						CompileFiles(i, elem.path().filename().wstring());
				}
			}

			if (InitDllDirectory())
			{
				if (InitReadDllDirectory())
				{
					std::cout << "Reading of DLL Directory: " << mDll_Folder_Name << " has failed \n";
				}
			}

			InitDllLoading();
			return true;
		}

		void Update()
		{
			DWORD bytes_read;
			for (unsigned i = 0; i < TOTAL_FILE_DIRECTORIES; ++i)
			{
				if (marrFileHandles[i] != INVALID_HANDLE_VALUE && marraOverlapped[i].hEvent != INVALID_HANDLE_VALUE)
				{
					if (GetOverlappedResult(marrFileHandles[i], &marraOverlapped[i], &bytes_read, false))
					{
						//ResetEvent(marraOverlapped[i].hEvent);
						auto pFileInfo = reinterpret_cast<FILE_NOTIFY_INFORMATION *>(&marrFileInfo[i].front());
						for(auto pFileIterator = pFileInfo;
							pFileIterator != nullptr;
							pFileIterator = 
							reinterpret_cast<FILE_NOTIFY_INFORMATION *>(pFileIterator->NextEntryOffset ? reinterpret_cast<char *>(pFileIterator) + pFileIterator->NextEntryOffset : nullptr))
						{
							std::wstring wstrFileName{ pFileIterator->FileName, pFileIterator->FileNameLength / sizeof(*(pFileIterator->FileName)) };

							if(CheckFileExtension(wstrFileName) == eCpp)
								CompileFiles(i, wstrFileName);
						}
						InitReadFileDirectory(i);
					}
				}
			}

		}

		void SetDllFolderPath(std::string _PathName)
		{
			mDll_Folder_Name = _PathName;
		}

		bool InitDllDirectory()
		{
			
			HANDLE hand = CreateFileA(mDll_Folder_Name.c_str(),
				                      FILE_LIST_DIRECTORY,
				                      FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
				                      NULL,
				                      OPEN_EXISTING,
				                      FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
				                      NULL);

			if (hand == INVALID_HANDLE_VALUE)
				return false;
			mDll_Handle = hand;
			mDll_Overlap.hEvent = CreateEventA(NULL, true, false, mDll_Folder_Name.c_str());

			if (mDll_Handle == INVALID_HANDLE_VALUE)
				return false;

			return true;
		}

		MagicArray<DLLWrapper> const & GetDlls()
		{
			return mvDLL;
		}

		void AddAdditionalSourcePath(std::wstring const & _Path)
		{
			mvAdditionalSourcePath.push_back(_Path);
		}

		void AddFilesToCrawl(std::wstring const & _File, ePurpose _FilePurpose)
		{
			mvFilesToCrawl.push_back(InterestedFiles{ _File,_FilePurpose });
		}

		void AddAdditionalFiles(std::wstring const & _FullPathFile)
		{
			mvAddtionalFiles.push_back(_FullPathFile);
		}

		void ReinitCrawl()
		{
			for (auto & elem : mvFilesToCrawl)
			{
				if (!elem.isFound())
					if (!LocateInterestedFiles(elem))
						std::cout << elem.GetFileName() << L" could not be found" << std::endl;
			}
		}

		void SetCompilerFlags(std::wstring const & _Flags)
		{
			mCompilerFlags = _Flags;
		}

	private:

		static constexpr unsigned NumOfFileInfo = 256;
		using  FileInfo = std::array<char, NumOfFileInfo * MAX_PATH * sizeof(WCHAR)>;


		std::string   mDll_Folder_Name;
		HANDLE        mDll_Handle;
		FileInfo	  mDll_FileInfo;
		_OVERLAPPED   mDll_Overlap;

		std::wstring  mCmdPath;

		std::wstring  mVcvarPath;
		std::wstring  mVcvarName;
		std::wstring  mVcvarBuildEnv;

		std::wstring  mCompilerFlags = L"cl /W3 /EHsc /nologo /std:c++latest /DLL /Za /LDd /IC:/Users/keith.goh/source/repos/Frozen-Anvil-Studio/DystopiaEngine/DystopiaEngine/Include";
			                            //"/IC:/Users/Keith/source/repos/Frozen-Anvil-Studio/DystopiaEngine/Dependancies/glew-2.1.0/include "
										//"/IC:/Users/Keith/source/repos/Frozen-Anvil-Studio/DystopiaEngine/Dependancies/ImGui";



		std::array<HANDLE,      TOTAL_FILE_DIRECTORIES>  marrFileHandles;
		std::array<FileInfo,    TOTAL_FILE_DIRECTORIES>  marrFileInfo;
		std::array<_OVERLAPPED, TOTAL_FILE_DIRECTORIES>  marraOverlapped;
		std::array<std::string, TOTAL_FILE_DIRECTORIES>  marrFilePath;

		std::vector<std::wstring>                        mvAdditionalSourcePath;
		std::vector<std::wstring>                        mvAddtionalFiles;
		std::vector<InterestedFiles>                     mvFilesToCrawl;
		MagicArray<DLLWrapper>                           mvDLL;










		bool SearchAndReplaceDll(std::wstring const & _FileName)
		{
			for (auto & elem : mvDLL)
			{
				if (elem == _FileName)
				{
					elem.ExitLibrary();
					return true;
				}
			}
			return false;
		}

		std::wstring GenerateDllName(std::wstring const & _FileName)
		{
			size_t pos = _FileName.find_last_of(L'.');
			std::wstring DllName = _FileName.substr(0,pos);
			return DllName + L".dll";
			
		}

		bool LocateAndLoadCompiler()
		{
			std::filesystem::path p{ "C:/" };
			std::error_code error;
			std::filesystem::recursive_directory_iterator i{ p,std::filesystem::directory_options::skip_permission_denied,error };
			std::wstring name;

			std::filesystem::path vcPath{ "C:/Program Files (x86)/Microsoft Visual Studio" };
			std::filesystem::recursive_directory_iterator vciter{ vcPath ,std::filesystem::directory_options::skip_permission_denied,error };

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
			}

			for(auto & elem : vciter)
			{

				name = (elem).path().filename().wstring();
				if (name == L"vcvarsall.bat" && !vcvars_status)
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
				std::make_pair(L".lib", eLib),
				std::make_pair(L".obj", eObj)
			};
			for (auto & elem : Table)
			{
				size_t pos = _FileName.rfind(elem.first);
				if (pos != std::wstring::npos && _FileName.substr(pos) == elem.first)
					return elem.second;
			}

			return eNotValid;
		}

		void InitDllLoading()
		{
			std::filesystem::path p{ mDll_Folder_Name };
			std::error_code error;
			std::filesystem::recursive_directory_iterator iter{ p,std::filesystem::directory_options::skip_permission_denied,error };
			for (auto & elem : iter)
			{
				if (CheckFileExtension(elem.path().filename().wstring()) == eDll)
				{
					HMODULE dllModule = LoadLibrary(elem.path().wstring().c_str());
					if (dllModule != NULL)
					{
						auto pointer = mvDLL.Emplace(elem.path().parent_path().wstring(), elem.path().filename().wstring(), dllModule);
					}
				}
			}
		}

		bool LocateInterestedFiles(InterestedFiles & _File)
		{
			auto const & PossibleLocation = _File.GetPossibleDirectories();
			for (auto const & elem : PossibleLocation)
			{
				std::filesystem::path p{ elem };
				std::error_code err;
				std::filesystem::recursive_directory_iterator iter { p,std::filesystem::directory_options::skip_permission_denied,err };
				for(auto & file : iter)
					if (file.path().filename().wstring() == _File.GetFileName())
					{
						_File.SetFullFilePath(file.path().wstring());
						_File.SetFound(true);

						return true;
					}
			}

			std::filesystem::path p{ "C:/" };
			std::error_code err;
			std::filesystem::recursive_directory_iterator iter{ p,std::filesystem::directory_options::skip_permission_denied,err };
			for (auto & file : iter)
				if (file.path().filename().wstring() == _File.GetFileName())
				{
					_File.SetFullFilePath(file.path().wstring());
					_File.SetFound(true);

					return true;
				}


			return false;
		}

	};

}
#endif
