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
	enum eObjFlag : unsigned
	{
		FLAG_NONE			= 0u,
		FLAG_ACTIVE			= 1u << 0,
		FLAG_STATIC			= 1u << 1,

		FLAG_LAYER_UI		= 1u << 5,
		FLAG_LAYER_WORLD	= 1u << 6,


		FLAG_EDITOR_OBJ		= 1u << 29,
		FLAG_RESERVED		= 1u << 30,
		FLAG_REMOVE			= 1u << 31,

		// ======================  Compound flags ====================== //

		FLAG_ALL_LAYERS     = FLAG_LAYER_UI | FLAG_LAYER_WORLD,
	};
}



#endif		// INCLUDE GUARD

