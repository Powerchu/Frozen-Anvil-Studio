/* HEADER *********************************************************************************/
/*!
\file	Lib\Shaders.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Contains the definitions for shaders used for the Graphics API

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _LIB_GFX_SHADERS_
#define _LIB_GFX_SHADERS_

#include "Utility/EnumOps.h"


namespace Gfx
{
	enum class ShaderStage : unsigned
	{
		VERTEX    = 0x1,
		FRAGMENT  = 0x2,
		GEOMETRY  = 0x4,
		COMPUTE   = 0x20,

		NONE = 0x0,
		ALL  = 0xFFFFFFFF,
	};

	using Shader = class SHADER_ *;
	using ShaderProg = class SHADERPROGRAM_ *;
	using ShaderPipeline = class SHADERPIPELINE_ *;
}

namespace Ut
{
	template <>
	struct EnumBitOps<::Gfx::ShaderStage>
	{
		static constexpr bool value = true;
	};
}



#endif      // INCLUDE GUARD

