/* HEADER *********************************************************************************/
/*!
\file	ImageParser.cpp
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	References:
		https://docs.microsoft.com/en-us/windows/desktop/direct3ddds/dx-graphics-dds-pguide
		https://docs.microsoft.com/en-us/windows/desktop/direct3ddds/dds-header


All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "IO/ImageParser.h"			 // File header
#include "IO/Image.h"	             // Image
#include "IO/BinarySerializer.h"
#include "Utility/DebugAssert.h"	 // DEBUG_PRINT
#include "Allocator/DefaultAlloc.h"

#include "../../Dependancies/lodepng/lodepng.h"

#include <string>		// memcpy
#include <cstdint>
#include <GL/glew.h>

using Dystopia::Image;


namespace
{
	// The following structs are taken from the reference links

	struct HeaderDDS
	{
		uint32_t mMagic;        // This must be "DDS "
		uint32_t mSize;         // Size of the header
		uint32_t mFlags;
		uint32_t mHeight;
		uint32_t mWidth;
		uint32_t mPitch;        // Apparently unreliable
		uint32_t mDepth;
		uint32_t mMipMaps;
		uint32_t mReserved[11];
	};

	struct PixelFormatDDS
	{
		uint32_t mSize;     // Sizeof the original pixelformat struct
		uint32_t mFlags;
		uint32_t mMagic;	// Magic number that describes the type of the DDS
		uint32_t mBitCount;
	};

	struct IgnoreDDS
	{
		char mIgnore[124 + sizeof(uint32_t) - sizeof(HeaderDDS) - sizeof(PixelFormatDDS)];
	};

	struct HeaderDX10DDS
	{
		uint32_t mFormat;
		uint32_t mDimension;
		uint32_t mFlags;
		uint32_t mSize;
		uint32_t mFlags2;
	};
}

Image* ImageParser::LoadDDS(const std::string& _path, Image* _pImage)
{
	auto file = Dystopia::Serialiser::OpenFile<Dystopia::BinarySerializer>(_path.c_str());

	HeaderDDS headerInfo;
	PixelFormatDDS pixelFormat;
	IgnoreDDS ignored;
	HeaderDX10DDS extra;

	file >> headerInfo;

	if (headerInfo.mMagic != ' SDD')
	{
		DEBUG_BREAK(true, "ImageLoader Error: File %s is not a dds image!\n", _path.c_str());
		Dystopia::DefaultAllocator<Image>::DestructFree(_pImage);

		return nullptr;
	}

	headerInfo.mMipMaps += 0 == headerInfo.mMipMaps;
	if (_pImage)
	{
		_pImage->mnWidth   = headerInfo.mWidth;
		_pImage->mnHeight  = headerInfo.mHeight;
		_pImage->mnMipMaps = headerInfo.mMipMaps;
	}
	else
	{
		_pImage = Dystopia::DefaultAllocator<Image>::ConstructAlloc(
			_path.substr(_path.find_last_of("/\\") + 1).c_str(), false, false,
			0u, 0u, headerInfo.mWidth, headerInfo.mHeight,
			4u, headerInfo.mMipMaps,
			nullptr
		);
	}

	file >> pixelFormat;
	file >> ignored;
	unsigned blksz = 0;

	// Check if the magic number is valid
	if (pixelFormat.mFlags & 0x4)
	{
		_pImage->mbCompressed = true;
		switch (pixelFormat.mMagic)
		{
		case '1TXD':
			_pImage->mnRawFormat = _pImage->mbRGB ? GL_COMPRESSED_RGBA_S3TC_DXT1_EXT : GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT;
			_pImage->mnChannels  = 4;
			blksz            = 8;
			break;
		case '3TXD':
			_pImage->mnRawFormat = _pImage->mbRGB ? GL_COMPRESSED_RGBA_S3TC_DXT3_EXT : GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT;
			_pImage->mnChannels  = 4;
			blksz            = 16;
			break;
		case '5TXD':
			_pImage->mnRawFormat = _pImage->mbRGB ? GL_COMPRESSED_RGBA_S3TC_DXT5_EXT : GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT;
			_pImage->mnChannels  = 4;
			blksz            = 16;
			break;
		case '01XD':
			file >> extra;
		default:
			DEBUG_LOG(true, "ImageLoader Warning: File %s is an unknown DDS format!\n", _path.c_str());
			Dystopia::DefaultAllocator<Image>::DestructFree(_pImage);
			return nullptr;
			break;
		}

		_pImage->mnFormat = GL_RGBA;
	}
	else
	{
		// Assume uncompressed
		_pImage->mbCompressed = false;

		if (pixelFormat.mBitCount == 32)
		{
			_pImage->mnRawFormat = GL_RGBA;
			_pImage->mnFormat    = GL_RGBA;
			_pImage->mnChannels  = 4;
		}
		else if (pixelFormat.mBitCount == 24)
		{
			_pImage->mnRawFormat = GL_RGB;
			_pImage->mnFormat    = GL_RGB;
			_pImage->mnChannels  = 3;
		}
		else if (pixelFormat.mBitCount == 8)
		{
			_pImage->mnRawFormat = GL_LUMINANCE;
			_pImage->mnFormat    = GL_RED;
			_pImage->mnChannels  = 1;
		}
		else
		{
			DEBUG_LOG(true, "ImageLoader Warning: File %s is an unknown DDS format!\n", _path.c_str());
			Dystopia::DefaultAllocator<Image>::DestructFree(_pImage);
			return nullptr;
		}
	}

	auto sz = 16 * ((headerInfo.mWidth + 3) >> 2) * ((headerInfo.mHeight + 3) >> 2);
	sz = sz + (sz >> 1);

	// Minimum of 64 bit blocks so we should be safe copying 64bits at a time
	auto buf = static_cast<char*>(Dystopia::DefaultAllocator<void>::Alloc(sz));
	_pImage->mpImageData = buf;

	while (!file.EndOfInput())
	{
		file >> *buf;
		++buf;
	}

	return _pImage;
}


