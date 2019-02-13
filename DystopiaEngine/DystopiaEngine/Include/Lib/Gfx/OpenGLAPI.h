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

#include "DataStructure/HashString.h"

#if defined(RUNTIME_GFXAPI) && RUNTIME_GFXAPI
#define OVERRIDE_ override
#else
#define OVERRIDE_
#endif


namespace Gfx
{
	class OpenGL_API final : public GraphicsAPI
	{
	public:

		OpenGL_API(void) noexcept;

		void PrintEnvironment(void) const noexcept OVERRIDE_;
		void ToggleVSync(bool) const noexcept OVERRIDE_;

		ShaderProg CreateShaderProgram(void) noexcept OVERRIDE_;
		ShaderPipeline CreateShaderPipeline(void) noexcept OVERRIDE_;
		Shader CompileGLSL(Gfx::ShaderStage const&, void const* _pData) noexcept OVERRIDE_;
		bool LinkShaderImpl(ShaderProg const&, Shader const*, size_t) noexcept OVERRIDE_;

		int GetUniformLocation(ShaderProg const&, char const*) noexcept OVERRIDE_;
		void UploadUniform1f(ShaderProg const&, int _nLoc, unsigned _nCount, float    const*) noexcept OVERRIDE_;
		void UploadUniform2f(ShaderProg const&, int _nLoc, unsigned _nCount, float    const*) noexcept OVERRIDE_;
		void UploadUniform3f(ShaderProg const&, int _nLoc, unsigned _nCount, float    const*) noexcept OVERRIDE_;
		void UploadUniform4f(ShaderProg const&, int _nLoc, unsigned _nCount, float    const*) noexcept OVERRIDE_;
		void UploadUniform1i(ShaderProg const&, int _nLoc, unsigned _nCount, int      const*) noexcept OVERRIDE_;
		void UploadUniform2i(ShaderProg const&, int _nLoc, unsigned _nCount, int      const*) noexcept OVERRIDE_;
		void UploadUniform3i(ShaderProg const&, int _nLoc, unsigned _nCount, int      const*) noexcept OVERRIDE_;
		void UploadUniform4i(ShaderProg const&, int _nLoc, unsigned _nCount, int      const*) noexcept OVERRIDE_;
		void UploadUniform1u(ShaderProg const&, int _nLoc, unsigned _nCount, unsigned const*) noexcept OVERRIDE_;
		void UploadUniform2u(ShaderProg const&, int _nLoc, unsigned _nCount, unsigned const*) noexcept OVERRIDE_;
		void UploadUniform3u(ShaderProg const&, int _nLoc, unsigned _nCount, unsigned const*) noexcept OVERRIDE_;
		void UploadUniform4u(ShaderProg const&, int _nLoc, unsigned _nCount, unsigned const*) noexcept OVERRIDE_;

		void UploadMatrix2(ShaderProg const&, int _nLoc, unsigned _nCount, float const*, bool _bTranspose = true) noexcept OVERRIDE_;
		void UploadMatrix4(ShaderProg const&, int _nLoc, unsigned _nCount, float const*, bool _bTranspose = true) noexcept OVERRIDE_;

		void UseShaderPipeline(ShaderPipeline const&) noexcept OVERRIDE_;
		void AttachShaderProgram(ShaderPipeline const&, ShaderProg const&, ShaderStage const&) noexcept OVERRIDE_;

		void QueryVariables(ShaderProg const&, AutoArray<std::pair<HashString, Gfx::eUniform_t>>&, AutoArray<std::pair<HashString, unsigned>>&) noexcept OVERRIDE_;

		void FreeShader(Shader&) noexcept OVERRIDE_;
		void FreeShaderProgram(ShaderProg&) noexcept OVERRIDE_;
		void FreeShaderPipeline(ShaderPipeline&) noexcept OVERRIDE_;

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

