/* HEADER *********************************************************************************/
/*!
\file	EditorProc.cpp
\author Digipen (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#include "Editor/EditorProc.h"
#include "Globals.h"

/* library includes */
#include <iostream>
#include <windows.h>
#include <bitset>
#include <ShlObj_core.h>
#include <tchar.h>
#include <objbase.h>
#include <string>
#include <cstdlib>

namespace Defaults
{
	static const std::wstring g_DefaultExt[eFileTypes::eLAST] =
	{
		L"dmap",
		L"dyst",
		L"dscene",
		L"dobj",
		L"dmesh",
		L"dfrag",
		L"dvert",
		L"dystor"
	};

	static constexpr COMDLG_FILTERSPEC g_Filter[eFileTypes::eLAST] =
	{
		{ L"DystopiaAtlas"		, L"*.dmap"		},
		{ L"DystopiaSettings"	, L"*.dyst"		},
		{ L"DystopiaScene"		, L"*.dscene"	},
		{ L"DystopiaPrefab"		, L"*.dobj"		},
		{ L"DystopiaMesh"		, L"*.dmesh"	},
		{ L"DystopiaFragS"		, L"*.dfrag"	},
		{ L"DystopiaVertS"		, L"*.dvert"	},
		{ L"DystopiaCrash"		, L"*.dystor"	},
	};
}

bool Dystopia::EditorProc::Load(HashString& _outPath, eFileTypes _type)
{
	bool result = false;
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	if (SUCCEEDED(hr))
	{
		IFileOpenDialog *pFileOpen;
		hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));
		if (SUCCEEDED(hr))
		{
			COMDLG_FILTERSPEC p[1] = { Defaults::g_Filter[_type] };
			pFileOpen->SetFileTypes(1, p);
			if (SUCCEEDED(pFileOpen->Show(NULL)))
			{
				IShellItem *pItem;
				if (SUCCEEDED(pFileOpen->GetResult(&pItem)))
				{
					PWSTR pszFilePath;
					hr = pItem->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING, &pszFilePath);
					if (SUCCEEDED(hr))
					{
						_outPath = std::wstring{ pszFilePath }.c_str();
						result = true;
						CoTaskMemFree(pszFilePath);
					}
					pItem->Release();
				}
			}
			pFileOpen->Release();
		}
		CoUninitialize();
	}
	return result;
}

bool Dystopia::EditorProc::BrowseFolder(HashString& _outPath)
{
	bool result = false;
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	if (SUCCEEDED(hr))
	{
		IFileOpenDialog *pFileOpen;
		hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));
		if (SUCCEEDED(hr))
		{
			pFileOpen->SetOptions(FOS_PICKFOLDERS);
			if (SUCCEEDED(pFileOpen->Show(NULL)))
			{
				IShellItem *pItem;
				if (SUCCEEDED(pFileOpen->GetResult(&pItem)))
				{
					PWSTR pszFilePath;
					hr = pItem->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING, &pszFilePath);
					if (SUCCEEDED(hr))
					{
						_outPath = std::wstring{ pszFilePath }.c_str();
						result = true;
						CoTaskMemFree(pszFilePath);
					}
					pItem->Release();
				}
			}
			pFileOpen->Release();
		}
		CoUninitialize();
	}
	return result;
}

bool Dystopia::EditorProc::SaveAs(HashString& _outName, HashString& _outPath, HWND _win, eFileTypes _type)
{
	bool result = false;
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	if (SUCCEEDED(hr))
	{
		IFileSaveDialog *pFileSave;
		hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL, IID_IFileSaveDialog, reinterpret_cast<void**>(&pFileSave));
		if (SUCCEEDED(hr))
		{
			pFileSave->SetDefaultExtension(Defaults::g_DefaultExt[_type].c_str());
			COMDLG_FILTERSPEC p[1] = { Defaults::g_Filter[_type] };
			pFileSave->SetFileTypes(1, p);
			if (SUCCEEDED(pFileSave->Show(_win)))
			{
				IShellItem *pItem;
				if (SUCCEEDED(pFileSave->GetResult(&pItem)))
				{
					PWSTR pszFilePath, pszFileName;
					if (SUCCEEDED(pItem->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING, &pszFilePath)) &&
						SUCCEEDED(pItem->GetDisplayName(SIGDN_NORMALDISPLAY, &pszFileName)))
					{
						std::wstring path{ pszFilePath };
						std::wstring name{ pszFileName };
						auto pos = name.find('*.');
						if (pos != std::string::npos)
							name.erase(pos);
						else
						{
							pos = name.find('.');
							if (pos != std::string::npos)
								name.erase(pos);
						}
						result = true;
						_outName = name.c_str();
						_outPath = path.c_str();
						CoTaskMemFree(pszFilePath);
					}
					pItem->Release();
				}
			}
			pFileSave->Release();
		}
		CoUninitialize();
	}
	return result;
}

#endif //EDITOR
