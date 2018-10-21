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
#include "IO/BinarySerializer.h"
#include "System/Graphics/Image.h"	 // Image
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

Image ImageParser::LoadDDS(const std::string& _path)
{
	auto file = Dystopia::Serialiser::OpenFile<Dystopia::BinarySerializer>(_path);

	HeaderDDS headerInfo;
	PixelFormatDDS pixelFormat;
	IgnoreDDS ignored;
	HeaderDX10DDS extra;
	Image img;
	unsigned channels;

	file >> headerInfo;

	if (headerInfo.mMagic != ' SDD')
	{
		DEBUG_BREAK(true, "ImageLoader Error: File %s is not a dds image!\n", _path.c_str());
		return { 0, 0, 0, nullptr };
	}

	file >> pixelFormat;
	file >> ignored;

	// Check if the magic number is valid
	if (pixelFormat.mFlags & 0x4)
	{
		switch (pixelFormat.mMagic)
		{
		case '1TXD':
			img.mnFormat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
			channels = 4;
				break;
		case '3TXD':
			img.mnFormat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
			channels = 4;
			break;
		case '5TXD':
			img.mnFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
			channels = 4;
			break;
		case '01XD':
			file >> extra;
			DEBUG_LOG(true, "ImageLoader Warning: Unable to open DXT10 format %s !\n", _path.c_str());
			return { 0, 0, 0, nullptr };
			break;
		default:
			DEBUG_LOG(true, "ImageLoader Warning: File %s is an unknown DDS format!\n", _path.c_str());
			return { 0, 0, 0, nullptr };
			break;
		}
	}
	else
	{
		// Assume uncompressed
		img.mnFormat = pixelFormat.mBitCount == 32 ? GL_RGBA : (pixelFormat.mBitCount == 24 ? GL_RGB: GL_LUMINANCE);
		channels = pixelFormat.mBitCount == 32 ? 4 : (pixelFormat.mBitCount == 24 ? 3 : 1);
	}

	img.mnWidth = headerInfo.mWidth;
	img.mnHeight = headerInfo.mHeight;

	uint8_t *buf = static_cast<uint8_t*>(Dystopia::DefaultAllocator<void>::Alloc(headerInfo.mPitch * (1 + (headerInfo.mMipMaps > 1))));
	img.mpImageData = buf;
	while (!file.EndOfInput())
	{
		file >> *buf;
		++buf;
	}

	return img;
}


