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
#include "Component/Transform.h"

#include "System/Driver/Driver.h"
#include "System/Base/ComponentDonor.h"
#include "System/Graphics/GraphicsSystem.h"
#include "System/Graphics/Texture.h"
#include "System/Graphics/TextureAtlas.h"
#include "System/Graphics/TextureSystem.h"
#include "System/Graphics/Shader.h"
#include "System/Graphics/Mesh.h"

#include "Object/GameObject.h"

#if EDITOR
#include "Editor/EGUI.h"
#include "Editor/EditorMain.h"
#include "Editor/EditorCommands.h"
#endif

Dystopia::SpriteRenderer::SpriteRenderer(void) noexcept
	: Renderer{}, mAnimations{ 1 }, mnID{ 0 }, mnCol{ 0 }, mnRow{ 0 },
	mfFrameTime{ 0.016f }, mfAccTime{ 0 }, mpAtlas{ nullptr }, mNextSectionPos{ 0,0 }, mbPlayOnStart{ false }, mbPlay{ false }, mbSimulate{ false },
	mvTintCol(1, 1, 1, 1)
{

}

Dystopia::SpriteRenderer::SpriteRenderer(Dystopia::SpriteRenderer&& _rhs) noexcept
	: Renderer{ Ut::Move(_rhs) }, mAnimations{ Ut::Move(_rhs.mAnimations) },
	mnID{ Ut::Move(_rhs.mnID) }, mnCol{ Ut::Move(_rhs.mnCol) }, mnRow{ Ut::Move(_rhs.mnRow) },
	mfFrameTime{ Ut::Move(_rhs.mfFrameTime) }, mfAccTime{ Ut::Move(_rhs.mfAccTime) }, mpAtlas{ Ut::Move(_rhs.mpAtlas) }, 
	mNextSectionPos{ Ut::Move(_rhs.mNextSectionPos) }, mbPlayOnStart{ Ut::Move(_rhs.mbPlayOnStart) }, mbPlay{ Ut::Move(_rhs.mbPlay) }, 
mbSimulate{ Ut::Move(_rhs.mbSimulate) }, mvTintCol{Ut::Move(_rhs.mvTintCol)}, mfTintPerc{Ut::Move(_rhs.mfTintPerc)}
{
	_rhs.mAnimations.clear();
	_rhs.mpAtlas = nullptr;
}

Dystopia::SpriteRenderer::SpriteRenderer(const SpriteRenderer& _rhs) noexcept
	: Renderer{ _rhs }, mpAtlas{ _rhs.mpAtlas },
	mAnimations{ _rhs.mAnimations }, mnID{ _rhs.mnID }, mnCol{ _rhs.mnCol },
	mnRow{_rhs.mnRow }, mfFrameTime{ _rhs.mfFrameTime }, mfAccTime{ _rhs.mfAccTime }, 
	mNextSectionPos{ _rhs.mNextSectionPos }, mbPlayOnStart{ _rhs.mbPlayOnStart }, 
	mbSimulate{ _rhs.mbSimulate }, mbPlay{ _rhs.mbPlay }, mvTintCol{ _rhs.mvTintCol }, mfTintPerc(_rhs.mfTintPerc)
{
}

void Dystopia::SpriteRenderer::Awake(void)
{
	Renderer::Awake();
	if (mpTexture)
	{
		mpAtlas = EngineCore::GetInstance()->GetSubSystem<TextureSystem>()->GetAtlas(mpTexture->GetName());
		if (mpAtlas)
		{
			if (!mpAtlas->GetAllSections().size())
				mpAtlas->AddSection(Math::Vec2{ 0,0 }, mpTexture->GetWidth(), mpTexture->GetHeight());
		}
		else
			mpAtlas = EngineCore::GetInstance()->GetSubSystem<TextureSystem>()->GenAtlas(mpTexture);
	}
	ResetFrames();
}

void Dystopia::SpriteRenderer::Init(void)
{
	Renderer::Init();
	if (mpTexture)
	{
		mpAtlas = EngineCore::GetInstance()->GetSubSystem<TextureSystem>()->GetAtlas(mpTexture->GetName());
		if (mpAtlas)
		{
			if (!mpAtlas->GetAllSections().size())
				mpAtlas->AddSection(Math::Vec2{ 0,0 }, mpTexture->GetWidth(), mpTexture->GetHeight());
		}
		else
			mpAtlas = EngineCore::GetInstance()->GetSubSystem<TextureSystem>()->GenAtlas(mpTexture);
	}
	ResetFrames();
	mbPlay = mbPlayOnStart;
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
	if (mpAtlas && mnID < mAnimations.size() && mbPlay)
	{
		int endIndex = mAnimations[mnID].mnEnd ? mAnimations[mnID].mnEnd : mAnimations[mnID].mnCol * mAnimations[mnID].mnRow;
		int startCol = GetStartCol();
		int startRow = GetStartRow();

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

			auto currIndex = (mnRow * mAnimations[mnID].mnCol) + mnCol;
			if (mAnimations[mnID].mbLoop)
			{
				if (!currIndex || (mAnimations[mnID].mnEnd && (mAnimations[mnID].mnEnd == mAnimations[mnID].mnStart)))
				{
					mnCol = startCol;
					mnRow = startRow;
				}

				if (endIndex && currIndex >= (endIndex - 1))
				{
					mnCol = startCol;
					mnRow = startRow;
				}
			}
			else
			{
				if (endIndex && currIndex >= (endIndex-1))
				{
					mnRow = endIndex / mAnimations[mnID].mnCol;
					mnCol = endIndex - (mAnimations[mnID].mnCol * mnRow);
					Stop();
				}
				else if (currIndex >= ( mAnimations[mnID].mnRow * mAnimations[mnID].mnCol) - 1)
				{
					mnRow = mAnimations[mnID].mnRow - 1;
					mnCol = mAnimations[mnID].mnCol - 1;
					Stop();
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
			mfAccTime = 0;
			mnCol = GetStartCol();
			mnRow = GetStartRow();
			return;
		}
	}
}

void Dystopia::SpriteRenderer::SetAnimation(unsigned _nID)
{
	DEBUG_BREAK(_nID > mAnimations.size(), "SpriteRenderer Error: Invalid Animation!");

	mnID = _nID;
	mfAccTime = 0;
	mnCol = GetStartCol();
	mnRow = GetStartRow();
}

Dystopia::SpriteRenderer* Dystopia::SpriteRenderer::Duplicate(void) const
{
	return static_cast<ComponentDonor<SpriteRenderer>*>(EngineCore::GetInstance()->Get<GraphicsSystem>())->RequestComponent(*this);
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
		_out << a.mnStart;
		_out << a.mnEnd;
	}
	_out << mnID;
	_out << mbPlayOnStart;
	_out << mvTintCol;
	_out << mfTintPerc;
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
		_in >> a.mnStart;
		_in >> a.mnEnd;
		mAnimations.Insert(a);
	}
	_in >> mnID;
	_in >> mbPlayOnStart;
	_in >> mvTintCol;
	_in >> mfTintPerc;

	_in.ConsumeEndBlock();
}

void Dystopia::SpriteRenderer::EditorUI(void) noexcept
{
#if EDITOR

	EGUI::PushLeftAlign(95);

	TintColorPicker();
	TextureFields();
	AnimFields();

	if (mpAtlas && mpTexture)
	{
		AddAnimations();

		EGUI::PushLeftAlign(110);
		for (auto& elem : mAnimations)
			if (elem.mnID >= mpAtlas->GetAllSections().size())
				elem.mnID = 0;

		for (size_t i = 0; i < mAnimations.size(); ++i)
		{
			EGUI::PushID(static_cast<int>(i));
			if (SpriteSheetFields(i))
				mAnimations.FastRemove(i--);
			EGUI::PopID();
		}
		EGUI::PopLeftAlign();
	}
	EGUI::PopLeftAlign();

#endif
}

void Dystopia::SpriteRenderer::SetTexture(Texture* _pTexture) noexcept
{
	Renderer::SetTexture(_pTexture);
	mpAtlas = nullptr;
	if (_pTexture)
	{
		mpAtlas = EngineCore::Get<TextureSystem>()->GetAtlas(_pTexture->GetName());
		if (!mpAtlas)
			mpAtlas = EngineCore::Get<TextureSystem>()->GenAtlas(_pTexture);
	}
	if (mpTexture && mpAtlas && !mpAtlas->GetAllSections().size())
		mpAtlas->AddSection(Math::Vec2{ 0,0 }, mpTexture->GetWidth(), mpTexture->GetHeight());

	if (!mpTexture && !mpAtlas)
		mnID = mnRow = mnCol = 0;
}

void Dystopia::SpriteRenderer::SetColorA(const Math::Vec4& _col)
{
	mvTintCol = _col;
}

void Dystopia::SpriteRenderer::SetColor(const Math::Vec3D& _col)
{
	const auto tempW = mvTintCol.w;
	mvTintCol = _col;
	mvTintCol.w = tempW;
}

void Dystopia::SpriteRenderer::SetAlpha(float _a)
{
	mvTintCol.w = _a;
}

void Dystopia::SpriteRenderer::SetAlphaPerc(float _perc)
{
	if (_perc <= 0.f) _perc = 0.f;
	else if (_perc >= 1.f) _perc = 1.0f;
	mfTintPerc = _perc;
}

HashString Dystopia::SpriteRenderer::GetCurrentAnimation(void) const
{
	if (!mAnimations.size())
		return HashString{ "" };

	return mAnimations[mnID].mstrName;
}

unsigned Dystopia::SpriteRenderer::GetCurrentIndex(void) const
{
	if (!mAnimations.size())
		return 0;

	return mnCol + (mnRow * mAnimations[mnID].mnCol);
}

Math::Vec4 Dystopia::SpriteRenderer::GetTint() const
{
	return mvTintCol;
}

float Dystopia::SpriteRenderer::GetTintPerc() const
{
	return mfTintPerc;
}

bool Dystopia::SpriteRenderer::IsPlaying(void) const
{
	if (!mAnimations.size())
		return false;

	return mbPlay;
}

void Dystopia::SpriteRenderer::Play(void)
{
	if (!mAnimations.size())
		return;

	mbPlay = true;
	mnCol = GetStartCol();
	mnRow = GetStartRow();
}

void Dystopia::SpriteRenderer::Stop(void)
{
	mfAccTime = 0;
	mbPlay = false;
}

void Dystopia::SpriteRenderer::SetSpeed(float _s)
{
	mfFrameTime = _s;
}

Math::Vec2 Dystopia::SpriteRenderer::Resized(float _xMult, float _yMult) const
{
	if (mpTexture)
	{
		auto nScale = GetOwner()->GetComponent<Transform>()->GetGlobalScale();
		nScale.x = static_cast<float>(mpTexture->GetWidth()) * _xMult;
		nScale.y = static_cast<float>(mpTexture->GetHeight()) * _yMult;

		if (mpAtlas && mnID < mAnimations.size())
		{
			auto& cur = mpAtlas->GetAllSections()[mAnimations[mnID].mnID];
			nScale.x = nScale.x * cur.mCol;
			nScale.y = nScale.y * cur.mRow;
		}
		return Math::Vec2{ nScale.x, nScale.y };
	}
	return Math::Vec2{};
}

void Dystopia::SpriteRenderer::ResizeToFit(float _xMult, float _yMult) const
{
	if (GetOwner())
	{
		auto xy = Resized(_xMult, _yMult);
		auto nScale = GetOwner()->GetComponent<Transform>()->GetGlobalScale();
		nScale.x = xy.x;
		nScale.y = xy.y;
		GetOwner()->GetComponent<Transform>()->SetGlobalScale(nScale);
	}
}

void Dystopia::SpriteRenderer::TextureFields(void)
{
#if EDITOR

	auto cmd = Editor::EditorMain::GetInstance()->GetSystem<Editor::EditorCommands>();
	EGUI::Display::EmptyBox("Texture", 150, (mpTexture) ? mpTexture->GetName().c_str() : "-empty-", true);
	if (const auto t = EGUI::Display::StartPayloadReceiver<::Editor::File>(EGUI::ALL_IMG))
	{
		cmd->FunctionCommand(GetOwnerID(), cmd->MakeFnCommand(&SpriteRenderer::SetTexture, mpTexture), 
										   cmd->MakeFnCommand(&SpriteRenderer::SetTexture, 
															  EngineCore::GetInstance()->GetSystem<GraphicsSystem>()->LoadTexture(t->mPath.c_str())
															  ));
		EGUI::Display::EndPayloadReceiver();
	}

	EGUI::SameLine();
	if (EGUI::Display::IconCross("Clear", 8.f))
	{
		cmd->FunctionCommand(GetOwnerID(), cmd->MakeFnCommand(&SpriteRenderer::SetTexture, mpTexture), 
										   cmd->MakeFnCommand(&SpriteRenderer::SetTexture, nullptr));
	}

	if (mpTexture)
	{
		EGUI::Display::Label("Preview");
		EGUI::SameLine(DefaultAlighnmentSpacing+35);
		const float ratio = static_cast<float>(mpTexture->GetHeight()) / static_cast<float>(mpTexture->GetWidth());
		EGUI::Display::Image(mpTexture->GetID(), Math::Vec2{ 140, 140 * ratio }, false, true);

		EGUI::Display::Dummy(DefaultAlighnmentSpacing+80);
		EGUI::SameLine();
		if (EGUI::Display::Button("Auto Resize", Math::Vec2{ 140, 25 }))
		{
			auto size = Resized();
			auto scale = GetOwner()->GetComponent<Transform>()->GetGlobalScale();
			auto nScale = GetOwner()->GetComponent<Transform>()->GetGlobalScale();
			nScale.x = size.x;
			nScale.y = size.y;
			cmd->FunctionCommand(GetOwnerID(), cmd->MakeFnCommand<Transform, const Math::Vec4&>(&Transform::SetGlobalScale, scale),
											   cmd->MakeFnCommand<Transform, const Math::Vec4&>(&Transform::SetGlobalScale, nScale));
		}
	}

	if (EGUI::Display::EmptyBox("Mesh", 150, (mpMesh) ? mpMesh->GetName().c_str() : "-no mesh-", true))
	{
	}
	if (::Editor::File *t = EGUI::Display::StartPayloadReceiver<::Editor::File>(EGUI::FILE))
	{
		EGUI::Display::EndPayloadReceiver();
	}
	if (EGUI::Display::EmptyBox("Shader", 150, "shader has no name or id", true))
	{
	}
	if (::Editor::File *t = EGUI::Display::StartPayloadReceiver<::Editor::File>(EGUI::FILE))
	{
		EGUI::Display::EndPayloadReceiver();
	}

#endif
}

void Dystopia::SpriteRenderer::AnimFields(void)
{
#if EDITOR

	auto cmd = Editor::EditorMain::GetInstance()->GetSystem<Editor::EditorCommands>();

	if (EGUI::Display::CheckBox("Simulate", &mbPlay))
	{
		cmd->ChangeValue(GetOwnerID(), &SpriteRenderer::mbPlay, !mbPlay, mbPlay);
		if (mbPlay) Play();
		else Stop();
	}

	if (EGUI::Display::CheckBox("On Awake", &mbPlayOnStart))
		cmd->ChangeValue(GetOwnerID(), &SpriteRenderer::mbPlayOnStart, !mbPlayOnStart, mbPlayOnStart);

	switch (EGUI::Display::DragFloat("Frame Time", &mfFrameTime, 0.01f, 0.016f, 10.f))
	{
	case EGUI::eDragStatus::eSTART_DRAG:
		cmd->StartRec(&SpriteRenderer::mfFrameTime, this);
		break;
	case EGUI::eDragStatus::eEND_DRAG:
	case EGUI::eDragStatus::eDEACTIVATED:
	case EGUI::eDragStatus::eENTER:
		cmd->EndRec(&SpriteRenderer::mfFrameTime, this);
		break;
	}

	if (mAnimations.size())
	{
		switch (EGUI::Display::DragInt("Using Anim", &mnID, 0.1f, 0, static_cast<int>(mAnimations.size()) - 1))
		{
		case EGUI::eDragStatus::eSTART_DRAG:
			cmd->StartRec(&SpriteRenderer::mnID, this);
			break;
		case EGUI::eDragStatus::eEND_DRAG:
		case EGUI::eDragStatus::eDEACTIVATED:
		case EGUI::eDragStatus::eENTER:
			cmd->EndRec(&SpriteRenderer::mnID, this);
			break;
		}
	}

#endif
}

void Dystopia::SpriteRenderer::AddAnimations(void)
{
#if EDITOR
	if (EGUI::Display::Button("Add Anim", Math::Vec2{150, 20}))
	{
		SpriteSheet ss;
		ss.mstrName = "Rename this";
		ss.mbLoop = false;
		ss.mnCol = 1;
		ss.mnRow = 1;
		ss.mnID = 0;
		ss.mnStart = 0;
		ss.mnEnd = 0;
		mAnimations.Insert(Ut::Move(ss));
	}
#endif 
}

#if EDITOR
bool Dystopia::SpriteRenderer::SpriteSheetFields(const size_t& _i)
{
	bool toRemove = false;
	auto& anim = mAnimations[_i];
	HashString identifier{ "Animation_" };
	identifier += _i;
	const bool showTree = EGUI::Display::StartTreeNode(identifier.c_str());
	if (ImGui::BeginPopupContextItem())
	{
		if (EGUI::Display::SelectableTxt("Remove"))
			toRemove = true;
		ImGui::EndPopup();
	}
	if (showTree)
	{
		EGUI::Indent();

		/*********** Name **********/
		static constexpr int maxSize = 256;
		char buffer[maxSize];
		strcpy_s(buffer, 256, anim.mstrName.c_str());
		if (EGUI::Display::TextField("Name", buffer, maxSize, true, 150))
		{
			anim.mstrName = static_cast<const char*>(buffer);
		}

		/******** Section ID *******/
		auto sID = static_cast<int>(anim.mnID);
		auto max = static_cast<unsigned>(mpAtlas->GetAllSections().size());
		if (EGUI::Display::DropDownSelection<21>("Section", sID, max))
		{
			anim.mnID = sID;
		}

		/********** Loop **********/
		EGUI::Display::CheckBox("Loop", &anim.mbLoop);

		/******** Columns *********/
		auto& curSec = mpAtlas->GetAllSections()[anim.mnID];
		int maxC = static_cast<int>(roundf( (curSec.uEnd - curSec.uStart) / curSec.mCol));
		auto col = anim.mnCol;
		EGUI::Display::SliderInt("Columns", &col, 1, maxC);
		anim.mnCol = Math::Clamp(col, 1, maxC);

		/********* Rows *********/
		auto row = anim.mnRow;
		int maxR = static_cast<int>(roundf((curSec.vEnd - curSec.vStart) / curSec.mRow));
		EGUI::Display::SliderInt("Rows", &row, 1, maxR);
		anim.mnRow = Math::Clamp(row, 1, maxR);

		/******** Start *********/
		auto start = anim.mnStart;
		int maxStart = static_cast<int>(anim.mnCol * anim.mnRow) - 1;
		EGUI::Display::SliderInt("Start", &start, 0, maxStart);
		anim.mnStart = Math::Clamp(start, 0, maxStart);

		/******** End *********/
		auto end = anim.mnEnd;
		int maxEnd = static_cast<int>(anim.mnCol * anim.mnRow) - 1;
		EGUI::Display::SliderInt("End", &end, 0, maxEnd);
		anim.mnEnd = Math::Clamp(end, 0, maxEnd);

		EGUI::UnIndent();
		EGUI::Display::EndTreeNode();
	}
	return toRemove;
}
#endif

int Dystopia::SpriteRenderer::GetStartCol(void) const
{
	return mAnimations[mnID].mnStart ? (mAnimations[mnID].mnStart % mAnimations[mnID].mnCol) : 0;
}

int Dystopia::SpriteRenderer::GetStartRow(void) const
{
	return mAnimations[mnID].mnStart ? static_cast<int>(mAnimations[mnID].mnStart / mAnimations[mnID].mnCol) : 0;
}

void Dystopia::SpriteRenderer::ResetFrames(void)
{
	if (mnID < mAnimations.size())
	{
		mnCol = GetStartCol();
		mnRow = GetStartRow();
	}
	else
		mnID = mnCol = mnRow = 0;

	mfAccTime = 0;
}

void Dystopia::SpriteRenderer::TintColorPicker()
{
#if EDITOR
	EGUI::Display::Label("Tint");
	EGUI::SameLine(60);
	if (ImGui::ColorEdit4("Tint Color", &mvTintCol[0], (ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoLabel
														| ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoOptions)))
	{

	}

#endif
}










