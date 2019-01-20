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
#include "System/Graphics/Texture.h"

#include "Utility/Utility.h"

#include <GL/glew.h>
#include <utility>


Dystopia::Texture::Texture(void) noexcept :
	mnWidth{ 0 }, mnHeight{ 0 }, mnType{ 0 }, mnID{ 0 }
{
	glGenTextures(1, &mnID);
}

Dystopia::Texture::Texture(unsigned _nType, HashString const& _strPath) noexcept :
	mnWidth{ 0 }, mnHeight{ 0 }, mnType{ _nType }, mnID{ 0 }, mstrPath{ _strPath }
{
	glGenTextures(1, &mnID);
}

Dystopia::Texture::Texture(unsigned _nWidth, unsigned _nHeight, unsigned _nType) noexcept :
	mnWidth{ _nWidth }, mnHeight{ _nHeight }, mnType{ _nType }, mnID{ 0 }
{
	glGenTextures(1, &mnID);
}

Dystopia::Texture::Texture(Texture&& _other) noexcept :
	mnWidth{ _other.mnWidth }, mnHeight{ _other.mnHeight }, mnType{ _other.mnType }, mnID{ _other.mnID }
{
	_other.mnID = 0;
}

Dystopia::Texture::~Texture(void)
{
	glDeleteTextures(1, &mnID);

	mnID = 0;
}


void Dystopia::Texture::Bind(int _n) const noexcept
{
	glActiveTexture(GL_TEXTURE0 + _n);
	glBindTexture(mnType, mnID);
}

void Dystopia::Texture::Unbind(void) const noexcept
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

HashString Dystopia::Texture::GetName(void) const
{
	return mstrPath.substr(mstrPath.find_last_of("/\\") + 1);
}

HashString const& Dystopia::Texture::GetPath(void) const noexcept
{
	return mstrPath;
}

Dystopia::Texture& Dystopia::Texture::operator=(Texture&& _rhs) noexcept
{
	Ut::Swap(mnID, _rhs.mnID);
	Ut::Swap(mnType, _rhs.mnType);
	Ut::Swap(mnWidth, _rhs.mnWidth);
	Ut::Swap(mnHeight, _rhs.mnHeight);

	return *this;
}


