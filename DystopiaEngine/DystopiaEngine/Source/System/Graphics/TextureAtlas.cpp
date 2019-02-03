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

#include "Math/Vectors.h"
#include "IO/TextSerialiser.h"
#include "DataStructure/HashString.h"

#define MAX_ROW_COL 100


Dystopia::TextureAtlas::TextureAtlas(Texture* _ptr) noexcept
	: mpTexture{ _ptr }, mbChanged{ false }
{
}

Dystopia::TextureAtlas::TextureAtlas(HashString const& _strName)
	: mpTexture{ EngineCore::GetInstance()->GetSubSystem<TextureSystem>()->GetTexture(_strName) }, mbChanged{ false }
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
	const float uStride = (uEnd - uStart);
	const float vStride = (vEnd - vStart);

	mSections.EmplaceBack(uStart, vStart, uEnd, vEnd, float(uStride / _nCols), float(vStride / _nRows));

	mbChanged = true;
	return static_cast<unsigned>(ret);
}

void Dystopia::TextureAtlas::UpdateSection(unsigned _nIndex, const Math::Vec2 & _vPos, unsigned _nWidth, unsigned _nHeight, unsigned _nCols, unsigned _nRows)
{
	auto& e = mSections[_nIndex];

	auto w = 1.0 / mpTexture->GetWidth();
	auto h = 1.0 / mpTexture->GetHeight();

	e.uStart = static_cast<float>(w * _vPos.x);
	e.vStart = static_cast<float>(h * _vPos.y);
	e.uEnd = static_cast<float>(e.uStart + _nWidth * w);
	e.vEnd = static_cast<float>(e.vStart + _nHeight * h);
	const float uStride = (e.uEnd - e.uStart);
	const float vStride = (e.vEnd - e.vStart);
	e.mCol = Math::Clamp(float(uStride / _nCols), 0, MAX_ROW_COL);
	e.mRow = Math::Clamp(float(vStride / _nRows), 0, MAX_ROW_COL);

	mbChanged = true;
}

void Dystopia::TextureAtlas::SetSection(unsigned _nID, unsigned _nCol, unsigned _nRow, Shader const& _Active)
{
	auto& section = mSections[_nID];

	const float uStart = section.uStart + _nCol * section.mCol;
	const float vStart = section.vStart + _nRow * section.mRow;
	const float uEnd   = section.uStart + (_nCol + 1) * section.mCol;
	const float vEnd   = section.vStart + (_nRow + 1) * section.mRow;

	_Active.UploadUniform("vUVBounds", uStart, vStart, uEnd, vEnd);
}

void Dystopia::TextureAtlas::SetTexture(Texture* _pTexture)
{
	mpTexture = _pTexture;
}

AutoArray<Dystopia::TextureAtlas::SubTexture>& Dystopia::TextureAtlas::GetAllSections(void) noexcept
{
	return mSections;
}

HashString Dystopia::TextureAtlas::GetName(void) const
{
	return mpTexture->GetName();
}

const HashString& Dystopia::TextureAtlas::GetPath(void) const noexcept
{
	return mpTexture->GetPath();
}

bool Dystopia::TextureAtlas::IsChanged(void) const noexcept
{
	return mbChanged;
}

void Dystopia::TextureAtlas::SetChanged(bool b) noexcept
{
	mbChanged = b;
}

void Dystopia::TextureAtlas::SaveAtlas(Dystopia::TextSerialiser& _out) const noexcept
{
	_out.InsertStartBlock("ATLAS");
	_out << mSections.size();

	for (auto const& e : mSections)
	{
		_out << e.uStart;
		_out << e.vStart;
		_out << e.uEnd;
		_out << e.vEnd;
		_out << e.mRow;
		_out << e.mCol;
	}
	_out.InsertEndBlock("ATLAS");
}

void Dystopia::TextureAtlas::LoadAtlas(Dystopia::TextSerialiser& _in) noexcept
{
	_in.ConsumeStartBlock();
	size_t sz;
	_in >> sz;

	mSections.clear();
	mSections.reserve(sz);

	while (sz-- && !_in.EndOfInput())
	{
		mSections.EmplaceBack();

		_in >> mSections.back().uStart;
		_in >> mSections.back().vStart;
		_in >> mSections.back().uEnd;
		_in >> mSections.back().vEnd;
		_in >> mSections.back().mRow;
		_in >> mSections.back().mCol;
	}
	_in.ConsumeStartBlock();
}

Dystopia::Texture* Dystopia::TextureAtlas::GetInternal(void) const noexcept
{
	return mpTexture;
}


