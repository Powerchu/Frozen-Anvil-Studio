/* HEADER *********************************************************************************/
/*!
\file	GraphicsLib.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	TODO: Adaptor class to unify Graphics API calls

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

		template <typename T>
		inline void Free(T&) noexcept;

		// ====== SHADER

		virtual ShaderProg CreateShaderProgram(void) noexcept = 0;
		virtual Shader CompileGLSL(Gfx::ShaderStage, void const* _pData) noexcept = 0;
		//virtual unsigned CompilSPRIV(void* _pData) noexcept = 0;

		template <typename ... T>
		inline bool LinkShader(ShaderProg const& _nProgram, T&& ... _nArgs) noexcept;

		virtual unsigned GetUniformLocation(ShaderProg, char const*) noexcept = 0;


	protected:
		GraphicsAPI(void) {};

		virtual bool LinkShaderImpl(ShaderProg const&, Shader const*, size_t) noexcept = 0;

		virtual void FreeShader(Shader&) noexcept = 0;
		virtual void FreeShaderProgram(ShaderProg&) noexcept = 0;

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





// =====

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



#endif

