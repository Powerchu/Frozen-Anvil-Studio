#pragma once
#ifndef HOTLOADSYSTEM_H
#define HOTLOADSYSTEM_H


#include "System/Base/Systems.h"
#include "DataStructure/AutoArray.h"
#include <windows.h>
#include <map>
#include <functional>
#include <utility>
namespace Dystopia
{
	enum eFileHandle
	{
		eInclude_Index = 0,
		eSource_Index,
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

	private : 
		

		static LPCSTR HEADER_DEFAULT_PATH;
		static LPCSTR SOURCE_DEFAULT_PATH;

		AutoArray<DLLWrapper> mvDLL;
		HANDLE mFileHandles[eTotalFileHandles];
		HANDLE mIncludeHandle;

		void Recompile(HANDLE const & _File_Handle, FILE_NOTIFY_INFORMATION * pFileInfo = nullptr);
		void CheckReloadDll();
	};
}


#endif