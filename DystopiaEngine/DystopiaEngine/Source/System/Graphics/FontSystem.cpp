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

#include "IO/Image.h"
#include "Allocator/DefaultAlloc.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#if defined(_DEBUG) | defined(DEBUG)
#define FT_CHECK_ERR(_X_) \
if (_X_) __debugbreak()
#else
#define FT_CHECK_ERR(_X_) _X_
#endif


namespace
{
	static FT_Library mpLibFT;
}

Dystopia::FontSystem::FontSystem(void) noexcept
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

Dystopia::TextureAtlas* Dystopia::FontSystem::LoadFont(const std::string& _strPath)
{
	// https://www.freetype.org/freetype2/docs/reference/ft2-base_interface.html

	FT_Face pFontFace;
	Image charMap{
		false, 0x1903, 0x1903,
		1024, 1024, 1, 1, DefaultAllocator<char[]>::Alloc(1024 * 1024)
	};

	FT_CHECK_ERR(FT_New_Face(mpLibFT, _strPath.c_str(), 0, &pFontFace));
	FT_CHECK_ERR(FT_Set_Pixel_Sizes(pFontFace, 0, 64));

	for (auto c = '!'; c < 0x7F; ++c)
	{
		if (FT_Load_Glyph(pFontFace, c, FT_LOAD_RENDER))
		{
			pFontFace->glyph->bitmap.buffer;


		}
	}

	FT_Done_Face(pFontFace);

	EngineCore::GetInstance()->GetSubSystem<TextureSystem>()->LoadRaw<Texture2D>(&charMap);

	return nullptr;
}

void Dystopia::FontSystem::PackFont(Texture*)
{
	// We use a hardcoded texture size to prevent finding the "best fit" size
	// Then we simply tweak the font size to load so we have an acceptable fit
	// ie. Find a good size to hardcode through testing
	// Packing is done based off the algorithm described in the link below
	// Except where we do not want to rotate anything for simplicity
	// https://github.com/TeamHypersomnia/rectpack2D#algorithm


}

void Dystopia::FontSystem::Blit(const uchar* _src, unsigned _nWidth, unsigned _nHeight, uchar* _dest)
{
}


