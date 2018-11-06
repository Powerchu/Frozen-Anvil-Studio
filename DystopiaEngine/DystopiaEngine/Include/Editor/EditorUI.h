/* HEADER *********************************************************************************/
/*!
\file	EditorMain.h
\author Digipen (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#ifndef _EDITOR_UI_H_
#define _EDITOR_UI_H_
#include "Editor/EditorSystem.h"
#include "Editor/EditorGLState.h"

#include "DataStructure/HashString.h"

namespace Editor
{
	class EditorUI : public EditorSystem
	{
	public:
		EditorUI(void);
		~EditorUI(void);
		void Load(void);
		bool Init(void);
		void StartFrame(void);
		void Update(float);
		void EndFrame(void);
		void Shutdown(void);
		void Message(eEMessage);
		void SaveSettings(Dystopia::TextSerialiser& _out) const;
		void LoadSettings(Dystopia::TextSerialiser& _in);

		void SetLauncherMode(bool);

	private:
		GLState			mGLState;
		HashString		mGlslVersion;
		bool			mMouseJustPressed[3];
		int				mShaderHandle;
		int				mVertHandle;
		int				mFragHandle;
		int				mAttribLocationTex;
		int				mAttribLocationProjMtx;
		int				mAttribLocationPosition;
		int				mAttribLocationUV;
		int				mAttribLocationColor;
		unsigned int	mVboHandle;
		unsigned int	mElementsHandle;
		unsigned int    mFont;

		bool mbLauncherMode;

		void StartDock(void);
		void EndDock(void);
		void DefaultFont(void);
	};

}

#endif
#endif




