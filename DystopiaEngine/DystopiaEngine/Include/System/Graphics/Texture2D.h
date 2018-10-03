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

		Texture2D(void) noexcept;
		explicit Texture2D(const std::string&, bool _bAlpha = true);
		Texture2D(unsigned _nWidth, unsigned _nHeight, void*, bool _bAlpha = true);
		~Texture2D(void) noexcept;


		void GenerateMipmap(void) const;

		std::string GetPath() const noexcept;

	private:

		std::string mPath;
		void InitTexture(void*, bool _bAlpha);
		std::string mPath;
	};
}



#endif		// INCLUDE GUARD

