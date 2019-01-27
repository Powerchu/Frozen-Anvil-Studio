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
	class Texture2D : public Texture
	{
	public:

		explicit Texture2D(HashString const&, Image&) noexcept;

		void GenerateMipmap(void) const;

		void ReplaceTexture(unsigned w, unsigned h, void* _pData, bool _bAlpha = true);

	private:
		void Load(void);
		void ResolveLoad(void);
		void InitTexture(void);
		void InitCompressedTexture(void);

		void ReloadImage(Image&);
		void ReloadImageTrigger(void);
	};
}



#endif		// INCLUDE GUARD

