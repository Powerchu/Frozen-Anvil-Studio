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
#include "Editor/EGUI.h"
#include "Editor/Editor.h"
#include "Editor/EditorEvents.h"
#include "Editor/EditorInputs.h"
#include "Editor/ConsoleLog.h"
#include "Editor/DefaultFactory.h"
#include "Editor/Payloads.h"

#include "System/Driver/Driver.h"
#include "System/Scene/Scene.h"
#include "System/Graphics/Texture2D.h"
#include "System/Camera/CameraSystem.h"
#include "System/Graphics/GraphicsSystem.h"

#include "Object/GameObject.h"
#include "Component/Transform.h"
#include "Component/Camera.h"
#include "Component/Renderer.h"
#include "Behaviour/Behaviour.h"

#include "Math/MathUtility.h"

namespace Dystopia
{
	static constexpr float imageOffsetY = 27.f;
	static constexpr float dragMagnitudeEpsilon = 2.f;

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
		mpGfxSys{ nullptr }, mDelta{},
		mpSceneCamera{ nullptr },
		mSensitivity{ 0.1f },
		mpEditorInput{ nullptr },
		mAmFocused{ false }, mMoveVec{ 0,0 },
		mMoveSens{ 0.75f }, mDragging{ false }
	{}

	SceneView::~SceneView()
	{
		gpInstance = nullptr;
	}

	void SceneView::Init()
	{
		mpEditorInput = GetMainEditor().GetEditorInput();
		mpGfxSys = EngineCore::GetInstance()->GetSystem<GraphicsSystem>();
		GetEditorEventHND()->GetEvent(EDITOR_SCENE_CHANGED)->Bind(&SceneView::SceneChanged, this);
		GetEditorEventHND()->GetEvent(EDITOR_SCROLL_UP)->Bind(&SceneView::ScrollIn, this);
		GetEditorEventHND()->GetEvent(EDITOR_SCROLL_DOWN)->Bind(&SceneView::ScrollOut, this);

		//mpSceneCamera = Factory::CreateCamera("Scene Camera");
		//GetCurrentScene()->GetAllGameObjects().EmplaceBack(Utility::Move(*mpSceneCamera));
		//auto& g = GetCurrentScene()->GetAllGameObjects().back();
		//g.GetComponent<Transform>()->SetOwner(&g);
		//for (auto& c : g.GetAllComponents())
		//{
		//	c->SetOwner(&g);
		//	c->Init();
		//}
		//for (auto& b : g.GetAllBehaviours())
		//{
		//	b->SetOwner(&g);
		//	b->Init();
		//}
		//delete mpSceneCamera;
		//mpSceneCamera = nullptr;
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
		ResetMouseEvents();
		EGUI::UnIndent(2);

		if (GetMainEditor().CurrentState() == EDITOR_PLAY)
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 1.f);

		size_t id = mpGfxSys->GetFrameBuffer().AsTexture()->GetID();
		mImgSize = Math::Vec2{ Size().x,  Size().y - imageOffsetY };

		if (EGUI::Display::Image(id, mImgSize, true))
			mAmFocused = true;

		if (File *t = EGUI::Display::StartPayloadReceiver<Dystopia::File>(EGUI::PREFAB))
		{
			AcceptPrefab(t);
			EGUI::Display::EndPayloadReceiver();
		}
		if (File *t = EGUI::Display::StartPayloadReceiver<Dystopia::File>(EGUI::PNG))
		{
			AcceptTexture(t);
			EGUI::Display::EndPayloadReceiver();
		}

		if (GetMainEditor().CurrentState() == EDITOR_PLAY)
			ImGui::PopStyleVar();

		CheckMouseEvents();
		EGUI::Indent(2);
	}

	void SceneView::Shutdown()
	{
		mpGfxSys = nullptr;
		GetEditorEventHND()->GetEvent(EDITOR_SCENE_CHANGED)->Unbind(this);
		GetEditorEventHND()->GetEvent(EDITOR_SCROLL_UP)->Unbind(this);
		GetEditorEventHND()->GetEvent(EDITOR_SCROLL_DOWN)->Unbind(this);
	}

	std::string SceneView::GetLabel() const
	{
		return mLabel;
	}

	void SceneView::Move()
	{
		if (mpSceneCamera)
		{
			Math::Vec2 vDest	= FindMouseVector();
			Math::Vec2 vDir		= vDest - mMoveVec;

			if (vDir.MagnitudeSqr() < dragMagnitudeEpsilon) 
				return;

			Math::Point3D pos	= mpSceneCamera->GetComponent<Transform>()->GetPosition();
			pos.x				= pos.x - mMoveSens * vDir.x;
			pos.y				= pos.y - mMoveSens * vDir.y;

			mpSceneCamera->GetComponent<Transform>()->SetPosition(pos);
			mMoveVec = FindMouseVector();
			PrintToConsoleLog("Moving Vector: [" + std::to_string(pos.x) + "] / [" + std::to_string(pos.y) + "]");
		}
	}

	GameObject* SceneView::FindMouseObject()
	{
		if (Camera* pCam = GetCamera())
		{
			Math::Pt3D worldClickPos = GetWorldClickPos(pCam);

			GameObject* pTarget = nullptr;
			float vLength = FLT_MAX;
			for (auto& obj : GetCurrentScene()->GetAllGameObjects())
			{
				if (&obj == mpSceneCamera) continue;

				Math::Pt3D objPos = obj.GetComponent<Transform>()->GetGlobalPosition();
				Math::Vec4 distV = objPos - worldClickPos;
				Math::Vec2 flatVec = { distV.x, distV.y };

				const auto& trans = obj.GetComponent<Transform>();
				Math::Vec4 scale = trans->GetGlobalScale();
				Math::Pt3D pos = trans->GetGlobalPosition();

				if (worldClickPos.x >= (pos.x - (scale.x / 2)) &&
					worldClickPos.x <= (pos.x + (scale.x / 2)) &&
					worldClickPos.y >= (pos.y - (scale.y / 2)) &&
					worldClickPos.y <= (pos.y + (scale.y / 2)))
				{
					float lSquared = flatVec.MagnitudeSqr();
					if (lSquared < vLength)
					{
						vLength = lSquared;
						pTarget = &obj;
					}
				}
			}
			if (pTarget)
			{
				GetMainEditor().SetFocus(*pTarget);
				PrintToConsoleLog("HitPoint (Screen To world) : [" + pTarget->GetName() + "]");
				return pTarget;
			}
		}
		return nullptr;
	}

	Math::Vec2 SceneView::FindMouseVector()
	{
		if (Camera* pCam = GetCamera())
		{
			if (Transform* pTrans = mpSceneCamera->GetComponent<Transform>())
			{
				Math::Pt3D worldClickPos = GetWorldClickPos(pCam);
				Math::Vec2 v{ worldClickPos.x - pTrans->GetGlobalPosition().x,
							  worldClickPos.y - pTrans->GetGlobalPosition().y };
				return v;
			}
		}
		return Math::Vec2{ 0,0 };
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
		float s = _in ? 1 - mSensitivity : 1 + mSensitivity;
		Math::Vec4 newScalep = pObjTransform->GetScale() * s;
		pObjTransform->SetScale(newScalep);
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

	Math::Pt3D SceneView::GetWorldClickPos(const Camera * const _cam) const
	{
		Math::Vec2 mousePos = ImGui::GetMousePos();
		Math::Vec2 relPos = mousePos - ImGui::GetItemRectMin();
		auto viewport = EngineCore::GetInstance()->GetSystem<CameraSystem>()->GetMasterViewport();
		Math::Vec2 hitPoint{ (relPos.x / mImgSize.x) * viewport.mnWidth,
							 (relPos.y / mImgSize.y) * viewport.mnHeight };
		auto worldPos = _cam->ScreenToWorld(Math::Vec3D{ hitPoint.x - (viewport.mnWidth / 2),
														 hitPoint.y - (viewport.mnHeight / 2),
														 0.f, 1.f });
		return worldPos;
	}

	Camera* SceneView::GetCamera()
	{
		if (!mpSceneCamera)
		{
			PrintToConsoleLog("Camera GameObject is NULL in SceneView");
			return nullptr;
		}

		Camera* pCam = mpSceneCamera->GetComponent<Camera>();
		if (!pCam)
		{
			PrintToConsoleLog("Camera is NULL in SceneView");
			return nullptr;
		}
		return pCam;
	}

	void SceneView::ResetMouseEvents()
	{
		if (ImGui::IsMouseClicked(0))
			mAmFocused = false;
		if (mDragging && !ImGui::IsMouseDown(1))
			mDragging = false;

	}

	void SceneView::CheckMouseEvents()
	{
		if (ImGui::IsItemHovered())
		{
			if (ImGui::IsMouseClicked(0))	FindMouseObject();
			if (ImGui::IsMouseClicked(1))
			{
				mDragging = true;
				mMoveVec = FindMouseVector();
			}
		}

		if (mAmFocused || mDragging)
		{
			if (mDragging)				Move();
			if (mToZoom != eZOOM_NONE)  Zoom(eZOOM_IN == mToZoom);
		}
		mToZoom = eZOOM_NONE;
	}

	void SceneView::AcceptPrefab(File* /*_pFile*/)
	{
		if (Camera* pCam = GetCamera())
		{
			GameObject* pTarget			= FindMouseObject();
			float betterZ				= (pTarget) ? pTarget->GetComponent<Transform>()->GetPosition().z + 0.1f : 0;
			Math::Pt3D worldClickPos	= GetWorldClickPos(pCam);
			Math::Pt3D spawnSite		= Math::Pt3D{ worldClickPos.x, worldClickPos.y, betterZ };

			/* TODO: Unserialise a prefab file */
		}
	}

	void SceneView::AcceptTexture(File* _pFile)
	{
		if (Camera* pCam = GetCamera())
		{
			GameObject* pTarget			= FindMouseObject();
			Renderer *pRend = nullptr;
			if (pTarget && (pRend = pTarget->GetComponent<Renderer>()))
			{
				auto fOld = EGUI::GetCommandHND()->Make_FunctionModWrapper(&Dystopia::Renderer::SetTexture, 
																			pRend->GetTexture());
				auto fNew = EGUI::GetCommandHND()->Make_FunctionModWrapper(&Dystopia::Renderer::SetTexture, 
																			mpGfxSys->LoadTexture(_pFile->mPath));
				GetCommandHND()->InvokeCommand(pTarget->GetID(), fOld, fNew);
			}
		}
	}

}


#endif //EDITOR


