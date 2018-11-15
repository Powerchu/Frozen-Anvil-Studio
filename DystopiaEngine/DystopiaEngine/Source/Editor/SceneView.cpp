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
#include "Object/ObjectFlags.h"

#include "Component/Transform.h"
#include "Component/Camera.h"
#include "Component/Renderer.h"
#include "Component/SpriteRenderer.h"
#include "Component/RigidBody.h"
#include "Behaviour/Behaviour.h"

#include "Math/MathUtility.h"

namespace Dystopia
{
	static const Math::Vec4 redColor = Math::Vec4{ 1.f,0.f,0.f,1.f };
	static const Math::Vec4 greenColor = Math::Vec4{ 0.f,1.f,0.f,1.f };
	static const Math::Vec4 blueColor = Math::Vec4{ 0.f,0.f,1.f,1.f };
	static constexpr float dragMagnitudeEpsilon = 0.001f;

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
		mMoveSens{ -0.6f }, mPrevMovePoint{ 0,0 },
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
		GetEditorEventHND()->GetEvent(EDITOR_GIZMO_TRANSLATE)->Bind(&SceneView::SetGizmoTranslate, this);
		GetEditorEventHND()->GetEvent(EDITOR_GIZMO_SCALE)->Bind(&SceneView::SetGizmoScaler, this);

		GameObject * temp = Factory::CreateCamera("Scene Camera");
		auto& g = *GetCurrentScene()->InsertGameObject(Ut::Move(*temp));
		g.GetComponent<Transform>()->SetOwner(&g);
		//auto sr = static_cast<ComponentDonor<SpriteRenderer>*>(EngineCore::GetInstance()->Get<typename SpriteRenderer::SYSTEM>())->RequestComponent();
		//g.AddComponent(sr, Component::TAG{});
		//auto s = g.GetComponent<SpriteRenderer>();
		for (auto& c : g.GetAllComponents())
		{
			c->SetOwner(&g);
			c->Init();
		}
		for (auto& b : g.GetAllBehaviours())
		{
			b->SetOwner(&g);
			b->Init();
		}
		mpSceneCamera = g.GetComponent<Camera>();
		g.SetActive(true);
		g.SetFlag(FLAG_ALL_LAYERS);
		delete temp;
		SceneChanged();
	}

	void SceneView::Update(const float& _dt)
	{
		GameObject *temp = GetCurrentScene()->FindGameObject("Scene Camera");
		if (temp)
			mpSceneCamera = temp->GetComponent<Camera>();
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
		if (mpSceneCamera)
			pTex = mpSceneCamera->GetSurface()->AsTexture();

		AdjustImageSize(pTex);
		AdjustDisplayPos();

		auto orig = ImGui::GetCursorPos();
		ImGui::SetCursorPos(ImVec2{ orig.x + mImgPos.x, orig.y + mImgPos.y - 1.f });
		EGUI::Display::Image(pTex->GetID(), mImgSize);
		if (ImGui::IsItemHovered())
			mAmFocused = true;
		else
			mAmFocused = false;
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
			if (File *t = EGUI::Display::StartPayloadReceiver<Dystopia::File>(EGUI::DDS))
			{
				AcceptTexture(t);
				EGUI::Display::EndPayloadReceiver();
			}
			if (File *t = EGUI::Display::StartPayloadReceiver<Dystopia::File>(EGUI::BMP))
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
		mPrevMovePoint = Math::Vec2{ ImGui::GetMousePos() } -ImGui::GetItemRectMin();
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
		GetEditorEventHND()->GetEvent(EDITOR_GIZMO_TRANSLATE)->Unbind(this);
		GetEditorEventHND()->GetEvent(EDITOR_GIZMO_SCALE)->Unbind(this);

	}

	std::string SceneView::GetLabel() const
	{
		return mLabel;
	}

	void SceneView::Move()
	{
		if (mpSceneCamera)
		{
			Math::Vec2 relPos = Math::Vec2{ ImGui::GetMousePos() } - ImGui::GetItemRectMin();

			Math::Vec2 vToMove{ relPos.x - mPrevMovePoint.x, relPos.y - mPrevMovePoint.y };
			if (vToMove.MagnitudeSqr() < dragMagnitudeEpsilon)
				return;
			vToMove.x -= .5f;
			vToMove.y -= .5f;
			vToMove.x *= 2.f;
			vToMove.y *= -2.f;

			const auto pos = mpSceneCamera->GetOwner()->GetComponent<Transform>()->GetPosition();
			const auto scale = mpSceneCamera->GetOwner()->GetComponent<Transform>()->GetGlobalScale();
		
			mpSceneCamera->GetOwner()->GetComponent<Transform>()->SetPosition(pos + Math::Pt3D{ vToMove.x * mMoveSens * scale.x, vToMove.y * mMoveSens * scale.y, 0.f });
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
				if (&obj == mpSceneCamera->GetOwner()) continue;

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

	void SceneView::Zoom(bool _in)
	{
		mToZoom = eZOOM_NONE;
		if (!mpSceneCamera) return;

		Transform *pObjTransform = mpSceneCamera->GetOwner()->GetComponent<Transform>();
		if (!pObjTransform) return;

		float s = _in ? 1 - mSensitivity : 1 + mSensitivity;
		Math::Vec4 newScalep = pObjTransform->GetScale() * s;
		newScalep.z = 1.f;
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
 		mpSceneCamera = GetCurrentScene()->FindGameObject("Scene Camera")->GetComponent<Camera>();
	}

	void SceneView::AdjustImageSize(Texture *_pTex)
	{
		static constexpr float aspect = 16.f / 10.f;
		float ix = static_cast<float>(aspect * _pTex->GetWidth());
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
		Math::Vec2 hitPoint{ (relPos.x / mImgSize.x) - .5f ,
							 (relPos.y / mImgSize.y) - .5f };
		auto worldPos = _cam->ScreenToWorld(Math::MakePoint3D(2 * hitPoint.x, 2 * hitPoint.y , 0));
		return worldPos;
	}

	Camera* SceneView::GetCamera()
	{
		return mpSceneCamera;
	}

	void SceneView::ResetMouseEvents()
	{
		if (mDragging && !ImGui::IsMouseDown(1))
			mDragging = false;
	}

	void SceneView::CheckMouseEvents()
	{
		if (ImGui::IsItemHovered())
		{
			if (ImGui::IsMouseClicked(0))
			{
				GameObject* pObj = FindMouseObject();
				if (pObj && !mGizmoHovered)
					GetMainEditor()->AddSelection(pObj->GetID());
				else
					mClearSelection = true;
			}
			if (ImGui::IsMouseClicked(1))
				mDragging = true;
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
			Math::Pt3D spawnSite		= Math::Pt3D{ worldClickPos.x, worldClickPos.y, betterZ, 1.f };

			GameObject *pDupl = Factory::LoadFromPrefab("", _pFile->mPath.c_str());
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
			GameObject* pTarget = FindMouseObject();
			Renderer *pRend = pTarget ? pTarget->GetComponent<Renderer>() : nullptr;
			if (pRend)
			{
				auto fOld = EGUI::GetCommandHND()->Make_FunctionModWrapper(&Dystopia::Renderer::SetTexture,
					pRend->GetTexture());
				auto fNew = EGUI::GetCommandHND()->Make_FunctionModWrapper(&Dystopia::Renderer::SetTexture,
					mpGfxSys->LoadTexture(_pFile->mPath.c_str()));
				GetCommandHND()->InvokeCommand(pTarget->GetID(), fOld, fNew);
				GetMainEditor()->NewSelection(pTarget->GetID());
			}
			else
			{
				HashString defaultName = "GameObject";
				if (!pTarget)
				{
					Math::Pt3D worldClickPos = GetWorldClickPos(GetCamera());
					Math::Pt3D spawnSite = Math::Pt3D{ worldClickPos.x, worldClickPos.y, 0.f, 1.f };

					pTarget = Factory::CreateGameObj(defaultName);
					pTarget->SetFlag(eObjFlag::FLAG_LAYER_WORLD);
					pTarget->GetComponent<Transform>()->SetPosition(spawnSite);

					GetCommandHND()->InvokeCommandInsert(*pTarget, *GetCurrentScene());
				}
				GameObject* pGuaranteedTarget = FindMouseObject();
				Renderer* pNewRend = static_cast<ComponentDonor<Renderer>*>(EngineCore::GetInstance()->Get<typename Renderer::SYSTEM>())->RequestComponent();
				pGuaranteedTarget->AddComponent(pNewRend, typename Component::TAG{});
				pNewRend->SetTexture(mpGfxSys->LoadTexture(_pFile->mPath.c_str()));
				pNewRend->SetOwner(pGuaranteedTarget);
				pNewRend->SetActive(pGuaranteedTarget->IsActive());
				pNewRend->Awake();
				if (pGuaranteedTarget->GetName() == defaultName)
				{
					pGuaranteedTarget->SetName(_pFile->mName);
				}
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
		mpSceneCamera = GetCurrentScene()->FindGameObject("Scene Camera")->GetComponent<Camera>();
		if (!mpSceneCamera) return Math::Vec2{ 0,0 };

		auto equation1 = mpSceneCamera->GetProjectionMatrix() * (mpSceneCamera->GetViewMatrix() * curPos);


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
		Math::Point3D pos{};
		const auto size = static_cast<unsigned int>(_arr.size());
		for (auto& obj : _arr)
		{
			obj->SetActive(true);
			pos = obj->GetComponent<Transform>()->GetGlobalPosition();
			avgPos.x = avgPos.x + pos.x;
			avgPos.y = avgPos.y + pos.y;
		}
		avgPos.x = avgPos.x / size;
		avgPos.y = avgPos.y / size;
		const Math::Vec2 screenPos = GetWorldToScreen(avgPos);
		float changeX = 0;
		float changeY = 0;

		const auto scale = mpSceneCamera->GetOwner()->GetComponent<Transform>()->GetGlobalScale();

		switch (mCurrGizTool)
		{
		case eTRANSLATE:
			switch (EGUI::Gizmo2D::ArrowLeft("##LeftArrow", changeX, screenPos, 0.5f, redColor, &mGizmoHovered))
			{
			case EGUI::eDRAGGING:
				for (auto& obj : _arr)
				{
					const auto cpos = obj->GetComponent<Transform>()->GetGlobalPosition();
					obj->GetComponent<Transform>()->SetGlobalPosition(Math::Pt3D{ cpos.x + changeX*scale.x, cpos.y, cpos.z, cpos.w });
					obj->SetActive(false);
				}
				mClearSelection = false;
				break;
			case EGUI::eSTART_DRAG:
				mClearSelection = false;
				for (auto& obj : _arr)
				{
					obj->SetActive(false);
				}
				break;
			case EGUI::eEND_DRAG:
				mClearSelection = false;
				for (auto& obj : _arr)
				{
					obj->SetActive(true);
				}
				break;
			}
			switch (EGUI::Gizmo2D::ArrowUp("##UpArrow", changeY, screenPos, 0.5f, greenColor, &mGizmoHovered))
			{
			case EGUI::eDRAGGING:
				for (auto& obj : _arr)
				{
					const auto cpos = obj->GetComponent<Transform>()->GetGlobalPosition();
					obj->GetComponent<Transform>()->SetPosition(Math::Pt3D{ cpos.x, cpos.y + changeY * scale.y, cpos.z, cpos.w });
					if (obj->GetComponent<RigidBody>())
						obj->SetActive(false);
				}
				mClearSelection = false;
				break;
			case EGUI::eSTART_DRAG:
				mClearSelection = false;
				for (auto& obj : _arr)
				{
					if (obj->GetComponent<RigidBody>())
						obj->SetActive(false);
				}
				break;
			case EGUI::eEND_DRAG:
				mClearSelection = false;
				for (auto& obj : _arr)
				{
					obj->SetActive(true);
				}
				break;
			}
			switch (EGUI::Gizmo2D::Box("##BothArrow", changeX, changeY, screenPos, 0.5f, blueColor, &mGizmoHovered))
			{
			case EGUI::eDRAGGING:
				for (auto& obj : _arr)
				{
					const auto cpos = obj->GetComponent<Transform>()->GetGlobalPosition();
					obj->GetComponent<Transform>()->SetPosition(Math::Pt3D{ cpos.x + changeX * scale.x, cpos.y + changeY * scale.y, cpos.z, cpos.w });
					if (obj->GetComponent<RigidBody>())
						obj->SetActive(false);
				}
				mClearSelection = false;
				break;
			case EGUI::eSTART_DRAG:
				mClearSelection = false;
				for (auto& obj : _arr)
				{
					if (obj->GetComponent<RigidBody>())
						obj->SetActive(false);
				}
				break;
			case EGUI::eEND_DRAG:
				mClearSelection = false;
				for (auto& obj : _arr)
				{
					obj->SetActive(true);
				}
				break;
			}
			break;

		case eSCALE:
			switch (EGUI::Gizmo2D::ScalerLeft("##LeftScaler", changeX, screenPos, 0.5f, redColor, &mGizmoHovered))
			{
			case EGUI::eDRAGGING:
				for (auto& obj : _arr)
				{
					const auto cScale = obj->GetComponent<Transform>()->GetScale();
					obj->GetComponent<Transform>()->SetScale(Math::Vec4{ cScale.x + changeX*scale.x, cScale.y, cScale.z, cScale.w });
					if (obj->GetComponent<RigidBody>())
						obj->SetActive(false);
				}
				mClearSelection = false;
				break;
			case EGUI::eSTART_DRAG:
				mClearSelection = false;
				for (auto& obj : _arr)
				{
					if (obj->GetComponent<RigidBody>())
						obj->SetActive(false);
				}
				break;
			case EGUI::eEND_DRAG:
				mClearSelection = false;
				for (auto& obj : _arr)
				{
					if (obj->GetComponent<RigidBody>())
						obj->SetActive(true);
				}
				break;
			}
			switch (EGUI::Gizmo2D::ScalerUp("##UpScaler", changeY, screenPos, 0.5f, greenColor, &mGizmoHovered))
			{
			case EGUI::eDRAGGING:
				for (auto& obj : _arr)
				{
					const auto cScale = obj->GetComponent<Transform>()->GetScale();
					obj->GetComponent<Transform>()->SetScale(Math::Vec4{ cScale.x, cScale.y + changeY*scale.y, cScale.z, cScale.w });
					if (obj->GetComponent<RigidBody>())
						obj->SetActive(false);
				}
				mClearSelection = false;
				break;
			case EGUI::eSTART_DRAG:
				mClearSelection = false;
				for (auto& obj : _arr)
				{
					if (obj->GetComponent<RigidBody>())
						obj->SetActive(false);
				}
				break;
			case EGUI::eEND_DRAG:
				mClearSelection = false;
				for (auto& obj : _arr)
				{
					obj->SetActive(true);
				}
				break;
			}
			switch (EGUI::Gizmo2D::Box("##BothScaler", changeX, changeY, screenPos, 0.5f, blueColor, &mGizmoHovered))
			{
			case EGUI::eDRAGGING:
				for (auto& obj : _arr)
				{
					const auto cScale = obj->GetComponent<Transform>()->GetScale();
					obj->GetComponent<Transform>()->SetScale(Math::Vec4{ cScale.x + changeX * scale.x, cScale.y + changeX * scale.y, cScale.z, cScale.w });
					if (obj->GetComponent<RigidBody>())
						obj->SetActive(false);
				}
				mClearSelection = false;
				break;
			case EGUI::eSTART_DRAG:
				mClearSelection = false;
				for (auto& obj : _arr)
				{
					if (obj->GetComponent<RigidBody>())
						obj->SetActive(false);
				}
				break;
			case EGUI::eEND_DRAG:
				mClearSelection = false;
				for (auto& obj : _arr)
				{
					obj->SetActive(true);
				}
				break;
			}
			break;
		}
	
	}

	void SceneView::DrawGizmoSingle(GameObject& obj)
	{
		obj.SetActive(true);
		const Math::Pt3D curPos = obj.GetComponent<Transform>()->GetGlobalPosition();
		//const Math::Pt3D curPosLocal = obj.GetComponent<Transform>()->GetPosition();
		//const Math::Vec4 cScale = obj.GetComponent<Transform>()->GetGlobalScale();
		const Math::Vec4 cScaleLocal = obj.GetComponent<Transform>()->GetScale();
		const Math::Vec2 screenPos = GetWorldToScreen(curPos);
		float changeX = 0;
		float changeY = 0;
		static float ratio = 0;

		const auto scale = mpSceneCamera->GetOwner()->GetComponent<Transform>()->GetGlobalScale();

		switch (mCurrGizTool)
		{
		case eTRANSLATE:
			switch (EGUI::Gizmo2D::ArrowLeft("##LeftArrow", changeX, screenPos, 0.5F, redColor, &mGizmoHovered))
			{
			case EGUI::eDRAGGING:
				obj.GetComponent<Transform>()->SetGlobalPosition(Math::Pt3D{ curPos.x + changeX * scale.x, curPos.y, curPos.z, curPos.w });
				mClearSelection = false;
				if (obj.GetComponent<RigidBody>())
					obj.SetActive(false);
				break;
			case EGUI::eSTART_DRAG:
				mClearSelection = false;
				if (obj.GetComponent<RigidBody>())
					obj.SetActive(false);
				break;
			case EGUI::eEND_DRAG:
				mClearSelection = false;
				obj.SetActive(true);
				break;
			}
			switch (EGUI::Gizmo2D::ArrowUp("##UpArrow", changeY, screenPos, 0.5F, greenColor, &mGizmoHovered))
			{
			case EGUI::eDRAGGING:
				obj.GetComponent<Transform>()->SetGlobalPosition(Math::Pt3D{ curPos.x, curPos.y + changeY * scale.y, curPos.z, curPos.w });
				mClearSelection = false;
				if (obj.GetComponent<RigidBody>())
					obj.SetActive(false);
				break;
			case EGUI::eSTART_DRAG:
				mClearSelection = false;
				if (obj.GetComponent<RigidBody>())
					obj.SetActive(false);
				break;
			case EGUI::eEND_DRAG:
				mClearSelection = false;
				obj.SetActive(true);
				break;
			}
			switch (EGUI::Gizmo2D::Box("##BothArrow", changeX, changeY, screenPos, 0.5, blueColor, &mGizmoHovered))
			{
			case EGUI::eDRAGGING:
				obj.GetComponent<Transform>()->SetGlobalPosition(Math::Pt3D{ curPos.x + changeX * scale.x, curPos.y + changeY * scale.y, curPos.z, curPos.w });
				mClearSelection = false;
				if (obj.GetComponent<RigidBody>())
					obj.SetActive(false);
				break;
			case EGUI::eSTART_DRAG:
				mClearSelection = false;
				if (obj.GetComponent<RigidBody>())
					obj.SetActive(false);
				break;
			case EGUI::eEND_DRAG:
				mClearSelection = false;
				obj.SetActive(true);
				break;
			}
			break;

		case eSCALE:
			switch (EGUI::Gizmo2D::ScalerLeft("##LeftScaler", changeX, screenPos, 0.5, redColor, &mGizmoHovered))
			{
			case EGUI::eDRAGGING:
				obj.GetComponent<Transform>()->SetScale(Math::Vec4{ cScaleLocal.x + changeX * scale.x, cScaleLocal.y, cScaleLocal.z, cScaleLocal.w });
				mClearSelection = false;
				if (obj.GetComponent<RigidBody>())
					obj.SetActive(false);
				break;
			case EGUI::eSTART_DRAG:
				mClearSelection = false;
				if (obj.GetComponent<RigidBody>())
					obj.SetActive(false);
				break;
			case EGUI::eEND_DRAG:
				mClearSelection = false;
				obj.SetActive(true);
				break;
			}
			switch (EGUI::Gizmo2D::ScalerUp("##UpScaler", changeY, screenPos, 0.5, greenColor, &mGizmoHovered))
			{
			case EGUI::eDRAGGING:
				obj.GetComponent<Transform>()->SetScale(Math::Vec4{ cScaleLocal.x, cScaleLocal.y + changeY * scale.y, cScaleLocal.z, cScaleLocal.w });
				mClearSelection = false;
				if (obj.GetComponent<RigidBody>())
					obj.SetActive(false);
				break;
			case EGUI::eSTART_DRAG:
				mClearSelection = false;
				if (obj.GetComponent<RigidBody>())
					obj.SetActive(false);
				break;
			case EGUI::eEND_DRAG:
				mClearSelection = false;
				obj.SetActive(true);
				break;
			}
			switch (EGUI::Gizmo2D::Box("##BothScaler", changeX, changeY, screenPos, 0.5, blueColor, &mGizmoHovered))
			{
			case EGUI::eDRAGGING:
				changeX += changeY;
				obj.GetComponent<Transform>()->SetScale(Math::Vec4{ cScaleLocal.x + changeX * scale.x, cScaleLocal.y + ratio * changeX *scale.x, cScaleLocal.z, cScaleLocal.w });
				mClearSelection = false;
				if (obj.GetComponent<RigidBody>())
					obj.SetActive(false);
				break;
			case EGUI::eSTART_DRAG:
				mClearSelection = false;
				ratio = cScaleLocal.y / cScaleLocal.x;
				if (obj.GetComponent<RigidBody>())
					obj.SetActive(false);
				break;
			case EGUI::eEND_DRAG:
				mClearSelection = false;
				obj.SetActive(true);
				break;
			}
			break;
		case eROTATE: 
			break;
		default: 
			break;
		}
	}
}


#endif //EDITOR


