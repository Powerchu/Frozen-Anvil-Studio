/* HEADER *********************************************************************************/
/*!
\file	Texture.cpp
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Texture base class

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "System\Graphics\Texture.h"

#include <GL\glew.h>


Dystopia::Texture::Texture(void) noexcept :
	mnWidth{ 0 }, mnHeight{ 0 }, mnType{ 0 }, mnID{ 0 }
{
	glGenTextures(1, &mnID);
}

Dystopia::Texture::Texture(unsigned _nType) noexcept :
	mnWidth{ 0 }, mnHeight{ 0 }, mnType{ _nType }, mnID{ 0 }
{
	glGenTextures(1, &mnID);
}

Dystopia::Texture::Texture(unsigned _nWidth, unsigned _nHeight, unsigned _nType) noexcept :
	mnWidth{ _nWidth }, mnHeight{ _nHeight }, mnType{ _nType }, mnID{ 0 }
{
	glGenTextures(1, &mnID);
}

Dystopia::Texture::~Texture(void)
{
	glDeleteTextures(1, &mnID);
}


void Dystopia::Texture::BindTexture(void) const noexcept
{
	glBindTexture(mnType, mnID);
}

void Dystopia::Texture::UnbindTexture(void) const noexcept
{
	glBindTexture(mnType, 0);
}


unsigned Dystopia::Texture::GetWidth(void) const noexcept
{
	return mnWidth;
}

unsigned Dystopia::Texture::GetHeight(void) const noexcept
{
	return mnHeight;
}


void Dystopia::Texture::SetWidth(unsigned _nWidth) noexcept
{
	mnWidth = _nWidth;
}

void Dystopia::Texture::SetHeight(unsigned _nHeight) noexcept
{
	mnHeight = _nHeight;
}

void Dystopia::Texture::SetType(unsigned _nType) noexcept
{
	mnType = _nType;
}


unsigned Dystopia::Texture::GetID(void) const noexcept
{
	return mnID;
}


