/* HEADER *********************************************************************************/
/*!
\file	GfxOptions.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _LIB_GFX_OPTIONS_H_
#define _LIB_GFX_OPTIONS_H_

#include "Utility/EnumOps.h"


namespace Gfx
{
	enum class Opts : unsigned
	{
		GRAPHICS_MSAA    = 1 << 0,

		GRAPHICS_COMPUTE = 1 << 3,
		GRAPHICS_VSYNC   = 1 << 4,

		GRAPHICS_GEOG    = 1 << 12,
		GRAPHICS_TESS    = 1 << 13,

		GRAPHICS_ALL     = 0xFFFFFFFF
	};
}

namespace Ut
{
	template <>
	struct EnumBitOps<::Gfx::Opts>
	{
		static constexpr bool value = true;
	};
}



#endif      // INCLUDE GUARD

