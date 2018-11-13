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
//#include "EditorTab.h"
#include "Editor/EditorPanel.h"
#include "DataStructure/AutoArray.h"
#include "DataStructure/HashString.h"

namespace Dystopia
{
	class GraphicsSystem;
	class GameObject;
	class Camera;
	class Texture;
}

namespace Editor
{
	struct File;

	class SceneView : public EditorPanel//EditorTab
	{
	public:
		//static SceneView* GetInstance();
		SceneView(void);
		void Load(void);
		bool Init(void);
		void Update(float);
		void EditorUI(void);
		void Shutdown(void);
		void Message(eEMessage);
		void SaveSettings(Dystopia::TextSerialiser& _out) const;
		void LoadSettings(Dystopia::TextSerialiser& _in);
		HashString GetLabel(void) const;
		~SceneView(void)

		void SetSensitivity(float);
		float GetSensitivity(void) const;

	private:
		enum eGizTool
		{
			eTRANSLATE,
			eSCALE,
			eROTATE
		};

		enum eZoom 
		{
			eZOOM_IN,
			eZOOM_OUT,
			eZOOM_NONE
		};

		eGizTool		mCurrGizTool;
		bool			mGizmoHovered;
		bool			mClearSelection;
		bool			mDragging;
		bool			mAmFocused;
		float			mSensitivity;
		float			mMoveSens;
		eZoom			mToZoom;
		HashString		mLabel;
		Dystopia::GraphicsSystem *mpGfxSys;
		Dystopia::Camera *mpSceneCamera;
		Math::Vec2		mImgSize;
		Math::Vec2		mImgPos;
		Math::Vec2		mPrevMovePoint;
		
		void			ScrollIn();
		void			ScrollOut();
		void			SceneChanged();
		void			Zoom(bool);
		void			Move();
		Dystopia::GameObject* FindMouseObject();
		void			ResetMouseEvents();
		void			CheckMouseEvents();
		void			AcceptPrefab(::Editor::File *t);
		void			AcceptTexture(::Editor::File *t);
		Math::Pt3D		GetWorldClickPos(const Dystopia::Camera * const _cam) const;
		void			AdjustImageSize(Dystopia::Texture*);
		void			AdjustDisplayPos(void);
		Math::Vec2		GetAdjustedRatio(float _sX, float _sY, float _iX, float _iY);
		Math::Vec2		GetAdjustedPosition(float _sX, float _sY, float _iX, float _iY);
		Math::Vec2		GetWorldToScreen(const Math::Pt3D&);
		Dystopia::Camera* GetCamera();
		void			DrawGizmos(void);
		void			DrawGizmoSingle(Dystopia::GameObject&);
		void			DrawGizmoMul(const AutoArray<Dystopia::GameObject*>&);

		void			SetGizmoTranslate(void);
		void			SetGizmoScaler(void);
	};

}

#endif //_SCENE_VIEW_H_
#endif //EDITOR

