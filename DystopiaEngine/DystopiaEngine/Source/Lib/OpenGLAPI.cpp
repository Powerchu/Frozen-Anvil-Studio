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
#include "DataStructure/AutoArray.h"
#include "DataStructure/HashString.h"

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
		DEBUG_PRINT(eLog::ERROR, _ERR_MSG_ " %s", buffer);                                  \
	} while (false)

	template <typename T, typename U>
	inline T StrongToGLType(U const& _v) noexcept
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
	: mOpenGL{ nullptr }, mAvailable{ Opts::GRAPHICS_ALL }
{

}

void Gfx::OpenGL_API::PrintEnvironment(void) const noexcept
{
	// Gets for the openGL version
	GLint a, b;
	glGetIntegerv(GL_MAJOR_VERSION, &a);
	glGetIntegerv(GL_MINOR_VERSION, &b);

	DEBUG_PRINT(eLog::SYSINFO, "Using %s, %s\n", glGetString(GL_VENDOR), glGetString(GL_RENDERER));
	DEBUG_PRINT(eLog::SYSINFO, "OpenGL Version %s\n", glGetString(GL_VERSION));
}

void Gfx::OpenGL_API::ToggleVSync(bool _b) const noexcept
{
	if (Opts::GRAPHICS_VSYNC == (mAvailable & Opts::GRAPHICS_VSYNC))
	{
		wglSwapIntervalEXT(_b ? -1 : 1);
	}
}

unsigned Gfx::OpenGL_API::StageToBitmask(Gfx::ShaderStage _stage) noexcept
{
	unsigned stage{ 0 };

	if (Gfx::ShaderStage::ALL == (Gfx::ShaderStage::ALL & _stage))
		return GL_ALL_SHADER_BITS;

	stage |= GL_VERTEX_SHADER_BIT   * (Gfx::ShaderStage::VERTEX   == (Gfx::ShaderStage::VERTEX   & _stage));
	stage |= GL_FRAGMENT_SHADER_BIT * (Gfx::ShaderStage::FRAGMENT == (Gfx::ShaderStage::FRAGMENT & _stage));
	stage |= GL_GEOMETRY_SHADER_BIT * (Gfx::ShaderStage::GEOMETRY == (Gfx::ShaderStage::GEOMETRY & _stage));
	stage |= GL_COMPUTE_SHADER_BIT  * (Gfx::ShaderStage::COMPUTE  == (Gfx::ShaderStage::COMPUTE  & _stage));

	return stage;
}

Gfx::ShaderProg Gfx::OpenGL_API::CreateShaderProgram(void) noexcept
{
	auto ret = glCreateProgram();
	glProgramParameteri(ret, GL_PROGRAM_SEPARABLE, GL_TRUE);
	return GLToStrongType<ShaderProg>(ret);
}

Gfx::ShaderPipeline Gfx::OpenGL_API::CreateShaderPipeline(void) noexcept
{
	GLuint ret;
	glCreateProgramPipelines(1, &ret);
	return GLToStrongType<ShaderPipeline>(ret);
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

Gfx::Shader Gfx::OpenGL_API::CompileGLSL(Gfx::ShaderStage const& _stage, void const * _pData) noexcept
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

void Gfx::OpenGL_API::FreeShaderPipeline(ShaderPipeline& _n) noexcept
{
	GLuint data = StrongToGLType<GLuint>(_n);
	glDeleteProgramPipelines(1, &data);
	_n = 0;
}


bool Gfx::OpenGL_API::LinkShaderImpl(ShaderProg const& _nProgram, Shader const* _pShaders, size_t _sz) noexcept
{
	for (size_t n = 0; n < _sz; ++n)
		glAttachShader(StrongToGLType<GLuint>(_nProgram), StrongToGLType<GLuint>(_pShaders[n]));
	glLinkProgram(StrongToGLType<GLuint>(_nProgram));

	int nStatus;
	glGetProgramiv(StrongToGLType<GLuint>(_nProgram), GL_LINK_STATUS, &nStatus);

	if (GL_FALSE == nStatus)
	{
		PRINT_GFX_SHADER_ERROR(StrongToGLType<GLuint>(_nProgram), glGetProgramInfoLog, "Shader Link Error");
	}

	return GL_TRUE == nStatus;
}

void Gfx::OpenGL_API::UseShaderPipeline(ShaderPipeline const& _pipeline) noexcept
{
	glBindProgramPipeline(StrongToGLType<GLuint>(_pipeline));
}

void Gfx::OpenGL_API::AttachShaderProgram(ShaderPipeline const& _pipeline, ShaderProg const& _prog, ShaderStage const& _stages) noexcept
{
	glUseProgramStages(StrongToGLType<GLuint>(_pipeline), StageToBitmask(_stages), StrongToGLType<GLuint>(_prog));
}


namespace
{
	Gfx::eUniform_t ResolveUniformType(GLuint ty)
	{
		switch (ty)
		{
		case GL_INT:		  return Gfx::eUniform_t::INT;
		case GL_BOOL:		  return Gfx::eUniform_t::BOOL;
		case GL_FLOAT:		  return Gfx::eUniform_t::FLOAT;
		case GL_FLOAT_VEC2:	  return Gfx::eUniform_t::VEC2;
		case GL_FLOAT_VEC3:	  return Gfx::eUniform_t::VEC3;
		case GL_FLOAT_VEC4:	  return Gfx::eUniform_t::VEC4;
		case GL_FLOAT_MAT2:	  return Gfx::eUniform_t::MAT2;
		case GL_FLOAT_MAT3:	  return Gfx::eUniform_t::MAT3;
		case GL_FLOAT_MAT4:	  return Gfx::eUniform_t::MAT4;
		case GL_SAMPLER_2D:	  return Gfx::eUniform_t::TEXTURE_2D;
		case GL_UNSIGNED_INT: return Gfx::eUniform_t::UINT;
		}

		return Gfx::eUniform_t::ERROR;
	}
}

AutoArray<std::pair<HashString, Gfx::eUniform_t>> Gfx::OpenGL_API::QueryVariables(ShaderProg const& _nProgram)
{
	auto nProg = StrongToGLType<GLuint>(_nProgram);
	GLint count = 0;

#   if defined(_DEBUG) | defined(DEBUG)
	if (auto err = glGetError())
		__debugbreak();
#   endif 

	glGetProgramiv(nProg, GL_ACTIVE_UNIFORMS, &count);

	GLint typeBuffer[100];
	GLuint indices[100];
	char nameBuffer[256];

#   if defined(_DEBUG) | defined(DEBUG)
	if (auto err = glGetError())
		__debugbreak();
#   endif 

	count = Ut::Min(count, Ut::Constant<GLint, sizeof(indices)/sizeof(GLuint)>::value);
	for (GLint n = 0; n < count; ++n)
		indices[n] = n;

	glGetActiveUniformsiv(nProg, count, indices, GL_UNIFORM_TYPE, typeBuffer);

#   if defined(_DEBUG) | defined(DEBUG)
	if (auto err = glGetError())
		__debugbreak();
#   endif 

	AutoArray<std::pair<HashString, eUniform_t>> ty( count );

	for (GLint n = 0; n < count; ++n)
	{
		switch (typeBuffer[n])
		{
		case GL_INT:
		case GL_BOOL:
		case GL_FLOAT:
		case GL_FLOAT_VEC2:
		case GL_FLOAT_VEC3:
		case GL_FLOAT_VEC4:
		//case GL_FLOAT_MAT2:
		//case GL_FLOAT_MAT3:
		//case GL_FLOAT_MAT4:
		//case GL_SAMPLER_2D:
		case GL_UNSIGNED_INT:
			glGetActiveUniformName(nProg, n, 256, nullptr, nameBuffer);
			ty.EmplaceBack(static_cast<char*>(nameBuffer), ResolveUniformType(typeBuffer[n]));
			break;
		default:
			break;
		}
	}

#   if defined(_DEBUG) | defined(DEBUG)
	if (auto err = glGetError())
		__debugbreak();
#   endif 

	return ty;
}


int Gfx::OpenGL_API::GetUniformLocation(ShaderProg const& _prog, char const* _strName) noexcept
{
	return glGetUniformLocation(StrongToGLType<GLuint>(_prog), _strName);
}

void Gfx::OpenGL_API::UploadUniform1f(ShaderProg const& _prog, unsigned _nLoc, unsigned _nCount, float const* _pVal) noexcept
{
	glProgramUniform1fv(StrongToGLType<GLuint>(_prog), _nLoc, _nCount, _pVal);
}

void Gfx::OpenGL_API::UploadUniform2f(ShaderProg const& _prog, unsigned _nLoc, unsigned _nCount, float const* _pVal) noexcept
{
	glProgramUniform2fv(StrongToGLType<GLuint>(_prog), _nLoc, _nCount, _pVal);
}

void Gfx::OpenGL_API::UploadUniform3f(ShaderProg const& _prog, unsigned _nLoc, unsigned _nCount, float const* _pVal) noexcept
{
	glProgramUniform3fv(StrongToGLType<GLuint>(_prog), _nLoc, _nCount, _pVal);
}

void Gfx::OpenGL_API::UploadUniform4f(ShaderProg const& _prog, unsigned _nLoc, unsigned _nCount, float const* _pVal) noexcept
{
	glProgramUniform4fv(StrongToGLType<GLuint>(_prog), _nLoc, _nCount, _pVal);
}

void Gfx::OpenGL_API::UploadUniform1i(ShaderProg const& _prog, unsigned _nLoc, unsigned _nCount, int const* _pVal) noexcept
{
	glProgramUniform1iv(StrongToGLType<GLuint>(_prog), _nLoc, _nCount, _pVal);
}

void Gfx::OpenGL_API::UploadUniform2i(ShaderProg const& _prog, unsigned _nLoc, unsigned _nCount, int const* _pVal) noexcept
{
	glProgramUniform2iv(StrongToGLType<GLuint>(_prog), _nLoc, _nCount, _pVal);
}

void Gfx::OpenGL_API::UploadUniform3i(ShaderProg const& _prog, unsigned _nLoc, unsigned _nCount, int const* _pVal) noexcept
{
	glProgramUniform3iv(StrongToGLType<GLuint>(_prog), _nLoc, _nCount, _pVal);
}

void Gfx::OpenGL_API::UploadUniform4i(ShaderProg const& _prog, unsigned _nLoc, unsigned _nCount, int const* _pVal) noexcept
{
	glProgramUniform4iv(StrongToGLType<GLuint>(_prog), _nLoc, _nCount, _pVal);
}

void Gfx::OpenGL_API::UploadUniform1u(ShaderProg const& _prog, unsigned _nLoc, unsigned _nCount, unsigned const* _pVal) noexcept
{
	glProgramUniform1uiv(StrongToGLType<GLuint>(_prog), _nLoc, _nCount, _pVal);
}

void Gfx::OpenGL_API::UploadUniform2u(ShaderProg const& _prog, unsigned _nLoc, unsigned _nCount, unsigned const* _pVal) noexcept
{
	glProgramUniform2uiv(StrongToGLType<GLuint>(_prog), _nLoc, _nCount, _pVal);
}

void Gfx::OpenGL_API::UploadUniform3u(ShaderProg const& _prog, unsigned _nLoc, unsigned _nCount, unsigned const* _pVal) noexcept
{
	glProgramUniform3uiv(StrongToGLType<GLuint>(_prog), _nLoc, _nCount, _pVal);
}

void Gfx::OpenGL_API::UploadUniform4u(ShaderProg const& _prog, unsigned _nLoc, unsigned _nCount, unsigned const* _pVal) noexcept
{
	glProgramUniform4uiv(StrongToGLType<GLuint>(_prog), _nLoc, _nCount, _pVal);
}

void Gfx::OpenGL_API::UploadMatrix2(ShaderProg const& _prog, unsigned _nLoc, unsigned _nCount, float const* _pVal, bool _bTrans) noexcept
{
	glProgramUniformMatrix2fv(StrongToGLType<GLuint>(_prog), _nLoc, _nCount, _bTrans ? GL_TRUE : GL_FALSE, _pVal);
}

void Gfx::OpenGL_API::UploadMatrix4(ShaderProg const& _prog, unsigned _nLoc, unsigned _nCount, float const* _pVal, bool _bTrans) noexcept
{
	glProgramUniformMatrix4fv(StrongToGLType<GLuint>(_prog), _nLoc, _nCount, _bTrans ? GL_TRUE : GL_FALSE, _pVal);
}


bool Gfx::OpenGL_API::InitGraphicsAPI(void const* _phwnd) noexcept
{
	HWND const& hwnd = *static_cast<HWND const*>(_phwnd);
	HDC dc = GetDC(hwnd);

	// Use to specify the color format we want and openGL support
	PIXELFORMATDESCRIPTOR pfd{};

	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);	// Windows requirement
	pfd.nVersion = 1;							// Windows requirement
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
	if (!wglewIsSupported("WGL_ARB_create_context"))
	{
		__debugbreak();
		return false;
	}

	if (!VersionSelect(dc))
	{
		__debugbreak();
		return false;
	}

	// Unbind the dummy context
	wglMakeCurrent(nullptr, nullptr);

	// Delete the dummy context
	wglDeleteContext(dummyGL);

	if (!wglewIsSupported("WGL_EXT_swap_control"))
	{
		mAvailable &= ~Opts::GRAPHICS_VSYNC;
	}

	// Make our newly created context the active context
	auto ret = BindContext(dc);

	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LINE_SMOOTH);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	return ret;
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

	// Try to create at least OpenGL 4.3
	mOpenGL = wglCreateContextAttribsARB(static_cast<HDC>(hdc), NULL, attrbs);
	if (mOpenGL)
	{
		return true;
	}

	// Try 4.1...
	attrbs[3] = 1;
	mOpenGL = wglCreateContextAttribsARB(static_cast<HDC>(hdc), NULL, attrbs);
	if (mOpenGL)
	{
		return true;
	}

	// Failed, try 3.2...
	mAvailable &= ~(Opts::GRAPHICS_COMPUTE | Opts::GRAPHICS_TESS);

	attrbs[1] = 3; attrbs[3] = 2;
	mOpenGL = wglCreateContextAttribsARB(static_cast<HDC>(hdc), NULL, attrbs);
	if (mOpenGL)
	{
		return true;
	}

	// Failed, try 3.1...
	mAvailable &= ~(Opts::GRAPHICS_MSAA);

	attrbs[3] = 1;
	mOpenGL = wglCreateContextAttribsARB(static_cast<HDC>(hdc), NULL, attrbs);

	return !!mOpenGL;
}



#undef PRINT_GFX_SHADER_ERROR


