/* HEADER *********************************************************************************/
/*!
\file	GameView.cpp
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
#include "Editor/EGUI.h"
#include "Editor/ConsoleLog.h"
#include "Editor/EditorStates.h"

#include "Editor/EInput.h"
#include "Editor/EditorMain.h"
#include "Editor/EditorFactory.h"

#include "System/Input/InputSystem.h"
#include "System/Driver/Driver.h"
#include "System/Scene/Scene.h"
#include "System/Scene/SceneSystem.h"
#include "System/Graphics/Texture2D.h"
#include "System/Camera/CameraSystem.h"
#include "System/Graphics/GraphicsSystem.h"

#include "Utility/GUID.h"
#include "Utility/DebugAssert.h"

#include "Object/GameObject.h"
#include "Object/ObjectFlags.h"

#include "Component/Transform.h"
#include "Component/Camera.h"
#include "Component/SpriteRenderer.h"
#include "Behaviour/Behaviour.h"


#include "Math/MathUtility.h"

#include "Editor/GameView.h"

namespace Editor
{

	GameView::GameView(void)
		: mLabel("Game View")
	{
		EditorPanel::SetOpened(true);
	}

	GameView::~GameView(void)
	{
	}

	void GameView::Load(void)
	{}

	bool GameView::Init(void)
	{
		mpGfxSys = Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::GraphicsSystem>();
		mpSceneSys = Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::SceneSystem>();
		EditorMain::GetInstance()->GetSystem<EditorFactory>()->DefaultGameCamera();
		//SceneChanged();
		return true;
	}

	void GameView::Update(float _dt)
	{
		/*Dystopia::GameObject *temp = mpSceneSys->GetCurrentScene().FindGameObject("___Scene_Camera___");
		if (temp)
			mpGameCamera = temp->GetComponent<Dystopia::Camera>();
		if (EditorMain::GetInstance()->GetCurState() == eState::MAIN)
			mpGfxSys->Update(_dt);
*/
	}

	void GameView::EditorUI(void)
	{
		/*EGUI::UnIndent(2);
		if (EditorMain::GetInstance()->GetCurState() == eState::PLAY)
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 1.f);

		Dystopia::Texture *pTex = mpGfxSys->GetFrameBuffer().AsTexture();

		if (mpGameCamera)
			pTex = mpGameCamera->GetSurface()->AsTexture();

	

		const auto orig = ImGui::GetCursorPos();
		ImGui::SetCursorPos(ImVec2{ orig.x + mImgPos.x, orig.y + mImgPos.y - 1.f });
		EGUI::Display::Image(pTex->GetID(), mImgSize);
		if (ImGui::IsItemHovered())
			mAmFocused = true;
		else
			mAmFocused = false;
		ImGui::SetCursorPos(ImVec2{ orig.x, orig.y + mImgSize.y });

		if (mpSceneSys->GetCurrentScene().FindGameObject("___Scene_Camera___"))
		{
			if (const auto t = EGUI::Display::StartPayloadReceiver<::Editor::File>(EGUI::PREFAB))
			{
				AcceptPrefab(t);
				EGUI::Display::EndPayloadReceiver();
			}
			if (const auto t = EGUI::Display::StartPayloadReceiver<::Editor::File>(EGUI::PNG))
			{
				AcceptTexture(t);
				EGUI::Display::EndPayloadReceiver();
			}
			if (const auto t = EGUI::Display::StartPayloadReceiver<::Editor::File>(EGUI::DDS))
			{
				AcceptTexture(t);
				EGUI::Display::EndPayloadReceiver();
			}
			if (const auto t = EGUI::Display::StartPayloadReceiver<::Editor::File>(EGUI::BMP))
			{
				AcceptTexture(t);
				EGUI::Display::EndPayloadReceiver();
			}
		}
		if (const auto t = EGUI::Display::StartPayloadReceiver<::Editor::File>(EGUI::SCENE))
		{
			EditorMain::GetInstance()->GetSystem<EditorStates>()->OpenScene(t->mPath);
			EGUI::Display::EndPayloadReceiver();
		}

		if (EditorMain::GetInstance()->GetCurState() == eState::PLAY)
			ImGui::PopStyleVar();



		EGUI::Indent(2);*/
	}

	void GameView::Shutdown(void)
	{
		mpGfxSys = nullptr;
		mpSceneSys = nullptr;
		mpGameCamera = nullptr;
	}

	void GameView::Message(eEMessage _msg)
	{
		if (_msg == eEMessage::SCENE_CHANGED)
		{
			if (!Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::SceneSystem>()->GetCurrentScene().FindGameObject("Main Camera"))
			{
				EditorMain::GetInstance()->GetSystem<EditorFactory>()->DefaultGameCamera();
			}
		}
	}

	void GameView::SaveSettings(Dystopia::TextSerialiser&) const
	{}

	void GameView::LoadSettings(Dystopia::TextSerialiser&)
	{}

	HashString GameView::GetLabel(void) const
	{
		return mLabel;
	}

	void GameView::SceneChanged()
	{
 		//mpGameCamera = mpSceneSys->GetCurrentScene().FindGameObject("___Scene_Camera___")->GetComponent<Dystopia::Camera>();
	}

	Dystopia::Camera* GameView::GetCamera()
	{
		return mpGameCamera;
	}
}


#endif //EDITOR


