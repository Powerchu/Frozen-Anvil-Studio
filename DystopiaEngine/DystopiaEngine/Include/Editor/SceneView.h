/* HEADER *********************************************************************************/
/*!
\file	SceneView.h
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief
BRIEF HERE

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "System/Physics/PhysicsSystem.h"
#if EDITOR
#ifndef _SCENE_VIEW_H_
#define _SCENE_VIEW_H_
#include "EditorTab.h"

namespace Dystopia
{
	class GraphicsSystem;
	class GameObject;
	class Camera;
	class EditorInput;

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

		void SetSensitivity(float);
		float GetSensitivity() const;

	private:
		SceneView(void);

		enum eZoom 
		{
			eZOOM_IN,
			eZOOM_OUT,
			eZOOM_NONE
		};

		bool			mDragging;
		bool			mAmFocused;
		float			mDelta;
		float			mSensitivity;
		float			mMoveSens;
		eZoom			mToZoom;
		std::string		mLabel;
		GraphicsSystem	*mpGfxSys;
		GameObject		*mpSceneCamera;
		EditorInput		*mpEditorInput;
		Math::Vec2		mImgSize;
		Math::Vec2		mMoveVec;

		GameObject*		mpBoxObject;
		GameObject*		mpBoxObject2;
		GameObject*		mpStaticBoxObject;
		
		void			ScrollIn();
		void			ScrollOut();
		void			SceneChanged();
		void			Zoom(bool);
		void			Move();
		void			FindMouseObject();
		Math::Pt3D		GetWorldClickPos(const Camera * const _cam) const;
		Math::Vec2		FindMouseVector();
		Camera*			GetCamera();
	};

}

#endif //_SCENE_VIEW_H_
#endif //EDITOR

