/* HEADER *********************************************************************************/
/*!
\file	GameObject.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Flags for Game Objects

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _OBJFLAGS_H_
#define _OBJFLAGS_H_

namespace Dystopia
{
	enum eObjFlag
	{
		FLAG_NONE			= 0,
		FLAG_ACTIVE			= 1 << 0,

		FLAG_LAYER_UI		= 1 << 5,
		FLAG_LAYER_WORLD	= 1 << 6,

		FLAG_REMOVE			= 1 << 31,
	};


	// ======================  Compound flags ====================== //

	constexpr unsigned FLAG_EXAMPLE = FLAG_LAYER_UI | FLAG_LAYER_WORLD;
}



#endif		// INCLUDE GUARD

