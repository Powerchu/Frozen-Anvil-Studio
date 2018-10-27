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

#include "Object/GameObject.h"

#if EDITOR
#include "Editor/EGUI.h"
#include "Editor/Payloads.h"
#include "Editor/CommandList.h"
#include "Editor/Commands.h"
#endif

Dystopia::SpriteRenderer::SpriteRenderer(void) noexcept
	: Renderer{}, mAnimations{ 1 }, mnID{ 0 }, mnCol{ 0 }, mnRow{ 0 },
	mfFrameTime{ 0.016f }, mfAccTime{ 0 }, mpAtlas{ nullptr }, mbPlayAnim{ false },
	mnTotalCols{ 1 }, mnTotalRows{ 1 }
{

}

Dystopia::SpriteRenderer::SpriteRenderer(Dystopia::SpriteRenderer&& _rhs) noexcept
	: Renderer{ Ut::Move(_rhs) }, mAnimations{ Ut::Move(_rhs.mAnimations) }, 
	mnID{ Ut::Move(_rhs.mnID) }, mnCol{ Ut::Move(_rhs.mnCol) }, mnRow{ Ut::Move(_rhs.mnRow) },
	mfFrameTime{ Ut::Move(_rhs.mfFrameTime) }, mfAccTime{ Ut::Move(_rhs.mfAccTime) }, mpAtlas{ Ut::Move(_rhs.mpAtlas) },
	mbPlayAnim{ Ut::Move(_rhs.mbPlayAnim) }, mnTotalCols{ Ut::Move(_rhs.mnTotalCols) }, mnTotalRows{ Ut::Move(_rhs.mnTotalRows) }
{
	_rhs.mpAtlas = nullptr;
}

Dystopia::SpriteRenderer::SpriteRenderer(const SpriteRenderer& _rhs) noexcept
	: Renderer{ _rhs }, mAnimations{ _rhs.mAnimations },
	mnID{ _rhs.mnID }, mnCol{ _rhs.mnCol }, mnRow{_rhs.mnRow },
	mfFrameTime{ _rhs.mfFrameTime }, mfAccTime{ _rhs.mfAccTime }, mpAtlas{ _rhs.mpAtlas },
	mbPlayAnim{ _rhs.mbPlayAnim }, mnTotalCols{ _rhs.mnTotalCols }, mnTotalRows{ _rhs.mnTotalRows }
{
}

void Dystopia::SpriteRenderer::Init(void)
{
	Renderer::Init();

	if (auto tex = GetTexture())
	{
		mpAtlas = EngineCore::GetInstance()->Get<TextureSystem>()->GetAtlas(tex->GetName());
		SpriteSheet a;
		a.mnCol = mnTotalCols;
		a.mnRow = mnTotalRows;
		a.mnID = 0;
		a.mstrName = "testing spritesheet";
		mAnimations.Insert(a);
		SetAnimation("testing spritesheet");
		mpAtlas->AddSection(Math::Vec2{ 0,0 }, tex->GetWidth() / a.mnCol, tex->GetHeight() / a.mnRow);
	}
}

void Dystopia::SpriteRenderer::Draw(void) const noexcept
{
	if (auto shader = GetShader())
	{
		if (mpAtlas)
		{
			mpAtlas->SetSection(mnID, mnCol, mnRow, *shader);
		}
		Renderer::Draw();
	}
}

void Dystopia::SpriteRenderer::Update(float _fDT)
{
	if (mpAtlas && mbPlayAnim)
	{
		mpAtlas->SetTexture(GetTexture());

		mfAccTime += _fDT;

		while (mfAccTime > mfFrameTime)
		{
			if (++mnCol >= mAnimations[mnID].mnCol)
			{
				mnCol = 0;

				if (++mnRow >= mAnimations[mnID].mnRow)
				{
					mnRow = 0;
				}
			}
			mfAccTime -= mfFrameTime;
		}
	}
}

void Dystopia::SpriteRenderer::SetAnimation(const char* _strAnimation)
{
	for(unsigned n = 0; n < mAnimations.size(); ++n)
	{
		if (mAnimations[n].mstrName == _strAnimation)
		{
			mnID = n;
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
	_out.InsertStartBlock("Sprite Renderer");
	Renderer::Serialise(_out);
	_out.InsertEndBlock("Sprite Renderer");
}

void Dystopia::SpriteRenderer::Unserialise(TextSerialiser& _in)
{
	_in.ConsumeStartBlock();
	Renderer::Unserialise(_in);
	_in.ConsumeEndBlock();
}

void Dystopia::SpriteRenderer::EditorUI(void) noexcept
{
#if EDITOR
	Renderer::EditorUI();
	EGUI::PushLeftAlign(80);

	if (EGUI::Display::CheckBox("Play", &mbPlayAnim))
	{
		EGUI::GetCommandHND()->InvokeCommand<SpriteRenderer>(mnOwner, &SpriteRenderer::mbPlayAnim, !mbPlayAnim);
	}

	switch (EGUI::Display::DragFloat("Speed", &mfFrameTime, 0.01f, 0.016f, 1.f))
	{
	case EGUI::eDragStatus::eEND_DRAG:
	case EGUI::eDragStatus::eTABBED:
	case EGUI::eDragStatus::eDEACTIVATED:
	case EGUI::eDragStatus::eENTER:
		EGUI::GetCommandHND()->EndRecording();
		break;
	case EGUI::eDragStatus::eSTART_DRAG:
		EGUI::GetCommandHND()->StartRecording<SpriteRenderer>(mnOwner, &SpriteRenderer::mfFrameTime);
		break;
	default:
	case EGUI::eDragStatus::eNO_CHANGE:
	case EGUI::eDragStatus::eDRAGGING:
		break;
	}
	
	switch (EGUI::Display::DragInt("Columns", &mnTotalCols, 1, 1, INT_MAX))
	{
	case EGUI::eDragStatus::eEND_DRAG:
	case EGUI::eDragStatus::eTABBED:
	case EGUI::eDragStatus::eDEACTIVATED:
	case EGUI::eDragStatus::eENTER:
		EGUI::GetCommandHND()->EndRecording();
		break;
	case EGUI::eDragStatus::eSTART_DRAG:
		EGUI::GetCommandHND()->StartRecording<SpriteRenderer>(mnOwner, &SpriteRenderer::mnTotalCols);
		break;
	default:
	case EGUI::eDragStatus::eNO_CHANGE:
	case EGUI::eDragStatus::eDRAGGING:
		break;
	}

	switch (EGUI::Display::DragInt("Rows", &mnTotalRows, 1, 1, INT_MAX))
	{
	case EGUI::eDragStatus::eEND_DRAG:
	case EGUI::eDragStatus::eTABBED:
	case EGUI::eDragStatus::eDEACTIVATED:
	case EGUI::eDragStatus::eENTER:
		EGUI::GetCommandHND()->EndRecording();
		break;
	case EGUI::eDragStatus::eSTART_DRAG:
		EGUI::GetCommandHND()->StartRecording<SpriteRenderer>(mnOwner, &SpriteRenderer::mnTotalRows);
		break;
	default:
	case EGUI::eDragStatus::eNO_CHANGE:
	case EGUI::eDragStatus::eDRAGGING:
		break;
	}

	EGUI::PopLeftAlign();
#endif
}


