/* HEADER *********************************************************************************/
/*!
\file	TextRenderer.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Renders text

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _TEXTRENDERER_H_
#define _TEXTRENDERER_H_

#include "Component/Component.h"		// Base Class
#include "Component/Renderer.h"			// Base Class
#include "Component/ComponentList.h"

#include "DataStructure/AutoArray.h"	// AutoArray
#include "DataStructure/HashString.h"
#include "Utility/MetaAlgorithms.h"		// MetaFind
#include "System/Graphics/Font.h"

#include "Math/Vectors.h"

#include <string>

#include "System/Graphics/VertexDefs.h"

namespace Dystopia
{
	namespace Gfx
	{
		struct Vertex;
	}

	class  Mesh;
	class  Texture;
	struct CharSpace;
	class  TextureAtlas;
	class  GraphicsSystem;

	namespace TextEditor
	{
		enum eTextState
		{
			DEFAULT,
			ON_HOVER,
			ON_CLICK,
			DISABLED
		};
	}

	class _DLL_EXPORT TextRenderer : public Renderer
	{
	public:

		using SYSTEM = GraphicsSystem;
		unsigned GetComponentType(void) const
		{
			return Ut::MetaFind_t<class Renderer, AllComponents>::value;
		};
		unsigned GetRealComponentType(void) const
		{
			return Ut::MetaFind_t<Ut::Decay_t<TextRenderer>, UsableComponents>::value;
		};
		static const std::string GetCompileName(void) { return "Text Renderer"; }
		const std::string GetEditorName(void) const { return GetCompileName(); }


		// ====================================== CONSTRUCTORS ======================================= // 

		TextRenderer(void) noexcept;
		TextRenderer(TextRenderer&&) noexcept = default;
		TextRenderer(const TextRenderer&) noexcept;


		// ===================================== MEMBER FUNCTIONS ==================================== // 

		void Awake(void) override;

		void Draw(void) const noexcept;

		void SetText(const char*);
		void SetText(const std::string&);

		void SetFont(const char*);
		void SetFont(const std::string&);

		void SetFontSize(float _sz);
		
		void SetColor(const Math::Vec3D& _rgb);
		void SetColorA(const Math::Vec4 _rgba);
		void SetColor(float r, float g, float b);
		void SetColor(float r, float g, float b, float a);
		void SetAlpha(float _a);
		void SetAlphaPerc(float _perc);

		float GetAlphaPerc() const;

		TextRenderer* Duplicate(void) const;
		void Serialise(TextSerialiser&) const;
		void Unserialise(TextSerialiser&);

		void ChangeState(TextEditor::eTextState _state);
		void RevertState();
		void ApplyChanges();

		TextEditor::eTextState mCurrState = TextEditor::DEFAULT;
		TextEditor::eTextState mPrevState;

#if EDITOR
		void EditorUI(void) noexcept override;
#endif

	private:

		Font* mpData;
		HashString mText;
		AutoArray<Gfx::Vertex> mVerts;
		int mnAnchorX, mnAnchorY;
		
		Math::Vector4 mColor;
		float mfTintPerc = 1.f;

		Math::Vector4 mDefaultCol = { 1.f,1.f,1.f,1.f };
		Math::Vector4 mHoverCol   = {.5f, .1f, .1f, 1.f};
		Math::Vector4 mClickColor = {.7f,.2f,.2f,1.f};
		Math::Vector4 mDisabledColor = {.5f,.5f,.5f,1.f};

		void RegenMesh(void);

	};
}



#endif		// INCLUDE GUARD

