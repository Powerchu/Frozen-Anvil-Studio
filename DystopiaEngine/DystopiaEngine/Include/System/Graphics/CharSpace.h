/* HEADER *********************************************************************************/
/*!
\file	CharSpace.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Definition for struct containing font size info

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _CHARSPACE_H_
#define _CHARSPACE_H_

namespace Dystopia
{
	struct CharSpace
	{
		unsigned x, y;
		unsigned mnWidth;
		unsigned mnHeight;
		int mnBearingX;
		int mnBearingY;

		unsigned mnAdvance;
	};
}



#endif		// INCLUDE GUARD

