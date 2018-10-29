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
#include "Component/ComponentList.h"	// TRANSFORM
#include "DataStructure/AutoArray.h"	// AutoArray
#include "Utility/MetaAlgorithms.h"		// MetaFind

#include <string>


namespace Dystopia
{
	class Texture;
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

		void Init(void);

		void Draw(void) const;

		void SetShader(Shader*) noexcept;
		void SetShader(const std::string&) noexcept;
		Shader* GetShader(void) const noexcept;

		void SetTexture(Texture*) noexcept;
		Texture* GetTexture(void) const noexcept;

		bool HasTransparency(void) const noexcept;

		Renderer* Duplicate(void) const;

		void Serialise(TextSerialiser&) const;
		void Unserialise(TextSerialiser&);

		void EditorUI(void) noexcept override;


	private:

		
	};
}


#endif

