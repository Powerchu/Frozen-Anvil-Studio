/* HEADER *********************************************************************************/
/*!
\file	SpriteRenderer.cpp
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Renderer for Sprite sheets

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "Component/SpriteRenderer.h"

#include "System/Driver/Driver.h"
#include "System/Base/ComponentDonor.h"
#include "System/Graphics/GraphicsSystem.h"
#include "System/Graphics/Texture.h"
#include "System/Graphics/TextureAtlas.h"
#include "System/Graphics/TextureSystem.h"


Dystopia::SpriteRenderer::SpriteRenderer(void) noexcept
	: Renderer{}, mAnimations{ 1 }, mnID{ 0 }, mnCol{ 0 }, mnRow{ 0 },
	mfFrameTime{ 0 }, mfAccTime{ 0 }
{

}

Dystopia::SpriteRenderer::SpriteRenderer(Dystopia::SpriteRenderer&& _rhs) noexcept
	: Renderer{ Ut::Move(_rhs) }, mAnimations{ Ut::Move(_rhs.mAnimations) }, 
	mnID{ Ut::Move(_rhs.mnID) }, mnCol{ Ut::Move(_rhs.mnCol) }, mnRow{ Ut::Move(_rhs.mnRow) },
	mfFrameTime{ Ut::Move(_rhs.mfFrameTime) }, mfAccTime{ Ut::Move(_rhs.mfAccTime) }
{

}

Dystopia::SpriteRenderer::SpriteRenderer(const SpriteRenderer& _rhs) noexcept
	: Renderer{ _rhs }, mAnimations{ _rhs.mAnimations },
	mnID{ _rhs.mnID }, mnCol{ _rhs.mnCol }, mnRow{_rhs.mnRow },
	mfFrameTime{ _rhs.mfFrameTime }, mfAccTime{ _rhs.mfAccTime }
{
}

void Dystopia::SpriteRenderer::Init(void)
{
	Renderer::Init();

	if (auto tex = GetTexture())
	{
		mpAtlas = EngineCore::GetInstance()->Get<TextureSystem>()->GetAtlas(tex->GetName());
	}
}

void Dystopia::SpriteRenderer::Draw(void) const noexcept
{
	if (auto shader = GetShader())
	{
		if (mpAtlas)
		{
			mpAtlas->SetSection(mnID, mnCol, mnRow, *shader);
			Renderer::Draw();
		}
	}
}

void Dystopia::SpriteRenderer::Update(float _fDT)
{
	mpAtlas->SetTexture(GetTexture());

	mfAccTime += _fDT;

	while (mfAccTime > mfFrameTime)
	{
		if (++mnCol > mAnimations[mnID].mnCol)
		{
			mnCol = 0;

			if (++mnRow > mAnimations[mnID].mnRow)
			{
				mnRow = 0;
			}
		}

		mfAccTime -= mfFrameTime;
	}
}

void Dystopia::SpriteRenderer::SetAnimation(const char* _strAnimation)
{
	for (auto& e : mAnimations)
	{
		if (e.mstrName == _strAnimation)
		{
			mnID = e.mnID;
			return;
		}
	}
}

void Dystopia::SpriteRenderer::SetAnimation(unsigned _nID)
{
	DEBUG_BREAK(_nID > mAnimations.size(), "SpriteRenderer Error: Invalid Animation!");

	mnID = _nID;
}

Dystopia::SpriteRenderer* Dystopia::SpriteRenderer::Duplicate(void) const
{
	return static_cast<ComponentDonor<SpriteRenderer>*>(
		EngineCore::GetInstance()->Get<GraphicsSystem>()
	)->RequestComponent(*this);
}

void Dystopia::SpriteRenderer::Serialise(TextSerialiser& _out) const
{
}

void Dystopia::SpriteRenderer::Unserialise(TextSerialiser& _in)
{
}

void Dystopia::SpriteRenderer::EditorUI(void) noexcept
{

}


