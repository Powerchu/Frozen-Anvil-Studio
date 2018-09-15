/* HEADER *********************************************************************************/
/*!
\file	SceneView.h
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
#ifndef _SCENE_VIEW_H_
#define _SCENE_VIEW_H_
#include "EditorTab.h"

namespace Dystopia
{
	class GraphicsSystem;
	class SceneView : public EditorTab
	{
	public:
		static SceneView* GetInstance();
		~SceneView();

		/* Init() is called immediately after the creation of the object */
		virtual void Init();

		/* Update() is called before Window(), so alter most variables (frame based) here to be printed in Window() later */
		virtual void Update(const float&);

		/* Window() is where you do the EGUI/IMGUI functions. GUI variable changes will be recorded here */
		virtual void EditorUI();

		/* Shutdown() is called right before deleting this object */
		virtual void Shutdown();

		/* GetLabel() returns the string to identify this class. EditorTab requires this to create a tab for you using the label */
		virtual std::string GetLabel() const;

	private:
		SceneView(void);

		float			mDelta;
		std::string		mLabel;
		GraphicsSystem* mpGfxSys;
	};

}

#endif //_SCENE_VIEW_H_
#endif //EDITOR

