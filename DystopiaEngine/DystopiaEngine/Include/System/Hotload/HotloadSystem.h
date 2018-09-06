#pragma once
#ifndef HOTLOADSYSTEM_H
#define HOTLOADSYSTEM_H


#include "System/Base/Systems.h"
#include "DataStructure/AutoArray.h"
#include "DataStructure/MagicArray.h"
#include <windows.h>
#include <map>
#include <functional>
#include <utility>
#include <array>
namespace Dystopia
{
	enum eFileHandle
	{
		eInclude_Index = 0,
		eSource_Index,
		eDll_Index,
		eTotalFileHandles
	};
	struct DLLWrapper
	{
		DLLWrapper(std::wstring _DllFileName, HMODULE _DllModule)
			:mDllFileName{ _DllFileName },
			mDllModule{ _DllModule }
		{

		}
		bool operator==(DLLWrapper const & _rhs)
		{
			return _rhs.mDllFileName == mDllFileName;
		}
		bool operator==(std::wstring _rhsName)
		{
			return _rhsName == mDllFileName;
		}
		HMODULE ReloadDll()
		{
			FreeLibrary(mDllModule);
			mDllModule = LoadLibrary(mDllFileName.c_str());
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

		template<typename ReturnType, typename ... ParamType>
		ReturnType operator()(std::string _ExportedFuncName, ParamType && ... params)
		{
			using fpType = ReturnType(*) (ParamType...);
			FARPROC dllFunc = GetProcAddress(mDllModule, _ExportedFuncName.c_str());
			assert(dllFunc == NULL)
			return (reinterpret_cast<fpType>(*dllFunc))();
		}
	private:
		std::wstring mDllFileName;
		HMODULE		 mDllModule;
		
	};

	struct HotloadSystem : public Systems
	{
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

		bool isDllModified(DLLWrapper * _pDLLWrapperBuffer = nullptr, unsigned size = 0);

	private :

		static constexpr unsigned NumOfFileInfo = 256;
		using  ArrayFileInfo = std::array<char, NumOfFileInfo * MAX_PATH * sizeof(WCHAR)>;

		static LPCSTR HEADER_DEFAULT_PATH;
		static LPCSTR SOURCE_DEFAULT_PATH;

		std::string   mDll_Folder_Name = "C:/Users/Owner/AppData/Roaming/Dystopia/Behaviours/";

		MagicArray<DLLWrapper> mvDLL;

		HANDLE        mFileHandles[eTotalFileHandles];
		ArrayFileInfo marrFileInfo[eTotalFileHandles];
		_OVERLAPPED   marrOverlapped[eTotalFileHandles];

		void Recompile(HANDLE const & _File_Handle, FILE_NOTIFY_INFORMATION * pFileInfo = nullptr);
		void CheckReloadDll();
	};
}


#endif