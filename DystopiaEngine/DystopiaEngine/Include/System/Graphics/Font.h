/* HEADER *********************************************************************************/
/*!
\file	TextRenderer.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
Renders text

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _FONTDATA_H_
#define _FONTDATA_H_

#include "DataStructure/HashString.h"

#include "System/Graphics/TextureAtlas.h"
#include "System/Graphics/CharSpace.h"


namespace Dystopia
{
	struct Font
	{
		HashString mstrName;
		TextureAtlas* mpAtlas;
		AutoArray<CharSpace> mSpaces;
	};
}



#endif		// INCLUDE GUARD

