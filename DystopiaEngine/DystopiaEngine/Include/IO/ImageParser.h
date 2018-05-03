/* HEADER *********************************************************************************/
/*!
\file	ImageParser.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Helper functions to load images into an OpenGL recognisable format
	I have no idea if I'm doing this right.

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _IMAGE_PARSER_H_
#define _IMAGE_PARSER_H_

#include <cstdint>
#include <string>

namespace Dystopia
{
	struct Image;
}

namespace ImageParser
{
	Dystopia::Image LoadBMP(const std::string&);

	// External - Use external library, ask later
	// too troublesome
	Dystopia::Image LoadPNG(const std::string&);

	// Not implemented - too troublesome
	Dystopia::Image LoadJPG(const std::string&);

	bool WriteBMP(const std::string&, void* _pImg, int _nWidth, int _nHeight);
}



#endif		// INCLUDE GUARD

