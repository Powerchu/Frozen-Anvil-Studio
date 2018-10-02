/* HEADER *********************************************************************************/
/*!
\file	Texture2D.cpp
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	sRGB 2D Textures

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "System/Graphics/Texture2D.h"
#include "System/Graphics/Image.h"
#include "IO/ImageParser.h"
#include "Allocator/DefaultAlloc.h"

#include <GL/glew.h>

Dystopia::Texture2D::Texture2D(void) noexcept : Texture{ GL_TEXTURE_2D }, mPath{}
{

}

Dystopia::Texture2D::Texture2D(const std::string& _strPath, bool _bAlpha) : 
	Texture{ GL_TEXTURE_2D }, mPath{ _strPath }
{
	auto fileType = (_strPath.end() - 3);
	Image img;

	if ('p' == *fileType || 'P' == *fileType)
	{
		img = ImageParser::LoadPNG(_strPath);
	}
	else if ('b' == *fileType || 'B' == *fileType)
	{
		img = ImageParser::LoadBMP(_strPath);
	}
	else if ('j' == *fileType || 'J' == *fileType)
	{
		img = ImageParser::LoadJPG(_strPath);
	}
	else
	{
		return;
	}

	SetWidth(img.mnWidth);
	SetHeight(img.mnHeight);

	InitTexture(img.mpImageData, _bAlpha);
	DefaultAllocator<void>::Free(img.mpImageData);
}

Dystopia::Texture2D::Texture2D(unsigned _nWidth, unsigned _nHeight, void* _pData, bool _bAlpha) :
	Texture{ _nWidth, _nHeight, GL_TEXTURE_2D }, mPath{}
{
	InitTexture(_pData, _bAlpha);
}

void Dystopia::Texture2D::GenerateMipmap(void) const
{
	BindTexture();

	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);

	UnbindTexture();
}

void Dystopia::Texture2D::InitTexture(void* _pData, bool _bAlpha)
{
	BindTexture();

	if (_bAlpha)
	{
		glTexImage2D(mnType, 0, GL_SRGB_ALPHA, GetWidth(), GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, _pData);
	}
	else
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, GetWidth(), GetHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, _pData);
	}

	if (auto err = glGetError())
	{
		__debugbreak();
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	UnbindTexture();
}

std::string Dystopia::Texture2D::GetPath() const noexcept
{
	return mPath;
}
