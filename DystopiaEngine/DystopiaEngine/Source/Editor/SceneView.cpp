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

#include "System/Driver/Driver.h"
#include "System/Scene/Scene.h"
#include "System/Graphics/Texture2D.h"
#include "System/Camera/CameraSystem.h"
#include "System/Graphics/GraphicsSystem.h"
#include "System/Input/InputSystem.h"

#include "Object/GameObject.h"
#include "Component/Transform.h"
#include "Component/Camera.h"
#include "Component/Renderer.h"
#include "Component/RigidBody.h"
#include "Component/ColliderList.h"

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

		GameObject *p = Factory::CreateCamera("Scene Camera");
		mpSceneCamera = GetCurrentScene()->InsertGameObject(Utility::Move(*p));
		mpSceneCamera->GetComponent<Camera>()->Init();
		mpSceneCamera->GetComponent<Transform>()->SetScale(Math::Vec4{ 1.f, 1.f, 1.f });
		delete p;

		////// Sample Box Object
		//GameObject *b = Factory::CreateBox("Box Object");
		//Texture2D* _bt = new Texture2D{ "Resource/Editor/red_box.png" };
		//mpBoxObject = GetCurrentScene()->InsertGameObject(Utility::Move(*b));
		//mpBoxObject->GetComponent<RigidBody>()->SetRestitution(1.0F);
		//mpBoxObject->GetComponent<RigidBody>()->SetStaticFriction(0.1F);
		//mpBoxObject->GetComponent<RigidBody>()->SetKineticFriction(0.4F);
		//mpBoxObject->GetComponent<Transform>()->SetScale(Math::Vec4{ 64.f, 64.f, 1.f });
		//mpBoxObject->GetComponent<Renderer>()->SetTexture(_bt);
		//mpBoxObject->GetComponent<Collider>()->Init();
		//mpBoxObject->GetComponent<RigidBody>()->Init();
		//delete b;

		//// Sample Box Object
		//GameObject *bb = Factory::CreateBox("Another box");
		//Texture2D* _bbt = new Texture2D{ "Resource/Editor/red_box.png" };
		//mpBoxObject2 = GetCurrentScene()->InsertGameObject(Utility::Move(*bb));
		//mpBoxObject2->GetComponent<Transform>()->SetScale(Math::Vec4{ 64.f, 64.f, 1.f });
		//mpBoxObject2->GetComponent<Transform>()->SetGlobalPosition({ 50.0F, 100.0f, 1 });
		//mpBoxObject2->GetComponent<Renderer>()->SetTexture(_bbt);
		//mpBoxObject2->GetComponent<Collider>()->Init();
		//mpBoxObject2->GetComponent<RigidBody>()->SetRestitution(0.4F);
		//mpBoxObject2->GetComponent<RigidBody>()->SetStaticFriction(0.2F);
		//mpBoxObject2->GetComponent<RigidBody>()->SetKineticFriction(0.6F);
		//mpBoxObject2->GetComponent<RigidBody>()->Init();
		//delete bb;

		//bb = Factory::CreateBox("Another box");
		//mpBoxObject2 = GetCurrentScene()->InsertGameObject(Utility::Move(*bb));
		//mpBoxObject2->GetComponent<Transform>()->SetScale(Math::Vec4{ 64.f, 64.f, 1.f });
		//mpBoxObject2->GetComponent<Transform>()->SetGlobalPosition({ 50.0F, -100.0f, 1 });
		//mpBoxObject2->GetComponent<Renderer>()->SetTexture(_bbt);
		//mpBoxObject2->GetComponent<Collider>()->Init();
		//mpBoxObject2->GetComponent<RigidBody>()->SetRestitution(0.5F);
		//mpBoxObject2->GetComponent<RigidBody>()->SetStaticFriction(0.1F);
		//mpBoxObject2->GetComponent<RigidBody>()->SetKineticFriction(0.6F);
		//mpBoxObject2->GetComponent<RigidBody>()->Init();
		//delete bb;

		//// Sample Circle Object
		////GameObject *c = Factory::CreateCircle("Circle Object");
		////Texture2D* _ct = new Texture2D{ "Resource/Editor/red_circle.png" };
		////mpCircleObject = GetCurrentScene()->InsertGameObject(Utility::Move(*c));
		////mpCircleObject->GetComponent<RigidBody>()->Init();
		////mpCircleObject->GetComponent<RigidBody>()->SetRestitution(1.0F);
		////mpCircleObject->GetComponent<RigidBody>()->SetStaticFriction(0.3F);
		////mpCircleObject->GetComponent<RigidBody>()->SetKineticFriction(0.3F);
		////mpCircleObject->GetComponent<Transform>()->SetGlobalPosition({ 60.0F, 150.0f, 0});
		////mpCircleObject->GetComponent<Transform>()->SetScale(Math::Vec3D{ 100, 100.f, 1.f });
		////mpCircleObject->GetComponent<Renderer>()->SetTexture(_ct);
		////mpCircleObject->GetComponent<Collider>()->Init();
		////delete c;

		////GameObject *cc = Factory::CreateCircle("Static Circle");
		////mpStaticCircleObject = GetCurrentScene()->InsertGameObject(Utility::Move(*cc));
		//////Texture2D* _ct = new Texture2D{ "Resource/Editor/red_circle.png" };
		////mpStaticCircleObject->GetComponent<RigidBody>()->Init();
		////mpStaticCircleObject->GetComponent<RigidBody>()->Set_IsStatic(true);
		////mpStaticCircleObject->GetComponent<RigidBody>()->SetRestitution(0.4F);
		////mpStaticCircleObject->GetComponent<RigidBody>()->SetStaticFriction(0.3F);
		////mpStaticCircleObject->GetComponent<RigidBody>()->SetKineticFriction(0.3F);
		////mpStaticCircleObject->GetComponent<Transform>()->SetGlobalPosition({ 0.0f, -185.f, 0});
		////mpStaticCircleObject->GetComponent<Transform>()->SetScale(Math::Vec3D{ 100.f, 100.f, 1.f });
		////mpStaticCircleObject->GetComponent<Renderer>()->SetTexture(_ct);
		////mpStaticCircleObject->GetComponent<Collider>()->Init();
		////delete cc;

		//// Sample Static Object
		//GameObject *s = Factory::CreateStaticBox("Ground");
		//Texture2D* _st = new Texture2D{ "Resource/Editor/white_box.png" };
		//mpStaticBoxObject = GetCurrentScene()->InsertGameObject(Utility::Move(*s));
		//mpStaticBoxObject->GetComponent<RigidBody>()->SetRestitution(0.5F);
		//mpStaticBoxObject->GetComponent<RigidBody>()->SetStaticFriction(0.1F);
		//mpStaticBoxObject->GetComponent<RigidBody>()->SetKineticFriction(0.4F);
		//mpStaticBoxObject->GetComponent<Transform>()->SetGlobalPosition({ 0, -240.f, 1 });
		//mpStaticBoxObject->GetComponent<Transform>()->SetScale(Math::Vec3D{ 2048.f, 32.f, 1.f });
		//mpStaticBoxObject->GetComponent<RigidBody>()->SetMass(0.0F);
		//mpStaticBoxObject->GetComponent<Collider>()->Init();
		//mpStaticBoxObject->GetComponent<RigidBody>()->Init();
		//mpStaticBoxObject->GetComponent<Renderer>()->SetTexture(_st);
		//delete s;

		//s = Factory::CreateStaticBox("Left");
		//mpStaticBoxObject = GetCurrentScene()->InsertGameObject(Utility::Move(*s));
		//mpStaticBoxObject->GetComponent<RigidBody>()->SetRestitution(0.5F);
		//mpStaticBoxObject->GetComponent<RigidBody>()->SetStaticFriction(0.4F);
		//mpStaticBoxObject->GetComponent<RigidBody>()->SetKineticFriction(0.4F);
		//mpStaticBoxObject->GetComponent<Transform>()->SetGlobalPosition({ -390.0F, 0.0F, 1 });
		//mpStaticBoxObject->GetComponent<Transform>()->SetScale(Math::Vec3D{ 32.f, 1024.0f, 1.f });
		//mpStaticBoxObject->GetComponent<Collider>()->Init();
		//mpStaticBoxObject->GetComponent<RigidBody>()->SetMass(0.0F);
		//mpStaticBoxObject->GetComponent<Renderer>()->SetTexture(_st);
		//mpStaticBoxObject->GetComponent<RigidBody>()->Init();
		//delete s;

		//s = Factory::CreateStaticBox("Right");
		//mpStaticBoxObject = GetCurrentScene()->InsertGameObject(Utility::Move(*s));
		//mpStaticBoxObject->GetComponent<RigidBody>()->SetRestitution(0.5F);
		//mpStaticBoxObject->GetComponent<RigidBody>()->SetStaticFriction(0.1F);
		//mpStaticBoxObject->GetComponent<RigidBody>()->SetKineticFriction(0.4F);
		//mpStaticBoxObject->GetComponent<Transform>()->SetGlobalPosition({ 390.0F, 0.0F, 1 });
		//mpStaticBoxObject->GetComponent<Transform>()->SetScale(Math::Vec3D{ 32.f, 1024.0f, 1.f });
		//mpStaticBoxObject->GetComponent<Collider>()->Init();
		//mpStaticBoxObject->GetComponent<RigidBody>()->SetMass(0.0F);
		//mpStaticBoxObject->GetComponent<Renderer>()->SetTexture(_st);
		//mpStaticBoxObject->GetComponent<RigidBody>()->Init();
		//delete s;

		//s = Factory::CreateStaticBox("Top");
		//mpStaticBoxObject = GetCurrentScene()->InsertGameObject(Utility::Move(*s));
		//mpStaticBoxObject->GetComponent<RigidBody>()->SetRestitution(0.5F);
		//mpStaticBoxObject->GetComponent<RigidBody>()->SetStaticFriction(0.1F);
		//mpStaticBoxObject->GetComponent<RigidBody>()->SetKineticFriction(0.4F);
		//mpStaticBoxObject->GetComponent<Transform>()->SetGlobalPosition({ 0.0F, 240.0F, 1 });
		//mpStaticBoxObject->GetComponent<Transform>()->SetScale(Math::Vec3D{ 1024.f, 32.f, 1.f });
		//mpStaticBoxObject->GetComponent<Collider>()->Init();
		//mpStaticBoxObject->GetComponent<RigidBody>()->SetMass(0.0F);
		//mpStaticBoxObject->GetComponent<Renderer>()->SetTexture(_st);
		//mpStaticBoxObject->GetComponent<RigidBody>()->Init();
		//delete s;

		SceneChanged();
	}

	void SceneView::Update(const float& _dt)
	{
		mDelta = _dt;
		const auto * input = EngineCore::GetInstance()->GetSystem<InputManager>();
		if (GetMainEditor().CurrentState() == EDITOR_MAIN)
		{
			mpGfxSys->Update(mDelta);
		}

		if (GetMainEditor().CurrentState() == EDITOR_PLAY)
		{
			if (input->IsKeyTriggered(eUserButton::MOUSE_M))
			{
				/*mpCircleObject->GetComponent<Transform>()->SetGlobalPosition({ 0,90,0,0 });
				mpCircleObject->GetComponent<RigidBody>()->SetVelocity({ 0,0,0,0 });*/
				mpBoxObject->GetComponent<Transform>()->SetGlobalPosition({ 0,0,0,0 });
				mpBoxObject->GetComponent<RigidBody>()->SetVelocity({ 0,0,0,0 });
			}

			if (input->IsKeyPressed(eUserButton::BUTTON_LEFT))
			{
				mpBoxObject->GetComponent<RigidBody>()->AddForce({ -10,0,0 });
			}
			if (input->IsKeyPressed(eUserButton::BUTTON_RIGHT))
			{
				mpBoxObject->GetComponent<RigidBody>()->AddForce({ 10,0,0 });
			}
			if (input->IsKeyPressed(eUserButton::BUTTON_SPACEBAR))
			{
				mpBoxObject->GetComponent<RigidBody>()->AddForce({ 0,15,0 });
			}
		}

	}

	void SceneView::EditorUI()
	{
		if (ImGui::IsMouseClicked(0))
			mAmFocused = false;
		if (mDragging && !ImGui::IsMouseDown(1))
			mDragging = false;

		EGUI::UnIndent(2);
		size_t id = mpGfxSys->GetFrameBuffer().AsTexture()->GetID();
		mImgSize = Math::Vec2{ Size().x,  Size().y - imageOffsetY };

		if (GetMainEditor().CurrentState() == EDITOR_PLAY)
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 1.f);

		if (ImGui::ImageButton(reinterpret_cast<void*>(id), mImgSize, ImVec2{ 0,0 }, ImVec2{ 1,1 }, 0))
			mAmFocused = true;

		if (GetMainEditor().CurrentState() == EDITOR_PLAY)
			ImGui::PopStyleVar();

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

		EGUI::Indent(2);
		mToZoom = eZOOM_NONE;
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

	void SceneView::FindMouseObject()
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
			}
		}
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

}


#endif //EDITOR


