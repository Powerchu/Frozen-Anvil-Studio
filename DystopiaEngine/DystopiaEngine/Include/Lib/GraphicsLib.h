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

	class GraphicsAPI
	{
	public:

		virtual unsigned CreateShaderProgram(void) noexcept = 0;
		virtual unsigned CreateShader(Gfx::ShaderStage) noexcept = 0;
		virtual unsigned CompileGLSL(void const* _pData) noexcept = 0;
		//virtual unsigned CompilSPRIV(void* _pData) noexcept = 0;
		

	protected:
		GraphicsAPI(void) {};

	private:
		GraphicsAPI(GraphicsAPI&&)                   = delete;
		GraphicsAPI(GraphicsAPI const&)              = delete;
		GraphicsAPI& operator = (GraphicsAPI&&)      = delete;
		GraphicsAPI& operator = (GraphicsAPI const&) = delete;
	};

	using GfxAPI = GraphicsAPI;

	GfxAPI* GetInstance(void) noexcept;
	GfxMode GetActiveMode(void) noexcept;

	bool InitGraphicsAPI(void* phwnd, GfxMode = GfxMode::OPENGL);
	void ShutdownGraphicsAPI(void) noexcept;
}



#endif

