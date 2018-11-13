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

#include "Editor/EditorMain.h"

#include "System/Driver/Driver.h"
#include "System/Scene/Scene.h"
#include "System/Scene/SceneSystem.h"
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
#include "Behaviour/Behaviour.h"

#include "Math/MathUtility.h"

namespace Editor //Dystopia
{
	static const Math::Vec4 redColor = Math::Vec4{ 1.f,0.f,0.f,1.f };
	static const Math::Vec4 greenColor = Math::Vec4{ 0.f,1.f,0.f,1.f };
	static const Math::Vec4 blueColor = Math::Vec4{ 0.f,0.f,1.f,1.f };
	static constexpr float dragMagnitudeEpsilon = 0.001f;

	//static SceneView* gpInstance = 0;
	//SceneView* SceneView::GetInstance()
	//{
	//	if (gpInstance) return gpInstance;
	//
	//	gpInstance = new SceneView{};
	//	return gpInstance;
	//}

	SceneView::SceneView(void)
		: //EditorTab{ false, true },
		mLabel{ "Scene View" }, mpGfxSys{ nullptr },
		mpSceneCamera{ nullptr }, mSensitivity{ 0.1f },
		mToZoom{ eZOOM_NONE }, mAmFocused{ false },
		mMoveSens{ -0.6f }, mPrevMovePoint{ 0,0 },
		mDragging{ false }, mImgPos{}, mImgSize{},
		mClearSelection{ false },
		mGizmoHovered{ false }, mCurrGizTool{ eTRANSLATE }, mpSceneSys{ nullptr }
	{
		EditorPanel::SetOpened(true);
	}

	SceneView::~SceneView(void)
	{
		//gpInstance = nullptr;
	}

	void SceneView::Load(void)
	{}

	bool SceneView::Init(void)
	{
		mpGfxSys = Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::GraphicsSystem>();
		mpSceneSys = Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::SceneSystem>();
		//GetEditorEventHND()->GetEvent(EDITOR_SCENE_CHANGED)->Bind(&SceneView::SceneChanged, this);
		//GetEditorEventHND()->GetEvent(EDITOR_SCROLL_UP)->Bind(&SceneView::ScrollIn, this);
		//GetEditorEventHND()->GetEvent(EDITOR_SCROLL_DOWN)->Bind(&SceneView::ScrollOut, this);
		//GetEditorEventHND()->GetEvent(EDITOR_GIZMO_TRANSLATE)->Bind(&SceneView::SetGizmoTranslate, this);
		//GetEditorEventHND()->GetEvent(EDITOR_GIZMO_SCALE)->Bind(&SceneView::SetGizmoScaler, this);

		Dystopia::GameObject * temp = Dystopia::Factory::CreateCamera("Scene Camera");
		auto g = mpSceneSys->GetCurrentScene().InsertGameObject(Ut::Move(*temp));
		g->GetComponent<Dystopia::Transform>()->SetOwner(g);
		for (auto& c : g->GetAllComponents())
		{
			c->SetOwner(g);
			c->Init();
		}
		for (auto& b : g->GetAllBehaviours())
		{
			b->SetOwner(g);
			b->Init();
		}
		mpSceneCamera = g->GetComponent<Dystopia::Camera>();
		g->SetActive(true);
		g->SetFlag(Dystopia::eObjFlag::FLAG_ALL_LAYERS);
		delete temp;
		SceneChanged();
		return true;
	}

	void SceneView::Update(float _dt)
	{
		Dystopia::GameObject *temp = mpSceneSys->GetCurrentScene().FindGameObject("Scene Camera");
		if (temp)
			mpSceneCamera = temp->GetComponent<Dystopia::Camera>();
		if (EditorMain::GetInstance()->GetCurState() == eState::MAIN)
			mpGfxSys->Update(_dt);
	}

	void SceneView::EditorUI(void)
	{
		ResetMouseEvents();
		EGUI::UnIndent(2);
		if (EditorMain::GetInstance()->GetCurState() == eState::PLAY)
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 1.f);

		Dystopia::Texture *pTex = mpGfxSys->GetFrameBuffer().AsTexture();
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
		if (mpSceneSys->GetCurrentScene().FindGameObject("Scene Camera"))
		{
			if (::Editor::File *t = EGUI::Display::StartPayloadReceiver<::Editor::File>(EGUI::PREFAB))
			{
				AcceptPrefab(t);
				EGUI::Display::EndPayloadReceiver();
			}
			if (::Editor::File *t = EGUI::Display::StartPayloadReceiver<::Editor::File>(EGUI::PNG))
			{
				AcceptTexture(t);
				EGUI::Display::EndPayloadReceiver();
			}
			if (::Editor::File *t = EGUI::Display::StartPayloadReceiver<::Editor::File>(EGUI::DDS))
			{
				AcceptTexture(t);
				EGUI::Display::EndPayloadReceiver();
			}
			if (::Editor::File *t = EGUI::Display::StartPayloadReceiver<::Editor::File>(EGUI::BMP))
			{
				AcceptTexture(t);
				EGUI::Display::EndPayloadReceiver();
			}
		}
		if (::Editor::File *t = EGUI::Display::StartPayloadReceiver<::Editor::File>(EGUI::SCENE))
		{
			//EditorMain::GetInstance()->OpenScene(std::wstring{ t->mPath.begin(), t->mPath.end() }, std::wstring{ t->mName.begin(), t->mName.end() });
			EGUI::Display::EndPayloadReceiver();
		}
		if (EditorMain::GetInstance()->GetCurState() == eState::PLAY)
			ImGui::PopStyleVar();

		CheckMouseEvents();
		mPrevMovePoint = Math::Vec2{ ImGui::GetMousePos() } -ImGui::GetItemRectMin();
		DrawGizmos();

		if (mClearSelection)
		{
			mClearSelection = false;
			//EditorMain::GetInstance()->ClearSelections();
		}
		EGUI::Indent(2);
	}

	void SceneView::Shutdown(void)
	{
		mpGfxSys = nullptr;
		//GetEditorEventHND()->GetEvent(EDITOR_SCENE_CHANGED)->Unbind(this);
		//GetEditorEventHND()->GetEvent(EDITOR_SCROLL_UP)->Unbind(this);
		//GetEditorEventHND()->GetEvent(EDITOR_SCROLL_DOWN)->Unbind(this);
		//GetEditorEventHND()->GetEvent(EDITOR_GIZMO_TRANSLATE)->Unbind(this);
		//GetEditorEventHND()->GetEvent(EDITOR_GIZMO_SCALE)->Unbind(this);
	}

	void SceneView::Message(eEMessage)
	{}

	void SceneView::SaveSettings(Dystopia::TextSerialiser& _out) const
	{}

	void SceneView::LoadSettings(Dystopia::TextSerialiser& _in)
	{}

	HashString SceneView::GetLabel(void) const
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

			const auto pos = mpSceneCamera->GetOwner()->GetComponent<Dystopia::Transform>()->GetPosition();
			const auto scale = mpSceneCamera->GetOwner()->GetComponent<Dystopia::Transform>()->GetGlobalScale();
		
			mpSceneCamera->GetOwner()->GetComponent<Dystopia::Transform>()->SetPosition(pos + Math::Pt3D{ vToMove.x * mMoveSens * scale.x, vToMove.y * mMoveSens * scale.y, 0.f });
		}
	}

	Dystopia::GameObject* SceneView::FindMouseObject()
	{
		Dystopia::GameObject* pTarget = nullptr;
		if (Dystopia::Camera* pCam = GetCamera())
		{
			Math::Pt3D worldClickPos = GetWorldClickPos(pCam);
			float vLength = FLT_MAX;
			const auto& allObj = mpSceneSys->GetCurrentScene().GetAllGameObjects();
			for (auto& obj : allObj)
			{
				if (&obj == mpSceneCamera->GetOwner()) continue;

				Math::Pt3D objPos	= obj.GetComponent<Dystopia::Transform>()->GetGlobalPosition();
				Math::Vec4 distV	= objPos - worldClickPos;
				Math::Vec2 flatVec	= { distV.x, distV.y };
				const auto& trans	= obj.GetComponent<Dystopia::Transform>();
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

		Dystopia::Transform *pObjTransform = mpSceneCamera->GetOwner()->GetComponent<Dystopia::Transform>();
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
 		mpSceneCamera = mpSceneSys->GetCurrentScene().FindGameObject("Scene Camera")->GetComponent<Dystopia::Camera>();
	}

	void SceneView::AdjustImageSize(Dystopia::Texture *_pTex)
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

	Math::Pt3D SceneView::GetWorldClickPos(const Dystopia::Camera * const _cam) const
	{
		Math::Vec2 mousePos = ImGui::GetMousePos();
		Math::Vec2 relPos = mousePos - ImGui::GetItemRectMin();
		Math::Vec2 hitPoint{ (relPos.x / mImgSize.x) - .5f ,
							 (relPos.y / mImgSize.y) - .5f };
		auto worldPos = _cam->ScreenToWorld(Math::MakePoint3D(2 * hitPoint.x, 2 * hitPoint.y , 0));
		return worldPos;
	}

	Dystopia::Camera* SceneView::GetCamera()
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
				Dystopia::GameObject* pObj = FindMouseObject();
				if (pObj && !mGizmoHovered)
					;// EditorMain::GetInstance()->AddSelection(pObj->GetID());
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

	void SceneView::AcceptPrefab(::Editor::File* _pFile)
	{
		if (Dystopia::Camera* pCam = GetCamera())
		{
			Dystopia::GameObject* pTarget = FindMouseObject();
			float betterZ				= (pTarget) ? pTarget->GetComponent<Dystopia::Transform>()->GetPosition().z + 0.1f : 0;
			Math::Pt3D worldClickPos	= GetWorldClickPos(pCam);
			Math::Pt3D spawnSite		= Math::Pt3D{ worldClickPos.x, worldClickPos.y, betterZ, 1.f };

			Dystopia::GameObject *pDupl = Dystopia::Factory::LoadFromPrefab("", _pFile->mPath.c_str());
			if (pDupl)
			{
				pDupl->GetComponent<Dystopia::Transform>()->SetPosition(spawnSite);
				//GetCommandHND()->InvokeCommandInsert(*pDupl, *GetCurrentScene());
			}
		}
	}

	void SceneView::AcceptTexture(::Editor::File* _pFile)
	{
		if (Dystopia::Camera* pCam = GetCamera())
		{
			Dystopia::GameObject* pTarget = FindMouseObject();
			Dystopia::Renderer *pRend = pTarget ? pTarget->GetComponent<Dystopia::Renderer>() : nullptr;
			if (pRend)
			{
				auto fOld = EGUI::GetCommandHND()->Make_FunctionModWrapper(&Dystopia::Renderer::SetTexture,
					pRend->GetTexture());
				auto fNew = EGUI::GetCommandHND()->Make_FunctionModWrapper(&Dystopia::Renderer::SetTexture,
					mpGfxSys->LoadTexture(_pFile->mPath.c_str()));
				//GetCommandHND()->InvokeCommand(pTarget->GetID(), fOld, fNew);
				//EditorMain::GetInstance()->NewSelection(pTarget->GetID());
			}
			else
			{
				std::string defaultName = "GameObject";
				if (!pTarget)
				{
					Math::Pt3D worldClickPos = GetWorldClickPos(GetCamera());
					Math::Pt3D spawnSite = Math::Pt3D{ worldClickPos.x, worldClickPos.y, 0.f, 1.f };

					pTarget = Dystopia::Factory::CreateGameObj(defaultName);
					pTarget->SetFlag(Dystopia::eObjFlag::FLAG_LAYER_WORLD);
					pTarget->GetComponent<Dystopia::Transform>()->SetPosition(spawnSite);

					//GetCommandHND()->InvokeCommandInsert(*pTarget, *GetCurrentScene());
				}
				Dystopia::GameObject* pGuaranteedTarget = FindMouseObject();
				Dystopia::Renderer* pNewRend = static_cast<Dystopia::ComponentDonor<Dystopia::Renderer>*>(Dystopia::EngineCore::GetInstance()->Get<typename Dystopia::Renderer::SYSTEM>())->RequestComponent();
				pGuaranteedTarget->AddComponent(pNewRend, typename Dystopia::Component::TAG{});
				pNewRend->SetTexture(mpGfxSys->LoadTexture(_pFile->mPath.c_str()));
				pNewRend->SetOwner(pGuaranteedTarget);
				pNewRend->SetActive(pGuaranteedTarget->IsActive());
				pNewRend->Awake();
				if (pGuaranteedTarget->GetName() == defaultName)
				{
					pGuaranteedTarget->SetName(_pFile->mName.c_str());
				}
			}
		}
	}

	void SceneView::DrawGizmos()
	{
		//mGizmoHovered = false; 
		//auto& clipboardObjs = GetMainEditor()->GetSelectionObjects();
		//if (!clipboardObjs.size())
		//	return;
		//else if (clipboardObjs.size() == 1)
		//{
		//	auto& obj = **clipboardObjs.begin();
		//	DrawGizmoSingle(obj);
		//}
		//else
		//	DrawGizmoMul(clipboardObjs);
	}

	Math::Vec2 SceneView::GetWorldToScreen(const Math::Pt3D& curPos)
	{
		mpSceneCamera = mpSceneSys->GetCurrentScene().FindGameObject("Scene Camera")->GetComponent<Dystopia::Camera>();
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

	void SceneView::DrawGizmoMul(const AutoArray<Dystopia::GameObject*>& _arr)
	{
		Math::Pt3D avgPos{ 0, 0, 0, 1.f };
		unsigned int size = static_cast<unsigned int>(_arr.size());
		for (auto& obj : _arr)
		{
			const auto pos = obj->GetComponent<Dystopia::Transform>()->GetPosition();
			avgPos.x = avgPos.x + pos.x;
			avgPos.y = avgPos.y + pos.y;
		}
		avgPos.x = avgPos.x / size;
		avgPos.y = avgPos.y / size;
		Math::Vec2 screenPos = GetWorldToScreen(avgPos);
		float changeX = 0;
		float changeY = 0;

		const auto scale = mpSceneCamera->GetOwner()->GetComponent<Dystopia::Transform>()->GetGlobalScale();

		switch (mCurrGizTool)
		{
		case eTRANSLATE:
			switch (EGUI::Gizmo2D::ArrowLeft("##LeftArrow", changeX, screenPos, 1.f, redColor, &mGizmoHovered))
			{
			case EGUI::eDRAGGING:
				for (auto& obj : _arr)
				{
					auto cpos = obj->GetComponent<Dystopia::Transform>()->GetPosition();
					obj->GetComponent<Dystopia::Transform>()->SetPosition(Math::Pt3D{ cpos.x + changeX*scale.x, cpos.y, cpos.z, cpos.w });
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
					auto cpos = obj->GetComponent<Dystopia::Transform>()->GetPosition();
					obj->GetComponent<Dystopia::Transform>()->SetPosition(Math::Pt3D{ cpos.x, cpos.y + changeY * scale.y, cpos.z, cpos.w });
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
					auto cpos = obj->GetComponent<Dystopia::Transform>()->GetPosition();
					obj->GetComponent<Dystopia::Transform>()->SetPosition(Math::Pt3D{ cpos.x + changeX * scale.x, cpos.y + changeY * scale.y, cpos.z, cpos.w });
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
					auto cScale = obj->GetComponent<Dystopia::Transform>()->GetGlobalScale();
					obj->GetComponent<Dystopia::Transform>()->SetScale(Math::Vec4{ cScale.x + changeX, cScale.y, cScale.z, cScale.w });
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
					auto cScale = obj->GetComponent<Dystopia::Transform>()->GetGlobalScale();
					obj->GetComponent<Dystopia::Transform>()->SetScale(Math::Vec4{ cScale.x, cScale.y + changeY, cScale.z, cScale.w });
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
					auto cScale = obj->GetComponent<Dystopia::Transform>()->GetGlobalScale();
					obj->GetComponent<Dystopia::Transform>()->SetScale(Math::Vec4{ cScale.x + changeX, cScale.y + changeX, cScale.z, cScale.w });
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

	void SceneView::DrawGizmoSingle(Dystopia::GameObject& obj)
	{
		Math::Pt3D curPos = obj.GetComponent<Dystopia::Transform>()->GetPosition();
		Math::Vec4 cScale = obj.GetComponent<Dystopia::Transform>()->GetGlobalScale();
		Math::Vec2 screenPos = GetWorldToScreen(curPos);
		float changeX = 0;
		float changeY = 0;
		static float ratio = 0;

		const auto scale = mpSceneCamera->GetOwner()->GetComponent<Dystopia::Transform>()->GetGlobalScale();

		switch (mCurrGizTool)
		{
		case eTRANSLATE:
			switch (EGUI::Gizmo2D::ArrowLeft("##LeftArrow", changeX, screenPos, 1.f, redColor, &mGizmoHovered))
			{
			case EGUI::eDRAGGING:
				obj.GetComponent<Dystopia::Transform>()->SetPosition(Math::Pt3D{ curPos.x + changeX * scale.x, curPos.y, curPos.z, curPos.w });
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
				obj.GetComponent<Dystopia::Transform>()->SetPosition(Math::Pt3D{ curPos.x, curPos.y + changeY * scale.y, curPos.z, curPos.w });
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
				obj.GetComponent<Dystopia::Transform>()->SetPosition(Math::Pt3D{ curPos.x + changeX * scale.x, curPos.y + changeY * scale.y, curPos.z, curPos.w });
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
				obj.GetComponent<Dystopia::Transform>()->SetScale(Math::Vec4{ cScale.x + changeX, cScale.y, cScale.z, cScale.w });
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
				obj.GetComponent<Dystopia::Transform>()->SetScale(Math::Vec4{ cScale.x, cScale.y + changeY, cScale.z, cScale.w });
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
				changeX += changeY;
				obj.GetComponent<Dystopia::Transform>()->SetScale(Math::Vec4{ cScale.x + changeX, cScale.y + ratio * changeX, cScale.z, cScale.w });
				mClearSelection = false;
				break;
			case EGUI::eSTART_DRAG:
				mClearSelection = false;
				ratio = cScale.y / cScale.x;
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


