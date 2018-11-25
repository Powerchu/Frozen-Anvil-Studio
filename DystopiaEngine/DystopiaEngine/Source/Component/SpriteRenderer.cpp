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
#include "System/Graphics/Shader.h"

#include "Object/GameObject.h"

#if EDITOR
#include "Editor/EGUI.h"
#endif

Dystopia::SpriteRenderer::SpriteRenderer(void) noexcept
	: Renderer{}, mAnimations{ 1 }, mnID{ 0 }, mnCol{ 0 }, mnRow{ 0 },
	mfFrameTime{ 0.016f }, mfAccTime{ 0 }, mpAtlas{ nullptr }
{

}

Dystopia::SpriteRenderer::SpriteRenderer(Dystopia::SpriteRenderer&& _rhs) noexcept
	: Renderer{ Ut::Move(_rhs) }, mAnimations{ Ut::Move(_rhs.mAnimations) },
	mnID{ Ut::Move(_rhs.mnID) }, mnCol{ Ut::Move(_rhs.mnCol) }, mnRow{ Ut::Move(_rhs.mnRow) },
	mfFrameTime{ Ut::Move(_rhs.mfFrameTime) }, mfAccTime{ Ut::Move(_rhs.mfAccTime) }, mpAtlas{ Ut::Move(_rhs.mpAtlas) }
{
	_rhs.mAnimations.clear();
	_rhs.mpAtlas = nullptr;
}

Dystopia::SpriteRenderer::SpriteRenderer(const SpriteRenderer& _rhs) noexcept
	: Renderer{ _rhs }, mAnimations{ _rhs.mAnimations },
	mnID{ _rhs.mnID }, mnCol{ _rhs.mnCol }, mnRow{_rhs.mnRow },
	mfFrameTime{ _rhs.mfFrameTime }, mfAccTime{ _rhs.mfAccTime }, mpAtlas{ _rhs.mpAtlas }
{
}

void Dystopia::SpriteRenderer::Awake(void)
{
	Renderer::Awake();
	mnID = mnCol = mnRow = 0;
}

void Dystopia::SpriteRenderer::Init(void)
{
	Renderer::Init();
}

void Dystopia::SpriteRenderer::Draw(void) const noexcept
{
	if (auto shader = GetShader())
	{
		if (mpAtlas)
		{
			if (mnID < mAnimations.size())
			{
				mpAtlas->SetSection(mAnimations[mnID].mnID, mnCol, mnRow, *shader);
			}
			else
			{
				shader->UploadUniform("vUVBounds", 0.f, 0.f, 1.f, 1.f);
			}
		}
		Renderer::Draw();
	}
}

void Dystopia::SpriteRenderer::Update(float _fDT)
{
	if (mpAtlas && (mnID < mAnimations.size()))
	{
		//int endPos = mAnimations[mnID].mnCutoff ? mAnimations[mnID].mnCutoff : mAnimations[mnID].mnCol * mAnimations[mnID].mnRow;
		//int startCol = mAnimations[mnID].mnStartAt ? (mAnimations[mnID].mnStartAt % mAnimations[mnID].mnCol) : 0;
		//int startRow = mAnimations[mnID].mnStartAt ? static_cast<int>((mAnimations[mnID].mnStartAt) / mAnimations[mnID].mnCol) : 0;

		mpAtlas->SetTexture(GetTexture());

		mfAccTime += _fDT;

		while (!Math::IsZero(mfFrameTime) && (mfAccTime > mfFrameTime))
		{
			if (++mnCol >= mAnimations[mnID].mnCol)
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
}

void Dystopia::SpriteRenderer::SetAnimation(const char* _strAnimation)
{
	for(unsigned n = 0; n < mAnimations.size(); ++n)
	{
		if (mAnimations[n].mstrName == _strAnimation)
		{
			mnID = n;
			mnCol = 0;
			mnRow = 0;
			return;
		}
	}
}

void Dystopia::SpriteRenderer::SetAnimation(unsigned _nID)
{
	DEBUG_BREAK(_nID > mAnimations.size(), "SpriteRenderer Error: Invalid Animation!");

	mnID = _nID;
	mnCol = 0;
	mnRow = 0;
}

Dystopia::SpriteRenderer* Dystopia::SpriteRenderer::Duplicate(void) const
{
	return static_cast<ComponentDonor<SpriteRenderer>*>(
		EngineCore::GetInstance()->Get<GraphicsSystem>()
	)->RequestComponent(*this);
}

void Dystopia::SpriteRenderer::Serialise(TextSerialiser& _out) const
{
	Renderer::Serialise(_out);
	_out.InsertStartBlock("Sprite Renderer");
	_out << mfFrameTime;
	_out << mAnimations.size();
	for (const auto& a : mAnimations)
	{
		_out << a.mnID;
		_out << a.mstrName;
		_out << a.mnCol;
		_out << a.mnRow;
		_out << a.mbLoop;
	}
	_out << mnID;
	_out.InsertEndBlock("Sprite Renderer");
}

void Dystopia::SpriteRenderer::Unserialise(TextSerialiser& _in)
{
	Renderer::Unserialise(_in);
	_in.ConsumeStartBlock();
	_in >> mfFrameTime;
	unsigned int size = 0;
	_in >> size;
	for (unsigned int i = 0; i < size; ++i)
	{
		SpriteSheet a;
		_in >> a.mnID;
		_in >> a.mstrName;
		_in >> a.mnCol;
		_in >> a.mnRow;
		_in >> a.mbLoop;
		mAnimations.Insert(a);
	}
	_in >> mnID;
	_in.ConsumeEndBlock();
}

void Dystopia::SpriteRenderer::EditorUI(void) noexcept
{
#if EDITOR
	EGUI::PushLeftAlign(80);

	TextureFields();


















	EGUI::PopLeftAlign();
#endif
}

void Dystopia::SpriteRenderer::TextureFields(void)
{
#if EDITOR

	EGUI::Display::EmptyBox("Texture", 150, (mpTexture) ? mpTexture->GetName() : "-empty-", true);
	if (auto t = EGUI::Display::StartPayloadReceiver<::Editor::File>(EGUI::ALL_IMG))
	{
		EGUI::Display::EndPayloadReceiver();
	}

	if (t)
	{
		Texture *pTex = EngineCore::GetInstance()->GetSystem<GraphicsSystem>()->LoadTexture(t->mPath.c_str());
		SetTexture(pTex);
		//auto fOld = EGUI::GetCommandHND()->Make_FunctionModWrapper(&Dystopia::Renderer::SetTexture, mpTexture);
		//auto fNew = EGUI::GetCommandHND()->Make_FunctionModWrapper(&Dystopia::Renderer::SetTexture, pTex);
		//EGUI::GetCommandHND()->InvokeCommand(GetOwner()->GetID(), fOld, fNew);
	}

	EGUI::SameLine();
	if (EGUI::Display::IconCross("Clear", 8.f))
	{
		SetTexture(nullptr);
		//auto fOld = EGUI::GetCommandHND()->Make_FunctionModWrapper(&Dystopia::Renderer::SetTexture, mpTexture);
		//auto fNew = EGUI::GetCommandHND()->Make_FunctionModWrapper(&Dystopia::Renderer::SetTexture, nullptr);
		//EGUI::GetCommandHND()->InvokeCommand(GetOwner()->GetID(), fOld, fNew);
	}

	if (mpTexture)
	{
		EGUI::Display::Label("Preview");
		EGUI::SameLine(DefaultAlighnmentSpacing, 80);
		float ratio = static_cast<float>(mpTexture->GetHeight()) / static_cast<float>(mpTexture->GetWidth());
		EGUI::Display::Image(mpTexture->GetID(), Math::Vec2{ 140, 140 * ratio }, false, true);

		EGUI::SameLine();
		if (EGUI::Display::Button("Auto", Math::Vec2{ 35, 20 }))
		{
			auto w = static_cast<float>(mpTexture->GetWidth());
			auto h = static_cast<float>(mpTexture->GetHeight());
			w /= 10;
			h /= 10;
			GetOwner()->GetComponent<Transform>()->SetScale(Math::Vec4{ w, h, 1.f });
		}
	}




#endif
}






