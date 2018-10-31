/* HEADER *********************************************************************************/
/*!
\file	SpriteEditor.h
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#ifndef _SPRITE_EDITOR_H_
#define _SPRITE_EDITOR_H_
#include "DataStructure/HashString.h"
#include "Math/Vector2.h"
#include "Editor/EditorTab.h"

namespace Dystopia
{
	class Texture;
	class TextureAtlas;
	class SpriteEditor : public EditorTab
	{
	public:
		static SpriteEditor* GetInstance(void);
		~SpriteEditor(void);

		void		Init(void);
		void		Update(const float&);
		void		EditorUI(void);
		void		Shutdown(void);
		std::string GetLabel(void) const;

		void SaveSettings(TextSerialiser&) const;
		void LoadSettings(TextSerialiser&);

	private:
		SpriteEditor(void);

		TextureAtlas *mpAtlas;
		Texture		 *mpTexture;

		std::string mLabel;
	};
}


#endif // _SPRITE_EDITOR_H_
#endif // EDITOR

