/* HEADER *********************************************************************************/
/*!
\file	TextureSystem.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Manages Textures

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _TEXTURESYS_H_
#define _TEXTURESYS_H_

#include "DataStructure\MagicArray.h"

#include <string>


namespace Dystopia
{
	class Texture;

	class TextureSystem
	{
	public:

		void Shutdown(void) noexcept;

		template <typename Ty>
		Ty* GetTexture(std::string&);


	private:

		MagicArray<Texture> mTextures;
	};
}



#endif		// INCLUDE GUARD

