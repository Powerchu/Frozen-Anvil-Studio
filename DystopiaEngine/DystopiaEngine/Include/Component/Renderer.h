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
#include "Component/ComponentList.h"
#include "DataStructure/Tuple.h"
#include "DataStructure/Variant.h"
#include "DataStructure/AutoArray.h"
#include "DataStructure/HashString.h"
#include "Utility/MetaAlgorithms.h"
#include "Math/MathFwd.h"

#include "Lib/Gfx/Shaders.h"

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
		unsigned GetComponentType(void) const override
		{
			return Ut::MetaFind_t<Ut::Decay_t<decltype(*this)>, AllComponents>::value; 
		};
		unsigned GetRealComponentType(void) const
		{
			return Ut::MetaFind_t<Ut::Decay_t<decltype(*this)>, UsableComponents>::value;
		};
		static const std::string GetCompileName(void) { return "Renderer"; }
		const std::string GetEditorName(void) const override { return GetCompileName(); }


		// ====================================== CONSTRUCTORS ======================================= // 

		Renderer(void) noexcept;
		Renderer(Renderer&&) noexcept;
		Renderer(const Renderer&) noexcept;


		// ===================================== MEMBER FUNCTIONS ==================================== // 

		void Awake(void);
		void Init(void);

		void Draw(void) const noexcept;
		
		void SetMesh(Mesh*) noexcept;
		void SetMesh(const std::string&) noexcept;

		void SetShader(Shader*) noexcept;
		void SetShader(const std::string&) noexcept;
		Shader* GetShader(void) const noexcept;

		void SetTexture(Texture*) noexcept;
		Texture* GetTexture(void) const noexcept;

		bool HasTransparency(void) const noexcept;

		template <typename T>
		void SetManualShaderOverride(char const*, T&&);

		Renderer* Duplicate(void) const;

		void Serialise(TextSerialiser&) const;
		void Unserialise(TextSerialiser&);

		void EditorUI(void) noexcept override;

	protected:

		std::pair<OString, ::Gfx::eUniform_t> const* FindUniformInShader(const char*);

		using ShaderVariant_t = Variant<int, bool, float, Math::Vec2, Math::Vec4, Math::Mat2, Math::Mat4>;

		unsigned mnUnique;

		Mesh* mpMesh;
		Shader* mpShader;
		Texture* mpTexture;
		HashString mTexturePath;
		AutoArray<Tuple<OString, ::Gfx::eUniform_t, ShaderVariant_t>> mOverride;

#   if EDITOR
		void TextureField();
		void MeshField();
		void ShaderField();
#   endif
	};
}






// ============================================ FUNCTION DEFINITIONS ============================================ // 


template <typename T>
void Dystopia::Renderer::SetManualShaderOverride(char const* _strName, T&& _obj)
{
	for (auto& e : mOverride)
		if (e.Get<0>() == _strName)
		{
			e.Get<2>().KeepType() = Ut::Fwd<T>(_obj);
			return;
		}

	if (auto f = FindUniformInShader(_strName))
		// TODO : CHECK FOR CORRECT TYPE
		mOverride.EmplaceBack(f->first, f->second, Ut::Fwd<T>(_obj));
}


#pragma warning(pop)
#endif

