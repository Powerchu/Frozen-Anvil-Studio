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


namespace
{
	static Gfx::GfxMode ActiveMode = Gfx::GfxMode::NONE;
	static Gfx::GfxAPI* pInstance  = nullptr;
}

Gfx::GfxAPI* const& Gfx::GetInstance(void) noexcept
{
	return pInstance;
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

