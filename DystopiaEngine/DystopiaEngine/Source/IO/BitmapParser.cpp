/* HEADER *********************************************************************************/
/*!
\file	BitmapParser.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Implementation for BMP loader and writer
	References: Wikipedia
		: http://paulbourke.net/dataformats/bmp/
		: https://www.fileformat.info/format/os2bmp/egff.htm


All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "IO/ImageParser.h"			// File header
#include "IO/BinarySerializer.h"
#include "IO/Image.h"	            // Image
#include "Utility/DebugAssert.h"	// DEBUG_PRINT
#include "Allocator/DefaultAlloc.h"
#include "Math/MathUtility.h"

#include <string>		// string
#include <cstdint>
#include <GL/glew.h>
#include <GL/GL.h>

using Dystopia::Image;


namespace
{
	// Taken from Wikipedia
	struct HeaderBMP
	{
		uint16_t mType;
		uint32_t mBytes;
		uint32_t mReserved;	// Actually 2 seperate fields but ignored anyway
		uint32_t mOffset;
	};

	// Taken from Wikipedia
	struct InfoBMP
	{
		uint32_t mSize;
		int32_t  mWidth;
		int32_t  mHeight;
		uint16_t mPlanes;
		uint16_t mBits;
		uint32_t mCompression;
		uint32_t mBMPSize;
		int32_t  mResolutionX;
		int32_t  mResolutionY;
		uint32_t mNumColors;
		uint32_t mImptColors;
	};

	struct StructRGB
	{
		uint8_t r, g, b;
	};

	struct StructRGBA
	{
		uint8_t r, g, b, a;
	};

	union ColorRGBA
	{
		uint32_t mColor;

		StructRGB sub_c;
		StructRGBA sub;
	};
}

namespace BMP
{
	bool IsUnreadable(HeaderBMP _header, InfoBMP _info)
	{
		// Check if we know how to read the file type
		if (_header.mType != 'MB')
		{
//			DEBUG_PRINT("ImageParser Error: File type not BMP \"%s\"!", _path.c_str());
			return true;
		}

		// We don't know how to read compressed formats...
		if (_info.mCompression != 0)
			return true;

		// No idea on stuff that are not these either
		if (_info.mBits == 8 || _info.mBits == 24 || _info.mBits == 32)
			return false;

		return true;
	}

	void ColorPalette(Dystopia::BinarySerializer& _file, InfoBMP& _fileInfo, ColorRGBA(&_palette)[256])
	{
		for (unsigned n = 0; n < _fileInfo.mNumColors; ++n)
		{
			_file >> _palette[n].sub.b;
			_file >> _palette[n].sub.g;
			_file >> _palette[n].sub.r;
			_file >> _palette[n].sub.a;
		}
	}

	// 8 Bits using Color Palette
	void* Palette_ColorBMP(Dystopia::BinarySerializer& _in, InfoBMP& _info, ColorRGBA(&_palette)[256])
	{
		ColorRGBA* data = Dystopia::DefaultAllocator<ColorRGBA[]>::Alloc(_info.mWidth * Math::Abs(_info.mHeight));

		for (int n = 0; n < Math::Abs(_info.mHeight); ++n)
		{
			for (int m = 0; m < _info.mWidth; ++m)
			{
				uint8_t i;
				_in >> i;
				data[_info.mWidth * n + m] = _palette[i];
			}

			if (_info.mWidth % 4)
			{
				_in.Skip(4 - _info.mWidth % 4);
			}
		}

		return data;
	}

	// 24 Bits
	void* RGB_ColorBMP(Dystopia::BinarySerializer& _in, InfoBMP& _info)
	{
		unsigned chunkLength = _info.mWidth * 3;
		unsigned padding = (((chunkLength + 3) >> 2) << 2) - chunkLength; // pad widths to multiple of 4

		ColorRGBA* data = Dystopia::DefaultAllocator<ColorRGBA[]>::Alloc(_info.mWidth * Math::Abs(_info.mHeight));
		ColorRGBA* ptr = data + _info.mWidth * Math::Abs(_info.mHeight);

		for (int n = 0; n < Math::Abs(_info.mHeight); ++n)
		{
			ptr = ptr - _info.mWidth;
			for (int w = 0; w < _info.mWidth; ++ w)
			{
				_in >> ptr->sub.b;
				_in >> ptr->sub.g;
				_in >> ptr->sub.r;
				ptr->sub.a = 255;

				++ptr;
			}


			ptr = ptr - _info.mWidth;
			_in.Skip(padding);
		}

		return data;
	}

	// 32 Bits
	void* RGBA_ColorBMP(Dystopia::BinarySerializer& _in, InfoBMP& _info)
	{
		ColorRGBA* data = Dystopia::DefaultAllocator<ColorRGBA[]>::Alloc(_info.mWidth * Math::Abs(_info.mHeight));
		ColorRGBA* ptr = data + _info.mWidth * Math::Abs(_info.mHeight);

		for (int n = 0; n < Math::Abs(_info.mHeight); ++n)
		{
			ptr = ptr - _info.mWidth;

			for (int w = 0; w < _info.mWidth; ++w)
			{
				_in >> ptr->sub.b;
				_in >> ptr->sub.g;
				_in >> ptr->sub.r;
				_in >> ptr->sub.a;

				++ptr;
			}

			ptr = ptr - _info.mWidth;
		}

		return data;
	}

	void ReadData(Dystopia::BinarySerializer& _in, InfoBMP& _info, ColorRGBA(&_palette)[256], Image* _pImage)
	{
		switch (_info.mBits)
		{
		case 8:
			_pImage->mpImageData = Palette_ColorBMP(_in, _info, _palette);
			break;
		case 24:
			_pImage->mpImageData = RGB_ColorBMP(_in, _info);
			break;
		case 32:
			_pImage->mpImageData = RGBA_ColorBMP(_in, _info);
			break;
		}
	}

	template <typename Ty>
	void WriteData(Dystopia::BinarySerializer& _out, Image const* _pImg)
	{
		auto const stride = _pImg->mnWidth * _pImg->mnChannels;
		auto const padding = ((stride + 1) & ~3) - stride;

		if constexpr (sizeof(Ty) > 2)
		{
			_out.ManualEndainOverride(Dystopia::eEndianess::eENDIAN_LITTLE);
		}

		for (unsigned y = _pImg->mnHeight; y > 0;)
		{
			Ty* pImg = static_cast<Ty*>(_pImg->mpImageData) + --y * _pImg->mnWidth;

			for (unsigned x = 0; x < _pImg->mnWidth; ++x)
			{
				if constexpr (Ut::IsSame<Ut::Decay_t<Ty>, ColorRGBA>::value)
				{
					_out << pImg->sub_c;
					_out << pImg->sub.a;
				}
				else if constexpr (sizeof(Ty) == 1)
				{
					_out << *pImg;
					_out << *pImg;
					_out << *pImg;
				}
				else
				{
					_out << *pImg;
				}

				++pImg;
			}

			for (unsigned p = 0; p < padding; ++p)
				_out << '\0';
		}
	}
}

Image* ImageParser::LoadBMP(const std::string& _path, Image* _pImage)
{
	auto file = Dystopia::Serialiser::OpenFile<Dystopia::BinarySerializer>(_path.c_str());

	HeaderBMP fileHeader;
	InfoBMP fileInfo{};

	if (file.EndOfInput())
	{
		DEBUG_PRINT(eLog::WARNING, "ImageParser Warning: File \"%s\" not found!", _path.c_str());
		return nullptr;
	}

	// Read the BMP and info Headers
	file >> fileHeader.mType;
	file >> fileHeader.mBytes;
	file >> fileHeader.mReserved;
	file >> fileHeader.mOffset;
	file >> fileInfo.mSize;
	file >> fileInfo.mWidth;
	file >> fileInfo.mHeight;
	file >> fileInfo.mPlanes;
	file >> fileInfo.mBits;
	file >> fileInfo.mCompression;
	file >> fileInfo.mBMPSize;
	file >> fileInfo.mResolutionX;
	file >> fileInfo.mResolutionY;
	file >> fileInfo.mNumColors;
	file >> fileInfo.mImptColors;

	if (fileInfo.mSize > sizeof(InfoBMP))
	{
		file.Skip(fileInfo.mSize - sizeof(InfoBMP));
	}
	
	// Early bail checks before we do anything
	if (BMP::IsUnreadable(fileHeader, fileInfo))
	{
		return nullptr;
	}

	// Read color palette if there is one
	ColorRGBA mPalette[256];
	if (fileInfo.mBits < 16 && fileInfo.mNumColors)
	{
		 BMP::ColorPalette(file, fileInfo, mPalette);
	}

	// Seek to start of image data, just in case
	if ( fileHeader.mOffset > sizeof(HeaderBMP) + fileInfo.mSize)
		file.Skip(fileHeader.mOffset - (sizeof(HeaderBMP) + fileInfo.mSize));

	if (!_pImage)
	{
		// We don't know what to set it to
		// Default to something and allow change later
		_pImage = Dystopia::DefaultAllocator<Image>::ConstructAlloc(
			_path.substr(_path.find_last_of("/\\") + 1).c_str(), false, false,
			static_cast<unsigned>(GL_SRGB_ALPHA), static_cast<unsigned>(GL_RGBA),
			static_cast<unsigned>(fileInfo.mWidth),
			static_cast<unsigned>(fileInfo.mHeight),
			4u, 1u,
			nullptr
		);
	}

	BMP::ReadData(file, fileInfo, mPalette, _pImage);
	return _pImage;
}

bool ImageParser::WriteBMP(const std::string& _path, void* _pImg, int _nWidth, int _nHeight)
{
	Image mData{
		_path.substr(_path.find_last_of("/\\") + 1).c_str(), false, false,
		0u, 0u, static_cast<unsigned>(_nWidth), static_cast<unsigned>(_nHeight), 4u, 1u, _pImg
	};

	auto ret = WriteBMP(_path, &mData);

	// We're not responsible for freeing it
	mData.mpImageData = nullptr; 
	return ret;
}


bool ImageParser::WriteBMP(const std::string& _path, Image const* _pImg)
{
	auto file = Dystopia::Serialiser::OpenFile<Dystopia::BinarySerializer>(_path.c_str(), Dystopia::Serialiser::MODE_WRITE);

	if (file.EndOfInput())
		return false;

	HeaderBMP head{};
	InfoBMP   info{};

	info.mSize        = sizeof(InfoBMP);
	info.mWidth       = _pImg->mnWidth;
	info.mHeight      = _pImg->mnHeight;
	info.mPlanes      = 1; // Must be 1;
	info.mBits        = static_cast<uint16_t>(8 * (_pImg->mnChannels < 3 ? 3 : _pImg->mnChannels));
	info.mCompression = 0;
	info.mNumColors   = 0;
	info.mImptColors  = 0;
	info.mResolutionX = 0;
	info.mResolutionY = 0;
	info.mBMPSize     = (_pImg->mnChannels < 3 ? 3 : _pImg->mnChannels) * _pImg->mnWidth * _pImg->mnHeight;

	head.mBytes  = sizeof(HeaderBMP) + sizeof(InfoBMP) + info.mBMPSize;
	head.mOffset = sizeof(HeaderBMP) + sizeof(InfoBMP) - sizeof(head.mType);

	// Write the header
	file << 'B';
	file << 'M';
	file << head.mBytes;
	file << head.mReserved;
	file << head.mOffset;

	// Write the info section
	file << info.mSize;
	file << info.mWidth;
	file << info.mHeight;
	file << info.mPlanes;
	file << info.mBits;
	file << info.mCompression;
	file << info.mBMPSize;
	file << info.mResolutionX;
	file << info.mResolutionY;
	file << info.mNumColors;
	file << info.mImptColors;

	// Check for failure before continue
	if (file.EndOfInput())
	{
		remove(_path.c_str());	// Delete the erroneous file
		return false;
	}

	switch (_pImg->mnChannels)
	{
	case 1:
		BMP::WriteData<unsigned char>(file, _pImg);
		break;
	case 3:
		BMP::WriteData<StructRGB>(file, _pImg);
		break;
	case 4:
		BMP::WriteData<ColorRGBA>(file, _pImg);
		break;
	default:
		remove(_path.c_str());	// Delete the erroneous file
		return false;
	}

	return true;
}


