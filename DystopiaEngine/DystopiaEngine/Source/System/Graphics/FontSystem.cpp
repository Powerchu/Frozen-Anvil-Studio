/* HEADER *********************************************************************************/
/*!
\file	FontSystem.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Interface for generating font maps through freetype font library

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "System/Graphics/FontSystem.h"
#include "System/Graphics/Texture.h"
#include "System/Graphics/Texture2D.h"
#include "System/Graphics/TextureAtlas.h"
#include "System/Graphics/TextureSystem.h"

#include "System/Driver/Driver.h"
#include "System/File/FileSystem.h"

#include "Globals.h"
#include "Math/Vector2.h"
#include "Utility/Utility.h"

#include "IO/Image.h"
#include "IO/TextSerialiser.h"
#include "Allocator/DefaultAlloc.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#if defined(_DEBUG) | defined(DEBUG)
#define FT_CHECK_ERR(_X_) \
if (_X_) __debugbreak()
#else
#define FT_CHECK_ERR(_X_) _X_
#endif

#define WRITE_STEP 0
#if WRITE_STEP
static auto STEP_OUTPUT_DIR = "C:/Users/Laptop/Desktop/tmp/";
#endif


namespace
{
	static FT_Library mpLibFT;
}

Dystopia::FontSystem::FontSystem(void) noexcept
	: mnPaddingX{ 8 }, mnPaddingY{ 8 }, mnOutDim{ 1024 }
{
	FT_CHECK_ERR(FT_Init_FreeType(&mpLibFT));
}

Dystopia::FontSystem::~FontSystem(void) noexcept
{
	FT_CHECK_ERR(FT_Done_FreeType(mpLibFT));
	mpLibFT = nullptr;
}

void Dystopia::FontSystem::Init(void)
{
	 
}

void Dystopia::FontSystem::SetPadding(unsigned _x, unsigned _y)
{
	mnPaddingX = _x;
	mnPaddingY = _y;
}

Dystopia::TextureAtlas* Dystopia::FontSystem::LoadFont(const std::string& _strPath)
{
	//auto pFS = EngineCore::GetInstance()->Get<FileSystem>();

	std::string path = _strPath.substr(0, _strPath.rfind('.') + 1);

	//if(pFS->CheckFileExist(path + "dmap", eFileDir::eResource))
	//	return LoadExisting(path);
	return LoadFromFont(_strPath);
}

Dystopia::TextureAtlas* Dystopia::FontSystem::LoadExisting(const std::string& _strPath)
{
	Image* fontAtlas = ImageParser::LoadBMP(_strPath + "bmp");
	fontAtlas->mnRawFormat = fontAtlas->mnFormat; // Remove sRGB

	auto pTexture = EngineCore::GetInstance()->Get<TextureSystem>()->LoadRaw<Texture2D>(fontAtlas);
	auto pAtlas = EngineCore::GetInstance()->Get<TextureSystem>()->GenAtlas();

	pAtlas->SetTexture(pTexture);

	auto in = Serialiser::OpenFile<TextSerialiser>(_strPath + Gbl::FONTMAP_EXT);
	size_t sz;

	in >> sz;
	auto sections = pAtlas->GetAllSections();

	sections.reserve(sz);

	in.ConsumeStartBlock();
	while (!in.EndOfInput())
	{
		sections.EmplaceBack();

		in >> sections.back().uStart;
		in >> sections.back().uStart;
		in >> sections.back().vStart;
		in >> sections.back().uEnd;
		in >> sections.back().uEnd;
		in >> sections.back().mRow;
		in >> sections.back().mCol;
	}

	return nullptr;
}

Dystopia::TextureAtlas* Dystopia::FontSystem::LoadFromFont(const std::string& _strPath)
{
	// https://www.freetype.org/freetype2/docs/reference/ft2-base_interface.html

	FT_Face pFontFace;
	Image charMap{
		false, 0x1903, 0x1903,
		mnOutDim, mnOutDim, 1, 1, DefaultAllocator<char[]>::Alloc(mnOutDim * mnOutDim)
	};

	std::memset(charMap.mpImageData, 0, mnOutDim * mnOutDim);

	// We tweak the hardcoded font size to load so we have an acceptable fit
	// ie. Find a good size to hardcode through testing
	FT_CHECK_ERR(FT_New_Face(mpLibFT, _strPath.c_str(), 0, &pFontFace));
	FT_CHECK_ERR(FT_Set_Pixel_Sizes(pFontFace, 0, 140));

	AutoArray<CharSpace> mSetSpace;
	AutoArray<CharSpace> mFreeSpace;

	mFreeSpace.EmplaceBack(0u, 0u, charMap.mnWidth, charMap.mnHeight);

	for (unsigned char c = '!'; c < 0x7F; ++c)
	{
		if (!FT_Load_Char(pFontFace, c, FT_LOAD_RENDER))
		{
			auto& FontBmp = pFontFace->glyph->bitmap;

			mSetSpace.Insert(PackFont(
				mFreeSpace, static_cast<uchar*>(charMap.mpImageData),
				FontBmp.buffer, FontBmp.width, FontBmp.rows
			));
		}

#       if WRITE_STEP
			ImageParser::WriteBMP(STEP_OUTPUT_DIR + std::to_string(c) + ".bmp", &charMap);
#       endif
	}

	FT_Done_Face(pFontFace);

	TextureAtlas* pAtlas = EngineCore::GetInstance()->GetSubSystem<TextureSystem>()->GenAtlas();
	pAtlas->SetTexture(EngineCore::GetInstance()->GetSubSystem<TextureSystem>()->LoadRaw<Texture2D>(&charMap));

	for (auto& e : mSetSpace)
		pAtlas->AddSection(Math::Vec2{ float(e.x), float(e.y) }, e.mnWidth, e.mnHeight);

	OutputFontmap(pAtlas, &charMap, _strPath);

	return pAtlas;
}

Dystopia::CharSpace Dystopia::FontSystem::PackFont(AutoArray<CharSpace>& _spaces, uchar* _dest, uchar* _src, unsigned _nWidth, unsigned _nHeight)
{
	// Packing is done based off the algorithm described in the link below
	// Except where we do not want to rotate anything for simplicity
	// https://github.com/TeamHypersomnia/rectpack2D#algorithm

	const auto padWidth  = _nWidth  + (mnPaddingX << 1);
	const auto padHeight = _nHeight + (mnPaddingY << 1);

	for (auto& e : Ut::Range(_spaces.begin(), _spaces.end()).Reverse())
	{
		if (e.mnWidth >= (_nWidth + 2 * mnPaddingX) && e.mnHeight >= (_nHeight + 2 * mnPaddingY))
		{
			auto rWidth  = e.mnWidth  - padWidth;
			auto rHeight = e.mnHeight - padHeight;
			Ut::Swap(e, _spaces.back());
			auto& at = _spaces.back();
			auto copy = at;

			Blit(_src, _nWidth, _nHeight, _dest + at.x + mnPaddingX + (at.y + mnPaddingY) * mnOutDim);

			if (!rWidth && !rHeight)
			{
				_spaces.pop_back();
			}
			else if (!rWidth || !rHeight)
			{
				at.x += (rWidth  > 0) * padWidth;
				at.y += (rHeight > 0) * padHeight;
				
				(rWidth ? at.mnWidth : at.mnHeight) = rWidth ? rWidth : rHeight;
			}
			else if (rWidth > rHeight)
			{
				at.x += padWidth;
				at.mnWidth = rWidth;
				_spaces.EmplaceBack(copy.x, copy.y + padHeight, padWidth, rHeight);
			}
			else
			{
				at.y += padHeight;
				at.mnHeight = rHeight;
				_spaces.EmplaceBack(copy.x + padWidth, copy.y, rWidth, padHeight);
			}

			copy.x += mnPaddingX;
			copy.y += mnPaddingY;
			copy.mnWidth  = _nWidth ;
			copy.mnHeight = _nHeight;

			_spaces.Sort([](auto const& _lhs, auto const& _rhs) -> bool {
				return (_lhs.mnWidth * _lhs.mnHeight) > (_rhs.mnWidth * _rhs.mnHeight);
			});

			return copy;
		}
	}

	DEBUG_BREAK(true, "FontSystem Error: Could not pack font into texture!\n");
	return CharSpace{};
}

void Dystopia::FontSystem::OutputFontmap(TextureAtlas* _pMap, Image* _pData, std::string const& _strPath)
{
	std::string path = _strPath.substr(0, _strPath.rfind('.') + 1);
	ImageParser::WriteBMP(path + "bmp", _pData);

	auto out = Serialiser::OpenFile<TextSerialiser>(path + Gbl::FONTMAP_EXT, Serialiser::MODE_WRITE);
	auto const& data = _pMap->GetAllSections();

	out << data.size();
	for (auto const& e : data)
	{
		out.InsertStartBlock("SECTION");
		out << e.uStart;
		out << e.vStart;
		out << e.uEnd;
		out << e.uEnd;
		out << e.mRow;
		out << e.mCol;
	}
}

void Dystopia::FontSystem::Blit(const uchar* _src, unsigned _nWidth, unsigned _nHeight, uchar* _dest)
{
	for (unsigned y = 0; y < _nHeight; ++y)
	{
		for (unsigned x = 0; x < _nWidth; ++x)
		{
			*(_dest + x) = *_src;

			++_src;
		}

		_dest += mnOutDim;
	}
}


