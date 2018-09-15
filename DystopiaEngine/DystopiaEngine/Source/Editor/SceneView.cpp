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
#include "System\Driver\Driver.h"
#include "System\Graphics\GraphicsSystem.h"

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
		mDelta{}
	{}

	SceneView::~SceneView()
	{
		gpInstance = nullptr;
	}

	void SceneView::Init()
	{
		mpGfxSys = EngineCore::GetInstance()->GetSystem<GraphicsSystem>();
	}

	void SceneView::Update(const float& _dt)
	{
		mDelta = _dt;
		float px = Position().x;
		float py = Position().y;
		float sx = Size().x;
		float sy = Size().y;
		mpGfxSys->SetMasterViewport(static_cast<int>(px),
									   static_cast<int>(py),
									   static_cast<int>(sx),
									   static_cast<int>(sy));
		mpGfxSys->Update(mDelta);
	}

	void SceneView::EditorUI()
	{
		for (const auto& e : mpGfxSys->textureInfos)
		{
			ImGui::Image((void*)e.mID, ImVec2{ e.x, e.y });
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

}


#endif //EDITOR


