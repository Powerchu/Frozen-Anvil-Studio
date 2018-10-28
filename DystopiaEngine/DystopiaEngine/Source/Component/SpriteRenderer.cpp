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
#include "Editor/Payloads.h"
#include "Editor/CommandList.h"
#include "Editor/Commands.h"
#include "DataStructure/Array.h"
#endif

Dystopia::SpriteRenderer::SpriteRenderer(void) noexcept
	: Renderer{}, mAnimations{ 1 }, mnID{ 0 }, mnCol{ 0 }, mnRow{ 0 },
	mfFrameTime{ 0.016f }, mfAccTime{ 0 }, mpAtlas{ nullptr }, mbPlayAnim{ false }
{

}

Dystopia::SpriteRenderer::SpriteRenderer(Dystopia::SpriteRenderer&& _rhs) noexcept
	: Renderer{ Ut::Move(_rhs) }, mAnimations{ Ut::Move(_rhs.mAnimations) }, 
	mnID{ Ut::Move(_rhs.mnID) }, mnCol{ Ut::Move(_rhs.mnCol) }, mnRow{ Ut::Move(_rhs.mnRow) },
	mfFrameTime{ Ut::Move(_rhs.mfFrameTime) }, mfAccTime{ Ut::Move(_rhs.mfAccTime) }, mpAtlas{ Ut::Move(_rhs.mpAtlas) },
	mbPlayAnim{ Ut::Move(_rhs.mbPlayAnim) }
{
	_rhs.mpAtlas = nullptr;
}

Dystopia::SpriteRenderer::SpriteRenderer(const SpriteRenderer& _rhs) noexcept
	: Renderer{ _rhs }, mAnimations{ _rhs.mAnimations },
	mnID{ _rhs.mnID }, mnCol{ _rhs.mnCol }, mnRow{_rhs.mnRow },
	mfFrameTime{ _rhs.mfFrameTime }, mfAccTime{ _rhs.mfAccTime }, mpAtlas{ _rhs.mpAtlas },
	mbPlayAnim{ _rhs.mbPlayAnim }
{
}

void Dystopia::SpriteRenderer::Init(void)
{
	Renderer::Init();
	GetAtlas();
}

void Dystopia::SpriteRenderer::Draw(void) const noexcept
{
	if (auto shader = GetShader())
	{
		if (mpAtlas)
		{
			if (mAnimations.size() && mpAtlas->GetAllSections().size())
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
	if (mpAtlas && mbPlayAnim && mAnimations.size())
	{
		mpAtlas->SetTexture(GetTexture());

		mfAccTime += _fDT;

		while (!Math::IsZero(mfFrameTime) && (mfAccTime > mfFrameTime))
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

		if (mAnimations[mnID].mnCutoff && (mnCol + (mAnimations[mnID].mnCol * mnRow)) >= mAnimations[mnID].mnCutoff)
			mnCol = mnRow = 0;
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

void Dystopia::SpriteRenderer::GetAtlas(void)
{
	if (auto tex = GetTexture())
	{
		mpAtlas = EngineCore::GetInstance()->Get<TextureSystem>()->GetAtlas(tex->GetName());
		LoadAnimIntoAtlas();
	}
}

void Dystopia::SpriteRenderer::RemoveAtlas(void)
{
	mpAtlas = nullptr;
}

void Dystopia::SpriteRenderer::LoadAnimIntoAtlas(void)
{
	if (!mpAtlas)
		return;

	for (auto& s : mAnimations)
	{
		if (s.mnID >= mpAtlas->GetAllSections().size())
			s.mnID = mpAtlas->AddSection(s.mUVCoord, s.mnWidth, s.mnHeight, s.mnCol, s.mnRow);
	}
}

void Dystopia::SpriteRenderer::AddDefaultToAtlas(void)
{
	SpriteSheet s;
	s.mstrName = "defualt";
	s.mUVCoord = Math::Vec2{ 0,0 };
	s.mnCol = s.mnRow = 1;
	s.mnWidth = GetTexture()->GetWidth();
	s.mnHeight = GetTexture()->GetHeight();
	s.mnCutoff = 0;
	s.mnID = mpAtlas->AddSection(s.mUVCoord, s.mnWidth, s.mnHeight, s.mnCol, s.mnRow);
	mAnimations.Insert(s);
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
	_out << mbPlayAnim;
	_out << mfFrameTime;
	_out << mAnimations.size();
	for (const auto& a : mAnimations)
	{
		_out << a.mnID;
		_out << static_cast<float>(a.mUVCoord.x);
		_out << static_cast<float>(a.mUVCoord.y);
		_out << a.mstrName;
		_out << a.mnCol;
		_out << a.mnRow;
		_out << a.mnWidth;
		_out << a.mnHeight;
		_out << a.mnCutoff;
	}
	_out.InsertEndBlock("Sprite Renderer");
}

void Dystopia::SpriteRenderer::Unserialise(TextSerialiser& _in)
{
	Renderer::Unserialise(_in);
	_in.ConsumeStartBlock();
	_in >> mbPlayAnim;
	_in >> mfFrameTime;
	unsigned int size = 0;
	_in >> size;
	for (unsigned int i = 0; i < size; ++i)
	{
		SpriteSheet a;
		_in >> a.mnID;
		_in >> a.mUVCoord[0];
		_in >> a.mUVCoord[1];
		_in >> a.mstrName;
		_in >> a.mnCol;
		_in >> a.mnRow;
		_in >> a.mnWidth;
		_in >> a.mnHeight;
		_in >> a.mnCutoff;
		mAnimations.Insert(a);
	}
	_in.ConsumeEndBlock();
	GetAtlas();
}

void Dystopia::SpriteRenderer::EditorUI(void) noexcept
{
#if EDITOR
	Renderer::EditorUI();

	if (GetTexture() && !mpAtlas)
		GetAtlas();
	else if (!GetTexture() && mpAtlas)
		RemoveAtlas();

	EGUI::PushLeftAlign(90);

	if (mAnimations.size())
	{
		EGUI::Display::Label(std::string{ "Playing - Animation_" + std::to_string(mnID) }.c_str());
	}
	if (EGUI::Display::CheckBox("Play Anim", &mbPlayAnim))
	{
		GetAtlas();
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

	if (GetTexture() && mpAtlas)
	{
		if (EGUI::Display::Button("New Anime", Math::Vec2{ 80.f, 20.f }))
		{
			AddDefaultToAtlas();
		}
	}
	EGUI::PopLeftAlign();

	int toRemove = -1;
	EGUI::PushLeftAlign(90);
	for (unsigned int i = 0; i < mAnimations.size(); i++)
	{
		EGUI::PushID(i);
		bool open = EGUI::Display::StartTreeNode("Animation_" + std::to_string(i));
		if (ImGui::BeginPopupContextItem())
		{
			if (EGUI::Display::SelectableTxt("Remove"))
				toRemove = i;
			ImGui::EndPopup();
		}
		if (open)
		{
			EGUI::Indent(20);
			SpriteSheetUI(mAnimations[i]);
			EGUI::UnIndent(20);
			EGUI::Display::EndTreeNode();
		}
		EGUI::PopID();

		if (toRemove == i)
		{
			mAnimations.FastRemove(i--);
			toRemove = -1;
		}
	}
	EGUI::PopLeftAlign();

#endif
}

void Dystopia::SpriteRenderer::SpriteSheetUI(SpriteSheet& _ss)
{
#if EDITOR
	static constexpr size_t SIZE = 256;
	char buffer[SIZE];
	strcpy_s(buffer, _ss.mstrName.c_str());
	if (EGUI::Display::TextField("Name", buffer, SIZE) && strlen(buffer))
	{
		_ss.mstrName = std::string{ buffer };
	}
	EGUI::Display::VectorFields("UV", &_ss.mUVCoord, 0.01f, 0.f, 1.f);
	EGUI::Display::DragInt("Width", &_ss.mnWidth, 1, 1, INT_MAX);
	EGUI::Display::DragInt("Heigh", &_ss.mnHeight, 1, 1, INT_MAX);
	EGUI::Display::DragInt("Column", &_ss.mnCol, 1, 1, 100);
	EGUI::Display::DragInt("Rows", &_ss.mnRow, 1, 1, 100);
	EGUI::Display::DragInt("End at", &_ss.mnCutoff, 1, 1, _ss.mnRow * _ss.mnCol);
	bool update = EGUI::Display::Button("Update", Math::Vec2{ 80.f, 20.f });
	EGUI::SameLine();
	if (EGUI::Display::Button("Set To Play", Math::Vec2{ 80.f, 20.f }))
	{
		for (unsigned n = 0; n < mAnimations.size(); ++n)
		{
			if (mAnimations[n].mnID == _ss.mnID)
			{
				SetAnimation(n);
			}
		}
		update = true;
	}
	if (update && mpAtlas && GetTexture())
	{
		auto& allSections = mpAtlas->GetAllSections();

		auto w = 1.0 / GetTexture()->GetWidth();
		auto h = 1.0 / GetTexture()->GetHeight();
		const float uStart = static_cast<float>(w * _ss.mUVCoord.x);
		const float vStart = static_cast<float>(h * _ss.mUVCoord.y);
		const float uEnd = static_cast<float>(uStart + _ss.mnWidth * w);
		const float vEnd = static_cast<float>(vStart + _ss.mnHeight * h);
		const float uStride = (uEnd - uStart);
		const float vStride = (vEnd - vStart);
		allSections[_ss.mnID] = TextureAtlas::SubTexture{ uStart, vStart, uEnd, vEnd, float(uStride / _ss.mnCol), float(vStride / _ss.mnRow) };
	}

#endif //EDITOR
}
