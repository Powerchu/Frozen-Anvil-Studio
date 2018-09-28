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

#include "System\Scene\Scene.h"
#include "System\Graphics\Texture2D.h"
#include "Component\Camera.h"

// #include "System/Scene/Scene.h"
// #include "System/Graphics/Texture2D.h"
// #include "Component/Camera.h"
// #include "System/Camera/CameraSystem.h" 


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
		mpSceneCamera{ nullptr }

	{}

	SceneView::~SceneView()
	{
		gpInstance = nullptr;
	}

	void SceneView::Init()
	{
		mpGfxSys = EngineCore::GetInstance()->GetSystem<GraphicsSystem>();

		// Scene Camera
		GameObject *p = Factory::CreateCamera("Scene Camera");
		mpSceneCamera = GetCurrentScene()->InsertGameObject(Utility::Move(*p));
		mpSceneCamera->GetComponent<Camera>()->Init();
		mpSceneCamera->GetComponent<Transform>()->SetScale(Math::Vec3D{ 1.f, 1.f, 1.f });
		delete p;
	}

	void SceneView::Update(const float& _dt)
	{
		mDelta = _dt;
		if (GetMainEditor().CurrentState() != EDITOR_PLAY)
		{
			mpGfxSys->Update(mDelta);
			//if (mpInputSys->IsKeyPressed(eUserButton::BUTTON_SPACEBAR))
			//{
			//	mpBoxObject->GetComponent<RigidBody>()->AddForce({0.0f,20.0F,0});
			//}
			//if (mpInputSys->IsKeyPressed(eUserButton::BUTTON_LEFT))
			//{
			//	mpBoxObject->GetComponent<RigidBody>()->AddForce({ -10.0f,0.0F,0 });
			//}
			//if (mpInputSys->IsKeyPressed(eUserButton::BUTTON_RIGHT))
			//{
			//	mpBoxObject->GetComponent<RigidBody>()->AddForce({ 10.0f,0.0F,0 });
			//}

			//if (mpInputSys->IsKeyTriggered(eUserButton::MOUSE_M))
			//{
			//	//mpCircleObject->GetComponent<Transform>()->SetGlobalPosition({ 0,0,0,0 });
			//	//mpCircleObject->GetComponent<RigidBody>()->SetVelocity({ 0,0,0,0 });
			//	mpBoxObject->GetComponent<Transform>()->SetGlobalPosition({ 0,0,0,0 });
			//	mpBoxObject->GetComponent<RigidBody>()->SetVelocity({ 0,0,0,0 });
			//}

			//if (mpInputSys->IsKeyTriggered(eUserButton::MOUSE_R))
			//{
			//	mpBoxObject2->GetComponent<Transform>()->SetGlobalPosition({ 0,0,0,0 });
			//	mpBoxObject2->GetComponent<RigidBody>()->SetVelocity({ 0,0,0,0 });
			//}

			//if (mpInputSys->IsKeyTriggered(eUserButton::BUTTON_PAUSE))
			//{
			//	mpGfxSys->ToggleDebugDraw();
			//}
		}
	}

	void SceneView::EditorUI()
	{
		const uint64_t id = mpGfxSys->GetFrameBuffer().AsTexture()->GetID();
		ImGui::Image(reinterpret_cast<void*>(id), ImVec2{ Size().x - 6.0F,  Size().y - 27.0F });
	}

	void SceneView::Shutdown()
	{
		mpGfxSys = nullptr;
	}

	std::string SceneView::GetLabel() const
	{
		return mLabel;
	}

}


#endif //EDITOR


