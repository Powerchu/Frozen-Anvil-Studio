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

#include "Utility/MetaAlgorithms.h"
#include "Utility/MetaDataStructures.h"

#include "DataStructure/AutoArray.h"


namespace Dystopia
{
	class Texture;

	struct ColourAttachment  {};
	//struct DepthAttachment   {};
	//struct StencilAttachment {};
	//struct DepthStencilAttachment : DepthAttachment, StencilAttachment {};

	class Framebuffer
	{
		//using AttachmentTypes = Ut::Collection<ColourAttachment, DepthAttachment, StencilAttachment, DepthStencilAttachment>;

	public:
		// ====================================== CONSTRUCTORS ======================================= // 

		//Framebuffer(void) noexcept;
		explicit Framebuffer(unsigned _nWidth, unsigned _nHeight, bool _bAlpha/*, int = 0x302, int = 0x303*/) noexcept;

		~Framebuffer(void) noexcept;


		// ===================================== MEMBER FUNCTIONS ==================================== // 

		void Init(void);
		void InitNoDepth(void);
		//void Init(unsigned _nWidth, unsigned _nHeight, bool _bAlpha = true/*, int = 0x302, int = 0x303*/);

		void Bind(void) const noexcept;
		void Unbind(void) const noexcept;

		bool HasAlpha(void) const noexcept;
		unsigned GetWidth (void) const noexcept;
		unsigned GetHeight(void) const noexcept;
		int GetClearColor(void) const noexcept;
		//int GetBlendSrc(void) const noexcept;
		//int GetBlendDst(void) const noexcept;

		Texture* AsTexture(unsigned = 0) const noexcept;

		void Attach(bool _bAlpha, int = 0, unsigned = 0x1401, unsigned = 0) noexcept;
		void Resize(unsigned _nWidth, unsigned _nHeight) noexcept;

		void SetClearColor(int) noexcept;

		//template <typename T>
		//auto Attach(Texture*, int = 0) -> Ut::EnableIf_t<Ut::MetaFind<T, AttachmentTypes>::value, Framebuffer&>;

	private:

		unsigned mnID, mDepthBuffer;
		AutoArray<Texture*> mAttachments;

		bool mbAlpha;
		unsigned mnWidth, mnHeight;
		int mClearCol;
		//int mBlendSrc, mBlendDst;
	};
}



#endif		// INCLUDE GUARD

