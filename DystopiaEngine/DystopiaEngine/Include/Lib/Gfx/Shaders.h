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

namespace Gfx
{
	enum class ShaderStage : unsigned
	{
		VERTEX    = 0x1,
		FRAGMENT  = 0x2,
		GEOMETRY  = 0x4,
	};

	enum class ShaderLanguage
	{
		GLSL,
		SPIRV,
	};
}


#endif      // INCLUDE GUARD

