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
	using Dystopia::Image;

	// Uses external library
	Image* LoadPNG(const std::string&);

	Image* LoadBMP(const std::string&);
	Image* LoadDDS(const std::string&);

	bool WriteBMP(const std::string&, void* _pImg, int _nWidth, int _nHeight);
	bool WriteBMP(const std::string&, Image const* _pImg);

	bool WriteDDS(const std::string&, Image const* _pImg);
}



#endif		// INCLUDE GUARD

