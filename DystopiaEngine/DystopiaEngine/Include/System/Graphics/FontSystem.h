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
#ifndef _FONTSYS_H_
#define _FONTSYS_H_

#include "Utility/Utility.h"
#include "DataStructure/MagicArray.h"

#include "System/Graphics/Font.h"

#include <string>


namespace Dystopia
{
	struct Image;
	class  Texture;
	struct CharSpace;
	class  TextureAtlas;

	class FontSystem
	{
	public:

		FontSystem(void) noexcept;
		~FontSystem(void) noexcept;

		void Init(void);

		Font* LoadFont(const std::string& _strPath);

		void SetPadding(unsigned _x, unsigned _y);

	private:

		MagicArray<Font> mFontMaps;

		unsigned mnOutDim;
		unsigned mnPaddingX, mnPaddingY;

		using uchar = unsigned char;

		Font* LoadFromFont(const HashString&, Font* _out);
		Font* LoadExisting(const HashString&, Font* _out);

		CharSpace PackFont(AutoArray<CharSpace>&, uchar* dest, uchar* src, unsigned _nWidth, unsigned _nHeight);
		void OutputFontmap(Font const*, HashString const&);

		void Blit(const uchar* _src, unsigned _nWidth, unsigned _nHeight, uchar* _dest);
	};
}






// ============================================ FUNCTION DEFINITIONS ============================================ // 


// Func defs goes here



#endif		// INCLUDE GUARD

