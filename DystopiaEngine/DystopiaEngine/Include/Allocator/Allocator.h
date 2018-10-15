/* HEADER *********************************************************************************/
/*!
\file	Allocator.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Header file for allocators

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _ALLOCLIB_H_
#define _ALLOCLIB_H_

#include "DefaultAlloc.h"


namespace Dystopia::Alloc
{
	void WriteFreeMemory(void*);
	void WriteActiveAllocations(void*);
}



#endif

