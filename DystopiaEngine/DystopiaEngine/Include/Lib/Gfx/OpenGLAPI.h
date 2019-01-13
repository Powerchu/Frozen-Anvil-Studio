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
#include "GfxOptions.h"
#include "../GraphicsLib.h"

namespace Gfx
{
	class OpenGL_API final : public GraphicsAPI
	{
	public:

		OpenGL_API(void) noexcept;

		void PrintEnvironment(void) const noexcept override;
		void ToggleVSync(bool) const noexcept override;

		ShaderProg CreateShaderProgram(void) noexcept override;
		ShaderPipeline CreateShaderPipeline(void) noexcept override;
		Shader CompileGLSL(Gfx::ShaderStage const&, void const* _pData) noexcept override;
		bool LinkShaderImpl(ShaderProg const&, Shader const*, size_t) noexcept override;

		int GetUniformLocation(ShaderProg const&, char const*) noexcept override;
		void UploadUniform1f(ShaderProg const&, unsigned _nLoc, unsigned _nCount, float    const*) noexcept override;
		void UploadUniform2f(ShaderProg const&, unsigned _nLoc, unsigned _nCount, float    const*) noexcept override;
		void UploadUniform3f(ShaderProg const&, unsigned _nLoc, unsigned _nCount, float    const*) noexcept override;
		void UploadUniform4f(ShaderProg const&, unsigned _nLoc, unsigned _nCount, float    const*) noexcept override;
		void UploadUniform1i(ShaderProg const&, unsigned _nLoc, unsigned _nCount, int      const*) noexcept override;
		void UploadUniform2i(ShaderProg const&, unsigned _nLoc, unsigned _nCount, int      const*) noexcept override;
		void UploadUniform3i(ShaderProg const&, unsigned _nLoc, unsigned _nCount, int      const*) noexcept override;
		void UploadUniform4i(ShaderProg const&, unsigned _nLoc, unsigned _nCount, int      const*) noexcept override;
		void UploadUniform1u(ShaderProg const&, unsigned _nLoc, unsigned _nCount, unsigned const*) noexcept override;
		void UploadUniform2u(ShaderProg const&, unsigned _nLoc, unsigned _nCount, unsigned const*) noexcept override;
		void UploadUniform3u(ShaderProg const&, unsigned _nLoc, unsigned _nCount, unsigned const*) noexcept override;
		void UploadUniform4u(ShaderProg const&, unsigned _nLoc, unsigned _nCount, unsigned const*) noexcept override;

		void UploadMatrix2(ShaderProg const&, unsigned _nLoc, unsigned _nCount, float const*, bool _bTranspose = true) noexcept override;
		void UploadMatrix4(ShaderProg const&, unsigned _nLoc, unsigned _nCount, float const*, bool _bTranspose = true) noexcept override;

		void UseShaderPipeline(ShaderPipeline const&) noexcept override;
		void AttachShaderProgram(ShaderPipeline const&, ShaderProg const&, ShaderStage const&) noexcept override;

		void FreeShader(Shader&) noexcept override;
		void FreeShaderProgram(ShaderProg&) noexcept override;
		void FreeShaderPipeline(ShaderPipeline&) noexcept override;

		bool InitGraphicsAPI(void const*) noexcept;

	private:
		void* mOpenGL;
		Opts mAvailable;

		unsigned CreateShader(Gfx::ShaderStage) noexcept;
		unsigned StageToBitmask(Gfx::ShaderStage) noexcept;

		bool BindContext(void*) noexcept;
		bool VersionSelect(void* const&) noexcept;
	};
}



#endif      // INCLUDE GUARD

