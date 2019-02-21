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
	mnType{ 0 }, mnID{ 0 }, mData{}
{
	glGenTextures(1, &mnID);
}

Dystopia::Texture::Texture(unsigned _nType, HashString const& _strPath, Image& _Image) noexcept :
	mnType{ _nType }, mnID{ 0 }, mstrPath{ _strPath }, mData{ Ut::Move(_Image) }
{
	glGenTextures(1, &mnID);
}

Dystopia::Texture::Texture(unsigned _nWidth, unsigned _nHeight, unsigned _nType) noexcept :
	mnType{ _nType }, mnID{ 0 }, mData{}
{
	glGenTextures(1, &mnID);
	mData.mnWidth  = _nWidth;
	mData.mnHeight = _nHeight;
}

Dystopia::Texture::Texture(Texture&& _other) noexcept :
	mnType{ _other.mnType }, mnID{ _other.mnID }, mData{ Ut::Move(_other.mData) }
{
	_other.mnID = 0;
}

Dystopia::Texture::~Texture(void)
{
	glDeleteTextures(1, &mnID);

	mnID = 0;
}


void Dystopia::Texture::Bind(unsigned _n) const noexcept
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
	return mData.mnWidth;
}

unsigned Dystopia::Texture::GetHeight(void) const noexcept
{
	return mData.mnHeight;
}


void Dystopia::Texture::SetWidth(unsigned _nWidth) noexcept
{
	mData.mnWidth = _nWidth;
	mData.mbChanged = true;
}

void Dystopia::Texture::SetHeight(unsigned _nHeight) noexcept
{
	mData.mnHeight = _nHeight;
	mData.mbChanged = true;
}

void Dystopia::Texture::SetType(unsigned _nType) noexcept
{
	mnType = _nType;
}


unsigned Dystopia::Texture::GetID(void) const noexcept
{
	return mnID;
}

bool Dystopia::Texture::Changed(void) const noexcept
{
	return mData.mbChanged && mstrPath.size();
}

Dystopia::Image const& Dystopia::Texture::GetSettings(void) const
{
	return mData;
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
	Ut::Swap(mData, _rhs.mData);

	return *this;
}

void Dystopia::Texture::OnEditorUI(void) noexcept
{
	mData.OnEditorUI();
}


