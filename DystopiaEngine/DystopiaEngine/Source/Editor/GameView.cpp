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
		mpCamSys = Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::CameraSystem>();
		
		// Instantiate New Main Camera
		EditorMain::GetInstance()->GetSystem<EditorFactory>()->DefaultGameCamera();
		SetScrollEnabled(false);
		//SceneChanged();
		return true;
	}

	void GameView::Update(float)
	{
		if (mpCamSys->GetMasterCamera())
		{
			if (nullptr != mpCamSys->GetMasterCamera()->GetOwner())
			{
				mpGameCamera = mpCamSys->GetMasterCamera();
			}
		}
		/*Dystopia::GameObject *temp = mpSceneSys->GetCurrentScene().FindGameObject("___Scene_Camera___");
		if (temp)
			mpGameCamera = temp->GetComponent<Dystopia::Camera>();
		if (EditorMain::GetInstance()->GetCurState() == eState::MAIN)
			mpGfxSys->Update(_dt);
*/
	}



	void GameView::EditorUI(void)
	{
		if (mpGameCamera)
		{
			EGUI::UnIndent(2);
			if (EditorMain::GetInstance()->GetCurState() == eState::PLAY)
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 1.f);

			// because game view should show final display with ui layer together, while game camera only paints world object
			Dystopia::Texture* pTex = mpGfxSys->GetFrameBuffer().AsTexture();

			AdjustImageSize(pTex);
			AdjustDisplayPos();

			const auto orig = ImGui::GetCursorPos();
			ImGui::SetCursorPos(ImVec2{ orig.x + mImgPos.x, orig.y + mImgPos.y - 1.f });
			EGUI::Display::Image(pTex->GetID(), mImgSize);
			ImGui::SetCursorPos(ImVec2{ orig.x, orig.y + mImgSize.y });

			if (EditorMain::GetInstance()->GetCurState() == eState::PLAY)
				ImGui::PopStyleVar();

			EGUI::Indent(2);
		}
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

	const HashString& GameView::GetLabel(void) const
	{
		return mLabel;
	}

	void GameView::AdjustImageSize(Dystopia::Texture* texture)
	{
		/*const auto camSize = mpGameCamera->GetSize();
		static const float aspect = camSize.x / camSize.y;
		float ix = static_cast<float>(aspect * texture->GetWidth());
		float iy = static_cast<float>(texture->GetHeight());*/
		float sx = Size().x;
		float sy = Size().y - EGUI::TabsImageOffsetY;

		float minScale = Math::Min(sx / static_cast<float>(texture->GetWidth()), 
								   sy / static_cast<float>(texture->GetHeight()));

		//float ix = static_cast<float>(aspect * texture->GetWidth());
		//float iy = static_cast<float>(texture->GetHeight());

		mImgSize = Math::Vec2{ texture->GetWidth() * minScale, texture->GetHeight() * minScale};// GetAdjustedRatio(sx, sy, ix, iy);
	}

	void GameView::AdjustDisplayPos()
	{
		mImgPos = GetAdjustedPosition(Size().x, Size().y - EGUI::TabsImageOffsetY, mImgSize.x, mImgSize.y);
	}

	Math::Vec2 GameView::GetAdjustedRatio(float _sX, float _sY, float _iX, float _iY)
	{
		float iRatio = _iX / _iY;
		float sRatio = _sX / _sY;
		return sRatio > iRatio ? Math::Vec2{ _iX * (_sY / _iY), _sY } : Math::Vec2{ _sX, _iY * (_sX / _iX) };
	}

	Math::Vec2 GameView::GetAdjustedPosition(float _sX, float _sY, float _iX, float _iY)
	{
		float xDiff = _sX - _iX;
		float yDiff = _sY - _iY;
		return Math::Vec2{ xDiff / 2, yDiff / 2 };
	}

	void GameView::SceneChanged()
	{
		if (mpCamSys->GetMasterCamera())
		{
			if (nullptr != mpCamSys->GetMasterCamera()->GetOwner())
			{
				mpGameCamera = mpCamSys->GetMasterCamera();
			}
		}
	}

	Dystopia::Camera* GameView::GetCamera()
	{
		return mpGameCamera;
	}
}


#endif //EDITOR


