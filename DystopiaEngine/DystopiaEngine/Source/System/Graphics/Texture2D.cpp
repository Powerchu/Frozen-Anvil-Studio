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
#include "System/Driver/Driver.h"
#include "System/File/FileSystem.h"

#include "IO/Image.h"
#include "IO/ImageParser.h"

#include "Allocator/DefaultAlloc.h"

#include <cstdint>
#include <GL/glew.h>


Dystopia::Texture2D::Texture2D(HashString const& _strPath) noexcept
	: Texture{ GL_TEXTURE_2D, _strPath }
{
	Bind();

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	Unbind();
}

void Dystopia::Texture2D::GenerateMipmap(void) const
{
	Bind();

	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);

	Unbind();
}

void Dystopia::Texture2D::LoadTexture(Image const * _pData)
{
	auto id = CORE::Get<FileSystem>()->TrackFile(GetPath());
	CORE::Get<FileSystem>()->BindFileTrackEvent(id, &Texture2D::ReloadImage, this);

#   if defined(_DEBUG) | defined(DEBUG)
	if (auto err = glGetError())
		__debugbreak();
#   endif 

	if (_pData->mbCompressed)
		InitCompressedTexture(_pData);
	else
		InitTexture(_pData);
}

void Dystopia::Texture2D::InitTexture(Image const* _pData)
{
	Bind();

#   if defined(_DEBUG) | defined(DEBUG)
	if (auto err = glGetError())
		__debugbreak();
#   endif 

	auto w = mnWidth  = _pData->mnWidth;
	auto h = mnHeight = _pData->mnHeight;
	uint8_t* data_ptr = static_cast<uint8_t*>(_pData->mpImageData);

	for (unsigned n = 0; (n < _pData->mnMipMaps) && w && h; ++n)
	{
		glTexImage2D(mnType, n, _pData->mnRawFormat, w, h, 0, _pData->mnFormat, GL_UNSIGNED_BYTE, data_ptr);
		data_ptr += _pData->mnChannels * w * h;

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
	auto format = _bAlpha ? GL_RGBA : GL_RGB;

	glTexImage2D(mnType, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, _pData);
}

void Dystopia::Texture2D::InitCompressedTexture(Image const* _pData)
{
	Bind();

	unsigned blksz = _pData->mnRawFormat == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT ? 8 : 16;

	auto w = mnWidth  = _pData->mnWidth;
	auto h = mnHeight = _pData->mnHeight;
	uint8_t* data_ptr = static_cast<uint8_t*>(_pData->mpImageData);

	unsigned n;
	for (n = 0; (n < _pData->mnMipMaps) && w && h; ++n)
	{
		auto sz = blksz * ((w + 3) >> 2) * ((h + 3) >> 2);

		glCompressedTexImage2D(mnType, n, _pData->mnRawFormat, w, h, 0, sz, data_ptr);
		data_ptr += sz;

		// Assume each MipMap is half of the previous
		w >>= 1;
		h >>= 1;

#   if defined(_DEBUG) | defined(DEBUG)
		if (auto err = glGetError())
			__debugbreak();
#   endif
	}

	//if(n > 1)
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);

	Unbind();
}

void Dystopia::Texture2D::ReloadImage(void)
{
	__debugbreak();
}


