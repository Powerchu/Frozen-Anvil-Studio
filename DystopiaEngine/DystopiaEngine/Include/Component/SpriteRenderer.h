/* HEADER *********************************************************************************/
/*!
\file	SpriteRenderer.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Renderer for Sprite sheets

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _SPRITERENDERER_H_
#define _SPRITERENDERER_H_

#pragma warning(push)
#pragma warning(disable : 4251)
#include "Component/Renderer.h"		   // Base Class
#include "Component/Component.h"
#include "Component/ComponentList.h"
#include "DataStructure/AutoArray.h"   // AutoArray
#include "DataStructure/HashString.h"
#include "Utility/MetaAlgorithms.h"	   // MetaFind

#include "Math/Vector2.h"
#include <string>


namespace Dystopia
{
	class TextureAtlas;
	class GraphicsSystem;

	class _DLL_EXPORT SpriteRenderer : public Renderer
	{
	public:

		using SYSTEM = GraphicsSystem;
		unsigned GetComponentType(void) const override
		{
			return Ut::MetaFind_t<Ut::Decay_t<Renderer>, AllComponents>::value;
		};
		unsigned GetRealComponentType(void) const
		{
			return Ut::MetaFind_t<Ut::Decay_t<SpriteRenderer>, UsableComponents>::value;
		};
		static const std::string GetCompileName(void) { return "Sprite Renderer"; }
		const std::string GetEditorName(void) const override { return GetCompileName(); }


		// ====================================== CONSTRUCTORS ======================================= // 

		SpriteRenderer(void) noexcept;
		SpriteRenderer(SpriteRenderer&&) noexcept;
		SpriteRenderer(const SpriteRenderer&) noexcept;


		// ===================================== MEMBER FUNCTIONS ==================================== // 

		void Awake(void);
		void Init(void);

		void Draw(void) const noexcept;

		void Update(float _fDT);

		void SetAnimation(const char*);
		void SetAnimation(unsigned);

		SpriteRenderer* Duplicate(void) const;

		void Serialise(TextSerialiser&) const override;
		void Unserialise(TextSerialiser&)     override;

		void EditorUI(void) noexcept override;

		void SetTexture(Texture* _pTexture) noexcept;

	private:
		struct SpriteSheet
		{
			HashString mstrName;
			unsigned mnID;
			int mnCol, mnRow;
			bool mbLoop;
		};

		TextureAtlas* mpAtlas;
		AutoArray<SpriteSheet> mAnimations;
		int mnID, mnCol, mnRow;
		float mfFrameTime, mfAccTime;
		Math::Vec2 mNextSectionPos;

		void TextureFields(void);
		void AnimFields(void);
		void AddAnimations(void);
		bool SpriteSheetFields(const size_t&);
	};
}

#pragma warning(pop)
#endif



