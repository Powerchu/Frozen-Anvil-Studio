/* HEADER *********************************************************************************/
/*!
\file	OpenGLAPI.cpp
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Implementation of interface to OpenGL

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "Lib/Gfx/OpenGLAPI.h"

#include <gl/glew.h>
#include <gl/wglew.h>
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#undef far
#undef near
#undef ERROR
#undef NOMINMAX
#undef WIN32_LEAN_AND_MEAN


Gfx::OpenGL_API::OpenGL_API(void) noexcept
	: mOpenGL{ nullptr }
{

}

unsigned Gfx::OpenGL_API::CreateShaderProgram(void) noexcept
{
	auto ret = glCreateProgram();
	glProgramParameteri(ret, GL_PROGRAM_SEPARABLE, GL_TRUE);
	return ret;
}

unsigned Gfx::OpenGL_API::CreateShader(Gfx::ShaderStage _stage) noexcept
{
	unsigned stage;

	switch (_stage)
	{
	case Gfx::ShaderStage::VERTEX:
		stage = GL_VERTEX_SHADER; break;
	case Gfx::ShaderStage::FRAGMENT:
		stage = GL_FRAGMENT_SHADER; break;
	case Gfx::ShaderStage::GEOMETRY:
		stage = GL_GEOMETRY_SHADER; break;

	default:
		return 0;
	}

	return glCreateShader(stage);
}

unsigned Gfx::OpenGL_API::CompileGLSL(void const * _pData) noexcept
{
	return 0;
}

bool Gfx::OpenGL_API::InitGraphicsAPI(void* _phwnd) noexcept
{
	HWND hwnd = *static_cast<HWND*>(_phwnd);
	HDC dc = GetDC(hwnd);

	// Use to specify the color format we want and openGL support
	PIXELFORMATDESCRIPTOR pfd{};

	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);	// Windows requirement
	pfd.nVersion = 1;								// Windows requirement
	pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 24;
	pfd.cStencilBits = 8;
	pfd.iLayerType = PFD_MAIN_PLANE;

	// Ask windows to give us a pixel format based on what we asked for
	int nPxFormat = ChoosePixelFormat(_window.GetDeviceContext(), &pfd);

	if (0 == nPxFormat) // Check if we got something back
	{
		__debugbreak();
		return false;
	}

	// Attempt to set the format based on the returned pixel formal
	BOOL bResult = SetPixelFormat(_window.GetDeviceContext(), nPxFormat, &pfd);

	if (!bResult) // This shouldn't happen
	{
		__debugbreak();
		return false;
	}

	// Create a fake context so we can create context
	HGLRC dummyGL = wglCreateContext(dc);
	wglMakeCurrent(_window.GetDeviceContext(), dummyGL);

	// attempt to init glew so that there is an active GL context
	if (glewInit() != GLEW_OK)
	{
		__debugbreak();
		return false;
	}

	// Check if gl 3.1 and above context is supported
	if (wglewIsSupported("WGL_ARB_create_context") == 1 && !VersionSelect(dc))
	{
		__debugbreak();
		return false;
	}

	// Unbind the dummy context
	wglMakeCurrent(nullptr, nullptr);

	// Delete the dummy context
	wglDeleteContext(dummyGL);

	// Make our newly created context the active context
	wglMakeCurrent(dc, static_cast<HGLRC>(mOpenGL));

	return false;
}

bool Gfx::OpenGL_API::VersionSelect(void* const& hdc) noexcept
{
	int attrbs[] {
		WGL_CONTEXT_MAJOR_VERSION_ARB, 4,	// OpenGL minimum Major ver
		WGL_CONTEXT_MINOR_VERSION_ARB, 3,	// OpenGL minimum Minor ver
		WGL_CONTEXT_FLAGS_ARB, 0,
		0
	};

	mOpenGL = nullptr;
	mAvailable = eGfxSettings::GRAPHICS_ALL;

	// Try to create at least OpenGL 4.3
	attrbs[3] = 3;
	mOpenGL = wglCreateContextAttribsARB(static_cast<HDC>(hdc), NULL, attrbs);
	if (mOpenGL)
	{
		return true;
	}

	// Failed, try 3.2...
	mAvailable &= ~(eGfxSettings::GRAPHICS_COMPUTE | eGfxSettings::GRAPHICS_TESS);

	attrbs[1] = 3; attrbs[3] = 2;
	mOpenGL = wglCreateContextAttribsARB(static_cast<HDC>(hdc), NULL, attrbs);
	if (mOpenGL)
	{
		return true;
	}

	// Failed, try 3.1...
	mAvailable &= ~(eGfxSettings::GRAPHICS_MSAA);

	attrbs[3] = 1;
	mOpenGL = wglCreateContextAttribsARB(static_cast<HDC>(hdc), NULL, attrbs);

	return !!mOpenGL;
}


