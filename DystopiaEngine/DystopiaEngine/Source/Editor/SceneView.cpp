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
#include "Editor\SceneView.h"
#include "Editor\Editor.h"
#include "Editor\EGUI.h"
#include "Editor\DefaultFactory.h"

#include "System\Driver\Driver.h"
#include "System\Graphics\GraphicsSystem.h"
#include "System\Camera\CameraSystem.h"
#include <System/Physics/PhysicsSystem.h>
#include "System\Scene\Scene.h"
#include "System\Graphics\Texture2D.h"
#include "Component\Camera.h"

#include <Component/RigidBody.h>

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
		mpGfxSys{ nullptr },
		mDelta{},
		mpSceneCamera{ nullptr },
		mpPhysSys{nullptr}
	{}

	SceneView::~SceneView()
	{
		gpInstance = nullptr;
	}

	void SceneView::Init()
	{
		mpGfxSys = EngineCore::GetInstance()->GetSystem<GraphicsSystem>();
		mpPhysSys = EngineCore::GetInstance()->GetSystem<PhysicsSystem>();

		// Scene Camera
		GameObject *p = Factory::CreateCamera("Scene Camera");
		mpSceneCamera = GetCurrentScene()->InsertGameObject(Utility::Move(*p));
		mpSceneCamera->GetComponent<Camera>()->Init();
		mpSceneCamera->GetComponent<Transform>()->SetScale(Math::Vec4{ 1.f, 1.f, 1.f });
		delete p;

		// Sample Box Object
		GameObject *b = Factory::CreateBox("Box Object");
		mpBoxObject = GetCurrentScene()->InsertGameObject(Utility::Move(*b));
		mpBoxObject->GetComponent<RigidBody>()->Init();
		mpBoxObject->GetComponent<Transform>()->SetScale(Math::Vec4{ 128.f, 128.f, 128.f });
		delete b;
	}

	void SceneView::Update(const float& _dt)
	{
		mDelta = _dt;
		mpGfxSys->Update(mDelta);
		mpPhysSys->Update(mDelta);
	}

	void SceneView::EditorUI()
	{
		unsigned id = mpGfxSys->GetFrameBuffer().AsTexture()->GetID();
		ImGui::Image(reinterpret_cast<void*>(id), ImVec2{ Size().x - 6.0F,  Size().y - 27.0F });
	}

	void SceneView::Shutdown()
	{
		mpGfxSys = nullptr;
		mpPhysSys = nullptr;
	}

	std::string SceneView::GetLabel() const
	{
		return mLabel;
	}

}


#endif //EDITOR


