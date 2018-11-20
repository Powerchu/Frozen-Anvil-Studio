/* HEADER *********************************************************************************/
/*!
\file	ImageParser.cpp
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Calls library to load images


All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "IO/ImageParser.h"			 // File header
#include "IO/BinarySerializer.h"
#include "IO/Image.h"                // Image
#include "Utility/DebugAssert.h"	 // DEBUG_PRINT
#include "Allocator/DefaultAlloc.h"

#include "../../Dependancies/lodepng/lodepng.h"

#include <string>		// memcpy
#include <cstdint>
#include <GL/glew.h>
#include <GL/GL.h>

using Dystopia::Image;


namespace
{
	struct HeaderPNG
	{
		uint8_t mIgnore1;
		uint8_t mFormat[3];			// This must be ASCII "PNG" 
		uint8_t mIgnore2[4];
	};

	struct ChunkPNG
	{
		uint32_t	mLength;
		uint8_t		mType[4];
		std::string	mData;
		uint32_t	mCRC;
	};
}

Image* ImageParser::LoadPNG(const std::string& _strName, Image* _pImage)
{
	std::vector<unsigned char> buf;

	if (!_pImage)
	{
		_pImage = Dystopia::DefaultAllocator<Image>::ConstructAlloc(
			_strName.substr(_strName.find_last_of("/\\") + 1).c_str(),
			false, false, unsigned(GL_SRGB_ALPHA), unsigned(GL_RGBA),
			0u, 0u, 4u, 1u, nullptr
		);
	}

	lodepng::decode(buf, _pImage->mnWidth, _pImage->mnHeight, _strName.c_str());

	_pImage->mpImageData = Dystopia::DefaultAllocator<void>::Alloc(buf.size());
	std::memcpy(_pImage->mpImageData, &buf[0], buf.size());

	return _pImage;
}


