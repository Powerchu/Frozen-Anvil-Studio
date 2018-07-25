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
#include "Editor\SceneView.h"

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
		mLabel{ "Scene View" }
	{}

	SceneView::~SceneView()
	{}

	void SceneView::Init()
	{
	}

	void SceneView::Update(const float& _dt)
	{
		_dt;
	}

	void SceneView::Window()
	{
	}

	void SceneView::Shutdown()
	{
	}

	std::string SceneView::GetLabel() const
	{
		return mLabel;
	}

}


#endif //EDITOR

