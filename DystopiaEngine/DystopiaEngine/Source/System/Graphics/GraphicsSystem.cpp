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
#include "System\Graphics\MeshSystem.h"
#include "System\Graphics\Shader.h"
#include "System\Graphics\Texture2D.h"
#include "System\Window\WindowManager.h"	// Window Manager
#include "System\Window\Window.h"			// Window
#include "System\Scene\SceneSystem.h"
#include "System\Scene\Scene.h"
#include "System\Camera\CameraSystem.h"     // Camera System
#include "System\Driver\Driver.h"			// EngineCore
#include "System\Time\ScopedTimer.h"
#include "System\Profiler\ProfilerAction.h"
#include "System\Logger\LogPriority.h"
#include "System\Logger\LoggerSystem.h"

#include "IO\TextSerialiser.h"
#include "IO\ImageParser.h"

#include "Object\GameObject.h"              // GameObject
#include "Object\ObjectFlags.h"
#include "Component\Transform.h"
#include "Component\Renderer.h"
#include "Component\Camera.h"				// Camera

#include "Utility\DebugAssert.h"			// DEBUG_ASSERT

#define WIN32_LEAN_AND_MEAN		// Exclude rarely used stuff from Windows headers
#define NOMINMAX				// Disable Window header min & max macros

#include <windows.h>			// WinAPI
#include <GL\glew.h>
#include <GL\wglew.h>			// glew Windows ext
#include <GL\GL.h>
#include <cstdio>
#include <string>

#undef WIN32_LEAN_AND_MEAN		// Stop defines from spilling into code
#undef NOMINMAX
#undef ERROR


int Dystopia::GraphicsSystem::DRAW_MODE = GL_TRIANGLES;
const int& Dystopia::GraphicsSystem::GetDrawMode(void) noexcept
{
	return DRAW_MODE;
}

void Dystopia::GraphicsSystem::SetDrawMode(int _nMode) noexcept
{
	DRAW_MODE = _nMode;
}


Dystopia::GraphicsSystem::GraphicsSystem(void) noexcept :
	mOpenGL{ nullptr }, mPixelFormat{ 0 }, mAvailable{ 0 }, mfGamma{ 2.2f }
{

}

Dystopia::GraphicsSystem::~GraphicsSystem(void)
{

}


void Dystopia::GraphicsSystem::SetGamma(float _fGamma) noexcept
{
	mfGamma = _fGamma;
}

float Dystopia::GraphicsSystem::GetGamma(void) noexcept
{
	return mfGamma;
}


void Dystopia::GraphicsSystem::PreInit(void)
{
	Window& window = EngineCore::GetInstance()->GetSystem<WindowManager>()->GetMainWindow();
	InitOpenGL(window);
	BindOpenGL(window);

	LoadShader("Resource/Shader/DefaultShaderList.txt");
	auto file = Serialiser::OpenFile<TextSerialiser>("Resource/Meshes/DefaultMeshList.txt", Serialiser::MODE_READ);
	MeshSystem* pMeshSys = EngineCore::GetInstance()->GetSubSystem<MeshSystem>();

	std::string MeshPath;

	pMeshSys->Init();
	pMeshSys->StartMesh();
	while (!file.EndOfInput())
	{
		file >> MeshPath;
		pMeshSys->LoadMesh(MeshPath);
	}
	pMeshSys->EndMesh();

	DrawSplash();
}

void Dystopia::GraphicsSystem::DrawSplash(void)
{
	WindowManager* pWinSys = EngineCore::GetInstance()->GetSystem<WindowManager>();
	MeshSystem* pMeshSys = EngineCore::GetInstance()->GetSubSystem<MeshSystem>();

	Mesh*   mesh = pMeshSys->GetMesh("Quad");
	Shader* shader = shaderlist.begin()->second;
	Texture2D* texture = new Texture2D{ "Resource/Editor/EditorStartup.png" };

	unsigned w = texture->GetWidth(), h = texture->GetHeight();

	pWinSys->GetMainWindow().SetSizeNoAdjust(w, h);
	pWinSys->GetMainWindow().CenterWindow();

	Math::Matrix4 View{}, Project{
			2.f / w, .0f, .0f, .0f,
			.0f, 2.f / h, .0f, .0f,
			.0f, .0f, 2.f / 100.f, .0f,
			.0f, .0f, .0f, 1.f
	};

	glViewport(0, 0, w, h);
	glClearColor(.0f, .0f, .0f, .0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	shader->UseShader();
	texture->BindTexture();

	shader->UploadUniform("ProjectViewMat", Project * View);
	shader->UploadUniform("ModelMat", Math::Scale(w * 1.f, h * 1.f));
	shader->UploadUniform("Gamma", mfGamma);

	mesh->UseMesh(GL_TRIANGLES);
	texture->UnbindTexture();

	pWinSys->GetMainWindow().Show();
	SwapBuffers(
		pWinSys->GetMainWindow().GetDeviceContext()
	);

	delete texture;
}

bool Dystopia::GraphicsSystem::Init(void)
{
	mGameView.Init(2048, 2048);
	mUIView.Init(1024, 1024);

	return true;
}

void Dystopia::GraphicsSystem::PostInit(void)
{

}

void Dystopia::GraphicsSystem::Update(float)
{
	ScopedTimer<ProfilerAction> timeKeeper{"Graphics System", "Update"};
	StartFrame();

	mGameView.BindFramebuffer();
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	auto& AllCam = EngineCore::GetInstance()->GetSystem<CameraSystem>()->GetAllCameras();
	auto& AllObj = EngineCore::GetInstance()->GetSystem<SceneSystem>()->GetCurrentScene().GetAllGameObjects();

	// For every camera in the game window (can be more than 1!)
	for (auto& Cam : AllCam)
	{
		auto ActiveFlags = Cam.GetOwner()->GetFlags();
		Math::Matrix4 ProjView = Cam.GetProjectionMatrix() * Cam.GetViewMatrix();

		// If the camera is inactive, skip
		if (Cam.GetOwner()->GetFlags() & eObjFlag::FLAG_ACTIVE)
		{
			Cam.SetCamera();

			// Get Camera's layer, we only want to draw inclusive stuff
			ActiveFlags &= eObjFlag::FLAG_ALL_LAYERS | eObjFlag::FLAG_ACTIVE;

			AllObj.Sort([](const auto& _rhs, const auto& _lhs) {
				return _rhs.GetComponent<Transform>()->GetGlobalPosition().z < _lhs.GetComponent<Transform>()->GetGlobalPosition().z;
			});
			// Draw the game objects to screen based on the camera
			for (auto& Obj : AllObj)
			{
				if (Obj.GetFlags() & ActiveFlags)
				{
					if (Renderer* r = Obj.GetComponent<Renderer>())
					{
						if (Shader* s = r->GetShader())
						{
							if (Texture2D* t = static_cast<Texture2D*>(r->GetTexture()))
							{
								s->UseShader();

								t->BindTexture();
								s->UploadUniform("ProjectViewMat", ProjView);
								s->UploadUniform("ModelMat", Obj.GetComponent<Transform>()->GetTransformMatrix());
								s->UploadUniform("Gamma", mfGamma);

								r->Draw();

								t->UnbindTexture();
							}
						}
					}
				}
			}
		}
	}
	mGameView.UnbindFramebuffer();

	// TODO: Final draw to combine layers & draw to screen

	EndFrame();
}

void Dystopia::GraphicsSystem::StartFrame(void)
{

}

void Dystopia::GraphicsSystem::EndFrame(void)
{
#if !EDITOR
	// TODO: Draw a fullscreen quad fusing the GameView and UIView

	SwapBuffers(mCurrent->GetDeviceContext());
#endif
}

void Dystopia::GraphicsSystem::Shutdown(void)
{
	auto pCore = EngineCore::GetInstance();

	for (auto& e : shaderlist)
		delete e.second;
	shaderlist.clear();

	for (auto& e : texturelist)
		delete static_cast<Texture2D*>(e.second);
	texturelist.clear();

	// We are responsible for this
	pCore->GetSubSystem<MeshSystem>()->FreeMeshes();
}

void Dystopia::GraphicsSystem::LoadDefaults(void)
{
	DRAW_MODE = GL_TRIANGLES;
}

void Dystopia::GraphicsSystem::LoadSettings(DysSerialiser_t&)
{
	// TODO
}


void Dystopia::GraphicsSystem::LevelLoad(TextSerialiser&)
{
	// TODO
}

void Dystopia::GraphicsSystem::LoadMesh(const std::string& _filePath)
{
	MeshSystem* sys = EngineCore::GetInstance()->GetSubSystem<MeshSystem>();

	sys->StartMesh();
	sys->LoadMesh(_filePath);
	sys->EndMesh();
}

Dystopia::Texture* Dystopia::GraphicsSystem::LoadTexture(const std::string& _strName)
{
	size_t first = _strName.rfind("/");
	return texturelist[_strName.substr(first, _strName.find_first_of('.', first))] = new Texture2D{ _strName };
}

Dystopia::Shader* Dystopia::GraphicsSystem::LoadShader(const std::string& _filePath)
{
	auto file = Serialiser::OpenFile<TextSerialiser>(_filePath, Serialiser::MODE_READ);
	std::string strName, strVert, strGeo, strFrag;

	file.ConsumeStartBlock();
	file >> strName;
	file >> strVert;
	file >> strFrag;

	shaderlist[strName] = new Shader{};
	if (file.EndOfInput())
	{
		shaderlist[strName]->CreateShader(strVert, strFrag);
	}
	else
	{
		file >> strGeo;
		shaderlist[strName]->CreateShader(strVert, strFrag, strGeo);
	}

	return nullptr;
}

void Dystopia::GraphicsSystem::SetMasterViewport(int _nX, int _nY, int _nWidth, int _nHeight) noexcept
{
	EngineCore::GetInstance()->GetSystem<CameraSystem>()->SetMasterViewport(
		_nX, _nY, _nWidth, _nHeight
	);
}

void Dystopia::GraphicsSystem::BindOpenGL(Window& _window) noexcept
{
	wglMakeCurrent(_window.GetDeviceContext(), static_cast<HGLRC>(mOpenGL));
}

bool Dystopia::GraphicsSystem::InitOpenGL(Window& _window)
{
	// Use to specify the color format we want and openGL support
	PIXELFORMATDESCRIPTOR pfd{};

	pfd.nSize		 = sizeof(PIXELFORMATDESCRIPTOR);	// Windows requirement
	pfd.nVersion	 = 1;								// Windows requirement
	pfd.dwFlags		 = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
	pfd.iPixelType	 = PFD_TYPE_RGBA;
	pfd.cColorBits	 = 32;
	pfd.cDepthBits	 = 24;
	pfd.cStencilBits = 8;
	pfd.iLayerType	 = PFD_MAIN_PLANE;

	// Ask windows to give us a pixel format based on what we asked for
	int nPxFormat = ChoosePixelFormat(_window.GetDeviceContext(), &pfd);

	if (0 == nPxFormat) // Check if we got something back
	{
		DEBUG_PRINT(eLog::ERROR, "Graphics System Error: ChoosePixelFormat fail! \n");

		return false;
	}

	// Attempt to set the format based on the returned pixel formal
	BOOL bResult = SetPixelFormat(_window.GetDeviceContext(), nPxFormat, &pfd);

	if (!bResult) // This shouldn't happen
	{
		DEBUG_PRINT(eLog::ERROR, "Graphics System Error: SetPixelFormat fail! \n");

		return false;
	}

	// Create a fake context so we can create context
	HGLRC dummyGL = wglCreateContext(_window.GetDeviceContext());
	wglMakeCurrent(_window.GetDeviceContext(), dummyGL);

	// attempt to init glew so that there is an active GL context
	unsigned err = glewInit();

	if (err != GLEW_OK)
	{
		DEBUG_PRINT(eLog::ERROR, "Graphics System Error: GLEW init fail! \n");

		return false;
	}

	// Check if gl 3.1 and above context is supported
	if (wglewIsSupported("WGL_ARB_create_context") == 1 && !SelectOpenGLVersion(_window))
	{
		DEBUG_PRINT(eLog::ERROR, "Graphics System Error: OpenGL 3.1 and above not supported! \n");

		return false;
	}

	// Unbind the dummy context
	wglMakeCurrent(nullptr, nullptr);

	// Delete the dummy context
	wglDeleteContext(dummyGL);

	// Make our newly created context the active context
	wglMakeCurrent(_window.GetDeviceContext(), static_cast<HGLRC>(mOpenGL));

#if EDITOR

	// Gets for the openGL version
	int mOpenGLMajor, mOpenGLMinor;
	glGetIntegerv(GL_MAJOR_VERSION, &mOpenGLMajor);
	glGetIntegerv(GL_MINOR_VERSION, &mOpenGLMinor);

	LoggerSystem::ConsoleLog(eLog::SYSINFO, "Graphics System: %s, %s\n", glGetString(GL_VENDOR), glGetString(GL_RENDERER));
	LoggerSystem::ConsoleLog(eLog::SYSINFO, "Graphics System: Using OpenGL Version %d.%d\n", mOpenGLMajor, mOpenGLMinor);
	LoggerSystem::ConsoleLog(eLog::SYSINFO, "Graphics System: %d bit colour, %d bits depth, %d bit stencil\n", pfd.cColorBits, pfd.cDepthBits, pfd.cStencilBits);

#endif

	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Return true to indicate success
	return true;
}

bool Dystopia::GraphicsSystem::SelectOpenGLVersion(Window& _window) noexcept
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
	mOpenGL = wglCreateContextAttribsARB(_window.GetDeviceContext(), NULL, attrbs);
	if (mOpenGL)
	{
		return true;
	}

	// Failed, try 3.2...
	mAvailable &= ~(eGfxSettings::GRAPHICS_COMPUTE | eGfxSettings::GRAPHICS_TESS);

	attrbs[1] = 3; attrbs[3] = 2;
	mOpenGL = wglCreateContextAttribsARB(_window.GetDeviceContext(), NULL, attrbs);
	if (mOpenGL)
	{
		return true;
	}

	// Failed, try 3.1...
	mAvailable &= ~(eGfxSettings::GRAPHICS_MSAA);

	attrbs[3] = 1;
	mOpenGL = wglCreateContextAttribsARB(_window.GetDeviceContext(), NULL, attrbs);
	if (mOpenGL)
	{
		return true;
	}

	return false;
}

Dystopia::Framebuffer& Dystopia::GraphicsSystem::GetFrameBuffer()
{
	return mGameView;
}


