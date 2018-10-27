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

#include "Math/Vector2.h"
#include "Utility/Utility.h"

#include "IO/Image.h"
#include "Allocator/DefaultAlloc.h"

#include <algorithm>
#include <ft2build.h>
#include FT_FREETYPE_H

#if defined(_DEBUG) | defined(DEBUG)
#define FT_CHECK_ERR(_X_) \
if (_X_) __debugbreak()
#else
#define FT_CHECK_ERR(_X_) _X_
#endif

#define WRITE_STEP 1
#if WRITE_STEP
static auto STEP_OUTPUT_DIR = "C:/Users/Laptop/Desktop/tmp/";
#endif


namespace
{
	static FT_Library mpLibFT;
}

Dystopia::FontSystem::FontSystem(void) noexcept
	: mnPaddingX{ 0 }, mnPaddingY{ 0 }
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
	// https://www.freetype.org/freetype2/docs/reference/ft2-base_interface.html

	FT_Face pFontFace;
	Image charMap{
		false, 0x1903, 0x1903,
		1024, 1024, 1, 1, DefaultAllocator<char[]>::Alloc(1024 * 1024)
	};

	std::memset(charMap.mpImageData, 0, 1024 * 1024);

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

			mSetSpace.Insert( PackFont(
				mFreeSpace, static_cast<uchar*>(charMap.mpImageData), charMap.mnWidth,
				FontBmp.buffer, FontBmp.width, FontBmp.rows
			));
		}

#       if WRITE_STEP
			ImageParser::WriteBMP(STEP_OUTPUT_DIR + std::to_string(c) +".bmp", &charMap);
#       endif
	}

	FT_Done_Face(pFontFace);

	TextureAtlas* pAtlas = EngineCore::GetInstance()->GetSubSystem<TextureSystem>()->GenAtlas();
	pAtlas->SetTexture(EngineCore::GetInstance()->GetSubSystem<TextureSystem>()->LoadRaw<Texture2D>(&charMap));

	for (auto& e : mSetSpace)
		pAtlas->AddSection(Math::Vec2{ e.x, e.y }, e.mnWidth, e.mnHeight);

	return mpAtlas;
}

Dystopia::FontSystem::CharSpace Dystopia::FontSystem::PackFont(AutoArray<CharSpace>& _spaces, uchar* _dest, unsigned _nImageWidth, uchar* _src, unsigned _nWidth, unsigned _nHeight)
{
	// We use a hardcoded texture size to prevent finding the "best fit" size
	// Then we simply tweak the font size to load so we have an acceptable fit
	// ie. Find a good size to hardcode through testing
	// Packing is done based off the algorithm described in the link below
	// Except where we do not want to rotate anything for simplicity
	// https://github.com/TeamHypersomnia/rectpack2D#algorithm

	for (auto& e : Ut::Range(_spaces.begin(), _spaces.end()).Reverse())
	{
		if (e.mnWidth >= (_nWidth + 2 * mnPaddingX) && e.mnHeight >= (_nHeight + 2 * mnPaddingY))
		{
			auto rWidth  = e.mnWidth  - (_nWidth  + (mnPaddingX << 1));
			auto rHeight = e.mnHeight - (_nHeight + (mnPaddingY << 1));
			Ut::Swap(e, _spaces.back());
			auto& at = _spaces.back();
			auto copy = at;

			Blit(_src, _nWidth, _nHeight, _dest + at.x + mnPaddingX + (at.y + mnPaddingY) * _nImageWidth, _nImageWidth);

			if (!rWidth && !rHeight)
			{
				_spaces.pop_back();
			}
			else if (!rWidth || !rHeight)
			{
				at.x += (rWidth  > 0) * _nWidth;
				at.y += (rHeight > 0) * _nHeight;
				
				(rWidth ? at.mnWidth : at.mnHeight) = rWidth ? rWidth : rHeight;
			}
			else if (rWidth > rHeight)
			{
				// Make a copy!
				auto copy = at;

				at.x += _nWidth + 2 * mnPaddingX;
				at.mnWidth = rWidth;
				_spaces.EmplaceBack(copy.x, copy.y + _nHeight + 2 * mnPaddingY, _nWidth + 2 * mnPaddingX, rHeight);
			}
			else
			{

				at.y += _nHeight + 2 * mnPaddingY;
				at.mnHeight = rHeight;
				_spaces.EmplaceBack(copy.x + _nWidth + 2 * mnPaddingX, copy.y, rWidth, _nHeight + 2 * mnPaddingY);
			}

			copy.mnWidth  = _nWidth + 2 * mnPaddingX;
			copy.mnHeight = _nHeight + 2 * mnPaddingY;

			std::sort(_spaces.begin(), _spaces.end(), [](auto const& _lhs, auto const& _rhs) -> bool {
				return (_lhs.mnWidth * _lhs.mnHeight) < (_rhs.mnWidth * _rhs.mnHeight);
			});

			return copy;
		}
	}

	DEBUG_BREAK(true, "FontSystem Error: Could not pack font into texture!\n");
}

void Dystopia::FontSystem::Blit(const uchar* _src, unsigned _nWidth, unsigned _nHeight, uchar* _dest, unsigned _nImageWidth)
{
	for (unsigned y = 0; y < _nHeight; ++y)
	{
		for (unsigned x = 0; x < _nWidth; ++x)
		{
			*(_dest + x) = *_src;

			++_src;
		}

		_dest += _nImageWidth;
	}
}


