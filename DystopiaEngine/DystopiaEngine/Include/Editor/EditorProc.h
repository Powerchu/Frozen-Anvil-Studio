/* HEADER *********************************************************************************/
/*!
\file	EditorProc.h
\author Digipen (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#ifndef _EDITOR_PROCESS_H_
#define _EDITOR_PROCESS_H_
#include "DataStructure/HashString.h"

typedef struct HWND__* HWND;

enum eFileTypes : unsigned
{
	eATLAS = 0,
	eSETTINGS,
	eSCENE,
	ePREFABS,
	eMESH,
	eFRAG_S,
	eVERT_S,
	eCRASH,
	eLAST
};

namespace Dystopia
{
	struct EditorProc
	{
		bool Load(HashString& _outPath, eFileTypes);
		bool BrowseFolder(HashString& _outPath);
		bool SaveAs(HashString& _outName, HashString& _outPath, HWND _win, eFileTypes);
	};
}

#endif //_EDITOR_PROCESS_H_
#endif //EDITOR


