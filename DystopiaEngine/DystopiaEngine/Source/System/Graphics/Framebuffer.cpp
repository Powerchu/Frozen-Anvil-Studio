/* HEADER *********************************************************************************/
/*!
\file	Framebuffer.cpp
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Framebuffer

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "System/Graphics/Framebuffer.h"
#include "System/Graphics/GraphicsSystem.h"
#include "System/Graphics/TextureSystem.h"
#include "System/Graphics/Texture.h"
#include "System/Graphics/Texture2D.h"

#include "System/Driver/Driver.h"
#include "IO/Image.h"

#include "Utility/DebugAssert.h"

#include <GL/glew.h>


//Dystopia::Framebuffer::Framebuffer(void) noexcept
//	: mAttachments{ }, mbAlpha{ false }, mnWidth{ 0 }, mnHeight{ 0 }
//	//,mBlendSrc{ GL_SRC_ALPHA }, mBlendDst{ GL_ONE_MINUS_SRC_ALPHA }
//{
//	glGenFramebuffers(1, &mnID);
//}

Dystopia::Framebuffer::Framebuffer(unsigned _nWidth, unsigned _nHeight, bool _bAlpha/*, int _blendSrc, int _blendDst*/) noexcept
	: mAttachments{ }, mbAlpha{ _bAlpha }, mDepthBuffer{ 0 },
	mnWidth{ _nWidth }, mnHeight{ _nHeight }, 
	mClearCol{ 0 }//, mBlendSrc{ _blendSrc }, mBlendDst{ _blendDst }
{
	glGenFramebuffers(1, &mnID);
}

Dystopia::Framebuffer::~Framebuffer(void) noexcept
{
	glDeleteFramebuffers(1, &mnID);
	glDeleteRenderbuffers(1, &mDepthBuffer);

	for (auto& e : mAttachments)
		CORE::Get<TextureSystem>()->Free(e);
	mAttachments.clear();
}


void Dystopia::Framebuffer::Init(void)
{
	GLenum buffers[]{
		GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4,
		GL_COLOR_ATTACHMENT5, GL_COLOR_ATTACHMENT6, GL_COLOR_ATTACHMENT7, GL_COLOR_ATTACHMENT8, GL_COLOR_ATTACHMENT9,
		GL_COLOR_ATTACHMENT10, GL_COLOR_ATTACHMENT11, GL_COLOR_ATTACHMENT12, GL_COLOR_ATTACHMENT13, GL_COLOR_ATTACHMENT14
	};

	glGenRenderbuffers(1, &mDepthBuffer);

	// Depth + Stencil buffer for the FrameBuffer Object
	glBindRenderbuffer(GL_RENDERBUFFER, mDepthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, mnWidth, mnHeight);

	// Bind the texture and stencil buffer to the FBO
	Bind();
	glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mDepthBuffer);

	glDrawBuffers(static_cast<GLsizei>(mAttachments.size()), buffers);

	Unbind();
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

#   if defined(_DEBUG) | defined(DEBUG)
	if (auto err = glGetError())
		__debugbreak();
#   endif 
}

void Dystopia::Framebuffer::InitNoDepth(void)
{
	GLenum buffers[]{
		GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4,
		GL_COLOR_ATTACHMENT5, GL_COLOR_ATTACHMENT6, GL_COLOR_ATTACHMENT7, GL_COLOR_ATTACHMENT8, GL_COLOR_ATTACHMENT9,
		GL_COLOR_ATTACHMENT10, GL_COLOR_ATTACHMENT11, GL_COLOR_ATTACHMENT12, GL_COLOR_ATTACHMENT13, GL_COLOR_ATTACHMENT14
	};

	Bind();
	glDrawBuffers(static_cast<GLsizei>(mAttachments.size()), buffers);

	Unbind();
}

//void Dystopia::Framebuffer::Init(unsigned _nWidth, unsigned _nHeight, bool _bAlpha/*, int _blendSrc, int _blendDst*/)
//{
//	mbAlpha   = _bAlpha;
//	mnWidth   = _nWidth;
//	mnHeight  = _nHeight;
//	//mBlendSrc = _blendSrc;
//	//mBlendDst = _blendDst;
//
//	Init();
//}


void Dystopia::Framebuffer::Bind(void) const noexcept
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mnID);

	//glBlendFunc(mBlendSrc, mBlendDst);
}

void Dystopia::Framebuffer::Unbind(void) const noexcept
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

bool Dystopia::Framebuffer::HasAlpha(void) const noexcept
{
	return mbAlpha;
}

unsigned Dystopia::Framebuffer::GetWidth(void) const noexcept
{
	return mnWidth;
}

unsigned Dystopia::Framebuffer::GetHeight(void) const noexcept
{
	return mnHeight;
}

int Dystopia::Framebuffer::GetClearColor(void) const noexcept
{
	return mClearCol;
}

//int Dystopia::Framebuffer::GetBlendSrc(void) const noexcept
//{
//	return mBlendSrc;
//}
//
//int Dystopia::Framebuffer::GetBlendDst(void) const noexcept
//{
//	return mBlendDst;
//}


Dystopia::Texture* Dystopia::Framebuffer::AsTexture(unsigned _nIdx) const noexcept
{
	return mAttachments[_nIdx];
}

void Dystopia::Framebuffer::Attach(bool _bAlpha, int num, unsigned datatype, unsigned internalFmt) noexcept
{
	unsigned format = _bAlpha ? GL_RGBA : GL_RGB;
	internalFmt = internalFmt ? internalFmt : format;
	Image tmp = { "", false, true, internalFmt, format, mnWidth, mnHeight, mbAlpha ? 4u : 3u, 1u, nullptr };
	tmp.mDataType = datatype;
	mAttachments.EmplaceBack(CORE::Get<TextureSystem>()->LoadRaw<Texture2D>(&tmp));

	Bind();
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + num, GL_TEXTURE_2D, mAttachments.back()->GetID(), 0);
}

void Dystopia::Framebuffer::Resize(unsigned _nWidth, unsigned _nHeight) noexcept
{
	glBindRenderbuffer(GL_RENDERBUFFER, mDepthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, mnWidth, mnHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	for (auto& e : mAttachments)
	{
		e->Bind();
		static_cast<Texture2D*>(e)->ReplaceTexture(_nWidth, _nHeight, nullptr, e->GetSettings().mnChannels > 3);
	}
}

void Dystopia::Framebuffer::SetClearColor(int _nColor) noexcept
{
	mClearCol = _nColor;
}

