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
#include "System/Input/InputSystem.h"
#include "System/Physics/PhysicsSystem.h"
#include "System/Collision/CollisionSystem.h"
#include "Component/Collider.h"

#include "System\Scene\Scene.h"
#include "System\Graphics\Texture2D.h"
#include "Component\Camera.h"

// #include "System/Scene/Scene.h"
// #include "System/Graphics/Texture2D.h"
// #include "Component/Camera.h"
// #include "System/Camera/CameraSystem.h" 

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
		//mpColSys = EngineCore::GetInstance()->GetSystem<CollisionSystem>();
		mpInputSys = EngineCore::GetInstance()->GetSystem<InputManager>();


		// Scene Camera
		GameObject *p = Factory::CreateCamera("Scene Camera");
		mpSceneCamera = GetCurrentScene()->InsertGameObject(Utility::Move(*p));
		mpSceneCamera->GetComponent<Camera>()->Init();
		mpSceneCamera->GetComponent<Transform>()->SetScale(Math::Vec3D{ 1.f, 1.f, 1.f });
		delete p;

		// Sample Box Object
		GameObject *b = Factory::CreateBox("Box Object");
		Texture2D* _bt = new Texture2D{ "Resource/Editor/red_box.png" };
		mpBoxObject = GetCurrentScene()->InsertGameObject(Utility::Move(*b));
		mpBoxObject->GetComponent<RigidBody>()->Init();
		mpBoxObject->GetComponent<RigidBody>()->SetRestitution(1.0F);
		mpBoxObject->GetComponent<RigidBody>()->SetStaticFriction(0.6F);
		mpBoxObject->GetComponent<RigidBody>()->SetKineticFriction(0.6F);
		mpBoxObject->GetComponent<Transform>()->SetScale(Math::Vec4{ 64.f, 64.f, 1.f });
		mpBoxObject->GetComponent<Renderer>()->SetTexture(_bt);
		mpBoxObject->GetComponent<Collider>()->Init();
		mpBoxObject->GetComponent<RigidBody>()->SetStaticFriction(0.3F);
		delete b;

		//// Sample Box Object
		//GameObject *bb = Factory::CreateBox("Another box");
		//Texture2D* _bbt = new Texture2D{ "Resource/Editor/red_box.png" };
		//mpBoxObject2 = GetCurrentScene()->InsertGameObject(Utility::Move(*bb));
		//mpBoxObject2->GetComponent<RigidBody>()->Init();
		//mpBoxObject2->GetComponent<Transform>()->SetScale(Math::Vec4{ 64.f, 64.f, 1.f });
		//mpBoxObject2->GetComponent<Transform>()->SetGlobalPosition({ 0.0F, 200.0f, 1 });
		//mpBoxObject2->GetComponent<Renderer>()->SetTexture(_bbt);
		//mpBoxObject2->GetComponent<Collider>()->Init();
		//mpBoxObject2->GetComponent<RigidBody>()->SetStaticFriction(0.3F);
		//delete bb;

		//// Sample Circle Object
		//GameObject *c = Factory::CreateCircle("Circle Object");
		//Texture2D* _ct = new Texture2D{ "Resource/Editor/red_circle.png" };
		//mpCircleObject = GetCurrentScene()->InsertGameObject(Utility::Move(*c));
		//mpCircleObject->GetComponent<RigidBody>()->Init();
		//mpCircleObject->GetComponent<RigidBody>()->SetRestitution(1.0F);
		//mpCircleObject->GetComponent<RigidBody>()->SetStaticFriction(0.3F);
		//mpCircleObject->GetComponent<RigidBody>()->SetKineticFriction(0.3F);
		//mpCircleObject->GetComponent<Transform>()->SetGlobalPosition({ 0.0F, 150.0f, 0});
		//mpCircleObject->GetComponent<Transform>()->SetScale(Math::Vec3D{ 100, 100.f, 1.f });
		//mpCircleObject->GetComponent<Renderer>()->SetTexture(_ct);
		//mpCircleObject->GetComponent<Collider>()->Init();
		//delete c;

		//GameObject *cc = Factory::CreateCircle("Static Circle");
		//mpStaticCircleObject = GetCurrentScene()->InsertGameObject(Utility::Move(*cc));
		//mpStaticCircleObject->GetComponent<RigidBody>()->Init();
		//mpStaticCircleObject->GetComponent<RigidBody>()->Set_IsStatic(true);
		//mpStaticCircleObject->GetComponent<RigidBody>()->SetRestitution(1.0F);
		//mpStaticCircleObject->GetComponent<RigidBody>()->SetStaticFriction(0.3F);
		//mpStaticCircleObject->GetComponent<RigidBody>()->SetKineticFriction(0.3F);
		//mpStaticCircleObject->GetComponent<Transform>()->SetGlobalPosition({ 0.0f, -185.f, 0});
		//mpStaticCircleObject->GetComponent<Transform>()->SetScale(Math::Vec3D{ 100.f, 100.f, 1.f });
		//mpStaticCircleObject->GetComponent<Renderer>()->SetTexture(_ct);
		//mpStaticCircleObject->GetComponent<Collider>()->Init();
		//delete cc;

		// Sample Static Object
		GameObject *s = Factory::CreateStaticBox("Static Box");
		Texture2D* _st = new Texture2D{ "Resource/Editor/white_box.png" };
		mpStaticBoxObject = GetCurrentScene()->InsertGameObject(Utility::Move(*s));
		mpStaticBoxObject->GetComponent<RigidBody>()->Init();
		mpStaticBoxObject->GetComponent<RigidBody>()->Set_IsStatic(true);
		mpStaticBoxObject->GetComponent<RigidBody>()->SetRestitution(1.0F);
		mpStaticBoxObject->GetComponent<RigidBody>()->SetStaticFriction(0.7F);
		mpStaticBoxObject->GetComponent<RigidBody>()->SetKineticFriction(0.5F);
		mpStaticBoxObject->GetComponent<Transform>()->SetGlobalPosition ({ 0, -185.f, 1 });
		mpStaticBoxObject->GetComponent<Transform>()->SetScale(Math::Vec3D{ 2048.f, 32.f, 1.f });
		mpStaticBoxObject->GetComponent<Collider>()->Init();
		mpStaticBoxObject->GetComponent<Renderer>()->SetTexture(_st);
		delete s;

		s = Factory::CreateStaticBox("Right");
		mpStaticBoxObject = GetCurrentScene()->InsertGameObject(Utility::Move(*s));
		mpStaticBoxObject->GetComponent<RigidBody>()->Init();
		mpStaticBoxObject->GetComponent<RigidBody>()->Set_IsStatic(true);
		mpStaticBoxObject->GetComponent<RigidBody>()->SetRestitution(1.0F);
		mpStaticBoxObject->GetComponent<RigidBody>()->SetStaticFriction(0.7F);
		mpStaticBoxObject->GetComponent<RigidBody>()->SetKineticFriction(0.5F);
		mpStaticBoxObject->GetComponent<Transform>()->SetGlobalPosition({ -200.0F, 0.0F, 1 });
		mpStaticBoxObject->GetComponent<Transform>()->SetScale(Math::Vec3D{ 32.f, 256.f, 1.f });
		mpStaticBoxObject->GetComponent<Collider>()->Init();
		mpStaticBoxObject->GetComponent<Renderer>()->SetTexture(_st);
		delete s;

		s = Factory::CreateStaticBox("Top");
		mpStaticBoxObject = GetCurrentScene()->InsertGameObject(Utility::Move(*s));
		mpStaticBoxObject->GetComponent<RigidBody>()->Init();
		mpStaticBoxObject->GetComponent<RigidBody>()->Set_IsStatic(true);
		mpStaticBoxObject->GetComponent<RigidBody>()->SetRestitution(1.0F);
		mpStaticBoxObject->GetComponent<RigidBody>()->SetStaticFriction(0.7F);
		mpStaticBoxObject->GetComponent<RigidBody>()->SetKineticFriction(0.5F);
		mpStaticBoxObject->GetComponent<Transform>()->SetGlobalPosition({ 0.0F, 64.0F, 1 });
		mpStaticBoxObject->GetComponent<Transform>()->SetScale(Math::Vec3D{ 256.f, 32.f, 1.f });
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
			mpPhysSys->FixedUpdate(mDelta);
			//mpColSys->FixedUpdate(mDelta);

			if (mpInputSys->IsKeyPressed(eUserButton::BUTTON_SPACEBAR))
			{
				mpBoxObject->GetComponent<RigidBody>()->AddForce({0.0f,20.0F,0});
			}
			if (mpInputSys->IsKeyPressed(eUserButton::BUTTON_LEFT))
			{
				mpBoxObject->GetComponent<RigidBody>()->AddForce({ -10.0f,0.0F,0 });
			}
			if (mpInputSys->IsKeyPressed(eUserButton::BUTTON_RIGHT))
			{
				mpBoxObject->GetComponent<RigidBody>()->AddForce({ 10.0f,0.0F,0 });
			}

			if (mpInputSys->IsKeyTriggered(eUserButton::MOUSE_M))
			{
				//mpCircleObject->GetComponent<Transform>()->SetGlobalPosition({ 0,0,0,0 });
				//mpCircleObject->GetComponent<RigidBody>()->SetVelocity({ 0,0,0,0 });
				mpBoxObject->GetComponent<Transform>()->SetGlobalPosition({ 0,0,0,0 });
				mpBoxObject->GetComponent<RigidBody>()->SetVelocity({ 0,0,0,0 });
			}

			if (mpInputSys->IsKeyTriggered(eUserButton::MOUSE_R))
			{
				mpBoxObject2->GetComponent<Transform>()->SetGlobalPosition({ 0,0,0,0 });
				mpBoxObject2->GetComponent<RigidBody>()->SetVelocity({ 0,0,0,0 });
			}

			if (mpInputSys->IsKeyTriggered(eUserButton::BUTTON_PAUSE))
			{
				mpGfxSys->ToggleDebugDraw();
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


