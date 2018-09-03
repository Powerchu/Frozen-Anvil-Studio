/* HEADER *********************************************************************************/
/*!
\file	GUID.h
\author Digipen (100%)
\par    email: keith.goh\@digipen.edu
\brief
	The Base class for generation of unique IDs.
	IDs are generated based on time stamps


All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#pragma once
#ifndef GUID_H
#define GUID_H
#define NUM_EMPTY_BITS 9
#define NUM_MSCOUNT_BITS 10
#define NUM_ID_BITS 45
#include "Component/ComponentList.h"

namespace Dystopia
{

	using ulong = unsigned long;

/********************************************************************************************
First 55 Bits are just ID  Numbers
\brief

This class will return a unsigned long as a unique ID.
unsigned long has 64 bits.

Inside the first 55 Bits,
The left most 45 bits are timestamp in milliseconds
The remainding 10 bits are the number of times this function is called within 1 millisecond

Next 9 Bits can be used for anything you want
********************************************************************************************/
	struct GUIDGenerator
	{
		static ulong GetUniqueID(void);
	};
}


#endif /*GUID_H*/
