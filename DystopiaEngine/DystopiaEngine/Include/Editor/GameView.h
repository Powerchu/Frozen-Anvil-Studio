/* HEADER *********************************************************************************/
/*!
\file	GameView.h
\author Aaron (100%)
\par    email: m.chu\@digipen.edu
\brief
BRIEF HERE

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#ifndef _GAME_VIEW_H_
#define _GAME_VIEW_H_
#include "Editor/EditorPanel.h"
#include "DataStructure/AutoArray.h"
#include "DataStructure/HashString.h"

namespace Dystopia
{
	class GraphicsSystem;
	class GameObject;
	class Camera;
	class Texture;
	class SceneSystem;
}

namespace Editor
{
	struct File;

	class GameView : public EditorPanel //EditorTab
	{
	public:
		//static SceneView* GetInstance();
		GameView(void);
		void Load(void);
		bool Init(void);
		void Update(float);
		void AdjustImageSize(Dystopia::Texture* texture);
		void AdjustDisplayPos();
		Math::Vec2 GetAdjustedRatio(float _sX, float _sY, float _iX, float _iY);
		Math::Vec2 GetAdjustedPosition(float _sX, float _sY, float _iX, float _iY);
		void EditorUI(void);
		void Shutdown(void);
		void Message(eEMessage);
		void SaveSettings(Dystopia::TextSerialiser& _out) const;
		void LoadSettings(Dystopia::TextSerialiser& _in);
		HashString GetLabel(void) const;
		~GameView(void);

	private:
	
		HashString	mLabel;

		Dystopia::Camera			*mpGameCamera = nullptr;
		Dystopia::GraphicsSystem	*mpGfxSys = nullptr;
		Dystopia::SceneSystem		*mpSceneSys = nullptr;
		Dystopia::CameraSystem		*mpCamSys = nullptr;

		Math::Vec2	mImgSize;
		Math::Vec2	mImgPos;

		Dystopia::Camera*		GetCamera();
		void					SceneChanged();
	};

}

#endif //_GAME_VIEW_H
#endif //EDITOR

