/* HEADER *********************************************************************************/
/*!
\file	Renderer.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Basic Renderer.

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _BASICRENDERER_H_
#define _BASICRENDERER_H_

#pragma warning(push)
#pragma warning(disable : 4251)
#include "Component/Component.h"		// Base Class
#include "Component/ComponentList.h"	// TRANSFORM
#include "DataStructure/AutoArray.h"	// AutoArray
#include "Utility/MetaAlgorithms.h"		// MetaFind


#include <string>


namespace Dystopia
{
	class Mesh;
	class Shader;
	class Texture;
	class GraphicsSystem;

	class _DLL_EXPORT Renderer : public Component
	{
	public:

		using SYSTEM = GraphicsSystem;
		unsigned GetComponentType(void) const
		{
			return Utility::MetaFind_t<Utility::Decay_t<decltype(*this)>, AllComponents>::value; 
		};
		static const std::string GetCompileName(void) { return "Renderer"; }
		const std::string GetEditorName(void) const { return GetCompileName(); }


		// ====================================== CONSTRUCTORS ======================================= // 

		Renderer(void) noexcept;
		Renderer(Renderer&&) noexcept;
		//Renderer(const Renderer&) noexcept;


		// ===================================== MEMBER FUNCTIONS ==================================== // 

		void Init(void) override;

		void Draw(void) const noexcept;
		
		void SetMesh(Mesh*) noexcept;
		void SetMesh(const std::string&) noexcept;

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

		//Renderer(const Renderer&) = delete;
	private:

		unsigned mnUnique;

		Mesh* mpMesh;
		Shader* mpShader;
		Texture* mpTexture;
		std::string mTexturePath;
		std::string mTextureName;

		void TextureField();
		void MeshField();
		void ShaderField();
		std::string GetTextureName();
	};
}


#pragma warning(pop)
#endif

