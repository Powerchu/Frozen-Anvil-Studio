/* HEADER *********************************************************************************/
/*!
\file	GraphicsSystem.cpp
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	BRIEF HERE

	! ---- INCLUDES TEMPORARY CODE ---- !
	ctrl+F TEMPORARY to find them and see how to replace them

	TODO: Draw batching.

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "System\Graphics\GraphicsSystem.h"	// File header
#include "System\Graphics\GraphicsDefs.h"	// eGraphicSettings
#include "Utility\DebugAssert.h"			// DEBUG_ASSERT
#include "Component\Camera.h"				// Camera

//#define GLEW_STATIC 			// Use glew as a static library
#define WIN32_LEAN_AND_MEAN		// Exclude rarely used stuff from Windows headers
#define NOMINMAX				// Disable window's min & max macros

#include <windows.h>			// WinAPI
#include <GL\glew.h>
#include <GL\wglew.h>			// glew Windows ext
#include <GL\GL.h>
#include <cstdio>
#include <string>

#undef GLEW_STATIC				// Stop defines from spilling into code
#undef WIN32_LEAN_AND_MEAN
#undef NOMINMAX

Dystopia::GraphicsSystem::GraphicsSystem(void) noexcept :
	mOpenGL{ nullptr }, mPixelFormat{ 0 }, mAvailable{ 0 }
{

}

Dystopia::GraphicsSystem::~GraphicsSystem(void)
{

}


bool Dystopia::GraphicsSystem::Init(void)
{
	return true;
}

void Dystopia::GraphicsSystem::Update(float)
{
	// We only care about the game view
	
	// For every camera in the game window (can be more than 1!)
	for (; false;)
	{
		// If the camera is inactive, skip

		// Get Camera's layer, we only want to draw inclusive stuff

		// Draw the game objects to screen based on the camera
		for (; false;)
		{
			// if Game Object is inactive or
			// Game Object's layer does not match the camera
			// Skip it


			// Draw batching?
		}
	}
}

void Dystopia::GraphicsSystem::StartFrame(Window& )
{

}

void Dystopia::GraphicsSystem::EndFrame(Window& )
{

}

void Dystopia::GraphicsSystem::Shutdown(void)
{

}

void Dystopia::GraphicsSystem::LoadDefaults(void)
{

}

void Dystopia::GraphicsSystem::LoadSettings(TextSerialiser&)
{

}


void Dystopia::GraphicsSystem::LevelLoad(TextSerialiser&)
{

}

Dystopia::Mesh* Dystopia::GraphicsSystem::LoadMesh(const std::string&)
{

}

Dystopia::Texture* Dystopia::GraphicsSystem::LoadTexture(const std::string&)
{

}

Dystopia::Shader* Dystopia::GraphicsSystem::LoadShader(const std::string& _filePath)
{
	std::string strName, strVert, strGeo, strFrag;

	_filePath; strName; strVert; strGeo; strFrag;
}

bool Dystopia::GraphicsSystem::BindOpenGL(void* _HWND) noexcept
{
	// Get the device context for our window 
	HDC mDeviceContext = GetDC(static_cast<HWND>(_HWND));

	wglMakeCurrent(static_cast<HDC>(mDeviceContext), static_cast<HGLRC>(mOpenGL));
}

bool Dystopia::GraphicsSystem::InitOpenGL(void* _HWND)
{
	// Get the device context for our window 
	HDC mDeviceContext = GetDC(static_cast<HWND>(_HWND));

	// Use to specify the color format we want and openGL support
	PIXELFORMATDESCRIPTOR pfd{};

	pfd.nSize		 = sizeof(PIXELFORMATDESCRIPTOR);	// Specified by Windows
	pfd.nVersion	 = 1;								// Specified by Windows
	pfd.dwFlags		 = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
	pfd.iPixelType	 = PFD_TYPE_RGBA;
	pfd.cColorBits	 = 32;
	pfd.cDepthBits	 = 24;
	pfd.cStencilBits = 8;
	pfd.iLayerType	 = PFD_MAIN_PLANE;

	// Ask windows to give us a pixel format based on what we asked for
	int nPxFormat = ChoosePixelFormat(mDeviceContext, &pfd);

	if (0 == nPxFormat) // Check if we got something back
	{
		DEBUG_ASSERT("Graphics System Error: ChoosePixelFormat fail! \n");

		return false;
	}

	// Attempt to set the format based on the returned pixel formal
	BOOL bResult = SetPixelFormat(mDeviceContext, nPxFormat, &pfd);

	if (!bResult) // This shouldn't happen
	{
		DEBUG_ASSERT("Graphics System Error: SetPixelFormat fail! \n");

		return false;
	}

	// Create a fake context so we can create context
	HGLRC dummyGL = wglCreateContext(mDeviceContext);
	wglMakeCurrent(mDeviceContext, dummyGL);

	// attempt to init glew no that there is an active GL context
	unsigned err = glewInit();

	if (err != GLEW_OK)
	{
		DEBUG_ASSERT("Graphics System Error: GLEW init fail! \n");

		return false;
	}

	// Check if gl 3.1 and above context is supported
	if (wglewIsSupported("WGL_ARB_create_context") == 1 && !SelectOpenGLVersion(mDeviceContext))
	{
		DEBUG_ASSERT("Graphics System Error: OpenGL 3.1 and above not supported! \n");

		return false;
	}

	// Unbind the dummy context
	wglMakeCurrent(nullptr, nullptr);

	// Delete the dummy context
	wglDeleteContext(static_cast<HGLRC>(dummyGL));

	// Make our newly created context the active context
	wglMakeCurrent(static_cast<HDC>(mDeviceContext), static_cast<HGLRC>(mOpenGL));

	// Gets for the openGL version
	int mOpenGLMajor, mOpenGLMinor;
	glGetIntegerv(GL_MAJOR_VERSION, &mOpenGLMajor);
	glGetIntegerv(GL_MINOR_VERSION, &mOpenGLMinor);

	// TEMPORARY print to see what OpenGL version we got
	// REPLACEMENT : LOGGER OUTPUT
	std::fprintf(stdout, "Using OpenGL Version %d.%d!\n", mOpenGLMajor, mOpenGLMinor);

	// Return true to indicate success
	return true;
}

bool Dystopia::GraphicsSystem::SelectOpenGLVersion(void* _deviceContext) noexcept
{
	int attrbs[] =
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB, 4,	// OpenGL minimum Major ver
		WGL_CONTEXT_MINOR_VERSION_ARB, 3,	// OpenGL minimum Minor ver
		WGL_CONTEXT_FLAGS_ARB, 0,
		0
	};
	mOpenGL		= nullptr;
	mAvailable	= eGfxSettings::GRAPHICS_ALL;

	// Try to create at least OpenGL 4.3
	attrbs[3] = 3;
	mOpenGL = wglCreateContextAttribsARB(static_cast<HDC>(_deviceContext), NULL, attrbs);
	if (mOpenGL)
	{
		return true;
	}

	// Failed, try 3.2...
	mAvailable &= ~(GRAPHICS_COMPUTE | GRAPHICS_TESS);

	attrbs[1] = 3; attrbs[3] = 2;
	mOpenGL = wglCreateContextAttribsARB(static_cast<HDC>(_deviceContext), NULL, attrbs);
	if (mOpenGL)
	{
		return true;
	}

	// Failed, try 3.1...
	mAvailable &= ~(eGfxSettings::GRAPHICS_MSAA);

	attrbs[3] = 1;
	mOpenGL = wglCreateContextAttribsARB(static_cast<HDC>(_deviceContext), NULL, attrbs);
	if (mOpenGL)
	{
		return true;
	}

	return false;
}


