/* HEADER *********************************************************************************/
/*!
\file	TextureSystem.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Manages Textures

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _TEXTURESYS_H_
#define _TEXTURESYS_H_

#include "System/File/FileSystem.h"

#include "Utility/Utility.h"
#include "DataStructure/Delegate.h"
#include "DataStructure/AutoArray.h"
#include "DataStructure/MagicArray.h"
#include "DataStructure/HashString.h"
#include "DataStructure/Tuple.h"

#include "IO/Image.h"
#include "IO/ImageParser.h"

#include <string>


namespace Dystopia
{
	class Texture;
	class TextureAtlas;

	class TextureSystem
	{
	public:

		TextureSystem(void) noexcept;
		~TextureSystem(void) noexcept;

		void Shutdown(void) noexcept;

		void EditorUpdate(void) noexcept;

		Image* ImportImage(const HashString&);

		template <typename Ty = Texture>
		Ty* GetTexture(HashString const& _strName);

		TextureAtlas* GenAtlas(Texture* = nullptr);
		TextureAtlas* GetAtlas(HashString const& _strName);

		template <typename Ty = Texture>
		Ty* LoadTexture(HashString const&);

		template <typename Ty>
		Ty* LoadRaw(Image*);
		template <typename Ty>
		Ty* LoadRaw(Image*, HashString const&);

		void Free(Texture*) noexcept;

		void SaveAtlases(void);
		void SaveTexture(Texture const*);

		void RegisterReload(Texture* _strPath, Delegate<void(Image&)>&&);

	private:

		MagicArray<Texture> mTextures;
		MagicArray<TextureAtlas> mAtlas;
		AutoArray<Tuple<Texture*, Delegate<void(Image&)>>> mReloads;

		Image* LoadImage(std::string const& _strPath, Image*);

		void SaveTextureSetting(Image const&);
	};
}






// ============================================ FUNCTION DEFINITIONS ============================================ // 


template <typename Ty>
Ty* Dystopia::TextureSystem::GetTexture(HashString const& _strName)
{
	auto it = Ut::Find(mTextures.begin(), mTextures.end(), [&_strName](const Texture& _t) {
		return _strName == _t.GetName();
	});

	if (it != mTextures.end()) return static_cast<Ty*>(&*it);
	
	return nullptr;
}

template<typename Ty>
Ty* Dystopia::TextureSystem::LoadTexture(HashString const& _strPath)
{
	auto pFileSys = CORE::Get<FileSystem>();
	auto it = Ut::Find(mTextures.begin(), mTextures.end(), [&](const Texture& _t) {
		return pFileSys->IsSameFile(_strPath.c_str(), _t.GetPath().c_str()) || (_strPath.c_str() == _t.GetPath().c_str());
	});

	if (it != mTextures.end())
		return static_cast<Ty*>(&*it);

	auto import = ImportImage(_strPath);
	auto loaded = LoadImage(_strPath.c_str(), import);

	if (loaded)
		return mTextures.EmplaceAs<Ty>(_strPath, *loaded);

	DEBUG_BREAK(!loaded, "Texture System Error: Failed to load texture \"%s\"", _strPath.c_str());
	return nullptr;
}

template <>
Dystopia::Texture* Dystopia::TextureSystem::LoadTexture(HashString const&);

template<typename Ty>
Ty* Dystopia::TextureSystem::LoadRaw(Image*_ptr)
{
	auto ret = mTextures.EmplaceAs<Ty>(std::to_string(reinterpret_cast<uintptr_t>(_ptr)).c_str(), *_ptr);

	return ret;
}

template<typename Ty>
Ty* Dystopia::TextureSystem::LoadRaw(Image*_ptr, HashString const& _strName)
{
	auto ret = mTextures.EmplaceAs<Ty>(_strName, *_ptr);

	return ret;
}



#endif		// INCLUDE GUARD

