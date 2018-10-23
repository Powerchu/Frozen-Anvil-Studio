/* HEADER *********************************************************************************/
/*!
\file	Framebuffer.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Framebuffer

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _FRAMEBUFFER_H_
#define _FRAMEBUFFER_H_


namespace Dystopia
{
	class Texture;

	class Framebuffer
	{
	public:
		// ====================================== CONSTRUCTORS ======================================= // 

		Framebuffer(void) noexcept;

		~Framebuffer(void) noexcept;


		// ===================================== MEMBER FUNCTIONS ==================================== // 

		void Init(unsigned _nWidth, unsigned _nHeight, bool _bAlpha = true);

		void Bind(void) const noexcept;
		void Unbind(void) const noexcept;

		unsigned GetWidth(void) const;
		unsigned GetHeight(void) const;

		Texture* AsTexture(void) const noexcept;

	private:

		unsigned mnID, mDepthBuffer;
		Texture* mpTexture;
	};
}



#endif		// INCLUDE GUARD

