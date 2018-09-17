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
#include "System\Graphics\Framebuffer.h"
#include "System\Graphics\GraphicsSystem.h"
#include "System\Graphics\Texture2D.h"
#include "System\Driver\Driver.h"

#include <gl\glew.h>


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


void Dystopia::Framebuffer::Init(unsigned _nWidth, unsigned _nHeight)
{
	mpTexture = new Texture2D{ _nWidth, _nHeight, nullptr };
	//mpTexture = EngineCore::GetInstance()->GetSubSystem<TextureSystem>()->Raw<Texture2D>();
	glGenFramebuffers(1, &mnID);
	glGenRenderbuffers(1, &mDepthBuffer);

	// Stencil buffer for the FrameBuffer Object
	glBindRenderbuffer(GL_RENDERBUFFER, mDepthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, _nWidth, _nHeight);

	// Bind the texture and stencil buffer to the FBO
	BindFramebuffer();
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mpTexture->GetID(), 0);
	glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mDepthBuffer);

	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	UnbindFramebuffer();
}


void Dystopia::Framebuffer::BindFramebuffer(void) const noexcept
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mnID);
}

void Dystopia::Framebuffer::UnbindFramebuffer(void) const noexcept
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}


Dystopia::Texture2D* Dystopia::Framebuffer::AsTexture(void) const noexcept
{
	return mpTexture;
}


