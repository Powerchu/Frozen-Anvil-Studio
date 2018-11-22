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
#include "DataStructure/AutoArray.h"

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

		TextureAtlas* LoadFont(const std::string& _strPath, AutoArray<CharSpace>& _outCoords);

		void SetPadding(unsigned _x, unsigned _y);

	private:

		unsigned mnOutDim;
		unsigned mnPaddingX, mnPaddingY;

		using uchar = unsigned char;

		TextureAtlas* LoadFromFont(const std::string&, AutoArray<CharSpace>&);
		TextureAtlas* LoadExisting(const std::string&, AutoArray<CharSpace>&);

		CharSpace PackFont(AutoArray<CharSpace>&, uchar* dest, uchar* src, unsigned _nWidth, unsigned _nHeight);
		void OutputFontmap(TextureAtlas*, Image*, AutoArray<CharSpace> const&, std::string const&);

		void Blit(const uchar* _src, unsigned _nWidth, unsigned _nHeight, uchar* _dest);
	};
}






// ============================================ FUNCTION DEFINITIONS ============================================ // 


// Func defs goes here



#endif		// INCLUDE GUARD

