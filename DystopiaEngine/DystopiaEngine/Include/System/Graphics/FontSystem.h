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

	private:

		using uchar = unsigned char;

		void PackFont(Texture*);
		void OutputFontmap(TextureAtlas*);

		void Blit(const uchar* _src, unsigned _nWidth, unsigned _nHeight, uchar* _dest);
	};
}






// ============================================ FUNCTION DEFINITIONS ============================================ // 


// Func defs goes here



#endif		// INCLUDE GUARD

