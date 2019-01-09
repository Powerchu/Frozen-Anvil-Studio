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

#include "Utility/DebugAssert.h"
#include "Allocator/StackAlloc.h"

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


namespace
{
#define PRINT_GFX_SHADER_ERROR(_SHADER_, _PROC_, _ERR_MSG_)                                 \
	do {																			        \
		using StackA_t = Dystopia::StackAlloc_t;											\
		auto buffer = Dystopia::StackAlloc_t::GetBufferAs<char>();					        \
		_PROC_(_SHADER_, static_cast<GLsizei>(StackA_t::GetUsableSize()), nullptr, buffer); \
														                                    \
		DEBUG_PRINT(eLog::ERROR, _ERR_MSG_ ": %s", buffer);                                 \
	} while (false)

	template <typename T, typename U>
	inline T StrongToGLType(U _v) noexcept
	{
		if constexpr (Ut::IsSigned<T>::value)
		{
			return static_cast<T>(reinterpret_cast<intptr_t>(_v));
		}
		else
		{
			return static_cast<T>(reinterpret_cast<uintptr_t>(_v));
		}
	}

	template <typename T, typename U>
	inline T GLToStrongType(U _v) noexcept
	{
		if constexpr (Ut::IsSigned<T>::value)
		{
			return reinterpret_cast<T>(static_cast<intptr_t>(_v));
		}
		else
		{
			return reinterpret_cast<T>(static_cast<uintptr_t>(_v));
		}
	}
}

Gfx::OpenGL_API::OpenGL_API(void) noexcept
	: mOpenGL{ nullptr }
{

}

void Gfx::OpenGL_API::PrintEnvironment(void) const noexcept
{
	// Gets for the openGL version
	GLint a, b, c;
	glGetIntegerv(GL_MAJOR_VERSION, &a);
	glGetIntegerv(GL_MINOR_VERSION, &b);

	DEBUG_PRINT(eLog::SYSINFO, "Using %s, %s\n", glGetString(GL_VENDOR), glGetString(GL_RENDERER));
	DEBUG_PRINT(eLog::SYSINFO, "OpenGL Version %d.%d\n", a, b);

	glGetIntegerv(GL_ALPHA_BITS  , &a);
	glGetIntegerv(GL_RED_BITS    , &b); a += b;
	glGetIntegerv(GL_BLUE_BITS   , &b); a += b;
	glGetIntegerv(GL_GREEN_BITS  , &b); a += b;
	glGetIntegerv(GL_DEPTH_BITS  , &b);
	glGetIntegerv(GL_STENCIL_BITS, &c);

	DEBUG_PRINT(eLog::SYSINFO, "%d bit colour, %d bit depth, %d bit stencil\n", a, b, c);
}

Gfx::ShaderProg Gfx::OpenGL_API::CreateShaderProgram(void) noexcept
{
	auto ret = glCreateProgram();
	glProgramParameteri(ret, GL_PROGRAM_SEPARABLE, GL_TRUE);
	return { GLToStrongType<ShaderProg>(ret) };
}

void Gfx::OpenGL_API::FreeShader(Shader& _n) noexcept
{
	glDeleteShader(StrongToGLType<GLuint>(_n));
	_n = 0;
}

void Gfx::OpenGL_API::FreeShaderProgram(ShaderProg& _n) noexcept
{
	glDeleteProgram(StrongToGLType<GLuint>(_n));
	_n = 0;
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

Gfx::Shader Gfx::OpenGL_API::CompileGLSL(Gfx::ShaderStage _stage, void const * _pData) noexcept
{
	if (auto shader = CreateShader(_stage))
	{
		glShaderSource(shader, 1, reinterpret_cast<GLchar const* const*>(&_pData), nullptr);
		glCompileShader(shader);

		int nStatus;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &nStatus);

		if (GL_FALSE == nStatus)
		{
			PRINT_GFX_SHADER_ERROR(shader, glGetShaderInfoLog, "Shader Compile Error");
			glDeleteShader(shader);
			shader = 0;
		}

		return { GLToStrongType<Shader>(shader) };
	}

	return { 0 };
}

bool Gfx::OpenGL_API::LinkShaderImpl(ShaderProg const& _nProgram, Shader const* _pShaders, size_t _sz) noexcept
{
	for (size_t n = 0; n < _sz; ++n)
		glAttachShader(StrongToGLType<GLuint>(_nProgram), StrongToGLType<GLuint>(_pShaders[n]));
	glLinkProgram(StrongToGLType<GLuint>(_nProgram));

	int nStatus;
	glGetShaderiv(StrongToGLType<GLuint>(_nProgram), GL_LINK_STATUS, &nStatus);

	if (GL_FALSE == nStatus)
	{
		PRINT_GFX_SHADER_ERROR(StrongToGLType<GLuint>(_nProgram), glGetProgramInfoLog, "Shader Link Error");
	}

	return GL_TRUE == nStatus;
}

bool Gfx::OpenGL_API::InitGraphicsAPI(void const* _phwnd) noexcept
{
	HWND const& hwnd = *static_cast<HWND const*>(_phwnd);
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
	int nPxFormat = ChoosePixelFormat(dc, &pfd);

	if (0 == nPxFormat) // Check if we got something back
	{
		__debugbreak();
		return false;
	}

	// Attempt to set the format based on the returned pixel formal
	BOOL bResult = SetPixelFormat(dc, nPxFormat, &pfd);

	if (!bResult) // This shouldn't happen
	{
		__debugbreak();
		return false;
	}

	// Create a fake context so we can create context
	HGLRC dummyGL = wglCreateContext(dc);
	wglMakeCurrent(dc, dummyGL);

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
	return BindContext(dc);
}

bool Gfx::OpenGL_API::BindContext(void* _deviceContext) noexcept
{
	return wglMakeCurrent(static_cast<HDC>(_deviceContext), static_cast<HGLRC>(mOpenGL));
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
//	mAvailable = eGfxSettings::GRAPHICS_ALL;

	// Try to create at least OpenGL 4.3
	attrbs[3] = 3;
	mOpenGL = wglCreateContextAttribsARB(static_cast<HDC>(hdc), NULL, attrbs);
	if (mOpenGL)
	{
		return true;
	}

	// Failed, try 3.2...
//	mAvailable &= ~(eGfxSettings::GRAPHICS_COMPUTE | eGfxSettings::GRAPHICS_TESS);

	attrbs[1] = 3; attrbs[3] = 2;
	mOpenGL = wglCreateContextAttribsARB(static_cast<HDC>(hdc), NULL, attrbs);
	if (mOpenGL)
	{
		return true;
	}

	// Failed, try 3.1...
//	mAvailable &= ~(eGfxSettings::GRAPHICS_MSAA);

	attrbs[3] = 1;
	mOpenGL = wglCreateContextAttribsARB(static_cast<HDC>(hdc), NULL, attrbs);

	return !!mOpenGL;
}



#undef PRINT_GFX_SHADER_ERROR


