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

#include <GL/glew.h>


Dystopia::Framebuffer::Framebuffer(void) noexcept
	: mpTexture{ nullptr }
{
}

Dystopia::Framebuffer::~Framebuffer(void) noexcept
{
	glDeleteFramebuffers(1, &mnID);
	glDeleteRenderbuffers(1, &mDepthBuffer);

	delete mpTexture;
}


void Dystopia::Framebuffer::Init(unsigned _nWidth, unsigned _nHeight, bool _bAlpha)
{
	unsigned format = _bAlpha ? GL_RGBA : GL_RGB;
	Image tmp = { false, format, format, _nWidth, _nHeight, _bAlpha ? 4u : 3u, 1u, nullptr };
	mpTexture = EngineCore::GetInstance()->GetSubSystem<TextureSystem>()->LoadRaw<Texture2D>(&tmp);

	glGenFramebuffers(1, &mnID);
	glGenRenderbuffers(1, &mDepthBuffer);

	// Stencil buffer for the FrameBuffer Object
	glBindRenderbuffer(GL_RENDERBUFFER, mDepthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, _nWidth, _nHeight);

	// Bind the texture and stencil buffer to the FBO
	Bind();
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mpTexture->GetID(), 0);
	glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mDepthBuffer);

	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	Unbind();
}


void Dystopia::Framebuffer::Bind(void) const noexcept
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mnID);
}

void Dystopia::Framebuffer::Unbind(void) const noexcept
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

unsigned Dystopia::Framebuffer::GetWidth(void) const
{
	return mpTexture->GetWidth();
}

unsigned Dystopia::Framebuffer::GetHeight(void) const
{
	return mpTexture->GetHeight();
}


Dystopia::Texture* Dystopia::Framebuffer::AsTexture(void) const noexcept
{
	return mpTexture;
}


