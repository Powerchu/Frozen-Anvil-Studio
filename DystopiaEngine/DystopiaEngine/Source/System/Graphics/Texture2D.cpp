/* HEADER *********************************************************************************/
/*!
\file	Texture2D.cpp
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	sRGB 2D Textures

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "System/Graphics/Texture2D.h"
#include "System/Graphics/Texture.h"
#include "System/Graphics/TextureSystem.h"
#include "System/Driver/Driver.h"
#include "System/File/FileSystem.h"

#include "IO/Image.h"
#include "IO/ImageParser.h"

#include "Allocator/DefaultAlloc.h"

#include <cstdint>
#include <GL/glew.h>


Dystopia::Texture2D::Texture2D(HashString const& _strPath, Image& _Image) noexcept
	: Texture{ GL_TEXTURE_2D, _strPath, _Image }
{
	Bind();

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	Load();

	Unbind();
}

void Dystopia::Texture2D::GenerateMipmap(void) const
{
	Bind();

	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);

	Unbind();
}

void Dystopia::Texture2D::Load(void)
{
#if EDITOR
	auto id = CORE::Get<FileSystem>()->TrackFile(GetPath());
	CORE::Get<FileSystem>()->BindFileTrackEvent(id, &Texture2D::ReloadImageTrigger, this);
#endif

	ResolveLoad();
}

void Dystopia::Texture2D::ResolveLoad(void)
{
	if (mData.mbCompressed)
		InitCompressedTexture();
	else
		InitTexture();

	if (mData.mpImageData)
	{
		DefaultAllocator<void>::Free(mData.mpImageData);
		mData.mpImageData = nullptr;
	}

#   if defined(_DEBUG) | defined(DEBUG)
	if (auto err = glGetError())
		__debugbreak();
#   endif
}

void Dystopia::Texture2D::InitTexture(void)
{
	Bind();

	auto w = mData.mnWidth;
	auto h = mData.mnHeight;
	uint8_t* data_ptr = static_cast<uint8_t*>(mData.mpImageData);

	for (unsigned n = 0; (n < mData.mnMipMaps) && w && h; ++n)
	{
		glTexImage2D(mnType, n, mData.mnRawFormat, w, h, 0, mData.mnFormat, GL_UNSIGNED_BYTE, data_ptr);
		data_ptr += mData.mnChannels * w * h;

		// Assume each MipMap is half of the previous
		w >>= 1;
		h >>= 1;

#   if defined(_DEBUG) | defined(DEBUG)
		if (auto err = glGetError())
			__debugbreak();
#   endif 
	}

	Unbind();
}

void Dystopia::Texture2D::ReplaceTexture(unsigned w, unsigned h, void* _pData, bool _bAlpha)
{
	mData.mnWidth   = w;
	mData.mnHeight  = h;
	mData.mnFormat  = _bAlpha ? GL_RGBA : GL_RGB;
	mData.mbChanged = true;

	Bind();
	glTexImage2D(mnType, 0, mData.mnFormat, w, h, 0, mData.mnFormat, GL_UNSIGNED_BYTE, _pData);
}

void Dystopia::Texture2D::InitCompressedTexture(void)
{
	Bind();

	unsigned blksz = mData.mnRawFormat == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT ? 8 : 16;

	auto w = mData.mnWidth;
	auto h = mData.mnHeight;
	uint8_t* data_ptr = static_cast<uint8_t*>(mData.mpImageData);

	unsigned n;
	for (n = 0; (n < mData.mnMipMaps) && w && h; ++n)
	{
		auto sz = blksz * ((w + 3) >> 2) * ((h + 3) >> 2);

		glCompressedTexImage2D(mnType, n, mData.mnRawFormat, w, h, 0, sz, data_ptr);
		data_ptr += sz;

		// Assume each MipMap is half of the previous
		w >>= 1;
		h >>= 1;

#   if defined(_DEBUG) | defined(DEBUG)
		if (auto err = glGetError())
			__debugbreak();
#   endif
	}

	if(n > 1)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);

	Unbind();
}

void Dystopia::Texture2D::ReloadImageTrigger(void)
{
	auto pTexSys = CORE::Get<TextureSystem>();

	pTexSys->RegisterReload(this, Delegate<void(Image&)>{this, &Texture2D::ReloadImage});
}

void Dystopia::Texture2D::ReloadImage(Image& _img)
{
	mData = Ut::Move(_img);

	ResolveLoad();
}


