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
#include "Math/Vectors.h"
#include "Editor/EditorPanel.h"

namespace Dystopia
{
	class Texture;
	class TextureAtlas;
	class GraphicsSystem;
	class TextureSystem;
}

namespace Editor
{
	class SpriteEditor : public EditorPanel 
	{
	public:
		//static SpriteEditor* GetInstance(void);
		SpriteEditor(void);
		~SpriteEditor(void);

		void Load(void);
		bool Init(void);
		void Update(float);
		void EditorUI(void);
		void Shutdown(void);
		void Message(eEMessage);
		void SaveSettings(Dystopia::TextSerialiser& _out) const;
		void LoadSettings(Dystopia::TextSerialiser& _in);
		const HashString& GetLabel(void) const;

	private:

		Dystopia::TextureAtlas *mpAtlas;
		Dystopia::Texture *mpTexture;
		Dystopia::GraphicsSystem *mpGfxSys;
		Dystopia::TextureSystem *mpTextSys;

		HashString mLabel;
		Math::Vec2 mSettingsArea;;
		Math::Vec2 mPreviewArea;
		Math::Vec2 mSectionEditArea;

		int mnSelectedSection;
		Math::Vec2 mSectionPos;
		Math::Vec2 mSectionSize;
		Math::Vec2 mSectionDime;



		void AtlasArea(float,float);
		void SettingsArea(float, float);
		void PreviewArea(float, float);

		Math::Vec2 AdjustAspectSize(float, float, float , float);

		void FieldTexture(void);
		void FieldAtlas(void);

		void DrawSelectedGrid(float, float, float, float);
		void DrawTempGrid(float, float, float, float);
	};
}


#endif // _SPRITE_EDITOR_H_
#endif // EDITOR









