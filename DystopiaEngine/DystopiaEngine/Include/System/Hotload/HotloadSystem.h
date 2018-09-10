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
#include <shlwapi.h>

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
		eCpp    = 0,
		eHeader,
		eDll,
		eLib,
		eNotValid

	};
	struct DLLWrapper
	{
		DLLWrapper(std::wstring _DllPathName,std::wstring _DllFileName, HMODULE _DllModule)
			:mDllPathName{_DllPathName},
			 mDllFileName{ _DllFileName },
			 mDllModule  { _DllModule }
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

			if(mDllModule != INVALID_HANDLE_VALUE)

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

		virtual void PreInit(void)	{ };
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

	private :

		static constexpr unsigned NumOfFileInfo = 256;
		using  ArrayFileInfo = std::array<char, NumOfFileInfo * MAX_PATH * sizeof(WCHAR)>;

		static LPCSTR HEADER_DEFAULT_PATH;
		static LPCSTR SOURCE_DEFAULT_PATH;


		std::string   mDll_Folder_Name     = "C:/Users/Owner/AppData/Roaming/Dystopia/DLL/";
		std::string   mSource_Folder_Name  = SOURCE_DEFAULT_PATH;
		std::string   mHeader_Folder_Name  = HEADER_DEFAULT_PATH;

		std::wstring  mCmdPath;

		std::wstring  mVcvarPath;
		std::wstring  mVcvarName;
		std::wstring  mVcvarBuildEnv;

		std::wstring  mCompilerPath;
		std::wstring  mMSVCPath;
		std::wstring  mCompilerFlags = L"cl /W4 /EHsc /nologo /std:c++latest /DLL /Za /LD /IC:/Users/Owner/source/repos/Frozen-Anvil-Studio/DystopiaEngine/DystopiaEngine/Include";
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

}


#endif