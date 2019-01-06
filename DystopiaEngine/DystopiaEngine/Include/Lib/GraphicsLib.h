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

namespace Gfx
{
	enum class GfxMode
	{
		NONE,
		OPENGL,
	};

	enum class ShaderStage : unsigned;

	class GraphicsAPI
	{
	public:

		virtual void PrintEnvironment(void) const noexcept = 0;

		virtual unsigned CreateShaderProgram(void) noexcept = 0;
		virtual void DestroyShaderProgram(unsigned&) noexcept = 0;
		virtual unsigned CompileGLSL(Gfx::ShaderStage, void const* _pData) noexcept = 0;
		//virtual unsigned CompilSPRIV(void* _pData) noexcept = 0;
		
		template <typename ... T>
		inline bool LinkShader(unsigned _nProgram, T ... _nArgs) noexcept;


	protected:
		GraphicsAPI(void) {};

		virtual bool LinkShaderImpl(unsigned, unsigned const*, size_t) noexcept = 0;

	private:
		GraphicsAPI(GraphicsAPI&&)                   = delete;
		GraphicsAPI(GraphicsAPI const&)              = delete;
		GraphicsAPI& operator = (GraphicsAPI&&)      = delete;
		GraphicsAPI& operator = (GraphicsAPI const&) = delete;
	};

	using GfxAPI = GraphicsAPI;

	GfxAPI* GetInstance(void) noexcept;
	GfxMode GetActiveMode(void) noexcept;

	bool InitGraphicsAPI(void const* phwnd, GfxMode = GfxMode::OPENGL);
	void ShutdownGraphicsAPI(void) noexcept;
}





// =====

template <typename ... T>
inline bool Gfx::GraphicsAPI::LinkShader(unsigned _nProgram, T ... _nArgs) noexcept
{
	if constexpr (sizeof...(_nArgs) > 0)
	{
		static_assert((Ut::IsSame<Ut::RemoveRef_t<T>, unsigned>::value && ...), "All values must be a shader!");

		unsigned shaders[]{ _nArgs... };
		return LinkShaderImpl(_nProgram, shaders, sizeof...(_nArgs));
	}
	else
	{
		return false;
	}
}



#endif

