/* HEADER *********************************************************************************/
/*!
\file	ImageParser.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Helper functions to load images. I have no idea if I'm doing this right.
	References: Wikipedia
		-BMP : http://paulbourke.net/dataformats/bmp/
		     : https://www.fileformat.info/format/os2bmp/egff.htm

		-PNG : 
		-JPG : 

	TODO: Endianess, Compressed file formats

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "IO\ImageParser.h"			// File header
#include "System\Graphics\Image.h"	// Image
#include "Utility\DebugAssert.h"	// DEBUG_PRINT

#include <cmath>		// abs
#include <string>		// string
#include <fstream>		// ifstream

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
		uint32_t	mSize;
		int32_t		mWidth;
		int32_t		mHeight;
		uint16_t	mPlanes;
		uint16_t	mBits;
		uint32_t	mCompression;
		int32_t		mResolutionX;
		int32_t		mResolutionY;
		uint32_t	mNumColors;
		uint32_t	mImptColors;
		uint8_t		mIgnore[24];	// 24 bytes in some...
	};

	struct HeaderPNG
	{
		uint8_t mIgnore;
		uint8_t mFormat[3];			// This must be ASCII "PNG" 
		uint8_t mDontCare[4];
	};

	struct ChunkPNG
	{
		uint32_t	mLength;
		uint8_t		mType[4];
		std::string	mData;
		uint32_t	mCRC;
	};

	struct StructRGBA
	{
		uint8_t r, g, b, a;
	};

	union ColorRGBA
	{
		uint32_t mColor;

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
		if (_info.mBits != 8 || _info.mBits != 24 || _info.mBits != 32)
			return true;

		return false;
	}

	void ColorPalette(std::ifstream& _file, InfoBMP& _fileInfo, ColorRGBA(&_palette)[256])
	{
		for (unsigned n = 0; n < _fileInfo.mNumColors; ++n)
		{
			_file.read(reinterpret_cast<char*>(&_palette[n].sub.b), 1);
			_file.read(reinterpret_cast<char*>(&_palette[n].sub.g), 1);
			_file.read(reinterpret_cast<char*>(&_palette[n].sub.r), 1);
			_file.read(reinterpret_cast<char*>(&_palette[n].sub.a), 1);
		}
	}

	// 8 Bits using Color Palette
	void* Palette_ColorBMP(std::ifstream& _in, InfoBMP _info, ColorRGBA(&_palette)[256])
	{
		DEBUG_PRINT("Read image using color palette!\n");
		ColorRGBA* data = new ColorRGBA[_info.mWidth * std::abs(_info.mHeight)];

		for (int n = 0; n < std::abs(_info.mHeight); ++n)
		{
			for (int m = 0; m < _info.mWidth; ++m)
			{
				uint8_t i;
				_in.read(reinterpret_cast<char*>(&i), sizeof(uint8_t));
				data[_info.mWidth * n + m] = _palette[i];
			}

			if (_info.mWidth % 4)
			{
				_in.ignore(4 - _info.mWidth % 4);
			}
		}

		return data;
	}

	// 24 Bits
	void* RGB_ColorBMP(std::ifstream& _in, InfoBMP _info)
	{
		DEBUG_PRINT("Read image as RGB color!\n");
		
		unsigned chunkLength = _info.mWidth * 3;
		unsigned padding = ((chunkLength + 3) >> 2) * 4 - chunkLength; // pad widths to multiple of 4

		unsigned size = chunkLength * std::abs(_info.mHeight);
		uint8_t* data = new uint8_t[size];
		uint8_t* ptr = data;

		for (int n = 0; n < std::abs(_info.mHeight); ++n)
		{
			_in.read(reinterpret_cast<char*>(ptr), chunkLength);
			_in.ignore(padding);

			ptr += chunkLength;
		}

		return data;
	}

	// 32 Bits
	void* RGBA_ColorBMP(std::ifstream& _in, InfoBMP _info)
	{
		DEBUG_PRINT("Read image as RGBA color!\n");
		ColorRGBA* data = new ColorRGBA[_info.mWidth * std::abs(_info.mHeight)];

		for (int n = 0; n < _info.mWidth * std::abs(_info.mHeight); ++n)
		{
			_in.read(reinterpret_cast<char*>(&data[n].sub.b), 1);
			_in.read(reinterpret_cast<char*>(&data[n].sub.g), 1);
			_in.read(reinterpret_cast<char*>(&data[n].sub.r), 1);
			_in.read(reinterpret_cast<char*>(&data[n].sub.a), 1);
		}

		return data;
	}

	Image ReadData(std::ifstream& _in, InfoBMP _info, ColorRGBA(&_palette)[256])
	{
		Image data = { 
			static_cast<unsigned>(_info.mWidth ), 
			static_cast<unsigned>(_info.mHeight), 
			nullptr
		};

		switch (_info.mBits)
		{
		case 8:
			data.mpImageData = Palette_ColorBMP(_in, _info, _palette);
			break;
		case 24:
			data.mpImageData = RGB_ColorBMP(_in, _info);
			break;
		case 32:
			data.mpImageData = RGBA_ColorBMP(_in, _info);
			break;
		}

		return data;
	}
}

Image ImageParser::LoadBMP(const std::string& _path)
{
	std::ifstream file{ _path, std::ios::binary | std::ios::in };

	HeaderBMP fileHeader;
	InfoBMP fileInfo{};

	if (file.fail())
	{
		DEBUG_PRINT("ImageParser Warning: File \"%s\" not found!", _path.c_str());
		file.close();
		return { 0, 0, nullptr };
	}

	// Read the BMP and info Headers
	file.read(reinterpret_cast<char*>(&fileHeader), sizeof(HeaderBMP));
	file.read(reinterpret_cast<char*>(&fileInfo.mSize), sizeof(uint32_t));
	if (fileInfo.mSize > sizeof(InfoBMP))
	{
		file.read(reinterpret_cast<char*>(&fileInfo.mWidth), sizeof(InfoBMP) - sizeof(uint32_t));
		file.seekg(fileInfo.mSize - sizeof(InfoBMP), std::ios::cur);
	}
	else
	{
		file.read(reinterpret_cast<char*>(&fileInfo.mWidth), fileInfo.mSize - sizeof(uint32_t));
	}

	// Early bail checks before we do anything
	if (BMP::IsUnreadable(fileHeader, fileInfo))
	{
		file.close();
		return { 0, 0, nullptr };
	}

	// Read color palette if there is one
	ColorRGBA mPalette[256];
	if (fileInfo.mNumColors)
	{
		 BMP::ColorPalette(file, fileInfo, mPalette);
	}

	// Seek to start of image data, just in case
	file.seekg(fileHeader.mOffset, std::ios::beg);

	Image fileData = BMP::ReadData(file, fileInfo, mPalette);

	if (file.fail())
	{
		DEBUG_PRINT("ImageParser Error: Error reading file \"%s\"!", _path.c_str());
		__debugbreak();
	}

	file.close();
	return fileData;
}

Image ImageParser::LoadPNG(const std::string&)
{
	return Image{};
}

Image ImageParser::LoadJPG(const std::string&)
{
	return Image{};
}

bool ImageParser::WriteBMP(const std::string& _path, void* _pImg, int _nWidth, int _nHeight)
{
	std::ofstream file{ _path, std::ios::binary | std::ios::out };

	if (file.fail())
		return false;

	unsigned imgSize = _nWidth * _nHeight * 3;
	HeaderBMP head;
	InfoBMP   info;

	info.mWidth			= _nWidth;
	info.mHeight		= _nHeight;
	info.mPlanes		= 1; // Must be 1;
	info.mBits			= 24;
	info.mCompression	= 0;
	info.mNumColors		= 0;
	info.mImptColors	= 0;

	head.mBytes = sizeof(HeaderBMP) + sizeof(InfoBMP);
	head.mOffset = sizeof(HeaderBMP) + sizeof(InfoBMP);

	// Write "BM" infront to denote BMP
	file.write("B", 1);
	file.write("M", 1);

	// Write the rest of the header
	file.write(reinterpret_cast<char*>(&head), sizeof(HeaderBMP) - sizeof(uint16_t));
	
	// Write the info section
	file.write(reinterpret_cast<char*>(&info), sizeof(InfoBMP));

	// Check for failure before continue
	if (file.fail())
	{
		// Something bad happened
		file.close();
		remove(_path.c_str());	// Delete the erroneous file
		return false;
	}

	file.write(static_cast<char*>(_pImg), imgSize);
	return true;
}


