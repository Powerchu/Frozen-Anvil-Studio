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

#include "Utility/DebugAssert.h"
#include "Object/GameObject.h"
#include "Component/Transform.h"
#include "Component/Camera.h"
#include "Component/Renderer.h"
#include "Behaviour/Behaviour.h"

#include "Math/MathUtility.h"

namespace Dystopia
{
	static const Math::Vec4 redColor = Math::Vec4{ 1.f,0.f,0.f,1.f };
	static const Math::Vec4 greenColor = Math::Vec4{ 0.f,1.f,0.f,1.f };
	static const Math::Vec4 blueColor = Math::Vec4{ 0.f,0.f,1.f,1.f };
	static constexpr float dragMagnitudeEpsilon = 2.f;

	static SceneView* gpInstance = 0;
	SceneView* SceneView::GetInstance()
	{
		if (gpInstance) return gpInstance;

		gpInstance = new SceneView{};
		return gpInstance;
	}

	SceneView::SceneView()
		: EditorTab{ false, true },
		mLabel{ "Scene View" }, mpGfxSys{ nullptr }, 
		mpSceneCamera{ nullptr }, mSensitivity{ 0.1f },
		mToZoom{ eZOOM_NONE }, mAmFocused{ false }, 
		mMoveVec{ 0,0 }, mMoveSens{ 0.75f }, 
		mDragging{ false }, mImgPos{}, mImgSize{},
		mClearSelection{ false },
		mGizmoHovered{ false }, mCurrGizTool{ eTRANSLATE }
	{}

	SceneView::~SceneView()
	{
		gpInstance = nullptr;
	}

	void SceneView::Init()
	{
		mpGfxSys = EngineCore::GetInstance()->GetSystem<GraphicsSystem>();
		GetEditorEventHND()->GetEvent(EDITOR_SCENE_CHANGED)->Bind(&SceneView::SceneChanged, this);
		GetEditorEventHND()->GetEvent(EDITOR_SCROLL_UP)->Bind(&SceneView::ScrollIn, this);
		GetEditorEventHND()->GetEvent(EDITOR_SCROLL_DOWN)->Bind(&SceneView::ScrollOut, this);
		GetEditorEventHND()->GetEvent(EDITOR_W)->Bind(&SceneView::SetGizmoTranslate, this);
		GetEditorEventHND()->GetEvent(EDITOR_E)->Bind(&SceneView::SetGizmoScaler, this);

		//mpSceneCamera = Factory::CreateCamera("Scene Camera");
		//GetCurrentScene()->GetAllGameObjects().EmplaceBack(Ut::Move(*mpSceneCamera));
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
		mpSceneCamera = GetCurrentScene()->FindGameObject("Scene Camera");
		if (GetMainEditor()->CurrentState() == EDITOR_MAIN)
			mpGfxSys->Update(_dt);
	}

	void SceneView::EditorUI()
	{
		ResetMouseEvents();
		EGUI::UnIndent(2);
		if (GetMainEditor()->CurrentState() == EDITOR_PLAY)
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 1.f);

		Texture *pTex = mpGfxSys->GetFrameBuffer().AsTexture();
		AdjustImageSize(pTex);
		AdjustDisplayPos();

		auto orig = ImGui::GetCursorPos();
		ImGui::SetCursorPos(ImVec2{ orig.x + mImgPos.x, orig.y + mImgPos.y - 1.f });
		if (EGUI::Display::Image(pTex->GetID(), mImgSize, true))
			mAmFocused = true;
		ImGui::SetCursorPos(ImVec2{ orig.x, orig.y + mImgSize.y });
		if (GetCurrentScene()->FindGameObject("Scene Camera"))
		{
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
		}
		if (File *t = EGUI::Display::StartPayloadReceiver<Dystopia::File>(EGUI::SCENE))
		{
			GetMainEditor()->OpenScene(std::wstring{ t->mPath.begin(), t->mPath.end() }, std::wstring{ t->mName.begin(), t->mName.end() });
			EGUI::Display::EndPayloadReceiver();
		}
		if (GetMainEditor()->CurrentState() == EDITOR_PLAY)
			ImGui::PopStyleVar();

		CheckMouseEvents();
		DrawGizmos();

		if (mClearSelection)
		{
			mClearSelection = false;
			GetMainEditor()->ClearSelections();
		}
		EGUI::Indent(2);
	}

	void SceneView::Shutdown()
	{
		mpGfxSys = nullptr;
		GetEditorEventHND()->GetEvent(EDITOR_SCENE_CHANGED)->Unbind(this);
		GetEditorEventHND()->GetEvent(EDITOR_SCROLL_UP)->Unbind(this);
		GetEditorEventHND()->GetEvent(EDITOR_SCROLL_DOWN)->Unbind(this);
		GetEditorEventHND()->GetEvent(EDITOR_W)->Unbind(this);
		GetEditorEventHND()->GetEvent(EDITOR_E)->Unbind(this);

	}

	std::string SceneView::GetLabel() const
	{
		return mLabel;
	}

	void SceneView::Move()
	{
		mpSceneCamera = GetCurrentScene()->FindGameObject("Scene Camera");
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
		}
	}

	GameObject* SceneView::FindMouseObject()
	{
		GameObject* pTarget = nullptr;
		if (Camera* pCam = GetCamera())
		{
			Math::Pt3D worldClickPos = GetWorldClickPos(pCam);
			float vLength = FLT_MAX;
			for (auto& obj : GetCurrentScene()->GetAllGameObjects())
			{
				if (&obj == mpSceneCamera) continue;

				Math::Pt3D objPos	= obj.GetComponent<Transform>()->GetGlobalPosition();
				Math::Vec4 distV	= objPos - worldClickPos;
				Math::Vec2 flatVec	= { distV.x, distV.y };
				const auto& trans	= obj.GetComponent<Transform>();
				Math::Vec4 scale	= trans->GetGlobalScale();
				Math::Pt3D pos		= trans->GetGlobalPosition();

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
		}
		return pTarget;
	}

	Math::Vec2 SceneView::FindMouseVector()
	{
		Math::Vec2 v{ 0,0 };
		if (Camera* pCam = GetCamera())
		{
			if (Transform* pTrans = mpSceneCamera->GetComponent<Transform>())
			{
				Math::Pt3D worldClickPos = GetWorldClickPos(pCam);
				v = Math::Vec2{ worldClickPos.x - pTrans->GetGlobalPosition().x,
								worldClickPos.y - pTrans->GetGlobalPosition().y };
			}
		}
		return v;
	}

	void SceneView::Zoom(bool _in)
	{
		mToZoom = eZOOM_NONE;
		if (!mpSceneCamera) return;

		Transform *pObjTransform = mpSceneCamera->GetComponent<Transform>();
		if (!pObjTransform) return;

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

	void SceneView::AdjustImageSize(Texture *_pTex)
	{
		float ix = static_cast<float>(_pTex->GetWidth());
		float iy = static_cast<float>(_pTex->GetHeight());
		float sx = Size().x;
		float sy = Size().y - EGUI::TabsImageOffsetY;
		mImgSize = GetAdjustedRatio(sx, sy, ix, iy);
	}

	void SceneView::AdjustDisplayPos(void)
	{
		mImgPos= GetAdjustedPosition(Size().x, Size().y - EGUI::TabsImageOffsetY, mImgSize.x, mImgSize.y);
	}

	Math::Vec2 SceneView::GetAdjustedRatio(float _sX, float _sY, float _iX, float _iY)
	{
		float iRatio = _iX / _iY;
		float sRatio = _sX / _sY;
		return sRatio > iRatio ? Math::Vec2{ _iX * (_sY / _iY), _sY } :
								 Math::Vec2{ _sX, _iY * (_sX / _iX) };
	}

	Math::Vec2 SceneView::GetAdjustedPosition(float _sX, float _sY, float _iX, float _iY)
	{
		float xDiff = _sX - _iX;
		float yDiff = _sY - _iY;
		return Math::Vec2{ xDiff / 2, yDiff / 2};
	}

	Math::Pt3D SceneView::GetWorldClickPos(const Camera * const _cam) const
	{
		Math::Vec2 mousePos = ImGui::GetMousePos();
		Math::Vec2 relPos = mousePos - ImGui::GetItemRectMin();
		auto viewport = EngineCore::GetInstance()->GetSystem<CameraSystem>()->GetMasterViewport();
		Math::Vec2 hitPoint{ (relPos.x / mImgSize.x) * viewport.mnWidth,
							 (relPos.y / mImgSize.y) * viewport.mnHeight };
		auto worldPos = _cam->ScreenToWorld(Math::MakeVector3D( hitPoint.x,
																hitPoint.y,
																0.f));

		//PrintToConsoleLog("Ratio " +std::to_string(relPos.x / mImgSize.x) + " / " + std::to_string(relPos.y / mImgSize.y));
		//PrintToConsoleLog("Hitpt " +std::to_string(hitPoint.x) + " / " + std::to_string(hitPoint.y));
		//PrintToConsoleLog("Worpt " +std::to_string(worldPos.x) + " / " + std::to_string(worldPos.y));
		return worldPos;
	}

	Camera* SceneView::GetCamera()
	{
		if (!mpSceneCamera) 
			return nullptr;
		return mpSceneCamera->GetComponent<Camera>();
	}

	void SceneView::ResetMouseEvents()
	{
		if (ImGui::IsMouseClicked(0))				mAmFocused = false;
		if (mDragging && !ImGui::IsMouseDown(1))	mDragging = false;
	}

	void SceneView::CheckMouseEvents()
	{
		if (ImGui::IsItemHovered())
		{
			if (ImGui::IsMouseClicked(0))
			{
				GameObject* pObj = FindMouseObject();
				if (pObj && !mGizmoHovered)
				{
					auto ed = GetMainEditor();
					if (ed->IsCtrlDown())	ed->AddSelection(pObj->GetID());
					else					ed->NewSelection(pObj->GetID());
				}
				else
					mClearSelection = true;
			}
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

	void SceneView::AcceptPrefab(File* _pFile)
	{
		if (Camera* pCam = GetCamera())
		{
			GameObject* pTarget			= FindMouseObject();
			float betterZ				= (pTarget) ? pTarget->GetComponent<Transform>()->GetPosition().z + 0.1f : 0;
			Math::Pt3D worldClickPos	= GetWorldClickPos(pCam);
			Math::Pt3D spawnSite		= Math::Pt3D{ worldClickPos.x, worldClickPos.y, betterZ, 1 };

			GameObject *pDupl = Factory::LoadFromPrefab("", _pFile->mPath);
			if (pDupl)
			{
				pDupl->GetComponent<Transform>()->SetPosition(spawnSite);
				GetCommandHND()->InvokeCommandInsert(*pDupl, *GetCurrentScene());
			}
		}
	}

	void SceneView::AcceptTexture(File* _pFile)
	{
		if (Camera* pCam = GetCamera())
		{
			GameObject* pTarget			= FindMouseObject();
			Renderer *pRend				= pTarget ? pTarget->GetComponent<Renderer>() : nullptr;
			if (pRend)
			{
				auto fOld = EGUI::GetCommandHND()->Make_FunctionModWrapper(&Dystopia::Renderer::SetTexture, 
																			pRend->GetTexture());
				auto fNew = EGUI::GetCommandHND()->Make_FunctionModWrapper(&Dystopia::Renderer::SetTexture, 
																			mpGfxSys->LoadTexture(_pFile->mPath));
				GetCommandHND()->InvokeCommand(pTarget->GetID(), fOld, fNew);
				GetMainEditor()->NewSelection(pTarget->GetID());
			}
		}
	}

	void SceneView::DrawGizmos()
	{
		mGizmoHovered = false; 
		auto& clipboardObjs = GetMainEditor()->GetSelectionObjects();
		if (!clipboardObjs.size())
			return;
		else if (clipboardObjs.size() == 1)
		{
			auto& obj = **clipboardObjs.begin();
			DrawGizmoSingle(obj);
		}
		else
			DrawGizmoMul(clipboardObjs);
	}

	Math::Vec2 SceneView::GetWorldToScreen(const Math::Pt3D& curPos)
	{
		mpSceneCamera = GetCurrentScene()->FindGameObject("Scene Camera");
		if (!mpSceneCamera) return Math::Vec2{ 0,0 };

		Camera*	pCamera	= mpSceneCamera->GetComponent<Camera>();
		auto equation1 = pCamera->GetProjectionMatrix() * (pCamera->GetViewMatrix() * curPos);

		return Math::Vec2{ (equation1.x * (mImgSize.x / 2)) + (Size().x / 2),
						   (equation1.y * (mImgSize.y / 2)) + (Size().y / 2) };
	}

	void SceneView::SetGizmoTranslate(void)
	{
		mCurrGizTool = eGizTool::eTRANSLATE;
	}

	void SceneView::SetGizmoScaler(void)
	{
		mCurrGizTool = eGizTool::eSCALE;
	}

	void SceneView::DrawGizmoMul(const AutoArray<GameObject*>& _arr)
	{
		Math::Pt3D avgPos{ 0, 0, 0, 1.f };
		unsigned int size = static_cast<unsigned int>(_arr.size());
		for (auto& obj : _arr)
		{
			const auto pos = obj->GetComponent<Transform>()->GetGlobalPosition();
			avgPos.x = avgPos.x + pos.x;
			avgPos.y = avgPos.y + pos.y;
		}
		avgPos.x = avgPos.x / size;
		avgPos.y = avgPos.y / size;
		Math::Vec2 screenPos = GetWorldToScreen(avgPos);
		float changeX = 0;
		float changeY = 0;

		switch (mCurrGizTool)
		{
		case eTRANSLATE:
			switch (EGUI::Gizmo2D::ArrowLeft("##LeftArrow", changeX, screenPos, 1.f, redColor, &mGizmoHovered))
			{
			case EGUI::eDRAGGING:
				for (auto& obj : _arr)
				{
					auto cpos = obj->GetComponent<Transform>()->GetGlobalPosition();
					obj->GetComponent<Transform>()->SetGlobalPosition(Math::Pt3D{ cpos.x + changeX, cpos.y, cpos.z, cpos.w });
				}
				mClearSelection = false;
				break;
			case EGUI::eSTART_DRAG:
				mClearSelection = false;
				break;
			case EGUI::eEND_DRAG:
				mClearSelection = false;
				break;
			}
			switch (EGUI::Gizmo2D::ArrowUp("##UpArrow", changeY, screenPos, 1.f, greenColor, &mGizmoHovered))
			{
			case EGUI::eDRAGGING:
				for (auto& obj : _arr)
				{
					auto cpos = obj->GetComponent<Transform>()->GetGlobalPosition();
					obj->GetComponent<Transform>()->SetGlobalPosition(Math::Pt3D{ cpos.x, cpos.y + changeY, cpos.z, cpos.w });
				}
				mClearSelection = false;
				break;
			case EGUI::eSTART_DRAG:
				mClearSelection = false;
				break;
			case EGUI::eEND_DRAG:
				mClearSelection = false;
				break;
			}
			switch (EGUI::Gizmo2D::Box("##BothArrow", changeX, changeY, screenPos, 1.f, blueColor, &mGizmoHovered))
			{
			case EGUI::eDRAGGING:
				for (auto& obj : _arr)
				{
					auto cpos = obj->GetComponent<Transform>()->GetGlobalPosition();
					obj->GetComponent<Transform>()->SetGlobalPosition(Math::Pt3D{ cpos.x + changeX, cpos.y + changeY, cpos.z, cpos.w });
				}
				mClearSelection = false;
				break;
			case EGUI::eSTART_DRAG:
				mClearSelection = false;
				break;
			case EGUI::eEND_DRAG:
				mClearSelection = false;
				break;
			}
			break;

		case eSCALE:
			switch (EGUI::Gizmo2D::ScalerLeft("##LeftScaler", changeX, screenPos, 1.f, redColor, &mGizmoHovered))
			{
			case EGUI::eDRAGGING:
				for (auto& obj : _arr)
				{
					auto cScale = obj->GetComponent<Transform>()->GetGlobalScale();
					obj->GetComponent<Transform>()->SetScale(Math::Vec4{ cScale.x + changeX, cScale.y, cScale.z, cScale.w });
				}
				mClearSelection = false;
				break;
			case EGUI::eSTART_DRAG:
				mClearSelection = false;
				break;
			case EGUI::eEND_DRAG:
				mClearSelection = false;
				break;
			}
			switch (EGUI::Gizmo2D::ScalerUp("##UpScaler", changeY, screenPos, 1.f, greenColor, &mGizmoHovered))
			{
			case EGUI::eDRAGGING:
				for (auto& obj : _arr)
				{
					auto cScale = obj->GetComponent<Transform>()->GetGlobalScale();
					obj->GetComponent<Transform>()->SetScale(Math::Vec4{ cScale.x, cScale.y + changeY, cScale.z, cScale.w });
				}
				mClearSelection = false;
				break;
			case EGUI::eSTART_DRAG:
				mClearSelection = false;
				break;
			case EGUI::eEND_DRAG:
				mClearSelection = false;
				break;
			}
			switch (EGUI::Gizmo2D::Box("##BothScaler", changeX, changeY, screenPos, 1.f, blueColor, &mGizmoHovered))
			{
			case EGUI::eDRAGGING:
				for (auto& obj : _arr)
				{
					auto cScale = obj->GetComponent<Transform>()->GetGlobalScale();
					obj->GetComponent<Transform>()->SetScale(Math::Vec4{ cScale.x + changeX, cScale.y + changeY, cScale.z, cScale.w });
				}
				mClearSelection = false;
				break;
			case EGUI::eSTART_DRAG:
				mClearSelection = false;
				break;
			case EGUI::eEND_DRAG:
				mClearSelection = false;
				break;
			}
			break;
		}
	
	}

	void SceneView::DrawGizmoSingle(GameObject& obj)
	{
		Math::Pt3D curPos	 = obj.GetComponent<Transform>()->GetGlobalPosition();
		Math::Vec4 cScale	 = obj.GetComponent<Transform>()->GetGlobalScale();
		Math::Vec2 screenPos = GetWorldToScreen(curPos);
		float changeX = 0;
		float changeY = 0;

		switch (mCurrGizTool)
		{
		case eTRANSLATE:
			switch (EGUI::Gizmo2D::ArrowLeft("##LeftArrow", changeX, screenPos, 1.f, redColor, &mGizmoHovered))
			{
			case EGUI::eDRAGGING:
				obj.GetComponent<Transform>()->SetPosition(Math::Pt3D{ curPos.x + changeX, curPos.y, curPos.z, curPos.w });
				mClearSelection = false;
				break;
			case EGUI::eSTART_DRAG:
				mClearSelection = false;
				break;
			case EGUI::eEND_DRAG:
				mClearSelection = false;
				break;
			}
			switch (EGUI::Gizmo2D::ArrowUp("##UpArrow", changeY, screenPos, 1.f, greenColor, &mGizmoHovered))
			{
			case EGUI::eDRAGGING:
				obj.GetComponent<Transform>()->SetPosition(Math::Pt3D{ curPos.x, curPos.y + changeY, curPos.z, curPos.w });
				mClearSelection = false;
				break;
			case EGUI::eSTART_DRAG:
				mClearSelection = false;
				break;
			case EGUI::eEND_DRAG:
				mClearSelection = false;
				break;
			}
			switch (EGUI::Gizmo2D::Box("##BothArrow", changeX, changeY, screenPos, 1.f, blueColor, &mGizmoHovered))
			{
			case EGUI::eDRAGGING:
				obj.GetComponent<Transform>()->SetPosition(Math::Pt3D{ curPos.x + changeX, curPos.y + changeY, curPos.z, curPos.w });
				mClearSelection = false;
				break;
			case EGUI::eSTART_DRAG:
				mClearSelection = false;
				break;
			case EGUI::eEND_DRAG:
				mClearSelection = false;
				break;
			}
			break;

		case eSCALE:
			switch (EGUI::Gizmo2D::ScalerLeft("##LeftScaler", changeX, screenPos, 1.f, redColor, &mGizmoHovered))
			{
			case EGUI::eDRAGGING:
				obj.GetComponent<Transform>()->SetScale(Math::Vec4{ cScale.x + changeX, cScale.y, cScale.z, cScale.w });
				mClearSelection = false;
				break;
			case EGUI::eSTART_DRAG:
				mClearSelection = false;
				break;
			case EGUI::eEND_DRAG:
				mClearSelection = false;
				break;
			}
			switch (EGUI::Gizmo2D::ScalerUp("##UpScaler", changeY, screenPos, 1.f, greenColor, &mGizmoHovered))
			{
			case EGUI::eDRAGGING:
				obj.GetComponent<Transform>()->SetScale(Math::Vec4{ cScale.x, cScale.y + changeY, cScale.z, cScale.w });
				mClearSelection = false;
				break;
			case EGUI::eSTART_DRAG:
				mClearSelection = false;
				break;
			case EGUI::eEND_DRAG:
				mClearSelection = false;
				break;
			}
			switch (EGUI::Gizmo2D::Box("##BothScaler", changeX, changeY, screenPos, 1.f, blueColor, &mGizmoHovered))
			{
			case EGUI::eDRAGGING:
				obj.GetComponent<Transform>()->SetScale(Math::Vec4{ cScale.x + changeX, cScale.y + changeY, cScale.z, cScale.w });
				mClearSelection = false;
				break;
			case EGUI::eSTART_DRAG:
				mClearSelection = false;
				break;
			case EGUI::eEND_DRAG:
				mClearSelection = false;
				break;
			}
			break;
		}
	}
}


#endif //EDITOR


