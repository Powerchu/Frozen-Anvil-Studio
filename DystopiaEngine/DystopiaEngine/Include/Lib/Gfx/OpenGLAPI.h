/* HEADER *********************************************************************************/
/*!
\file	OpenGLAPI.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Interface to OpenGL

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _LIB_GFX_OPENGL_API_H_
#define _LIB_GFX_OPENGL_API_H_

#include "Shaders.h"
#include "../GraphicsLib.h"

namespace Gfx
{
	class OpenGL_API final : public GraphicsAPI
	{
	public:

		OpenGL_API(void) noexcept;

		unsigned CreateShaderProgram(void) noexcept override;
		unsigned CreateShader(Gfx::ShaderStage) noexcept override;
		unsigned CompileGLSL(void const* _pData) noexcept override;


		bool InitGraphicsAPI(void*) noexcept;

	private:
		void* mOpenGL;

		bool VersionSelect(void* const&) noexcept;
	};
}



#endif      // INCLUDE GUARD

