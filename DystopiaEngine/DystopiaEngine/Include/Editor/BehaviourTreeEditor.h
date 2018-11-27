/* HEADER *********************************************************************************/
/*!
\file	BehaviourTreeEditor.h
\author Aaron Chu (100%)
\par    email: m.chu\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#ifndef _BEHAVIOURTREE_H_
#define _BEHAVIOURTREE_H_
#include "Math/Vector2.h"
#include "DataStructure/HashString.h"
//#include "Editor/EditorTab.h"
#include "Editor/EditorPanel.h"

namespace Dystopia
{
	class Texture;
	class TextSerialiser;
	class GraphicsSystem;
}
namespace Editor
{
	class BehaviourTreeEditor : public EditorPanel//EditorTab
	{
	public:
		//static SpritePreviewer* GetInstance(void);
		BehaviourTreeEditor(void);
		~BehaviourTreeEditor(void);

		void Load(void) override;
		bool Init(void) override;
		void Update(float) override;
		void EditorUI(void) override;
		void Shutdown(void) override;
		void Message(eEMessage) override;
		void SaveSettings(Dystopia::TextSerialiser& _out) const override;
		void LoadSettings(Dystopia::TextSerialiser& _in) override;
		HashString GetLabel(void) const override;

	private:
		Dystopia::Texture* mpTargetTexture;
		Dystopia::GraphicsSystem* mpGfxSys;

		float		mImageRatio;
		float		mImageH;
		float		mImageW;
		Math::Vec2  mDisplaySize;
		HashString  mTextureName;
		HashString  mLabel;

		HashString  GetTextureName(void);
	};
}


#endif // _BEHAVIOURTREE_H_
#endif // EDITOR

