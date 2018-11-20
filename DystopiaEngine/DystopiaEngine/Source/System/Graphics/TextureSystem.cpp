/* HEADER *********************************************************************************/
/*!
\file	TextureSystem.cpp
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Manages Textures

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "System/Graphics/TextureSystem.h"

#include "System/Graphics/Texture.h"
#include "System/Graphics/Texture2D.h"
#include "System/Graphics/TextureAtlas.h"

#include "DataStructure/MagicArray.h"

#include "Globals.h"
#include "IO/TextSerialiser.h"

Dystopia::TextureSystem::TextureSystem(void) noexcept
	: mTextures{}
{
}

Dystopia::TextureSystem::~TextureSystem(void) noexcept
{
	Shutdown();
}

void Dystopia::TextureSystem::Shutdown(void) noexcept
{
	mTextures.clear();
}

Dystopia::TextureAtlas* Dystopia::TextureSystem::GenAtlas(Texture* _pTex)
{
	if (_pTex) if (auto p = GetAtlas(_pTex->GetName()))
		return p;

	return mAtlas.Emplace(_pTex);
}

Dystopia::TextureAtlas* Dystopia::TextureSystem::GetAtlas(const std::string& _strName)
{
	auto it = Ut::Find(mAtlas.begin(), mAtlas.end(), [&_strName](const TextureAtlas& _t) {
		return _strName == _t.GetName();
	});
	
	if (it != mAtlas.end()) return (&*it);

	return nullptr;
}

template <>
Dystopia::Texture* Dystopia::TextureSystem::LoadTexture(const std::string& _strPath)
{
	// Hardcoded default texture settings
	// Because we have no idea what the texture should be loaded as
	// We can allow the user to change it later

	return LoadTexture<Texture2D>(_strPath);
}

Dystopia::Image* Dystopia::TextureSystem::ImportImage(const HashString& _strPath)
{
	auto meta     = _strPath + Gbl::METADATA_EXT;
	auto metaFile = Serialiser::OpenFile<TextSerialiser>(meta.c_str());

	if (!metaFile.EndOfInput())
	{
		Image* p = DefaultAllocator<Image>::ConstructAlloc();

		metaFile.ConsumeStartBlock();
		metaFile >> p->mstrName;
		metaFile >> p->mbCompressed;
		metaFile >> p->mbRGB;
		metaFile >> p->mnRawFormat;
		metaFile >> p->mnFormat;
	}

	return nullptr;
}

