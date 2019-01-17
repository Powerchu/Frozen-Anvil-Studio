/* HEADER *********************************************************************************/
/*!
\file	GraphicsLib.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Adaptor class to unify Graphics API calls

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _LIB_GFX_GRAPHICSLIB_H_
#define _LIB_GFX_GRAPHICSLIB_H_

#include "Lib/Gfx/Shaders.h"


namespace Gfx
{
	enum class GfxMode
	{
		NONE,
		OPENGL,

		DEFAULT = OPENGL,
	};

	class GraphicsAPI
	{
	public:

		// Generics
		
		virtual void PrintEnvironment(void) const noexcept = 0;

		virtual void ToggleVSync(bool) const noexcept = 0;

		template <typename T>
		inline void Free(T&) noexcept;

		// ====== SHADER

		virtual ShaderProg CreateShaderProgram(void) noexcept = 0;
		virtual ShaderPipeline CreateShaderPipeline(void) noexcept = 0;
		virtual Shader CompileGLSL(Gfx::ShaderStage const&, void const* _pData) noexcept = 0;
		//virtual unsigned CompilSPRIV(void* _pData) noexcept = 0;

		virtual void UseShaderPipeline(ShaderPipeline const&) noexcept = 0;
		virtual void AttachShaderProgram(ShaderPipeline const&, ShaderProg const&, ShaderStage const&) noexcept = 0;

		template <typename ... T>
		inline bool LinkShader(ShaderProg const& _nProgram, T&& ... _nArgs) noexcept;

		virtual void QueryVariables(ShaderProg const&) = 0;

		virtual int GetUniformLocation(ShaderProg const&, char const*) noexcept = 0;
		virtual void UploadUniform1f(ShaderProg const&, unsigned _nLoc, unsigned _nCount, float    const*) noexcept = 0;
		virtual void UploadUniform2f(ShaderProg const&, unsigned _nLoc, unsigned _nCount, float    const*) noexcept = 0;
		virtual void UploadUniform3f(ShaderProg const&, unsigned _nLoc, unsigned _nCount, float    const*) noexcept = 0;
		virtual void UploadUniform4f(ShaderProg const&, unsigned _nLoc, unsigned _nCount, float    const*) noexcept = 0;
		virtual void UploadUniform1i(ShaderProg const&, unsigned _nLoc, unsigned _nCount, int      const*) noexcept = 0;
		virtual void UploadUniform2i(ShaderProg const&, unsigned _nLoc, unsigned _nCount, int      const*) noexcept = 0;
		virtual void UploadUniform3i(ShaderProg const&, unsigned _nLoc, unsigned _nCount, int      const*) noexcept = 0;
		virtual void UploadUniform4i(ShaderProg const&, unsigned _nLoc, unsigned _nCount, int      const*) noexcept = 0;
		virtual void UploadUniform1u(ShaderProg const&, unsigned _nLoc, unsigned _nCount, unsigned const*) noexcept = 0;
		virtual void UploadUniform2u(ShaderProg const&, unsigned _nLoc, unsigned _nCount, unsigned const*) noexcept = 0;
		virtual void UploadUniform3u(ShaderProg const&, unsigned _nLoc, unsigned _nCount, unsigned const*) noexcept = 0;
		virtual void UploadUniform4u(ShaderProg const&, unsigned _nLoc, unsigned _nCount, unsigned const*) noexcept = 0;

		virtual void UploadMatrix2(ShaderProg const&, unsigned _nLoc, unsigned _nCount, float const*, bool _bTranspose = true) noexcept = 0;
		virtual void UploadMatrix4(ShaderProg const&, unsigned _nLoc, unsigned _nCount, float const*, bool _bTranspose = true) noexcept = 0;


	protected:
		GraphicsAPI(void) {};

		virtual bool LinkShaderImpl(ShaderProg const&, Shader const*, size_t) noexcept = 0;

		virtual void FreeShader(Shader&) noexcept = 0;
		virtual void FreeShaderProgram(ShaderProg&) noexcept = 0;
		virtual void FreeShaderPipeline(ShaderPipeline&) noexcept = 0;

	private:
		GraphicsAPI(GraphicsAPI&&)                   = delete;
		GraphicsAPI(GraphicsAPI const&)              = delete;
		GraphicsAPI& operator = (GraphicsAPI&&)      = delete;
		GraphicsAPI& operator = (GraphicsAPI const&) = delete;
	};

	using GfxAPI = GraphicsAPI;

	GfxAPI* const& GetInstance(void) noexcept;
	GfxMode GetActiveMode(void) noexcept;

	bool InitGraphicsAPI(void const* phwnd, GfxMode = GfxMode::DEFAULT);
	void ShutdownGraphicsAPI(void) noexcept;
}






// ============================================ FUNCTION DEFINITIONS ============================================ // 

template <typename ... T>
inline bool Gfx::GraphicsAPI::LinkShader(ShaderProg const& _nProgram, T&& ... _nArgs) noexcept
{
	if constexpr (sizeof...(_nArgs) > 0)
	{
		static_assert((Ut::IsSame<Ut::RemoveRef_t<T>, Shader>::value && ...), "All values must be a shader!");

		Shader shaders[]{ Ut::Fwd<T>(_nArgs)... };
		return LinkShaderImpl(_nProgram, shaders, sizeof...(_nArgs));
	}
	else
	{
		return false;
	}
}

template <typename T>
inline void Gfx::GraphicsAPI::Free(T&) noexcept
{
	static_assert(false, "Graphics Lib: Unable to free unknown type!");
}
template <>
inline void Gfx::GraphicsAPI::Free<Gfx::Shader>(Shader& _arg) noexcept
{
	FreeShader(_arg);
}
template <>
inline void Gfx::GraphicsAPI::Free<Gfx::ShaderProg>(ShaderProg& _arg) noexcept
{
	FreeShaderProgram(_arg);
}
template <>
inline void Gfx::GraphicsAPI::Free<Gfx::ShaderPipeline>(ShaderPipeline& _arg) noexcept
{
	FreeShaderPipeline(_arg);
}



#endif

