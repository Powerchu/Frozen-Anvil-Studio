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
#include "Editor/EditorEvents.h"
#include "Editor/ConsoleLog.h"

#include "System/Driver/Driver.h"
#include "System/Graphics/GraphicsSystem.h"
#include "System/Scene/Scene.h"
#include "System/Graphics/Texture2D.h"

#include "Component/Transform.h"
#include "Component/Camera.h"

namespace Dystopia
{
	static constexpr float imageOffsetX = 6.f;
	static constexpr float imageOffsetY = 27.f;

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
		mSensitivity{ 1.f }
	{}

	SceneView::~SceneView()
	{
		GetEditorEventHND()->GetEvent(EDITOR_SCENE_CHANGED)->Unbind(this);
		GetEditorEventHND()->GetEvent(EDITOR_SCROLL_UP)->Unbind(this);
		GetEditorEventHND()->GetEvent(EDITOR_SCROLL_DOWN)->Unbind(this);
		gpInstance = nullptr;
	}

	void SceneView::Init()
	{
		mpGfxSys = EngineCore::GetInstance()->GetSystem<GraphicsSystem>();
		GetEditorEventHND()->GetEvent(EDITOR_SCENE_CHANGED)->Bind(&SceneView::SceneChanged, this);
		GetEditorEventHND()->GetEvent(EDITOR_SCROLL_UP)->Bind(&SceneView::ScrollIn, this);
		GetEditorEventHND()->GetEvent(EDITOR_SCROLL_DOWN)->Bind(&SceneView::ScrollOut, this);

		// Scene Camera
		//GameObject *p = Factory::CreateCamera("Scene Camera");
		//mpSceneCamera = GetCurrentScene()->InsertGameObject(Utility::Move(*p));
		//mpSceneCamera->GetComponent<Camera>()->Init();
		//mpSceneCamera->GetComponent<Transform>()->SetScale(Math::Vec4{ 1.f, 1.f, 1.f });
		//delete p;

		SceneChanged();
	}

	void SceneView::Update(const float& _dt)
	{
		mDelta = _dt;
		if (GetMainEditor().CurrentState() == EDITOR_MAIN)
		{
			mpGfxSys->Update(mDelta);
		}
	}

	void SceneView::EditorUI()
	{
		size_t id = mpGfxSys->GetFrameBuffer().AsTexture()->GetID();
		ImVec2 size{ Size().x - imageOffsetX,  Size().y - imageOffsetY };
		ImGui::Image(reinterpret_cast<void*>(id), size);
		if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
		{
			FindMouseObject(Math::Vec2{ size.x, size.y });
		}
	}

	void SceneView::Shutdown()
	{
		mpGfxSys = nullptr;
	}

	std::string SceneView::GetLabel() const
	{
		return mLabel;
	}

	void SceneView::FindMouseObject(const Math::Vec2& _imgSize)
	{
		if (!mpSceneCamera)
		{
			PrintToConsoleLog("Camera GameObject is NULL in SceneView::FindMouseObject()");
			return;
		}
		Camera* pCam = mpSceneCamera->GetComponent<Camera>();
		if (!pCam)
		{
			PrintToConsoleLog("Camera is NULL in SceneView::FindMouseObject()");
			return;
		}

		Math::Vec2 relPosToWindow{ ImGui::GetCursorPos().x, ImGui::GetCursorPos().y };
		float xRatio = relPosToWindow.x / _imgSize.x;
		float yRatio = relPosToWindow.y / _imgSize.y;
		float xNew = pCam->GetViewport().mnWidth * xRatio;
		float yNew = pCam->GetViewport().mnWidth * yRatio;
		Math::Pt3D worldPos = pCam->ScreenToWorld(Math::Vec3D{ xNew, yNew, 0.f });
		PrintToConsoleLog("World Pos From Scene View : " + std::to_string(worldPos.x) +" / " + std::to_string(worldPos.y));
	}
	
	void SceneView::ScrollIn()
	{
		if (!mpSceneCamera)
		{
			PrintToConsoleLog("Camera GameObject is NULL in SceneView::ScrollIn()");
			return;
		}
		Transform *pObjTransform = mpSceneCamera->GetComponent<Transform>();
		if (!pObjTransform)
		{
			PrintToConsoleLog("Camera Transform is NULL in SceneView::ScrollIn()");
			return;
		}
		pObjTransform->SetScale( pObjTransform->GetScale() - 
								Math::Vec4{ mSensitivity, mSensitivity, 0.f });
	}

	void SceneView::ScrollOut()
	{
		if (!mpSceneCamera)
		{
			PrintToConsoleLog("Camera GameObject is NULL in SceneView::ScrollOut()");
			return;
		}
		Transform *pObjTransform = mpSceneCamera->GetComponent<Transform>();
		if (!pObjTransform)
		{
			PrintToConsoleLog("Camera Transform is NULL in SceneView::ScrollOut()");
			return;
		}
		pObjTransform->SetScale(pObjTransform->GetScale() +
								Math::Vec4{ mSensitivity, mSensitivity, 0.f });
	}

	void SceneView::SetSensitivity(float _amt)
	{
		mSensitivity = _amt;
	}

	float SceneView::GetSensitivity() const
	{
		return mSensitivity;
	}

	void SceneView::SceneChanged()
	{
		mpSceneCamera = GetCurrentScene()->FindGameObject("Scene Camera");
	}

}


#endif //EDITOR


