/* HEADER *********************************************************************************/
/*!
\file	Framebuffer.cpp
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Framebuffer

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
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


Dystopia::Framebuffer::Framebuffer(void) noexcept
	: mpTexture{ nullptr }, mbAlpha{ false }, mnWidth{ 0 }, mnHeight{ 0 }
{
}

Dystopia::Framebuffer::Framebuffer(unsigned _nWidth, unsigned _nHeight, bool _bAlpha) noexcept
	: mpTexture{ nullptr }, mbAlpha{ _bAlpha }, 
	mnWidth{ _nWidth }, mnHeight{ _nHeight }
{
}

Dystopia::Framebuffer::~Framebuffer(void) noexcept
{
	glDeleteFramebuffers(1, &mnID);
	glDeleteRenderbuffers(1, &mDepthBuffer);
}


void Dystopia::Framebuffer::Init(void)
{
	unsigned format = mbAlpha ? GL_RGBA : GL_RGB;
	Image tmp = { "", false, true, format, format, mnWidth, mnHeight, mbAlpha ? 4u : 3u, 1u, nullptr };
	mpTexture = EngineCore::GetInstance()->GetSubSystem<TextureSystem>()->LoadRaw<Texture2D>(&tmp);

	DEBUG_ASSERT(!mpTexture, "Framebuffer Error: Failed to create texture!\n");

	glGenFramebuffers(1, &mnID);
	glGenRenderbuffers(1, &mDepthBuffer);

	// Depth + Stencil buffer for the FrameBuffer Object
	glBindRenderbuffer(GL_RENDERBUFFER, mDepthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, mnWidth, mnHeight);

	// Bind the texture and stencil buffer to the FBO
	Bind();
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mpTexture->GetID(), 0);
	glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mDepthBuffer);

	Unbind();
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

#   if defined(_DEBUG) | defined(DEBUG)
	if (auto err = glGetError())
		__debugbreak();
#   endif 
}

void Dystopia::Framebuffer::Init(unsigned _nWidth, unsigned _nHeight, bool _bAlpha)
{
	mbAlpha = _bAlpha;
	mnWidth = _nWidth;
	mnHeight = _nHeight;

	Init();
}


void Dystopia::Framebuffer::Bind(void) const noexcept
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mnID);
}

void Dystopia::Framebuffer::Unbind(void) const noexcept
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

bool Dystopia::Framebuffer::HasAlpha(void) const
{
	return mbAlpha;
}

unsigned Dystopia::Framebuffer::GetWidth(void) const
{
	return mnWidth;
}

unsigned Dystopia::Framebuffer::GetHeight(void) const
{
	return mnHeight;
}


Dystopia::Texture* Dystopia::Framebuffer::AsTexture(void) const noexcept
{
	return mpTexture;
}


