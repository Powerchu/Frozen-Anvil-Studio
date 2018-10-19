/* HEADER *********************************************************************************/
/*!
\file	TextureAtlas.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Texture containing multiple textures

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _TEXTURE2D_H_
#define _TEXTURE2D_H_

#include <string>


namespace Dystopia
{
	class Texture;

	class TextureAtlas
	{
	public:

		TextureAtlas(void) noexcept;
		explicit TextureAtlas(const std::string&);
		~TextureAtlas(void) noexcept;



	private:

		Texture* mpTexture;
	};
}



#endif		// INCLUDE GUARD

