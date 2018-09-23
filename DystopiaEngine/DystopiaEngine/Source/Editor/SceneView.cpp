/* HEADER *********************************************************************************/
/*!
\file	SceneView.cpp
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief
BRIEF HERE

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#include "Editor/SceneView.h"
#include "Editor/Editor.h"
#include "Editor/EGUI.h"
#include "Editor/DefaultFactory.h"

#include "System/Driver/Driver.h"
#include "System/Graphics/GraphicsSystem.h"
#include "Component/Renderer.h"
#include "System/Camera/CameraSystem.h"
#include "System/Input/InputSystem.h"
#include "System/Physics/PhysicsSystem.h"
#include "System/Collision/CollisionSystem.h"
#include "Component/Collider.h"

#include "System\Scene\Scene.h"
#include "System\Graphics\Texture2D.h"
#include "Component\Camera.h"

#include "System/Scene/Scene.h"
#include "System/Graphics/Texture2D.h"
#include "Component/Camera.h"

#include "Component/RigidBody.h"

namespace Dystopia
{
	static SceneView* gpInstance = 0;
	SceneView* SceneView::GetInstance()
	{
		if (gpInstance) return gpInstance;

		gpInstance = new SceneView{};
		return gpInstance;
	}

	SceneView::SceneView()
		: EditorTab{ true },
		mLabel{ "Scene View" },
		mDelta{},
		mpGfxSys{ nullptr },
		mpPhysSys{ nullptr },
		mpColSys{ nullptr },
		mpSceneCamera{ nullptr }

	{}

	SceneView::~SceneView()
	{
		gpInstance = nullptr;
	}

	void SceneView::Init()
	{
		mpGfxSys = EngineCore::GetInstance()->GetSystem<GraphicsSystem>();
		mpPhysSys = EngineCore::GetInstance()->GetSystem<PhysicsSystem>();
		mpColSys = EngineCore::GetInstance()->GetSystem<CollisionSystem>();
		mpInputSys = EngineCore::GetInstance()->GetSystem<InputManager>();


		// Scene Camera
		GameObject *p = Factory::CreateCamera("Scene Camera");
		mpSceneCamera = GetCurrentScene()->InsertGameObject(Utility::Move(*p));
		mpSceneCamera->GetComponent<Camera>()->Init();
		mpSceneCamera->GetComponent<Transform>()->SetScale(Math::Vec4{ 1.f, 1.f, 1.f });
		delete p;

		// Sample Box Object
		GameObject *b = Factory::CreateBox("Box Object");
		Texture2D* _bt = new Texture2D{ "Resource/Editor/red_box.png" };
		mpBoxObject = GetCurrentScene()->InsertGameObject(Utility::Move(*b));
		mpBoxObject->GetComponent<RigidBody>()->Init();
		mpBoxObject->GetComponent<Transform>()->SetScale(Math::Vec4{ 64.f, 64.f, 1.f });
		mpBoxObject->GetComponent<Renderer>()->SetTexture(_bt);
		mpBoxObject->GetComponent<Collider>()->Init();
		delete b;

		// Sample Static Object
		GameObject *s = Factory::CreateStaticBox("Static Box");
		Texture2D* _st = new Texture2D{ "Resource/Editor/white_box.png" };
		mpStaticBoxObject = GetCurrentScene()->InsertGameObject(Utility::Move(*s));
		mpStaticBoxObject->GetComponent<RigidBody>()->Init();
		mpStaticBoxObject->GetComponent<RigidBody>()->Set_IsStatic(true);
		mpStaticBoxObject->GetComponent<Transform>()->SetGlobalPosition ({ 0, -185.f, 0 });
		mpStaticBoxObject->GetComponent<Transform>()->SetScale(Math::Vec4{ 512.f, 32.f, 1.f });
		mpStaticBoxObject->GetComponent<Collider>()->Init();
		mpStaticBoxObject->GetComponent<Renderer>()->SetTexture(_st);
		delete s;
	}

	void SceneView::Update(const float& _dt)
	{
		mDelta = _dt;
		if (GetMainEditor().CurrentState() != EDITOR_PLAY)
		{
			mpGfxSys->Update(mDelta);
			mpInputSys->Update(mDelta);
			mpColSys->FixedUpdate(mDelta);
			mpPhysSys->FixedUpdate(mDelta);
			if (mpInputSys->IsKeyTriggered(eUserButton::BUTTON_SPACEBAR))
			{
				mpBoxObject->GetComponent<Transform>()->SetGlobalPosition({ 0,0,0,0 });
				mpBoxObject->GetComponent<RigidBody>()->SetVelocity({ 0,0,0,0 });
			}
		}
	}

	void SceneView::EditorUI()
	{
		unsigned id = mpGfxSys->GetFrameBuffer().AsTexture()->GetID();
		ImGui::Image(reinterpret_cast<void*>(id), ImVec2{ Size().x - 6.0F,  Size().y - 27.0F });
	}

	void SceneView::Shutdown()
	{
		mpGfxSys = nullptr;
		mpInputSys = nullptr;
		mpColSys = nullptr;
		mpPhysSys = nullptr;
	}

	std::string SceneView::GetLabel() const
	{
		return mLabel;
	}

}


#endif //EDITOR


