/* HEADER *********************************************************************************/
/*!
\file	FontSystem.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Interface for generating font maps through freetype font library

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _FONTSYS_H_
#define _FONTSYS_H_

#include "Utility/Utility.h"
#include "DataStructure/AutoArray.h"

#include <string>


namespace Dystopia
{
	class Texture;
	class TextureAtlas;

	class FontSystem
	{
	public:

		FontSystem(void) noexcept;
		~FontSystem(void) noexcept;

		void Init(void);

		TextureAtlas* LoadFont(const std::string& _strPath);

		void SetPadding(unsigned _x, unsigned _y);

	private:

		unsigned mnPaddingX, mnPaddingY;

		using uchar = unsigned char;

		struct CharSpace
		{
			unsigned x, y;
			unsigned mnWidth;
			unsigned mnHeight;
		};

		CharSpace PackFont(AutoArray<CharSpace>&, uchar* dest, unsigned _nImageWidth, uchar* src, unsigned _nWidth, unsigned _nHeight);

		void Blit(const uchar* _src, unsigned _nWidth, unsigned _nHeight, uchar* _dest, unsigned _nImageWidth);
	};
}






// ============================================ FUNCTION DEFINITIONS ============================================ // 


// Func defs goes here



#endif		// INCLUDE GUARD

