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
#include "System\Camera\CameraSystem.h"
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
		mpCameraSys{ nullptr },
		mpGfxSys{ nullptr }
	{}

	SceneView::~SceneView()
	{
		gpInstance = nullptr;
	}

	void SceneView::Init()
	{
		mpCameraSys = EngineCore::GetInstance()->GetSystem<CameraSystem>();
		mpGfxSys = EngineCore::GetInstance()->GetSystem<GraphicsSystem>();
	}

	void SceneView::Update(const float& _dt)
	{
		mpCameraSys->SetMasterViewport(static_cast<int>(Position().x),
									   static_cast<int>(Position().y),
									   static_cast<int>(Size().x),
									   static_cast<int>(Size().y));
		mpGfxSys->Update(_dt);
	}

	void SceneView::EditorUI()
	{
	}

	void SceneView::Shutdown()
	{
		mpCameraSys = nullptr;
	}

	std::string SceneView::GetLabel() const
	{
		return mLabel;
	}

}


#endif //EDITOR


