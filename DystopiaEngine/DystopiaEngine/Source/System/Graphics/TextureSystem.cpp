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

#include "DataStructure/MagicArray.h"

#include "Globals.h"
#include "IO/TextSerialiser.h"

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
	auto pFileSys = EngineCore::GetInstance()->Get<FileSystem>();
	std::string buf;

	for (auto& e : mTextures)
	{
		if (auto n = pFileSys->DetectFileChanges(e.GetPath()))
		{
			while (n = pFileSys->DetectFileChanges(e.GetPath()));

			if (Image* pImg = LoadImage(e.GetPath()))
			{
				Texture* pTex = LoadRaw<Texture2D>(pImg, e.GetPath().c_str());
				
				e.Unbind();
				Ut::Swap(e, *pTex);
				mTextures.Remove(pTex);
			}
		}
	}

#	if defined(_DEBUG) | defined(DEBUG)
		if (auto err = glGetError())
			__debugbreak();
#	endif 
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
	Image* pImg = nullptr;
	auto meta = _strPath + "." + Gbl::METADATA_EXT;

	// TODO
	if (EngineCore::GetInstance()->Get<FileSystem>()->CheckFileExist(meta.c_str(), eFileDir::eResource))
	{
		auto metaFile = Serialiser::OpenFile<TextSerialiser>(meta.c_str());

		if (!metaFile.EndOfInput())
		{
			pImg = DefaultAllocator<Image>::ConstructAlloc();

			metaFile.ConsumeStartBlock();
			metaFile >> pImg->mstrName;
			metaFile >> pImg->mbCompressed;
			metaFile >> pImg->mbRGB;
			metaFile >> pImg->mnRawFormat;
			metaFile >> pImg->mnFormat;
		}
	}

	return pImg;
}

Dystopia::Image* Dystopia::TextureSystem::LoadImage(std::string const& _strPath)
{
	Image *loaded = nullptr, *img = ImportImage(_strPath.c_str());
	auto fileType = _strPath[_strPath.rfind('.') + 1];

	if ('p' == fileType || 'P' == fileType)
	{
		loaded = ImageParser::LoadPNG(_strPath, img);
	}
	else if ('b' == fileType || 'B' == fileType)
	{
		loaded = ImageParser::LoadBMP(_strPath, img);
	}
	else if ('d' == fileType || 'D' == fileType)
	{
		loaded = ImageParser::LoadDDS(_strPath, img);
	}

	if (nullptr == img)
	{
		SaveTextureSetting(loaded);
	}

	return loaded;
}

void Dystopia::TextureSystem::SaveTextureSetting(Image* _pImg)
{
	auto path = EngineCore::GetInstance()->Get<FileSystem>()->GetFullPath(_pImg->mstrName.c_str(), eFileDir::eResource) + "." + Gbl::METADATA_EXT;
	auto metaFile = Serialiser::OpenFile<TextSerialiser>(path.c_str(), Serialiser::MODE_WRITE);

	if (!metaFile.EndOfInput())
	{
		metaFile.InsertStartBlock();
		metaFile << _pImg->mstrName;
		metaFile << _pImg->mbCompressed;
		metaFile << _pImg->mbRGB;
		metaFile << _pImg->mnRawFormat;
		metaFile << _pImg->mnFormat;
	}
}


