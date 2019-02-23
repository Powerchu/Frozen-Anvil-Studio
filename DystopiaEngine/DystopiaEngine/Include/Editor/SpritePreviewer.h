/* HEADER *********************************************************************************/
/*!
\file	SpritePreviewer.h
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#ifndef _SPRITE_PREVEIWER_H_
#define _SPRITE_PREVEIWER_H_
#include "Math/Vectors.h"
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
	class SpritePreviewer : public EditorPanel//EditorTab
	{
	public:
		//static SpritePreviewer* GetInstance(void);
		SpritePreviewer(void);
		~SpritePreviewer(void);

		void Load(void);
		bool Init(void);
		void Update(float);
		void EditorUI(void);
		void Shutdown(void);
		void Message(eEMessage);
		void SaveSettings(Dystopia::TextSerialiser& _out) const;
		void LoadSettings(Dystopia::TextSerialiser& _in);
		HashString GetLabel(void) const;

	private:
		Dystopia::Texture* mpTargetTexture;
		Dystopia::GraphicsSystem* mpGfxSys;

		float		mImageRatio;
		float		mImageH;
		float		mImageW;
		Math::Vec2  mDisplaySize;
		HashString  mTextureName;
		HashString  mLabel;

		HashString  GetTextureName(Dystopia::Texture* _pTex) const;
	};
}


#endif // _SPRITE_PREVEIWER_H_
#endif // EDITOR

