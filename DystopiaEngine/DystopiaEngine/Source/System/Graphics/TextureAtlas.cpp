/* HEADER *********************************************************************************/
/*!
\file	TextureAtlas.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Texture containing multiple textures

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "System/Graphics/TextureAtlas.h"
#include "System/Graphics/Texture.h"
#include "System/Graphics/Shader.h"

#include "System/Graphics/TextureSystem.h"
#include "System/Driver/Driver.h"

#include "Math/Vector2.h"

#include <GL/glew.h>


Dystopia::TextureAtlas::TextureAtlas(Texture* _ptr) noexcept
	: mpTexture{ _ptr }
{
}

Dystopia::TextureAtlas::TextureAtlas(const std::string& _strName)
	: mpTexture{ EngineCore::GetInstance()->GetSubSystem<TextureSystem>()->GetTexture(_strName) }
{
}

Dystopia::TextureAtlas::~TextureAtlas(void) noexcept
{
}

void Dystopia::TextureAtlas::Bind(void) const noexcept
{
	mpTexture->Bind();
}

void Dystopia::TextureAtlas::Unbind(void) const noexcept
{
	mpTexture->Unbind();
}

unsigned Dystopia::TextureAtlas::AddSection(const Math::Vec2 & _vPos, unsigned _nWidth, unsigned _nHeight, unsigned _nCols, unsigned _nRows)
{
	const auto ret = mSections.size();

	auto w = 1.0 / mpTexture->GetWidth();
	auto h = 1.0 / mpTexture->GetHeight();

	const float uStart = static_cast<float>(w * _vPos.x);
	const float vStart = static_cast<float>(h * _vPos.y);
	const float uEnd = static_cast<float>(uStart + _nWidth  * w);
	const float vEnd = static_cast<float>(vStart + _nHeight * h);

	mSections.EmplaceBack(uStart, vStart, uEnd, vEnd, float((_nWidth * w) / _nCols), float((_nHeight * h) / _nRows));

	return static_cast<unsigned>(ret);
}

void Dystopia::TextureAtlas::SetSection(unsigned _nID, unsigned _nCol, unsigned _nRow, Shader const& _Active)
{
	auto& section = mSections[_nID];

	const float uEnd   = section.uEnd   + _nCol * section.mCol;
	const float vEnd   = section.vEnd   + _nRow * section.mRow;
	const float uStart = section.uStart + _nCol * section.mCol;
	const float vStart = section.vStart + _nRow * section.mRow;

	_Active.UploadUniform("vUVBounds", uStart, vStart, uEnd, vEnd);
}

void Dystopia::TextureAtlas::SetTexture(Texture* _pTexture)
{
	mpTexture = _pTexture;
}

std::string Dystopia::TextureAtlas::GetName(void) const
{
	return mpTexture->GetName();
}

const std::string& Dystopia::TextureAtlas::GetPath(void) const noexcept
{
	return mpTexture->GetPath();
}


