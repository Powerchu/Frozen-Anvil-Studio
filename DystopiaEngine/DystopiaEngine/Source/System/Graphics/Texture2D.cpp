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
#include "System\Graphics\Texture2D.h"
#include "System\Graphics\Image.h"
#include "IO\ImageParser.h"

#include <GL\glew.h>

Dystopia::Texture2D::Texture2D(void) noexcept : Texture{ GL_TEXTURE_2D }
{

}

Dystopia::Texture2D::Texture2D(const std::string& _strPath, bool _bAlpha) : 
	Texture{ GL_TEXTURE_2D }
{
	auto fileType = (_strPath.end() - 2);
	Image img;

	if ('p' == *fileType || 'P' == *fileType)
	{
		img = ImageParser::LoadJPG(_strPath);
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

	InitTexture(img, _bAlpha);
}

Dystopia::Texture2D::Texture2D(unsigned _nWidth, unsigned _nHeight, void* _pData, bool _bAlpha) :
	Texture{ _nWidth, _nHeight, GL_TEXTURE_2D }
{
	InitTexture(Image{ _nWidth, _nHeight, _pData }, _bAlpha);
}

void Dystopia::Texture2D::GenerateMipmap(void) const
{
	BindTexture();

	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);

	UnbindTexture();
}

void Dystopia::Texture2D::InitTexture(const Image& _img, bool _bAlpha)
{
	BindTexture();

	if (_bAlpha)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, _img.mnWidth, _img.mnHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, _img.mpImageData);
	}
	else
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, _img.mnWidth, _img.mnHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, _img.mpImageData);
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	UnbindTexture();
}


