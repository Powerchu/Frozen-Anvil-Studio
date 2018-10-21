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

Dystopia::Texture2D::Texture2D(void) noexcept : Texture{ GL_TEXTURE_2D, "" }
{

}

Dystopia::Texture2D::Texture2D(Image const* _pData) noexcept
	: Texture{ _pData->mnWidth, _pData->mnHeight, GL_TEXTURE_2D }
{
	InitTexture(_pData);
}

Dystopia::Texture2D::Texture2D(const std::string& _strPath) noexcept
	: Texture{ GL_TEXTURE_2D, _strPath }
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
	else if ('d' == *fileType || 'D' == *fileType)
	{
		img = ImageParser::LoadDDS(_strPath);
	}
	else
	{
		return;
	}

	SetWidth(img.mnWidth);
	SetHeight(img.mnHeight);

	InitTexture(&img);
	DefaultAllocator<void>::Free(img.mpImageData);
}

Dystopia::Texture2D::~Texture2D(void) noexcept
{
}

void Dystopia::Texture2D::GenerateMipmap(void) const
{
	BindTexture();

	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);

	UnbindTexture();
}

void Dystopia::Texture2D::InitTexture(Image const* _pData)
{
	BindTexture();

	unsigned msz = 4 * 512 * 512;
	unsigned sz = 16 * ((_pData->mnWidth + 3) / 4) * ((_pData->mnHeight + 3) / 4);

	//glTexImage2D(mnType, 0, _pData->mnFormat, _pData->mnWidth, _pData->mnHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, _pData->mpImageData);
	glCompressedTexImage2D(mnType, 0, _pData->mnFormat, _pData->mnWidth, _pData->mnHeight, 0, sz, _pData->mpImageData);

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


