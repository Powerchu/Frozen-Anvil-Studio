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

#include "IO/Image.h"
#include "IO/ImageParser.h"

#include "Allocator/DefaultAlloc.h"

#include <cstdint>
#include <GL/glew.h>


Dystopia::Texture2D::Texture2D(const std::string& _strPath) noexcept
	: Texture{ GL_TEXTURE_2D, _strPath }
{

}

Dystopia::Texture2D::~Texture2D(void) noexcept
{
}

void Dystopia::Texture2D::GenerateMipmap(void) const
{
	BindTexture();

	glGenerateMipmap(GL_TEXTURE_2D);

	UnbindTexture();
}

void Dystopia::Texture2D::InitTexture(Image const* _pData)
{
	BindTexture();

	auto w = _pData->mnWidth;
	auto h = _pData->mnHeight;
	mnWidth  = _pData->mnWidth;
	mnHeight = _pData->mnHeight;

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

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);

	UnbindTexture();
}

void Dystopia::Texture2D::InitCompressedTexture(Image const* _pData)
{
	BindTexture();

	unsigned blksz = _pData->mnRawFormat == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT ? 8 : 16;

	auto w = _pData->mnWidth;
	auto h = _pData->mnHeight;
	mnWidth  = _pData->mnWidth;
	mnHeight = _pData->mnHeight;

	uint8_t* data_ptr = static_cast<uint8_t*>(_pData->mpImageData);

	for (unsigned n = 0; (n < _pData->mnMipMaps) && w && h; ++n)
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

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);

	UnbindTexture();
}


