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

#include "Allocator/DefaultAlloc.h"
#include "DataStructure/MagicArray.h"
#include "Utility/Utility.h"

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

		template <typename Ty = Texture>
		Ty* GetTexture(const std::string& _strName);
		TextureAtlas* GetAtlas(const std::string& _strName);

		template <typename Ty = Texture>
		Ty* LoadTexture(const std::string&);

		template <typename Ty>
		Ty* LoadRaw(Image const *);

	private:

		MagicArray<Texture> mTextures;
		MagicArray<TextureAtlas> mAtlas;
	};
}






// ============================================ FUNCTION DEFINITIONS ============================================ // 


template <typename Ty>
Ty* Dystopia::TextureSystem::GetTexture(const std::string& _strName)
{
	auto it = Ut::Find(mTextures.begin(), mTextures.end(), [&_strName](const Texture& _t) {
		return _strName == _t.GetName();
	});

	if (it) return static_cast<Ty*>(&*it);
	
	return nullptr;
}

template<typename Ty>
Ty* Dystopia::TextureSystem::LoadTexture(const std::string& _strPath)
{
	auto it = Ut::Find(mTextures.begin(), mTextures.end(), [&_strPath](const Texture& _t) {
		return _strPath == _t.GetPath();
	});

	if (it != mTextures.end())
	{
		return static_cast<Ty*>(&*it);
	}

	auto fileType = (_strPath.end() - 3);
	Ty*    ret = nullptr;
	Image* img = nullptr;

	if ('p' == *fileType || 'P' == *fileType)
	{
		img = ImageParser::LoadPNG(_strPath);
	}
	else if ('b' == *fileType || 'B' == *fileType)
	{
		img = ImageParser::LoadBMP(_strPath);
	}
	else if ('d' == *fileType || 'D' == *fileType)
	{
		img = ImageParser::LoadDDS(_strPath);
	}

	ret = mTextures.EmplaceAs<Ty>(_strPath);
	ret->LoadTexture(img);

	DefaultAllocator<Image>::Free(img);

	return ret;
}

template <>
Dystopia::Texture* Dystopia::TextureSystem::LoadTexture(const std::string&);

template<typename Ty>
Ty* Dystopia::TextureSystem::LoadRaw(Image const *_ptr)
{
	auto ret = mTextures.EmplaceAs<Ty>(std::to_string(reinterpret_cast<uintptr_t>(_ptr)));

	ret->InitTexture(_ptr);

	return ret;
}



#endif		// INCLUDE GUARD

