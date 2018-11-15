/* HEADER *********************************************************************************/
/*!
\file	TextRenderer.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Renders text

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
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
#include "System/Graphics/CharSpace.h"

#include "Math/Vector4.h"

#include <string>


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

	class _DLL_EXPORT TextRenderer : public Renderer
	{
	public:

		using SYSTEM = GraphicsSystem;
		unsigned GetComponentType(void) const
		{
			return Ut::MetaFind_t<class Renderer, AllComponents>::value;
		};
		static const std::string GetCompileName(void) { return "Text Renderer"; }
		const std::string GetEditorName(void) const { return GetCompileName(); }


		// ====================================== CONSTRUCTORS ======================================= // 

		TextRenderer(void) noexcept;
		TextRenderer(TextRenderer&&) noexcept = default;
		TextRenderer(const TextRenderer&) noexcept = default;


		// ===================================== MEMBER FUNCTIONS ==================================== // 

		void Awake(void) override;

		void Draw(void) const noexcept;

		void SetText(const char*);
		void SetText(const std::string&);

		void SetFont(const char*);
		void SetFont(const std::string&);

		//TextRenderer* Duplicate(void) const;
		void Serialise(TextSerialiser&) const;
		void Unserialise(TextSerialiser&);

		void EditorUI(void) noexcept override;

	private:

		HashString mText;
		AutoArray<Gfx::Vertex> mVerts;
		AutoArray<CharSpace> mSpaces;

		unsigned mnBaseMesh;
		TextureAtlas* mpAtlas;

		Math::Vector4 mColor;

		void RegenMesh(void);

	};
}


#endif

