/* HEADER *********************************************************************************/
/*!
\file	Image.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Class to hold raw image data loaded into memory

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _IMAGE_H_
#define _IMAGE_H_

#include "DataStructure/HashString.h"


namespace Dystopia
{
	struct Image
	{
		HashString mstrName;
		bool mbCompressed;
		unsigned mnRawFormat, mnFormat;
		unsigned mnWidth, mnHeight;
		unsigned mnChannels, mnMipMaps;
		void* mpImageData;

		~Image(void) noexcept;

		void OnEditorUI(void);
	};
}



#endif		// INCLUDE GUARD

