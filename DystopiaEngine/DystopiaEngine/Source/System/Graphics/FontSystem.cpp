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
#include "System/Graphics/CharSpace.h"
#include "System/Graphics/Texture.h"
#include "System/Graphics/Texture2D.h"
#include "System/Graphics/TextureAtlas.h"
#include "System/Graphics/TextureSystem.h"

#include "System/Driver/Driver.h"
#include "System/File/FileSystem.h"

#include "Globals.h"
#include "Math/Vectors.h"
#include "Utility/Utility.h"
#include "DataStructure/AutoArray.h"
#include "DataStructure/MagicArray.h"

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

Dystopia::Font* Dystopia::FontSystem::LoadFont(const std::string& _strName)
{
	auto pFileSys = EngineCore::GetInstance()->Get<FileSystem>();

	auto const at = _strName.find_last_of("/\\") + 1, end = _strName.rfind('.');
	HashString strName{ _strName.substr(at).c_str() };

	auto it = Ut::Find(mFontMaps, [&](auto& e)-> bool{ return strName == e.mstrName; });
	if (it != mFontMaps.end())
		return &*it;

	auto pRet = mFontMaps.Emplace(strName);
	if(pFileSys->CheckFileExist((strName + "." + Gbl::FONTMAP_EXT).c_str(), eFileDir::eResource))
		return LoadExisting(pFileSys->GetFullPath(strName.c_str(), eFileDir::eResource).c_str(), pRet);
	return LoadFromFont(pFileSys->GetFullPath(strName.c_str(), eFileDir::eResource).c_str(), pRet);
}

Dystopia::Font* Dystopia::FontSystem::LoadExisting(const HashString& _strPath, Font* _out)
{
	//Image* fontAtlas = ImageParser::LoadBMP((_strPath + ".bmp").c_str());
	auto pTexture = EngineCore::GetInstance()->Get<TextureSystem>()->LoadTexture<Texture2D>((_strPath + ".bmp").c_str());
	_out->mpAtlas = EngineCore::GetInstance()->Get<TextureSystem>()->GenAtlas();
	_out->mpAtlas->SetTexture(pTexture);

	auto in = Serialiser::OpenFile<TextSerialiser>((_strPath + "." + Gbl::FONTMAP_EXT).c_str());

	size_t sz;
	in >> sz;
	in.ConsumeStartBlock();
	while (sz--)
	{
		_out->mSpaces.EmplaceBack();
		auto& e = _out->mSpaces.back();

		in >> e.x;
		in >> e.y;
		in >> e.mnWidth;
		in >> e.mnHeight;
		in >> e.mnBearingX;
		in >> e.mnBearingY;
		in >> e.mnAdvance;
		in.ConsumeStartBlock();
	}

	_out->mpAtlas->LoadAtlas(in);
	return _out;
}

Dystopia::Font* Dystopia::FontSystem::LoadFromFont(const HashString& _strPath, Font* _out)
{
	// https://www.freetype.org/freetype2/docs/reference/ft2-base_interface.html

	FT_Face pFontFace;
	Image charMap{
		_out->mstrName, false, true, 0x1903, 0x1903,
		mnOutDim, mnOutDim, 1, 1, DefaultAllocator<char[]>::Alloc(mnOutDim * mnOutDim)
	};

	std::memset(charMap.mpImageData, 0, mnOutDim * mnOutDim);

	// We tweak the hardcoded font size to load so we have an acceptable fit
	// ie. Find a good size to hardcode through testing
	FT_CHECK_ERR(FT_New_Face(mpLibFT, _strPath.c_str(), 0, &pFontFace));
	FT_CHECK_ERR(FT_Set_Pixel_Sizes(pFontFace, 0, 140));

	AutoArray<CharSpace> mFreeSpace;
	mFreeSpace.EmplaceBack(0u, 0u, charMap.mnWidth, charMap.mnHeight);

	auto& outCoords = _out->mSpaces;
	for (unsigned char c = ' '; c < 0x7F; ++c)
	{
		if (!FT_Load_Char(pFontFace, c, FT_LOAD_RENDER))
		{
			auto& FontBmp = pFontFace->glyph->bitmap;

			outCoords.EmplaceBack(PackFont(
				mFreeSpace, static_cast<uchar*>(charMap.mpImageData),
				FontBmp.buffer, FontBmp.width, FontBmp.rows
			));

			outCoords.back().mnAdvance  = pFontFace->glyph->advance.x >> 6;
			outCoords.back().mnBearingX = pFontFace->glyph->bitmap_left;
			outCoords.back().mnBearingY = pFontFace->glyph->bitmap_top;
		}

#       if WRITE_STEP
			ImageParser::WriteBMP(STEP_OUTPUT_DIR + std::to_string(c) + ".bmp", &charMap);
#       endif
	}

	FT_Done_Face(pFontFace);
	ImageParser::WriteBMP((_strPath + ".bmp").c_str(), &charMap);

	auto pTexSys = EngineCore::GetInstance()->Get<TextureSystem>();
	_out->mpAtlas = pTexSys->GenAtlas();
	_out->mpAtlas->SetTexture(pTexSys->LoadRaw<Texture2D>(&charMap, _strPath.c_str()));

	for (auto& e : outCoords)
	{
		_out->mpAtlas->AddSection(Math::Vec2{ float(e.x), float(e.y) }, e.mnWidth, e.mnHeight);
	}

	OutputFontmap(&charMap, _out, _strPath);
	return _out;
}

Dystopia::CharSpace Dystopia::FontSystem::PackFont(AutoArray<CharSpace>& _spaces, uchar* _dest, uchar* _src, unsigned _nWidth, unsigned _nHeight)
{
	// Packing is done based on the algorithm described in the link below
	// However we skip some of the process to simplify things
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

void Dystopia::FontSystem::OutputFontmap(Image* _pData, Font const* _pFont, HashString const& _strPath)
{
	auto out = Serialiser::OpenFile<TextSerialiser>((_strPath + "." + Gbl::FONTMAP_EXT).c_str(), Serialiser::MODE_WRITE);

	auto& ch = _pFont->mSpaces;
	out << ch.size();
	out.InsertStartBlock("FONT_DATA");
	for (auto& e : ch)
	{
		out << e.x;
		out << e.y;
		out << e.mnWidth;
		out << e.mnHeight;
		out << e.mnBearingX;
		out << e.mnBearingY;
		out << e.mnAdvance;
		out.InsertStartBlock("PARTITION");
	}

	_pFont->mpAtlas->SaveAtlas(out);
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


