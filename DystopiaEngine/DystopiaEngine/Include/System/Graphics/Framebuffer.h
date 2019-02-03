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
		explicit Framebuffer(unsigned _nWidth, unsigned _nHeight, bool _bAlpha, int = 0x302, int = 0x303) noexcept;

		~Framebuffer(void) noexcept;


		// ===================================== MEMBER FUNCTIONS ==================================== // 

		void Init(void);
		void Init(unsigned _nWidth, unsigned _nHeight, bool _bAlpha = true, int = 0x302, int = 0x303);

		void Bind(void) const noexcept;
		void Unbind(void) const noexcept;

		bool HasAlpha(void) const noexcept;
		unsigned GetWidth (void) const noexcept;
		unsigned GetHeight(void) const noexcept;
		int GetBlendSrc(void) const noexcept;
		int GetBlendDst(void) const noexcept;

		Texture* AsTexture(void) const noexcept;

	private:

		unsigned mnID, mDepthBuffer;
		Texture* mpTexture;

		bool mbAlpha;
		unsigned mnWidth, mnHeight;
		int mBlendSrc, mBlendDst;
	};
}



#endif		// INCLUDE GUARD

