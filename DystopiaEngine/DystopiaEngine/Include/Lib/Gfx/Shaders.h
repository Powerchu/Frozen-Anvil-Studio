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
		COMPUTE   = 0x20,

		NONE = 0x0,
		ALL  = 0xFFFFFFFF,
	};

	using Shader = class SHADER_ *;
	using ShaderProg = class SHADERPROGRAM_ *;
	using ShaderPipeline = class SHADERPIPELINE_ *;
}






// ============================================ FUNCTION DEFINITIONS ============================================ // 

inline Gfx::ShaderStage operator | (Gfx::ShaderStage const& _lhs, Gfx::ShaderStage const& _rhs)
{
	return static_cast<Gfx::ShaderStage>(
		static_cast<unsigned>(_lhs) | static_cast<unsigned>(_rhs)
	);
}

inline Gfx::ShaderStage operator & (Gfx::ShaderStage const& _lhs, Gfx::ShaderStage const& _rhs)
{
	return static_cast<Gfx::ShaderStage>(
		static_cast<unsigned>(_lhs) & static_cast<unsigned>(_rhs)
	);
}

inline Gfx::ShaderStage operator ^ (Gfx::ShaderStage const& _lhs, Gfx::ShaderStage const& _rhs)
{
	return static_cast<Gfx::ShaderStage>(
		static_cast<unsigned>(_lhs) ^ static_cast<unsigned>(_rhs)
	);
}

inline Gfx::ShaderStage operator ~ (Gfx::ShaderStage const& _v)
{
	return static_cast<Gfx::ShaderStage>(
		~static_cast<unsigned>(_v)
	);
}

inline Gfx::ShaderStage& operator |= (Gfx::ShaderStage& _lhs, Gfx::ShaderStage const& _rhs)
{
	return _lhs = _lhs | _rhs;
}

inline Gfx::ShaderStage& operator &= (Gfx::ShaderStage& _lhs, Gfx::ShaderStage const& _rhs)
{
	return _lhs = _lhs & _rhs;
}

inline Gfx::ShaderStage& operator ^= (Gfx::ShaderStage& _lhs, Gfx::ShaderStage const& _rhs)
{
	return _lhs = _lhs ^ _rhs;
}



#endif      // INCLUDE GUARD

