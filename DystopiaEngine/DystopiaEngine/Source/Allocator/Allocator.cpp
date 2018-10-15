/* HEADER *********************************************************************************/
/*!
\file	Allocator.cpp
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Mainly debug functions

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "Allocator/Allocator.h"
#include "Allocator/DefaultAllocImpl.h"

#include <cstdio>


void Dystopia::Alloc::WriteFreeMemory(void* _p)
{
#if defined (DEBUGALLOC)

	DefaultAlloc::WriteFreeMemory(_p);

#endif
}


void Dystopia::Alloc::WriteActiveAllocations(void* _p)
{
#if defined (DEBUGALLOC)

	DefaultAlloc::WriteActiveAllocations(_p);

#endif
}
