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

#include "System/Driver/Driver.h"
#include "System/File/FileSystem.h"

#include "DataStructure/Delegate.h"
#include "DataStructure/AutoArray.h"
#include "DataStructure/MagicArray.h"
#include "DataStructure/HashString.h"

#include "Globals.h"
#include "IO/TextSerialiser.h"

#include "Math/Vectors.h"

#include <GL\glew.h>


Dystopia::TextureSystem::TextureSystem(void) noexcept
	: mTextures{}
{
}	

Dystopia::TextureSystem::~TextureSystem(void) noexcept
{
	Shutdown();
}

void Dystopia::TextureSystem::EditorUpdate(void) noexcept
{
#if EDITOR
	
	for (auto& e : mReloads)
	{
		if (auto img = LoadImage(e.Get<0>()->GetPath().c_str(), 
			const_cast<Image*>(&e.Get<0>()->GetSettings())))
			e.Get<1>()(*img);

#	if defined(_DEBUG) | defined(DEBUG)
		if (auto err = glGetError())
			__debugbreak();
#	endif 
	}

	mReloads.clear();

#endif
}

void Dystopia::TextureSystem::Shutdown(void) noexcept
{
#if EDITOR
	SaveAtlases();

	for (auto& e : mTextures)
		if (e.Changed())
			SaveTexture(&e);

	mTextures.clear();
#endif
}

void Dystopia::TextureSystem::SaveAtlases(void)
{
#if EDITOR
	for (auto& a : mAtlas)
	{
		if (!a.IsChanged())
			continue;

		auto fp = CORE::Get<FileSystem>()->FindFilePath(a.GetName().c_str(), eFileDir::eCurrent);
		HashString folder{ fp };
		folder += '.';
		folder += Gbl::ATLAS_EXT;
		auto serial = TextSerialiser::OpenFile(folder.c_str(), TextSerialiser::MODE_WRITE);
		a.SaveAtlas(serial);
	}
#endif
}

void Dystopia::TextureSystem::SaveTexture(Texture const* _pTexture)
{
	SaveTextureSetting(_pTexture->GetSettings());
}

void Dystopia::TextureSystem::RegisterReload(Texture* _pTarget, Delegate<void(Image&)>&& _callBack)
{
	mReloads.EmplaceBack(_pTarget, Ut::Move(_callBack));
}

Dystopia::TextureAtlas* Dystopia::TextureSystem::GenAtlas(Texture* _pTex)
{
	if (_pTex) if (auto p = GetAtlas(_pTex->GetName()))
		return p;

	auto pAtlas = mAtlas.Emplace(_pTex);

	if (_pTex)
	{
		HashString file{ pAtlas->GetName().c_str() };
		file += '.';
		file += Gbl::ATLAS_EXT;
		auto find = CORE::Get<FileSystem>()->FindFilePath(file.c_str(), eFileDir::eCurrent);
		if (find.size())
		{
			auto serial = TextSerialiser::OpenFile(find.c_str(), TextSerialiser::MODE_READ);
			pAtlas->LoadAtlas(serial);
		}
		else
			pAtlas->AddSection(Math::Vec2{ 0,0 }, _pTex->GetWidth(), _pTex->GetHeight(), 1, 1);
	}
	return pAtlas;
}

Dystopia::TextureAtlas* Dystopia::TextureSystem::GetAtlas(HashString const& _strName)
{
	auto it = Ut::Find(mAtlas.begin(), mAtlas.end(), [&_strName](const TextureAtlas& _t) {
		return _strName == _t.GetName();
	});
	
	if (it != mAtlas.end()) return (&*it);

	return nullptr;
}

template <>
Dystopia::Texture* Dystopia::TextureSystem::LoadTexture(HashString const& _strPath)
{
	// Hardcoded default texture settings
	// Because we have no idea what the texture should be loaded as
	// We can allow the user to change it later

	return LoadTexture<Texture2D>(_strPath);
}

Dystopia::Image* Dystopia::TextureSystem::ImportImage(HashString const& _strPath)
{
	Image* pImg = nullptr;
	auto meta = _strPath + "." + Gbl::METADATA_EXT;

	if (CORE::Get<FileSystem>()->CheckFileExist(meta.c_str(), eFileDir::eResource))
	{
		auto metaFile = Serialiser::OpenFile<TextSerialiser>(meta.c_str());

		if (!metaFile.EndOfInput())
		{
			pImg = DefaultAllocator<Image>::ConstructAlloc();

			metaFile.ConsumeStartBlock()
				>> pImg->mstrName
				>> pImg->mbCompressed
				>> pImg->mbRGB
				>> pImg->mnRawFormat
				>> pImg->mnFormat;
		}
	}

	return pImg;
}

Dystopia::Image* Dystopia::TextureSystem::LoadImage(std::string const& _strPath, Image* _img)
{
	Image *loaded = nullptr;
	auto fileType = _strPath[_strPath.rfind('.') + 1];

	if ('p' == fileType || 'P' == fileType)
	{
		loaded = ImageParser::LoadPNG(_strPath, _img);
	}
	else if ('b' == fileType || 'B' == fileType)
	{
		loaded = ImageParser::LoadBMP(_strPath, _img);
	}
	else if ('d' == fileType || 'D' == fileType)
	{
		loaded = ImageParser::LoadDDS(_strPath, _img);
	}

	loaded->mbChanged = nullptr == _img;
	return loaded;
}

void Dystopia::TextureSystem::SaveTextureSetting(Image const& _img)
{
	auto path = CORE::Get<FileSystem>()->GetFullPath(_img.mstrName.c_str(), eFileDir::eResource);

	if (path.size())
	{
		path = path + "." + Gbl::METADATA_EXT;
		auto metaFile = Serialiser::OpenFile<TextSerialiser>(path.c_str(), Serialiser::MODE_WRITE);

		if (!metaFile.EndOfInput())
		{
			metaFile.InsertStartBlock()
				<< _img.mstrName
				<< _img.mbCompressed
				<< _img.mbRGB
				<< _img.mnRawFormat
				<< _img.mnFormat;
		}
	}
}


