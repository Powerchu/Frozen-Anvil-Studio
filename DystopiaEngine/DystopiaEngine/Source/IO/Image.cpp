/* HEADER *********************************************************************************/
/*!
\file	Image.cpp
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Class to hold raw image data loaded into memory

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "IO/Image.h"

#include "Allocator/DefaultAlloc.h"


Dystopia::Image::~Image(void) noexcept
{
	DefaultAllocator<void>::Free(mpImageData);
}

