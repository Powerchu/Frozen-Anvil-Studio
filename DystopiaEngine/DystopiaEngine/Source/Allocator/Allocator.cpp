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

#include "System/Logger/FileLogger.h"


void Dystopia::Alloc::WriteFreeMemory(Dystopia::FileLogger& _file)
{
	DefaultAlloc::WriteFreeMemory(_file);
}


void Dystopia::Alloc::WriteActiveAllocations(Dystopia::FileLogger& _file)
{
	DefaultAlloc::WriteActiveAllocations(_file);
}
