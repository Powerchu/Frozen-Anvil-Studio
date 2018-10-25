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
#include "System/Graphics/GraphicsSystem.h"	// File header
#include "System/Graphics/GraphicsDefs.h"	// eGraphicSettings
#include "System/Driver/Driver.h"			// EngineCore

// Mesh Includes
#include "System/Graphics/MeshSystem.h"

// Texture Includes
#include "System/Graphics/TextureSystem.h"
#include "System/Graphics/Texture.h"
#include "System/Graphics/Texture2D.h"
#include "System/Graphics/TextureAtlas.h"

// Shader Includes
#include "System/Graphics/Shader.h"

// Camera Includes
#include "System/Camera/CameraSystem.h"     // Camera System
#include "Component/Camera.h"				// Camera

#include "System/Window/WindowManager.h"	// Window Manager
#include "System/Window/Window.h"			// Window
#include "System/Scene/SceneSystem.h"
#include "System/Scene/Scene.h"
#include "System/Collision/CollisionSystem.h"
#include "System/Logger/LoggerSystem.h"
#include "System/Logger/LogPriority.h"
#include "System/Profiler/ProfilerAction.h"
#include "System/Time/ScopedTimer.h"

#include "IO/TextSerialiser.h"

#include "Object/GameObject.h"              // GameObject
#include "Object/ObjectFlags.h"
#include "Component/Transform.h"
#include "Component/Renderer.h"
#include "Component/SpriteRenderer.h"
#include "Component/Collider.h"

#include "Globals.h"
#include "Utility/DebugAssert.h"			// DEBUG_ASSERT

#define WIN32_LEAN_AND_MEAN		// Exclude rarely used stuff from Windows headers
#define NOMINMAX				// Disable Window header min & max macros

#include <string>
#include <algorithm>			// TEMPORARY std::sort
#include <windows.h>			// WinAPI
#include <GL/glew.h>
#include <GL/wglew.h>			// glew Windows ext

#undef WIN32_LEAN_AND_MEAN		// Stop defines from spilling into code
#undef NOMINMAX
#undef ERROR

#if EDITOR
#include "Editor/EGUI.h"
#include "Editor/CommandList.h"
#include "Editor/Commands.h"
#endif 


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
	mOpenGL{ nullptr }, mPixelFormat{ 0 }, mAvailable{ 0 }, mfGamma{ 2.2f },
	mfDebugLineWidth{ 5.0f }, mvDebugColour{ .68f, 1.f, .278f, .65f }
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

void Dystopia::GraphicsSystem::ToggleDebugDraw(bool _bDebugDraw)
{
	auto CamSys = EngineCore::GetInstance()->Get<CameraSystem>();
	//auto bDebugDraw = !CamSys->GetMasterCamera()->DrawDebug();

	for (auto& e : CamSys->GetAllCameras())
		e.SetDebugDraw(_bDebugDraw);
}

bool Dystopia::GraphicsSystem::GetDebugDraw(void) const
{
	return EngineCore::GetInstance()->Get<CameraSystem>()->GetMasterCamera()->DrawDebug();
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

bool Dystopia::GraphicsSystem::Init(void)
{
	DEBUG_BREAK(mViews.size() < 3, "Graphics System Error: Graphics did not load settings properly!\n");

	for (auto& e : mViews)
		e.Init();

	glLineWidth(mfDebugLineWidth);

	return true;
}

void Dystopia::GraphicsSystem::PostInit(void)
{

}


Dystopia::Framebuffer& Dystopia::GraphicsSystem::GetGameView(void) const
{
	return mViews[0];
}

Dystopia::Framebuffer& Dystopia::GraphicsSystem::GetUIView(void) const
{
	return mViews[1];
}

Dystopia::Framebuffer& Dystopia::GraphicsSystem::GetFrameBuffer(void) const noexcept
{
	return mViews[2];
}


void Dystopia::GraphicsSystem::DrawSplash(void)
{
	auto pCore = EngineCore::GetInstance();
	MeshSystem*   pMeshSys = pCore->GetSubSystem<MeshSystem>();
	TextureSystem* pTexSys = pCore->GetSubSystem<TextureSystem>();
	WindowManager* pWinSys = pCore->GetSystem<WindowManager>();

	Mesh*   mesh = pMeshSys->GetMesh("Quad");
	Shader* shader = shaderlist["Logo Shader"];
	Texture2D* texture = pTexSys->LoadTexture<Texture2D>("Resource/Editor/EditorStartup.png");

	unsigned w = texture->GetWidth(), h = texture->GetHeight();

	pWinSys->GetMainWindow().SetSizeNoAdjust(w, h);
	pWinSys->GetMainWindow().CenterWindow();

	Math::Matrix4 View{}, Project{
			2.f / w, .0f, .0f, .0f,
			.0f, 2.f / h, .0f, .0f,
			.0f, .0f, .0f, .0f,
			.0f, .0f, .0f, 1.f
	};

	glViewport(0, 0, w, h);
	glClearColor(.0f, .0f, .0f, .0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	shader->Bind();
	texture->Bind();

	shader->UploadUniform("ProjectViewMat", Project * View);
	shader->UploadUniform("ModelMat", Math::Scale(w * 1.f, h * 1.f));
	shader->UploadUniform("Gamma", mfGamma);

	mesh->DrawMesh(GL_TRIANGLES);
	texture->Unbind();

	pWinSys->GetMainWindow().Show();
	SwapBuffers(
		pWinSys->GetMainWindow().GetDeviceContext()
	);
}

namespace
{
	template <typename T>
	inline void DrawRenderer(T& _renderer, Math::Mat4& _ProjView, Dystopia::Shader* s)
	{
		auto t = _renderer->GetTexture();

		auto m = _renderer->GetOwner()->GetComponent<Dystopia::Transform>()->GetTransformMatrix();

		if (t)
			t->Bind();

		s->Bind();

		s->UploadUniform("ProjectViewMat", _ProjView);
		s->UploadUniform("ModelMat", m);

		_renderer->Draw();

		if(t)
			t->Unbind();
	}
}

void Dystopia::GraphicsSystem::DrawScene(Camera& _cam, Math::Mat4& _ProjView)
{
	ScopedTimer<ProfilerAction> timeKeeper{ "Graphics System", "Scene Draw" };

	AutoArray<Renderer*> set1{ ComponentDonor<Renderer>::mComponents .size() };
	AutoArray<SpriteRenderer*> set2{ ComponentDonor<SpriteRenderer>::mComponents.size() };

	for (auto& e : ComponentDonor<Renderer>::mComponents)
	{
		if (e.GetFlags() & eObjFlag::FLAG_ACTIVE)
			set1.Insert(&e);
	}
	for (auto& e : ComponentDonor<SpriteRenderer>::mComponents)
	{
		if (e.GetFlags() & eObjFlag::FLAG_ACTIVE)
			set2.Insert(&e);
	}

	std::sort(set1.begin(), set1.end(), [](const auto& _rhs, const auto& _lhs) {
		return _rhs->GetOwner()->GetComponent<Transform>()->GetGlobalPosition().z < _lhs->GetOwner()->GetComponent<Transform>()->GetGlobalPosition().z;
	});
	std::sort(set2.begin(), set2.end(), [](const auto& _rhs, const auto& _lhs) {
		return _rhs->GetOwner()->GetComponent<Transform>()->GetGlobalPosition().z < _lhs->GetOwner()->GetComponent<Transform>()->GetGlobalPosition().z;
	});

	// Draw the game objects to screen based on the camera
	// Get Camera's layer, we only want to draw inclusive stuff
	auto ActiveFlags = _cam.GetOwner()->GetFlags();
	ActiveFlags &= eObjFlag::FLAG_ALL_LAYERS;

	for (auto& r : set1)
	{
		auto s = r->GetShader();
		s = r->GetTexture() ? s : shaderlist["No Texture"];

		if (r->GetOwner()->GetFlags() & ActiveFlags)
		{
			DrawRenderer(r, _ProjView, s);
		}
	}

	for (auto& r : set2)
	{
		auto s = r->GetShader();
		s = r->GetTexture() ? s : shaderlist["No Texture"];

		if (r->GetOwner()->GetFlags() & ActiveFlags)
		{
			DrawRenderer(r, _ProjView, s);
		}
	}
}

void Dystopia::GraphicsSystem::DrawDebug(Camera& _cam, Math::Mat4& _ProjView)
{
	ScopedTimer<ProfilerAction> timeKeeper{ "Graphics System", "Debug Draw" };
	auto AllObj = EngineCore::GetInstance()->GetSystem<CollisionSystem>()->GetAllColliders();
	auto ActiveFlags = _cam.GetOwner()->GetFlags();

	// Get Camera's layer, we only want to draw inclusive stuff
	ActiveFlags &= eObjFlag::FLAG_ALL_LAYERS | eObjFlag::FLAG_ACTIVE;

	glClear(GL_DEPTH_BUFFER_BIT);
	Shader* s = shaderlist["Colour Shader"];
	
	s->Bind();
	//s->UploadUniform("vColor", mvDebugColour);
	s->UploadUniform("ProjectViewMat", _ProjView);

	Math::Vec4 no_alpha{ mvDebugColour };

	Math::Vector4 CollidingColor{1.f, 0, 0, .75f}, activeColor;

	// Find out a way to allow stuff other than colliders to draw stuff

	// Draw the game objects to screen based on the camera
	for (auto& Obj : AllObj)
	{
		GameObject* pOwner = Obj->GetOwner();
		if (pOwner && (pOwner->GetFlags() & ActiveFlags))
		{
			
			s->UploadUniform("ModelMat", pOwner->GetComponent<Transform>()->GetLocalTransformMatrix() *Math::Translate(Obj->GetOffSet().x, Obj->GetOffSet().y, Obj->GetOffSet().z)  * Obj->GetTransformationMatrix());
			
			activeColor = Obj->HasCollision() ? CollidingColor : mvDebugColour;

			if (Mesh* pObjMesh = Obj->GetMesh())
			{
				s->UploadUniform("vColor", activeColor);
				pObjMesh->DrawMesh(GetDrawMode());
				activeColor.w = 1.f;
				s->UploadUniform("vColor", activeColor);
				pObjMesh->DrawMesh(GL_LINE_LOOP);
			}
		}
	}

#if defined(EDITOR) | defined(_DEBUG) | defined(DEBUG)
	if (glGetError() != GL_NO_ERROR)
	{
		__debugbreak();
	}
#endif
}

void Dystopia::GraphicsSystem::Update(float _fDT)
{
	StartFrame();

	glClearColor(.0f, .0f, .0f, 1.f);
	auto& AllCam = EngineCore::GetInstance()->GetSystem<CameraSystem>()->GetAllCameras();

	/*
	for (auto& e : mViews)
	{
		e.Bind();
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		e.Unbind();
	}

	// Do batching, depth sorting and grouping of translucent elements. 
	for (auto& e : ComponentDonor<Renderer>::mComponents)
	{
		auto flags = e.GetFlags();

#   if EDITOR
		if (flags & eObjFlag::FLAG_EDITOR_OBJ)
			continue;
#   endif

		if (flags & eObjFlag::FLAG_RESERVED)
		{
			// Remove the object from the pool

			// Modify the object's status in the render pool
			if (flags & eObjFlag::FLAG_ACTIVE)
			{
				// Add the object back in, into the correct spot
			}
		}
	}
	*/

	for (auto& e : ComponentDonor<SpriteRenderer>::mComponents)
	{
		auto flags = e.GetFlags();

		if (flags & eObjFlag::FLAG_ACTIVE)
		{
			e.Update(_fDT);
		}
	}

	// For every camera in the game window (can be more than 1!)
	for (auto& Cam : AllCam)
	{
#if EDITOR
		if (Cam.GetFlags() & eObjFlag::FLAG_EDITOR_OBJ) continue;
#endif 

		// If the camera is inactive, skip
		if (Cam.GetOwner()->GetFlags() & eObjFlag::FLAG_ACTIVE)
		{
			Cam.SetCamera();
			Math::Matrix4 ProjView = Cam.GetProjectionMatrix() * Cam.GetViewMatrix();

			auto surface = Cam.GetSurface();

			// Temporary code
			surface->Bind();
			glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

			DrawScene(Cam, ProjView);
			
			 if (Cam.DrawDebug())
				DrawDebug(Cam, ProjView);

			 surface->Unbind();
		}
	}

	EndFrame();
}

void Dystopia::GraphicsSystem::PostUpdate(void)
{
	for (auto& render : ComponentDonor<Renderer>::mComponents)
	{
		if (eObjFlag::FLAG_REMOVE & render.GetFlags())
		{
			ComponentDonor<Renderer>::mComponents.Remove(&render);
		}
	}

	for (auto& render : ComponentDonor<SpriteRenderer>::mComponents)
	{
		if (eObjFlag::FLAG_REMOVE & render.GetFlags())
		{
			ComponentDonor<SpriteRenderer>::mComponents.Remove(&render);
		}
	}
}


void Dystopia::GraphicsSystem::StartFrame(void)
{

}

void Dystopia::GraphicsSystem::EndFrame(void)
{
#if EDITOR
	// TODO: Final draw to combine layers & draw to screen
	// TODO: Draw a fullscreen quad fusing the GameView and UIView
	static Mesh* quad = EngineCore::GetInstance()->Get<MeshSystem>()->GetMesh("Quad");

	auto fb = GetFrameBuffer();

	fb.Bind();
#endif

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	GetGameView().AsTexture()->Bind(0);
	GetUIView().AsTexture()->Bind(1);

	Shader* shader = shaderlist["FinalStage"];
	shader->Bind();
	shader->UploadUniform("Scale", 2.f);
	shader->UploadUniform("Gamma", mfGamma);

	quad->DrawMesh(GL_TRIANGLES);

#if EDITOR
	fb.Unbind();
#else
	SwapBuffers(EngineCore::GetInstance()->GetSystem<WindowManager>()->GetMainWindow().GetDeviceContext());
#endif
}

void Dystopia::GraphicsSystem::Shutdown(void)
{
	auto pCore = EngineCore::GetInstance();

	for (auto& e : shaderlist)
		delete e.second;
	shaderlist.clear();

	// We are responsible for this
	pCore->GetSubSystem<MeshSystem>()->Shutdown();
	pCore->GetSubSystem<TextureSystem>()->Shutdown();
}

void Dystopia::GraphicsSystem::LoadDefaults(void)
{
	mViews.Emplace(2048u, 2048u, true);
	mViews.Emplace(1024u, 1024u, true);
	mViews.Emplace(
		static_cast<unsigned>(Gbl::SCREEN_WIDTH ), 
		static_cast<unsigned>(Gbl::SCREEN_HEIGHT), 
		false
	);

	DRAW_MODE = GL_TRIANGLES;
}

void Dystopia::GraphicsSystem::LoadSettings(DysSerialiser_t& _in)
{
	_in >> DRAW_MODE;

	int n;
	bool alpha;
	unsigned w, h;

	_in >> n;
	for (int j = 0; j < n; ++j)
	{
		_in >> w;
		_in >> h;
		_in >> alpha;

		mViews.Emplace(w, h, alpha);
	}
}

void Dystopia::GraphicsSystem::SaveSettings(DysSerialiser_t& _out)
{
	_out << DRAW_MODE;

	_out << mViews.size();
	for (auto& e : mViews)
	{
		_out << e.GetWidth();
		_out << e.GetHeight();
		_out << e.HasAlpha();
	}
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
	return EngineCore::GetInstance()->GetSubSystem<TextureSystem>()->LoadTexture(_strName);
}

Dystopia::Shader* Dystopia::GraphicsSystem::LoadShader(const std::string& _filePath)
{
	auto file = Serialiser::OpenFile<TextSerialiser>(_filePath, Serialiser::MODE_READ);
	std::string strName, strVert, strGeo, strFrag;

	file.ConsumeStartBlock();
	while (!file.EndOfInput())
	{
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
		file.ConsumeStartBlock();
	}

	return nullptr;
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
	LoggerSystem::ConsoleLog(eLog::SYSINFO, "Graphics System: %d bit colour, %d bit depth, %d bit stencil\n", pfd.cColorBits, pfd.cDepthBits, pfd.cStencilBits);

#endif

	if (!wglewIsSupported("WGL_EXT_swap_control"))
	{
		mAvailable &= ~(eGfxSettings::GRAPHICS_VSYNC);
	}

	glEnable(GL_BLEND);
//	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LINE_SMOOTH);

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


void Dystopia::GraphicsSystem::EditorUI(void)
{
#if EDITOR								   
	const auto result = EGUI::Display::DragFloat("Gamma       ", &mfGamma, 0.1f, 0.1f, 10.f);
	switch (result)
	{
	case EGUI::eDragStatus::eSTART_DRAG:
		EGUI::GetCommandHND()->StartRecording<GraphicsSystem>(&mfGamma);
		break;
	case EGUI::eDragStatus::eENTER:
	case EGUI::eDragStatus::eEND_DRAG:
	case EGUI::eDragStatus::eDEACTIVATED:
		EGUI::GetCommandHND()->EndRecording();
		break;
	case EGUI::eDragStatus::eDRAGGING:
	default:
		break;
	}

	bool tempBool = GetDebugDraw();
	if (EGUI::Display::CheckBox("Debug Draw  ", &tempBool))
	{
		//mbDebugDraw = tempBool;
		//EGUI::GetCommandHND()->InvokeCommand<GraphicsSystem>(&ToggleDebugDraw, this, tempBool);
	}

	auto result2 = EGUI::Display::VectorFields("Debug Color ", &mvDebugColour, 0.01f, 0.f, 1.f);
	for (const auto& elem : result2)
	{
		switch (elem)
		{
		case EGUI::eDragStatus::eSTART_DRAG:
			EGUI::GetCommandHND()->StartRecording<GraphicsSystem>(&mvDebugColour);
			break;
		case EGUI::eDragStatus::eENTER:
		case EGUI::eDragStatus::eEND_DRAG:
		case EGUI::eDragStatus::eDEACTIVATED:
			EGUI::GetCommandHND()->EndRecording();
			break;
		case EGUI::eDragStatus::eDRAGGING:
		default:
			break;
		}
	}

	const auto result3 = EGUI::Display::DragFloat("Debug Line Width", &mfDebugLineWidth, 0.01F, 0.F, 10.F);

	switch (result3)
	{
	case EGUI::eDragStatus::eSTART_DRAG:
		EGUI::GetCommandHND()->StartRecording<GraphicsSystem>(&mfDebugLineWidth);
		break;
	case EGUI::eDragStatus::eENTER:
	case EGUI::eDragStatus::eEND_DRAG:
	case EGUI::eDragStatus::eDEACTIVATED:
		EGUI::GetCommandHND()->EndRecording();
		break;
	case EGUI::eDragStatus::eDRAGGING:
	default:
		break;
	}
	

#endif 
}

