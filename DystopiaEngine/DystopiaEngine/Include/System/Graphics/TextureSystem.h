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
#include "Allocator/DefaultAlloc.h"
#include "DataStructure/MagicArray.h"
#include "DataStructure/HashString.h"
#include "System/Time/ScopedTimer.h"
#include "System/Profiler/Profiler.h"
#include "System/Profiler/ProfilerAction.h"

#include "IO/Image.h"
#include "IO/ImageParser.h"

#include <map>
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
		Ty* GetTexture(std::string const& _strName);

		TextureAtlas* GenAtlas(Texture* = nullptr);
		TextureAtlas* GetAtlas(std::string const& _strName);

		template <typename Ty = Texture>
		Ty* LoadTexture(std::string const&);

		template <typename Ty>
		Ty* LoadRaw(Image const*);
		template <typename Ty>
		Ty* LoadRaw(Image const*, std::string const&);

	private:

		std::map<HashString, Image> mImageData;
		MagicArray<Texture> mTextures;
		MagicArray<TextureAtlas> mAtlas;

		Image* LoadImage(std::string const& _strPath);
		void SaveTextureSetting(Image*);
	};
}






// ============================================ FUNCTION DEFINITIONS ============================================ // 


template <typename Ty>
Ty* Dystopia::TextureSystem::GetTexture(std::string const& _strName)
{
	auto it = Ut::Find(mTextures.begin(), mTextures.end(), [&_strName](const Texture& _t) {
		return _strName == _t.GetName();
	});

	if (it != mTextures.end()) return static_cast<Ty*>(&*it);
	
	return nullptr;
}

template<typename Ty>
Ty* Dystopia::TextureSystem::LoadTexture(std::string const& _strPath)
{
	auto pFileSys = EngineCore::Get<FileSystem>();
	auto it = Ut::Find(mTextures.begin(), mTextures.end(), [&](const Texture& _t) {
		return pFileSys->IsSameFile(_strPath, _t.GetPath()) || (_strPath == _t.GetPath());
	});

	if (it != mTextures.end())
	{
		return static_cast<Ty*>(&*it);
	}

	auto loaded = LoadImage(_strPath);

	if (loaded)
	{
		auto ret = mTextures.EmplaceAs<Ty>(_strPath);
		ret->LoadTexture(loaded);

		DefaultAllocator<void>::Free(loaded->mpImageData);
		loaded->mpImageData = nullptr;
		mImageData.emplace(HashString{ _strPath.c_str() }, Ut::Move(*loaded));
		DefaultAllocator<Image>::DestructFree(loaded);
		return ret;
	}

	DEBUG_BREAK(!loaded, "Texture System Error: Failed to load texture \"%s\"", _strPath.c_str());
	return nullptr;
}

template <>
Dystopia::Texture* Dystopia::TextureSystem::LoadTexture(std::string const&);

template<typename Ty>
Ty* Dystopia::TextureSystem::LoadRaw(Image const *_ptr)
{
	auto ret = mTextures.EmplaceAs<Ty>(std::to_string(reinterpret_cast<uintptr_t>(_ptr)));

	ret->LoadTexture(_ptr);

	return ret;
}

template<typename Ty>
Ty* Dystopia::TextureSystem::LoadRaw(Image const *_ptr, std::string const& _strName)
{
	auto ret = mTextures.EmplaceAs<Ty>(_strName);

	ret->LoadTexture(_ptr);

	return ret;
}



#endif		// INCLUDE GUARD

