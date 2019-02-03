/* HEADER *********************************************************************************/
/*!
\file	GraphicsSystem.cpp
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
BRIEF HERE

TODO: Draw batching.

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "System/Graphics/GraphicsSystem.h"	// File header
#include "System/Graphics/GraphicsDefs.h"	// eGraphicSettings
#include "System/Graphics/CharSpace.h"
#include "System/Driver/Driver.h"			// EngineCore
#include "Lib/GraphicsLib.h"

// Mesh Includes
#include "System/Graphics/MeshSystem.h"

// Texture Includes
#include "System/Graphics/FontSystem.h"
#include "System/Graphics/TextureSystem.h"
#include "System/Graphics/Texture.h"
#include "System/Graphics/Texture2D.h"
#include "System/Graphics/TextureAtlas.h"

// Shader Includes
#include "System/Graphics/Shader.h"
#include "System/Graphics/ShaderSystem.h"

// Camera Includes
#include "System/Camera/CameraSystem.h"     // Camera System
#include "Component/Camera.h"				// Camera

#include "System/Window/WindowManager.h"	// Window Manager
#include "System/Window/Window.h"			// Window
#include "System/Scene/SceneSystem.h"
#include "System/Scene/Scene.h"
#include "System/Collision/CollisionSystem.h"
#include "System/Physics/PhysicsSystem.h"
#include "System/Logger/LoggerSystem.h"
#include "System/Logger/LogPriority.h"
#include "System/Profiler/ProfilerAction.h"
#include "System/Time/ScopedTimer.h"
#include "System/File/FileSystem.h"

#include "IO/TextSerialiser.h"

#include "Object/GameObject.h"              // GameObject
#include "Object/ObjectFlags.h"
#include "Component/Transform.h"
#include "Component/Renderer.h"
#include "Component/TextRenderer.h"
#include "Component/SpriteRenderer.h"
#include "Component/Collider.h"

#include "Globals.h"
#include "Utility/DebugAssert.h"			// DEBUG_ASSERT
#include "Math/Vectors.h"

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
#include "Editor/EditorMain.h"
#include "Editor/EditorCommands.h"
#endif 

namespace
{
	static auto const& pGfxAPI = ::Gfx::GetInstance();
}


int Dystopia::GraphicsSystem::DRAW_MODE = GL_POINTS;
const int& Dystopia::GraphicsSystem::GetDrawMode(void) noexcept
{
	return DRAW_MODE;
}

void Dystopia::GraphicsSystem::SetDrawMode(int _nMode) noexcept
{
	DRAW_MODE = _nMode;
}


Dystopia::GraphicsSystem::GraphicsSystem(void) noexcept :
	mvDebugColour{.0f, 1.f, .0f, .1f}, mvClearCol{0, 0, 0, 0}, mfGamma{2.0f}, mfDebugLineThreshold{0.958f}, mOpenGL{nullptr},
	mPixelFormat{0}, mAvailable{0}, mSettings(0), mbVsync{false}, mvResolution{Gbl::WINDOW_WIDTH, Gbl::WINDOW_HEIGHT}
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

void Dystopia::GraphicsSystem::ToggleVsync(bool _b) noexcept
{
	mSettings &= _b ? eGfxSettings::GRAPHICS_ALL : ~eGfxSettings::GRAPHICS_VSYNC;

	pGfxAPI->ToggleVSync(_b);
}

void Dystopia::GraphicsSystem::ToggleDebugDraw(bool _bDebugDraw) const
{
	const auto CamSys = EngineCore::GetInstance()->Get<CameraSystem>();
	//auto bDebugDraw = !CamSys->GetMasterCamera()->DrawDebug();

	for (auto& e : CamSys->GetAllCameras())
		e.SetDebugDraw(_bDebugDraw);
}

void Dystopia::GraphicsSystem::SetAllCameraAspect(const float _x, const float _y) const
{
	const auto CamSys = EngineCore::GetInstance()->Get<CameraSystem>();

	for (auto& c : CamSys->GetAllCameras())
	{
		c.SetViewAspect(_x, _y);
		c.Awake();
	}
}

bool Dystopia::GraphicsSystem::GetDebugDraw(void) const
{
	return CORE::Get<CameraSystem>()->GetMasterCamera()->DrawDebug();
}


void Dystopia::GraphicsSystem::PreInit(void)
{
	Window& window = CORE::Get<WindowManager>()->GetMainWindow();
	::Gfx::InitGraphicsAPI(reinterpret_cast<void const*>(&window.GetWindowHandle()), ::Gfx::GfxMode::OPENGL);

	CORE::Get<ShaderSystem>()->LoadShaderList("Shader/DefaultShaderList.txt", eFileDir::eResource, false);
	auto file = Serialiser::OpenFile<TextSerialiser>("Resource/Meshes/DefaultMeshList.txt", Serialiser::MODE_READ);
	MeshSystem* pMeshSys = CORE::Get<MeshSystem>();

	std::string MeshPath;

	pMeshSys->Init();
	pMeshSys->StartMesh();
	while (!file.EndOfInput())
	{
		file >> MeshPath;
		pMeshSys->LoadMesh(MeshPath);
	}
	pMeshSys->EndMesh();

#if EDITOR
	DrawSplash();
#else
	window.Show();
#endif
}

bool Dystopia::GraphicsSystem::Init(void)
{
	DEBUG_BREAK(mViews.size() < 3, "Graphics System Error: Graphics did not load settings properly!\n");
	auto pShaderSys = CORE::Get<ShaderSystem>();

	for (auto& e : mViews)
		e.Init();

	auto shader = pShaderSys->GetShader("FinalStage");
	shader->UploadUniformi("texGame", 0);
	shader->UploadUniformi("texUI", 1);

	return true;
}

void Dystopia::GraphicsSystem::PostInit(void)
{
	pGfxAPI->PrintEnvironment();

	if (CORE::Get<FileSystem>()->CheckFileExist("Shader/ShaderList.txt", eFileDir::eResource))
		CORE::Get<ShaderSystem>()->LoadShaderList("Shader/ShaderList.txt", eFileDir::eResource);

	glPointSize(10);

#   if defined(_DEBUG) | defined(DEBUG)
	if (auto err = glGetError())
		__debugbreak();
#   endif 
}


Dystopia::Framebuffer& Dystopia::GraphicsSystem::GetGameView(void) const noexcept
{
	return mViews[0];
}

Dystopia::Framebuffer& Dystopia::GraphicsSystem::GetUIView(void) const noexcept
{
	return mViews[1];
}

Dystopia::Framebuffer& Dystopia::GraphicsSystem::GetFrameBuffer(void) const noexcept
{
	return mViews[2];
}

Dystopia::Framebuffer& Dystopia::GraphicsSystem::GetView(int _n) const
{
	return mViews[_n];
}


void Dystopia::GraphicsSystem::DrawSplash(void)
{
	auto pCore = EngineCore::GetInstance();
	MeshSystem    *pMeshSys   = pCore->GetSubSystem <MeshSystem   >();
	TextureSystem *pTexSys    = pCore->GetSubSystem <TextureSystem>();
	ShaderSystem  *pShaderSys = pCore->GetSubSystem <ShaderSystem >();
	WindowManager *pWinSys    = pCore->GetSystem    <WindowManager>();

	Mesh*      mesh    = pMeshSys->GetMesh("Quad");
	Shader*    shader  = (*pShaderSys)["Logo Shader"];
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

	shader->UploadUniform("ViewMat", View);
	shader->UploadUniform("ProjectMat", Project);
	shader->UploadUniform("ModelMat", Math::Scale(w * 1.f, h * 1.f));
	shader->UploadUniform("Gamma", mfGamma);

	mesh->DrawMesh(GL_TRIANGLES);
	texture->Unbind();

	pWinSys->GetMainWindow().Show();
	SwapBuffers(
		pWinSys->GetMainWindow().GetDeviceContext()
	);

#   if defined(_DEBUG) | defined(DEBUG)
	if (auto err = glGetError())
		__debugbreak();
#   endif 
}

namespace
{
	struct ShaderUploadVisitor
	{
		Dystopia::Shader*& s;
		OString& strName;

		template <typename T>
		void operator()(T&& value)
		{
			s->UploadUniform(strName.c_str(), Ut::Fwd<T>(value));
		}

		template <>
		void operator() < int > (int&& value)
		{
			s->UploadUniformi(strName.c_str(), value);
		}
		template <>
		void operator() <int&> (int& value)
		{
			s->UploadUniformi(strName.c_str(), value);
		}
	};

	template <typename T>
	inline void DrawRenderer(T& _renderer, Dystopia::Shader* s, float _fGamma)
	{
		auto t = _renderer->GetTexture();
		auto m = _renderer->GetOwner()->GetComponent<Dystopia::Transform>()->GetTransformMatrix();

		if (t) t->Bind();

		s->UploadUniform("ModelMat", m);
		s->UploadUniform("Gamma", _fGamma);

		for (auto&e : _renderer->GetOverrides())
		{
			e.Get<2>().Visit(ShaderUploadVisitor{ s, e.Get<0>() });
		}

		_renderer->Draw();

		if (t) t->Unbind();
	}
}

void Dystopia::GraphicsSystem::DrawScene(Camera& _cam, Math::Mat4& _View, Math::Mat4& _Proj)
{
	ScopedTimer<ProfilerAction> timeKeeper{ "Graphics System", "Scene Draw" };

	AutoArray<Renderer*> set1{ ComponentDonor<Renderer>::mComponents.size() };
	AutoArray<SpriteRenderer*> set2{ ComponentDonor<SpriteRenderer>::mComponents.size() };
	AutoArray<TextRenderer*> set3{ ComponentDonor<TextRenderer>::mComponents.size() };

	for (auto& e : ComponentDonor<Renderer>::mComponents)
	{
		if constexpr (EDITOR)
			if (e.GetFlags() & eObjFlag::FLAG_EDITOR_OBJ) continue;

		if (e.GetFlags() & eObjFlag::FLAG_ACTIVE &&	nullptr != e.GetOwner())
			set1.Insert(&e);
	}
	for (auto& e : ComponentDonor<SpriteRenderer>::mComponents)
	{
		if constexpr (EDITOR)
			if (e.GetFlags() & eObjFlag::FLAG_EDITOR_OBJ) continue;

		if (e.GetFlags() & eObjFlag::FLAG_ACTIVE &&	nullptr != e.GetOwner())
			set2.Insert(&e);
	}
	for (auto& e : ComponentDonor<TextRenderer>::mComponents)
	{
		if constexpr(EDITOR)
			if (e.GetFlags() & eObjFlag::FLAG_EDITOR_OBJ) continue;

		if (e.GetFlags() & eObjFlag::FLAG_ACTIVE &&	nullptr != e.GetOwner())
			set3.Insert(&e);
	}

	std::sort(set1.begin(), set1.end(), [](const auto& _rhs, const auto& _lhs) {
		return _rhs->GetOwner()->GetComponent<Transform>()->GetGlobalPosition().z < _lhs->GetOwner()->GetComponent<Transform>()->GetGlobalPosition().z;
	});
	std::sort(set2.begin(), set2.end(), [](const auto& _rhs, const auto& _lhs) {
		return _rhs->GetOwner()->GetComponent<Transform>()->GetGlobalPosition().z < _lhs->GetOwner()->GetComponent<Transform>()->GetGlobalPosition().z;
	});
	std::sort(set3.begin(), set3.end(), [](const auto& _rhs, const auto& _lhs) {
		return _rhs->GetOwner()->GetComponent<Transform>()->GetGlobalPosition().z < _lhs->GetOwner()->GetComponent<Transform>()->GetGlobalPosition().z;
	});

	// Draw the game objects to screen based on the camera
	// Get Camera's layer, we only want to draw inclusive stuff
	auto ActiveFlags = _cam.GetOwner()->GetFlags();
	ActiveFlags &= eObjFlag::FLAG_ALL_LAYERS;

	for (auto& r : set1)
	{
		if constexpr (EDITOR)
			if (r->GetOwner()->GetFlags() & eObjFlag::FLAG_EDITOR_OBJ) continue;

		auto s = r->GetShader();
		if (s && s->IsValid() && r->GetTexture())
		{
			s->Bind();
			s->UploadUniform("vUVBounds", 0.f, 0.f, 1.f, 1.f);
		}
		else
		{
			s = CORE::Get<ShaderSystem>()->GetShader("No Texture");
			s->Bind();
		}

		s->UploadUniform("ProjectMat", _Proj);
		s->UploadUniform("ViewMat", _View);

		if (r->GetOwner()->GetFlags() & ActiveFlags)
		{
			DrawRenderer(r, s, mfGamma);
		}
	}

	for (auto& r : set2)
	{
		if constexpr (EDITOR)
			if (r->GetOwner()->GetFlags() & eObjFlag::FLAG_EDITOR_OBJ) continue;

		if (r->GetOwner()->GetFlags() & ActiveFlags)
		{
			auto s = r->GetShader();
			s = r->GetTexture() ? s : CORE::Get<ShaderSystem>()->GetShader("No Texture");
			s->Bind();
			s->UploadUniform("ProjectMat", _Proj);
			s->UploadUniform("ViewMat", _View);
			s->UploadUniform("vColor", r->GetTint());
			DrawRenderer(r, s, mfGamma);
		}
	}

	auto s = CORE::Get<ShaderSystem>()->GetShader("Font Shader");
	s->Bind();
	s->UploadUniform("ProjectMat", _Proj);
	s->UploadUniform("ViewMat", _View);

	for (auto& r : set3)
	{
		if constexpr (EDITOR)
			if (r->GetOwner()->GetFlags() & eObjFlag::FLAG_EDITOR_OBJ) continue;
		
		if (r->GetOwner()->GetFlags() & ActiveFlags)
		{
			DrawRenderer(r, s, mfGamma);
		}
	}

#   if defined(_DEBUG) | defined(DEBUG)
	if (auto err = glGetError())
		__debugbreak();
#   endif 
}

void Dystopia::GraphicsSystem::DrawDebug(Camera& _cam, Math::Mat4& _View, Math::Mat4& _Proj)
{
	ScopedTimer<ProfilerAction> timeKeeper{ "Graphics System", "Debug Draw" };
	auto AllObj = EngineCore::GetInstance()->GetSystem<CollisionSystem>()->GetAllColliders();
	auto ActiveFlags = _cam.GetOwner()->GetFlags();

	// Get Camera's layer, we only want to draw inclusive stuff
	ActiveFlags &= eObjFlag::FLAG_ALL_LAYERS | eObjFlag::FLAG_ACTIVE;

	glClear(GL_DEPTH_BUFFER_BIT);
	Shader* s = CORE::Get<ShaderSystem>()->GetShader("Collider Shader");

	s->Bind();
	s->UploadUniform("ViewMat", _View);
	s->UploadUniform("ProjectMat", _Proj);
	s->UploadUniform("threshold", mfDebugLineThreshold);

	Math::Vector4 CollidingColor{ 1.f, 0, 0, .1f }, SleepingColor{ 1.f,1.f,0,.1f }, TriggerColor{ .8f,.8f,.8f,.1f }, activeColor;

	// Find out a way to allow stuff other than colliders to draw stuff

	// Draw the game objects to screen based on the camera
	for (auto& Obj : AllObj)
	{
		if constexpr (EDITOR)
			if (Obj->GetFlags() & eObjFlag::FLAG_EDITOR_OBJ) continue;
		
		GameObject* pOwner = Obj->GetOwner();
		if (pOwner && (pOwner->GetFlags() & ActiveFlags))
		{
			if (Obj->GetColliderType() != eColliderType::CIRCLE)
				s->UploadUniform("ModelMat", pOwner->GetComponent<Transform>()->GetTransformMatrix() * Math::Translate(Obj->GetOffSet())  * Obj->GetTransformationMatrix());
			else
			{
				auto pos = pOwner->GetComponent<Transform>()->GetGlobalPosition();
				auto scaleV = pOwner->GetComponent<Transform>()->GetGlobalScale();
				//auto LocalScale = Math::Scale(scaleV.x, scaleV.y);
				auto scale = Math::Abs(scaleV[0]) > Math::Abs(scaleV[1]) ? Math::Abs(scaleV[0]) : Math::Abs(scaleV[1]);
				auto scaleM = Math::Scale(scale, scale);
				auto Translation = Math::Translate(pos.x, pos.y);
				s->UploadUniform("ModelMat", Translation * pOwner->GetComponent<Transform>()->GetGlobalRotation().Matrix() * Math::Translate(scaleV*Obj->GetOffSet()) * scaleM * Obj->GetTransformationMatrix());
			}
			
			if (Obj->IsSleeping())
			{
				activeColor = SleepingColor;
			}
			
			else if (Obj->HasCollision())
			{
				activeColor = CollidingColor;
			}
			
			else if (Obj->IsTrigger())
			{
				activeColor = TriggerColor;
			}
			
			else
			{
				activeColor = mvDebugColour;
			}

			if (Mesh* pObjMesh = Obj->GetMesh())
			{
				s->UploadUniform("vColor", activeColor);
				pObjMesh->DrawMesh(GetDrawMode());
				//activeColor.w = .0f;
				//s->UploadUniform("vColor", activeColor);
				//pObjMesh->DrawMesh(GL_LINE_LOOP);
			}
		}
	}

#   if defined(_DEBUG) | defined(DEBUG)
	if (auto err = glGetError())
		__debugbreak();
#   endif 
}

void Dystopia::GraphicsSystem::Update(float _fDT)
{
	if constexpr (EDITOR)
	{
		ScopedTimer<ProfilerAction> timeKeeper{ "Graphics System", "Editor Update" };
		EngineCore::Get<TextureSystem>()->EditorUpdate();
		EngineCore::Get<ShaderSystem >()->EditorUpdate();

#		if defined(_DEBUG) | defined(DEBUG)
		if (auto err = glGetError())
			__debugbreak();
#		endif 
	}

	StartFrame();

	glClearColor(0, 0, 0, 0);
	auto& AllCam = EngineCore::GetInstance()->GetSystem<CameraSystem>()->GetAllCameras();

	/*
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
		if constexpr (EDITOR)
			if (flags & eObjFlag::FLAG_EDITOR_OBJ) continue;
		if (flags & eObjFlag::FLAG_ACTIVE)
		{
			e.Update(_fDT);
		}
	}

	for(auto& e : mViews)
	{
		e.Bind();

		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	}

	// For every camera in the game window (can be more than 1!)
	for (auto& Cam : AllCam)
	{
		if constexpr (EDITOR)
			if (Cam.GetFlags() & eObjFlag::FLAG_EDITOR_OBJ) continue;

		// If the camera is inactive, skip
		if (Cam.GetOwner() && (Cam.GetFlags() & eObjFlag::FLAG_ACTIVE))
		{
			Cam.SetCamera();
			Math::Matrix4 View = Cam.GetViewMatrix();
			Math::Matrix4 Proj = Cam.GetProjectionMatrix();

			const auto surface = Cam.GetSurface();
			const auto vp = Cam.GetViewport();

			glViewport(static_cast<int>(vp.mnX), static_cast<int>(vp.mnY),
				static_cast<int>(vp.mnWidth), static_cast<int>(vp.mnHeight));

			// Temporary code
			surface->Bind();
			//Cam.GetSurface().SetBlendMode();
			//glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);


			DrawScene(Cam, View, Proj);

			if (Cam.DrawDebug())
				DrawDebug(Cam, View, Proj);

			surface->Unbind();
		}
	}

#  if defined(_DEBUG) | defined(DEBUG)
		if (auto err = glGetError())
			__debugbreak();
#   endif 
}

void Dystopia::GraphicsSystem::PostUpdate(void)
{
	EndFrame();

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

	for (auto& render : ComponentDonor<TextRenderer>::mComponents)
	{
		if (eObjFlag::FLAG_REMOVE & render.GetFlags())
		{
			ComponentDonor<TextRenderer>::mComponents.Remove(&render);
		}
	}

#   if defined(_DEBUG) | defined(DEBUG)
	if (auto err = glGetError())
		__debugbreak();
#   endif 
}


void Dystopia::GraphicsSystem::StartFrame(void)
{
	//SetAllCameraAspect(mvResolution.x, mvResolution.y);
}

void Dystopia::GraphicsSystem::EndFrame(void)
{
	// TODO: Final draw to combine layers & draw to screen
	// TODO: Draw a fullscreen quad fusing the GameView and UIView
	static Mesh* quad = EngineCore::GetInstance()->Get<MeshSystem>()->GetMesh("Quad");
	auto& fb = GetFrameBuffer();

#if (EDITOR)
	fb.Bind();
	unsigned const w = fb.AsTexture()->GetWidth(), h = fb.AsTexture()->GetHeight();

	glViewport(0, 0, w, h);
#else
	auto& win = EngineCore::Get<WindowManager>()->GetMainWindow();

	fb.Unbind();
	glViewport(0, 0, win.GetWidth(), win.GetHeight());
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
#endif

	GetGameView().AsTexture()->Bind(0);
	GetUIView().AsTexture()->Bind(1);

	auto const model =  Math::Scale(2.f, (float) Ut::Constant<int, EDITOR ? -2 : 2>::value);
	Shader* shader = CORE::Get<ShaderSystem>()->GetShader("FinalStage");
	shader->Bind();
	shader->UploadUniform("ModelMat", model);
	//shader->UploadUniform("Gamma", mfGamma);

	quad->DrawMesh(GL_TRIANGLES);
	if (auto err = glGetError())
		__debugbreak();

#if (EDITOR)
	fb.Unbind();
#else
	SwapBuffers(win.GetDeviceContext());
#endif
}

void Dystopia::GraphicsSystem::Shutdown(void)
{
	// We are responsible for this
	CORE::Get<MeshSystem   >()->Shutdown();
	CORE::Get<TextureSystem>()->Shutdown();
	CORE::Get<ShaderSystem >()->Shutdown();
}

void Dystopia::GraphicsSystem::LoadDefaults(void)
{
	mViews.Emplace(2048u, 2048u, true);
	mViews.Emplace(2048u, 2048u, true, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	mViews.Emplace(
		static_cast<unsigned>(mvResolution.x),
		static_cast<unsigned>(mvResolution.y),
		false
	);

#if EDITOR
	mViews.Emplace(2048u, 2048u, false);
#endif
	mvClearCol = { 0,0,0,0 };
	DRAW_MODE = GL_TRIANGLES;
}

void Dystopia::GraphicsSystem::SetResolution(unsigned w, unsigned h) noexcept
{
	mvResolution.x = static_cast<float>(w);
	mvResolution.y = static_cast<float>(h);
}

void Dystopia::GraphicsSystem::UpdateResolution(void) const noexcept
{
#if !EDITOR
	EngineCore::Get<WindowManager>()->GetMainWindow().SetSize(
		static_cast<unsigned>(mvResolution.x), static_cast<unsigned>(mvResolution.y)
	);
#endif

	static_cast<Texture2D*>(GetFrameBuffer().AsTexture())->ReplaceTexture(
		static_cast<unsigned>(mvResolution.x), static_cast<unsigned>(mvResolution.y), nullptr, false
	);
}

void Dystopia::GraphicsSystem::LoadSettings(DysSerialiser_t& _in)
{
	_in >> DRAW_MODE;

	int n;
	bool alpha;
	unsigned w, h;
	int src, dst;

	_in >> n;
	for (int j = 0; j < n; ++j)
	{
		_in >> w;
		_in >> h;
		_in >> alpha;
		_in >> src;
		_in >> dst;

		mViews.Emplace(w, h, alpha, src, dst);
	}
	_in >> mbDebugDrawCheckBox;
	_in >> mfDebugLineThreshold;
	_in >> mvDebugColour;
	_in >> mbVsync;
	_in >> mvClearCol;
	_in >> mvResolution;

	ToggleVsync(mbVsync);
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
		_out << e.GetBlendSrc();
		_out << e.GetBlendDst();
	}

	_out << mbDebugDrawCheckBox;
	_out << mfDebugLineThreshold;
	_out << mvDebugColour;
	_out << mbVsync;
	_out << mvClearCol;
	_out << mvResolution;
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

Dystopia::Texture* Dystopia::GraphicsSystem::LoadTexture(HashString const& _strName)
{
	return CORE::Get<TextureSystem>()->LoadTexture(_strName);
}

Dystopia::Texture* Dystopia::GraphicsSystem::LoadTexture(const char * _strName)
{
	return CORE::Get<TextureSystem>()->LoadTexture(_strName);
}

Dystopia::Texture* Dystopia::GraphicsSystem::LoadFont(const std::string &)
{
	return nullptr;
}

void Dystopia::GraphicsSystem::BindOpenGL(Window& _window) noexcept
{
	wglMakeCurrent(_window.GetDeviceContext(), static_cast<HGLRC>(mOpenGL));
}

bool Dystopia::GraphicsSystem::InitOpenGL(Window&)
{
	// Return true to indicate success
	return true;
}


#if EDITOR			
void Dystopia::GraphicsSystem::EditorUI(void)
{
	mbDebugDrawCheckBox = GetDebugDraw();
	//const auto pCore = EngineCore::GetInstance();
	//const auto pCamSys = pCore->GetSystem<CameraSystem>();
	EGUI::PushLeftAlign(160);
	const auto result = EGUI::Display::DragFloat("Gamma", &mfGamma, 0.1f, 0.1f, 10.f);
	switch (result)
	{
	case EGUI::eDragStatus::eSTART_DRAG:
		//EGUI::GetCommandHND()->StartRecording<GraphicsSystem>(&GraphicsSystem::mfGamma);
		break;
	case EGUI::eDragStatus::eENTER:
	case EGUI::eDragStatus::eEND_DRAG:
	case EGUI::eDragStatus::eDEACTIVATED:
		//EGUI::GetCommandHND()->EndRecording();
		break;
	case EGUI::eDragStatus::eDRAGGING:
	default:
		break;
	}

	//const auto& sceneCam = pCamSys->GetMasterCamera();
	if (EGUI::Display::CheckBox("V Sync", &mbVsync))
	{
		ToggleVsync(mbVsync);
	}

	EditorAspectRatio();

	//const auto& sceneCam = pCamSys->GetMasterCamera();
	if (EGUI::Display::CheckBox("Debug Draw", &mbDebugDrawCheckBox))
	{
		ToggleDebugDraw(mbDebugDrawCheckBox);
		//EGUI::GetCommandHND()->InvokeCommand<Camera>(&Camera::mbDebugDraw, tempBool);
	}

	EGUI::Display::Label("Collider Color");
	EGUI::SameLine(59);
	if (ImGui::ColorEdit4("Active Color", &mvDebugColour[0], (ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoLabel
		| ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoOptions)))
	{

	}

	//EGUI::PushID(0);
	//auto result2 = EGUI::Display::VectorFields("", &mvDebugColour, 0.01f, 0.f, 1.f);
	//for (const auto& elem : result2)
	//{
	//	switch (elem)
	//	{
	//	case EGUI::eDragStatus::eSTART_DRAG:
	//		//EGUI::GetCommandHND()->StartRecording<GraphicsSystem>(&GraphicsSystem::mvDebugColour);
	//		break;
	//	case EGUI::eDragStatus::eENTER:
	//	case EGUI::eDragStatus::eEND_DRAG:
	//	case EGUI::eDragStatus::eDEACTIVATED:
	//		//EGUI::GetCommandHND()->EndRecording();
	//		break;
	//	case EGUI::eDragStatus::eDRAGGING:
	//	default:
	//		break;
	//	}
	//}
	//EGUI::PopID();

	const auto result3 = EGUI::Display::DragFloat("Debug Line Threshold", &mfDebugLineThreshold, 0.01F, 0.F, 10.F);

	switch (result3)
	{
	case EGUI::eDragStatus::eSTART_DRAG:
		// EGUI::GetCommandHND()->StartRecording<GraphicsSystem>(&GraphicsSystem::mfDebugLineThreshold);
		break;
	case EGUI::eDragStatus::eENTER:
	case EGUI::eDragStatus::eEND_DRAG:
	case EGUI::eDragStatus::eDEACTIVATED:
		//EGUI::GetCommandHND()->EndRecording();
		break;
	case EGUI::eDragStatus::eDRAGGING:
	default:
		break;
	}

	EGUI::Display::Label("Background");
	EGUI::SameLine(83);
	if (ImGui::ColorEdit4("BG Color", &mvClearCol[0], (ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoLabel
		| ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoOptions)))
	{
		
	}

	EGUI::PopLeftAlign();
}

void Dystopia::GraphicsSystem::EditorAspectRatio()
{
	static int eAspect = 0;
	if (mvResolution == Math::Vec2{ 1600, 900 })
	{
		eAspect = 0;
	}
	else if (mvResolution == Math::Vec2{ 1440, 900 })
	{
		eAspect = 1;
	}
	else
	{
		eAspect = 2;
	}

	std::string arr[3]{"1600x900px [16:9]", "1440x900px [16:10]", "1024x768px [4:3]",  };

	if (EGUI::Display::DropDownSelection("Resolution", eAspect, arr, 100.f))
	{
		switch (eAspect)
		{
		case 0: // 16:9
			mvResolution = { 1600, 900 };
			break;
		case 1: // 16:10
			mvResolution = { 1600, 1000 };
			break;
		case 2: // 4:3
			mvResolution = { 1024, 768 };
			break;
		default:
			break;
		}

		UpdateResolution();
	}
}
#endif 
