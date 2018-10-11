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
#if EDITOR
#ifndef _SCENE_VIEW_H_
#define _SCENE_VIEW_H_
#include "EditorTab.h"
#include "DataStructure/AutoArray.h"

namespace Dystopia
{
	class GraphicsSystem;
	class GameObject;
	class Camera;
	class Texture;
	struct File;

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

		bool			mClearSelection;
		bool			mDragging;
		bool			mAmFocused;
		float			mSensitivity;
		float			mMoveSens;
		eZoom			mToZoom;
		std::string		mLabel;
		GraphicsSystem	*mpGfxSys;
		GameObject		*mpSceneCamera;
		Math::Vec2		mImgSize;
		Math::Vec2		mMoveVec;
		Math::Vec2		mImgPos;
		
		void			ScrollIn();
		void			ScrollOut();
		void			SceneChanged();
		void			Zoom(bool);
		void			Move();
		GameObject*		FindMouseObject();
		void			ResetMouseEvents();
		void			CheckMouseEvents();
		void			AcceptPrefab(File *t);
		void			AcceptTexture(File *t);
		Math::Pt3D		GetWorldClickPos(const Camera * const _cam) const;
		Math::Vec2		FindMouseVector();
		void			AdjustImageSize(Texture*);
		void			AdjustDisplayPos(void);
		Math::Vec2		GetAdjustedRatio(float _sX, float _sY, float _iX, float _iY);
		Math::Vec2		GetAdjustedPosition(float _sX, float _sY, float _iX, float _iY);
		Math::Vec2		GetWorldToScreen(const Math::Pt3D&) const;
		Camera*			GetCamera();
		void			DrawGizmos(void);
		void			DrawGizmoSingle(GameObject&);
		void			DrawGizmoMul(const AutoArray<GameObject*>&);
	};

}

#endif //_SCENE_VIEW_H_
#endif //EDITOR

