/* HEADER *********************************************************************************/
/*!
\file	GraphicsLib.cpp
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	TODO: Adaptor class to unify Graphics API calls

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "Lib/GraphicsLib.h"
#include "Lib/Gfx/OpenGLAPI.h"

#include "Allocator/DefaultAlloc.h"

#if defined(RUNTIME_GFXAPI) && RUNTIME_GFXAPI
#define API_PTR ::Gfx::GfxAPI*
#else
#define API_PTR ::Gfx::OpenGL_API*
#endif


namespace
{
	static API_PTR      pInstance  = nullptr;
	static Gfx::GfxMode ActiveMode = Gfx::GfxMode::NONE;
}

::Gfx::GfxAPI* Gfx::GetInstance(void) noexcept
{
	return ((pInstance));
}

Gfx::GfxMode Gfx::GetActiveMode(void) noexcept
{
	return ActiveMode;
}

bool Gfx::InitGraphicsAPI(void const* phwnd, GfxMode _mode)
{
#   if defined(DEBUG) | defined(_DEBUG)
		if (pInstance)
			__debugbreak();
#   endif

#if defined(RUNTIME_GFXAPI) && RUNTIME_GFXAPI
	ActiveMode = _mode;

	switch (_mode)
	{
	case GfxMode::OPENGL:
		pInstance = Dystopia::DefaultAllocator<OpenGL_API>::ConstructAlloc();
		return static_cast<OpenGL_API*>(pInstance)->InitGraphicsAPI(phwnd);
		break;

	default:
		__debugbreak();
		break;
	}

	return false;
#else

	UNUSED_PARAMETER(_mode);
	pInstance = Dystopia::DefaultAllocator<Ut::RemovePtr_t<decltype(pInstance)>>::ConstructAlloc();
	return static_cast<OpenGL_API*>(pInstance)->InitGraphicsAPI(phwnd);

#endif
}

void Gfx::ShutdownGraphicsAPI(void) noexcept
{
	switch (ActiveMode)
	{
	case GfxMode::OPENGL:
		Dystopia::DefaultAllocator<OpenGL_API>::DestructFree(static_cast<OpenGL_API*>(pInstance));
		break;
	}
}


void Gfx::GfxAPI::PrintEnvironment(void) const noexcept
{
	return pInstance->PrintEnvironment();
}

void Gfx::GfxAPI::ToggleVSync(bool _b) const noexcept
{
	return pInstance->ToggleVSync(_b);
}

int Gfx::GfxAPI::GetUniformLocation(ShaderProg const& _prog, char const* _p) noexcept
{
	return pInstance->GetUniformLocation(_prog, _p);
}

Gfx::ShaderProg Gfx::GfxAPI::CreateShaderProgram(void) noexcept
{
	return pInstance->CreateShaderProgram();
}

Gfx::ShaderPipeline Gfx::GfxAPI::CreateShaderPipeline(void) noexcept
{
	return pInstance->CreateShaderPipeline();
}

Gfx::Shader Gfx::GfxAPI::CompileGLSL(Gfx::ShaderStage const& _prog, void const* _pData) noexcept
{
	return pInstance->CompileGLSL(_prog, _pData);
}

void Gfx::GfxAPI::UseShaderPipeline(ShaderPipeline const& _prog) noexcept
{
	return pInstance->UseShaderPipeline(_prog);
}

void Gfx::GfxAPI::AttachShaderProgram(ShaderPipeline const& _pl, ShaderProg const& _prog, ShaderStage const& _stage) noexcept
{
	return pInstance->AttachShaderProgram(_pl, _prog, _stage);
}

void Gfx::GfxAPI::QueryVariables(ShaderProg const& _prog, 
	AutoArray<std::pair<OString, Gfx::eUniform_t>>& _vars, 
	AutoArray<std::pair<OString, unsigned>>& _tex)
{
	return pInstance->QueryVariables(_prog, _vars, _tex);
}

#define DEFINE_UPLOAD_UNIFORM(_FUNC_, _TYPE_) \
void Gfx::GfxAPI:: ## _FUNC_ (ShaderProg const& _prog, int _nLoc, unsigned _nCount, _TYPE_ const* _p) noexcept \
{																											   \
	return pInstance-> _FUNC_ (_prog, _nLoc, _nCount, _p);                                                     \
}

DEFINE_UPLOAD_UNIFORM(UploadUniform1f, float)
DEFINE_UPLOAD_UNIFORM(UploadUniform2f, float)
DEFINE_UPLOAD_UNIFORM(UploadUniform3f, float)
DEFINE_UPLOAD_UNIFORM(UploadUniform4f, float)
DEFINE_UPLOAD_UNIFORM(UploadUniform1i, int)
DEFINE_UPLOAD_UNIFORM(UploadUniform2i, int)
DEFINE_UPLOAD_UNIFORM(UploadUniform3i, int)
DEFINE_UPLOAD_UNIFORM(UploadUniform4i, int)
DEFINE_UPLOAD_UNIFORM(UploadUniform1u, unsigned)
DEFINE_UPLOAD_UNIFORM(UploadUniform2u, unsigned)
DEFINE_UPLOAD_UNIFORM(UploadUniform3u, unsigned)
DEFINE_UPLOAD_UNIFORM(UploadUniform4u, unsigned)

#undef DEFINE_UPLOAD_UNIFORM

void Gfx::GfxAPI::UploadMatrix2(ShaderProg const& _prog, int _nLoc, unsigned _nCount, float const* _p, bool _bTranspose) noexcept
{
	return pInstance->UploadMatrix2(_prog, _nLoc, _nCount, _p, _bTranspose);
}

void Gfx::GfxAPI::UploadMatrix4(ShaderProg const& _prog, int _nLoc, unsigned _nCount, float const* _p, bool _bTranspose) noexcept
{
	return pInstance->UploadMatrix2(_prog, _nLoc, _nCount, _p, _bTranspose);
}


bool Gfx::GfxAPI::LinkShaderImpl(ShaderProg const& _prog, Shader const* _str, size_t _sz) noexcept
{
	return pInstance->LinkShaderImpl(_prog, _str, _sz);
}

void Gfx::GfxAPI::FreeShader(Shader& _shader) noexcept
{
	return pInstance->FreeShader(_shader);
}

void Gfx::GfxAPI::FreeShaderProgram(ShaderProg& _prog) noexcept
{
	return pInstance->FreeShaderProgram(_prog);
}

void Gfx::GfxAPI::FreeShaderPipeline(ShaderPipeline& _pl) noexcept
{
	return pInstance->FreeShaderPipeline(_pl);
}


