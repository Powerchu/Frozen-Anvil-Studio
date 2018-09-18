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
	class Texture2D;

	class Framebuffer
	{
	public:
		// ====================================== CONSTRUCTORS ======================================= // 

		Framebuffer(void) noexcept;

		~Framebuffer(void) noexcept;


		// ===================================== MEMBER FUNCTIONS ==================================== // 

		void Init(unsigned _nWidth, unsigned _nHeight);

		void BindFramebuffer(void) const noexcept;
		void UnbindFramebuffer(void) const noexcept;

		Texture2D* AsTexture(void) const noexcept;

	private:

		unsigned mnID, mDepthBuffer;
		Texture2D* mpTexture;
	};
}



#endif		// INCLUDE GUARD

