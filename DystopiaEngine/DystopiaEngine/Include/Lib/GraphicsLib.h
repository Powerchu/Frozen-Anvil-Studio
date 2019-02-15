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

#include "DataStructure/OString.h"
#include "DataStructure/AutoArray.h"

#if defined(RUNTIME_GFXAPI) && RUNTIME_GFXAPI
#define VIRTUAL_ virtual
#define ABSTRACT_ = 0
#else
#define VIRTUAL_
#define ABSTRACT_
#endif


namespace Gfx
{
	enum class GfxMode
	{
		NONE,
		OPENGL,

		DEFAULT = OPENGL,
	};

	class OpenGL_API;

	class GraphicsAPI
	{
	public:

		// Generics
		
		VIRTUAL_ bool BindContext(void* _hdc) const noexcept ABSTRACT_;

		VIRTUAL_ void ToggleVSync(bool) const noexcept ABSTRACT_;

		VIRTUAL_ void PrintEnvironment(void) const noexcept ABSTRACT_;

		template <typename T>
		inline void Free(T&) noexcept;

		// ====== SHADER

		VIRTUAL_ ShaderProg CreateShaderProgram(void) noexcept ABSTRACT_;
		VIRTUAL_ ShaderPipeline CreateShaderPipeline(void) noexcept ABSTRACT_;
		VIRTUAL_ Shader CompileGLSL(Gfx::ShaderStage const&, void const* _pData) noexcept ABSTRACT_;
		//virtual unsigned CompilSPRIV(void* _pData) noexcept ABSTRACT_;

		VIRTUAL_ void UseShaderPipeline(ShaderPipeline const&) noexcept ABSTRACT_;
		VIRTUAL_ void AttachShaderProgram(ShaderPipeline const&, ShaderProg const&, ShaderStage const&) noexcept ABSTRACT_;

		template <typename ... T>
		inline bool LinkShader(ShaderProg const& _nProgram, T&& ... _nArgs) noexcept;

		VIRTUAL_ void QueryVariables(ShaderProg const&, AutoArray<std::pair<OString, eUniform_t>>&, AutoArray<std::pair<OString, unsigned>>&) ABSTRACT_;

		VIRTUAL_ int GetUniformLocation(ShaderProg const&, char const*) noexcept ABSTRACT_;
		VIRTUAL_ void UploadUniform1f(ShaderProg const&, int _nLoc, unsigned _nCount, float    const*) noexcept ABSTRACT_;
		VIRTUAL_ void UploadUniform2f(ShaderProg const&, int _nLoc, unsigned _nCount, float    const*) noexcept ABSTRACT_;
		VIRTUAL_ void UploadUniform3f(ShaderProg const&, int _nLoc, unsigned _nCount, float    const*) noexcept ABSTRACT_;
		VIRTUAL_ void UploadUniform4f(ShaderProg const&, int _nLoc, unsigned _nCount, float    const*) noexcept ABSTRACT_;
		VIRTUAL_ void UploadUniform1i(ShaderProg const&, int _nLoc, unsigned _nCount, int      const*) noexcept ABSTRACT_;
		VIRTUAL_ void UploadUniform2i(ShaderProg const&, int _nLoc, unsigned _nCount, int      const*) noexcept ABSTRACT_;
		VIRTUAL_ void UploadUniform3i(ShaderProg const&, int _nLoc, unsigned _nCount, int      const*) noexcept ABSTRACT_;
		VIRTUAL_ void UploadUniform4i(ShaderProg const&, int _nLoc, unsigned _nCount, int      const*) noexcept ABSTRACT_;
		VIRTUAL_ void UploadUniform1u(ShaderProg const&, int _nLoc, unsigned _nCount, unsigned const*) noexcept ABSTRACT_;
		VIRTUAL_ void UploadUniform2u(ShaderProg const&, int _nLoc, unsigned _nCount, unsigned const*) noexcept ABSTRACT_;
		VIRTUAL_ void UploadUniform3u(ShaderProg const&, int _nLoc, unsigned _nCount, unsigned const*) noexcept ABSTRACT_;
		VIRTUAL_ void UploadUniform4u(ShaderProg const&, int _nLoc, unsigned _nCount, unsigned const*) noexcept ABSTRACT_;

		VIRTUAL_ void UploadMatrix2(ShaderProg const&, int _nLoc, unsigned _nCount, float const*, bool _bTranspose = true) noexcept ABSTRACT_;
		VIRTUAL_ void UploadMatrix4(ShaderProg const&, int _nLoc, unsigned _nCount, float const*, bool _bTranspose = true) noexcept ABSTRACT_;


	protected:
		GraphicsAPI(void) {};

		VIRTUAL_ bool LinkShaderImpl(ShaderProg const&, Shader const*, size_t) noexcept ABSTRACT_;

		VIRTUAL_ void FreeShader(Shader&) noexcept ABSTRACT_;
		VIRTUAL_ void FreeShaderProgram(ShaderProg&) noexcept ABSTRACT_;
		VIRTUAL_ void FreeShaderPipeline(ShaderPipeline&) noexcept ABSTRACT_;

	private:
		GraphicsAPI(GraphicsAPI&&)                   = delete;
		GraphicsAPI(GraphicsAPI const&)              = delete;
		GraphicsAPI& operator = (GraphicsAPI&&)      = delete;
		GraphicsAPI& operator = (GraphicsAPI const&) = delete;
	};

	using GfxAPI = GraphicsAPI;

	GfxAPI* GetInstance(void) noexcept;
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


#undef VIRTUAL_
#undef ABSTRACT_
#undef API_PTR
#endif

