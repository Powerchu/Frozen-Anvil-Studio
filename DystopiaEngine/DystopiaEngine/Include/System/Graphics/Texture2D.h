/* HEADER *********************************************************************************/
/*!
\file	Texture2D.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	sRGB 2D Textures

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _TEXTURE2D_H_
#define _TEXTURE2D_H_

#include "System/Graphics/Texture.h"

namespace Dystopia
{
	struct Image;

	class Texture2D : public Texture
	{
	public:

		explicit Texture2D(HashString const&) noexcept;

		void GenerateMipmap(void) const;

		void LoadTexture(Image const *);
		void ReplaceTexture(unsigned w, unsigned h, void* _pData, bool _bAlpha = true);

	private:
		void InitTexture(Image const*);
		void InitCompressedTexture(Image const*);

		void ReloadImage(void);
	};
}



#endif		// INCLUDE GUARD

