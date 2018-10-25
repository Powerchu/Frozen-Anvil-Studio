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

#include <string>

namespace Dystopia
{
	struct Image;
}

namespace ImageParser
{
	// Uses external library
	Dystopia::Image* LoadPNG(const std::string&);

	Dystopia::Image* LoadBMP(const std::string&);
	Dystopia::Image* LoadDDS(const std::string&);

	bool WriteBMP(const std::string&, void* _pImg, int _nWidth, int _nHeight);
}



#endif		// INCLUDE GUARD

