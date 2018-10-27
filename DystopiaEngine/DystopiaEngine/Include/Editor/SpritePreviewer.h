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
#include "Math/Vector2.h"
#include "Editor/EditorTab.h"

namespace Dystopia
{
	class Texture;
	class TextSerialiser;
	class SpritePreviewer : public EditorTab
	{
	public:
		static SpritePreviewer* GetInstance(void);
		~SpritePreviewer(void);

		void		Init(void);
		void		Update(const float&);
		void		EditorUI(void);
		void		Shutdown(void);
		std::string GetLabel(void) const;

		void SaveSettings(TextSerialiser& _out) const;
		void LoadSettings(TextSerialiser& _in);

	private:
		SpritePreviewer(void);

		Texture*	mpTargetTexture;
		float		mImageRatio;
		float		mImageH;
		float		mImageW;
		Math::Vec2  mDisplaySize;
		std::string mTextureName;
		std::string mLabel;

		std::string GetTextureName(void);
	};
}


#endif // _SPRITE_PREVEIWER_H_
#endif // EDITOR

