/* HEADER *********************************************************************************/
/*!
\file	SceneView.cpp
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief
BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#include "Editor/SceneView.h"
#include "Editor/EGUI.h"
#include "Editor/Editor.h"
#include "Editor/EditorEvents.h"
#include "Editor/EditorInputs.h"
#include "Editor/ConsoleLog.h"
#include "Editor/DefaultFactory.h"

#include "System/Driver/Driver.h"
#include "System/Scene/Scene.h"
#include "System/Graphics/Texture2D.h"
#include "System/Camera/CameraSystem.h"
#include "System/Graphics/GraphicsSystem.h"

#include "Object/GameObject.h"
#include "Component/Transform.h"
#include "Component/Camera.h"

#include "Math/MathUtility.h"

namespace Dystopia
{
	static constexpr float imageOffsetY = 27.f;

	static SceneView* gpInstance = 0;
	SceneView* SceneView::GetInstance()
	{
		if (gpInstance) return gpInstance;

		gpInstance = new SceneView{};
		return gpInstance;
	}

	SceneView::SceneView()
		: EditorTab{ false },
		mLabel{ "Scene View" },
		mpGfxSys{ nullptr },
		mDelta{},
		mpSceneCamera{ nullptr },
		mSensitivity{ 0.1f },
		mpEditorInput{ nullptr }
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
		mpEditorInput = GetMainEditor().GetEditorInput();
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
		EGUI::UnIndent(2);
		size_t id = mpGfxSys->GetFrameBuffer().AsTexture()->GetID();
		ImVec2 size{ Size().x,  Size().y - imageOffsetY };
		ImGui::Image(reinterpret_cast<void*>(id), size);
		if (ImGui::IsItemHovered())
		{
			if (mToZoom != eZOOM_NONE)		
				Zoom(eZOOM_IN == mToZoom);
			if (ImGui::IsMouseClicked(0))	
				FindMouseObject(size);
		}
		 if (ImGui::IsItemFocused())		
			Move();
		EGUI::Indent(2);
	}

	void SceneView::Shutdown()
	{
		mpGfxSys = nullptr;
	}

	std::string SceneView::GetLabel() const
	{
		return mLabel;
	}

	void SceneView::Move()
	{
		if (mpSceneCamera && !mpEditorInput->IsKeyPressed(KEY_CTRL))
		{
			Math::Point3D newPos = mpSceneCamera->GetComponent<Transform>()->GetPosition();
			if (mpEditorInput->IsKeyPressed(KEY_W))
			{
				newPos.y = newPos.y + mSensitivity;
			}
			if (mpEditorInput->IsKeyPressed(KEY_A))
			{
				newPos.x = newPos.x - mSensitivity;
				}
			if (mpEditorInput->IsKeyPressed(KEY_S))
			{
				newPos.y = newPos.y - mSensitivity;
			}
			if (mpEditorInput->IsKeyPressed(KEY_D))
			{
				newPos.x = newPos.x + mSensitivity;
			}
			mpSceneCamera->GetComponent<Transform>()->SetPosition(newPos);
		}
	}

	void SceneView::FindMouseObject(const Math::Vec2& _imgSize)
	{
		if (!mpSceneCamera)
		{
			PrintToConsoleLog("Camera GameObject is NULL in SceneView::FindMouseObject()");
			return;
		}

		Camera* pCam = mpSceneCamera->GetComponent<Camera>();
		if (nullptr == pCam)
		{
			PrintToConsoleLog("Camera is NULL in SceneView::FindMouseObject()");
			return;
		}

		Math::Pt3D worldClickPos = GetWorldClickPos(pCam, _imgSize);
		PrintToConsoleLog("HitPoint (Screen To world) : [" + std::to_string(worldClickPos.x) + "] / [" + std::to_string(worldClickPos.y) + "]");

		const GameObject* pTarget = nullptr;
		float vLength = FLT_MAX;
		for (const auto& obj : GetCurrentScene()->GetAllGameObjects())
		{
			if (&obj == mpSceneCamera) continue;

			Math::Pt3D objPos	= obj.GetComponent<Transform>()->GetGlobalPosition();
			Math::Vec4 distV	= objPos - worldClickPos;
			Math::Vec2 flatVec	= { distV.x, distV.y };
			float lSquared = flatVec.MagnitudeSqr();
			if (lSquared < vLength)
			{
				vLength = lSquared;
				pTarget = &obj;
			}
		}

		if (pTarget)
		{
			const auto& trans	= pTarget->GetComponent<Transform>();
			Math::Vec4 scale	= trans->GetGlobalScale();
			Math::Pt3D pos		= trans->GetGlobalPosition();
			if (worldClickPos.x >= (pos.x - (scale.x / 2)) &&
				worldClickPos.x <= (pos.x + (scale.x / 2)) &&
				worldClickPos.y >= (pos.y - (scale.y / 2)) &&
				worldClickPos.y <= (pos.y + (scale.y / 2)))
			{
				PrintToConsoleLog("Clicked on a GameObject from scene view!");
			}
		}
	}

	void SceneView::Zoom(bool _in)
	{
		mToZoom = eZOOM_NONE;
		if (!mpSceneCamera)
		{
			PrintToConsoleLog("Camera GameObject is NULL in SceneView::Zoom");
			return;
		}

		Transform *pObjTransform = mpSceneCamera->GetComponent<Transform>();
		if (!pObjTransform)
		{
			PrintToConsoleLog("Camera Transform is NULL in SceneView::Zoom");
			return;
		}

		Math::Vec4 movement{ mSensitivity, mSensitivity, 0.f };
		movement = _in ? movement * -1 : movement;
		pObjTransform->SetScale(pObjTransform->GetScale() + movement);
	}
	
	void SceneView::ScrollIn()
	{
		mToZoom = eZOOM_IN;
	}

	void SceneView::ScrollOut()
	{
		mToZoom = eZOOM_OUT;
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

	Math::Pt3D SceneView::GetWorldClickPos(const Camera * const _cam, const Math::Vec2& _imgSize) const
	{
		Math::Vec2 mousePos = ImGui::GetMousePos();
		Math::Vec2 relPos = mousePos - ImGui::GetItemRectMin();
		auto viewport = EngineCore::GetInstance()->GetSystem<CameraSystem>()->GetMasterViewport();
		Math::Vec2 hitPoint{ (relPos.x / _imgSize.x) * viewport.mnWidth,
							 (relPos.y / _imgSize.y) * viewport.mnHeight };
		auto worldPos = _cam->ScreenToWorld(Math::Vec3D{ hitPoint.x - (viewport.mnWidth / 2),
													     hitPoint.y - (viewport.mnHeight / 2), 
														 0.f });
		return worldPos;
	}

}


#endif //EDITOR


