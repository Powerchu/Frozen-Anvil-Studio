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
#include "Utility/MetaAlgorithms.h"		// MetaFind

#include <string>


namespace Dystopia
{
	struct Vertex;
	struct RawMesh;
	class Texture;
	class TextureAtlas;
	class GraphicsSystem;

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
		TextRenderer(TextRenderer&&) noexcept;
		TextRenderer(const TextRenderer&) noexcept;


		// ===================================== MEMBER FUNCTIONS ==================================== // 

		void SetText(const char*);
		void SetText(const std::string&);

		void SetFont(const char*);
		void SetFont(const std::string&);


	private:

		std::string mstrText;
		AutoArray<Vertex> mVerts;

		RawMesh* mpBaseMesh;
		TextureAtlas* mpAtlas;

		void RegenMesh(void);
	};
}


#endif

