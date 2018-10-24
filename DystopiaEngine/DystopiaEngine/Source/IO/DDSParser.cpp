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

Image* ImageParser::LoadDDS(const std::string& _path)
{
	auto file = Dystopia::Serialiser::OpenFile<Dystopia::BinarySerializer>(_path);
	Image* img = Dystopia::DefaultAllocator<Image>::Alloc();

	HeaderDDS headerInfo;
	PixelFormatDDS pixelFormat;
	IgnoreDDS ignored;
	HeaderDX10DDS extra;

	file >> headerInfo;

	if (headerInfo.mMagic != ' SDD')
	{
		DEBUG_BREAK(true, "ImageLoader Error: File %s is not a dds image!\n", _path.c_str());
		Dystopia::DefaultAllocator<Image>::Free(img);

		return nullptr;
	}

	file >> pixelFormat;
	file >> ignored;
	unsigned blksz = 0;

	// Check if the magic number is valid
	if (pixelFormat.mFlags & 0x4)
	{
		img->mbCompressed = true;
		switch (pixelFormat.mMagic)
		{
		case '1TXD':
			img->mnRawFormat = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT;
			img->mnChannels  = 4;
			blksz            = 8;
			break;
		case '3TXD':
			img->mnRawFormat = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT;
			img->mnChannels  = 4;
			blksz            = 16;
			break;
		case '5TXD':
			img->mnRawFormat = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT;
			img->mnChannels  = 4;
			blksz            = 16;
			break;
		case '01XD':
			file >> extra;
		default:
			DEBUG_LOG(true, "ImageLoader Warning: File %s is an unknown DDS format!\n", _path.c_str());
			Dystopia::DefaultAllocator<Image>::Free(img);
			return nullptr;
			break;
		}

		img->mnFormat = GL_RGBA;
	}
	else
	{
		// Assume uncompressed
		img->mbCompressed = false;

		if (pixelFormat.mBitCount == 32)
		{
			img->mnRawFormat = GL_RGBA;
			img->mnFormat    = GL_RGBA;
			img->mnChannels  = 4;
		}
		else if (pixelFormat.mBitCount == 24)
		{
			img->mnRawFormat = GL_RGB;
			img->mnFormat    = GL_RGB;
			img->mnChannels  = 3;
		}
		else if (pixelFormat.mBitCount == 8)
		{
			img->mnRawFormat = GL_LUMINANCE;
			img->mnFormat    = GL_RED;
			img->mnChannels  = 1;
		}
		else
		{
			DEBUG_LOG(true, "ImageLoader Warning: File %s is an unknown DDS format!\n", _path.c_str());
			Dystopia::DefaultAllocator<Image>::Free(img);
			return nullptr;
		}
	}

	img->mnWidth   = headerInfo.mWidth;
	img->mnHeight  = headerInfo.mHeight;
	img->mnMipMaps = headerInfo.mMipMaps;

	auto sz = 16 * ((headerInfo.mWidth + 3) >> 2) * ((headerInfo.mHeight + 3) >> 2);
	sz = sz + (sz << 1);

	// Minimum of 64 bit blocks so we should be safe copying 64bits at a time
	uint64_t *buf = static_cast<uint64_t*>(Dystopia::DefaultAllocator<void>::Alloc(sz));
	img->mpImageData = buf;

	while (!file.EndOfInput())
	{
		file >> *buf;
		++buf;
	}

	return img;
}


