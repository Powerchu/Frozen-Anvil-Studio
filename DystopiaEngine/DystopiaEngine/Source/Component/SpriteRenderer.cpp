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
#include <fmod.h>

#if EDITOR
#include "Editor/EGUI.h"
#include "Editor/EditorMain.h"
#include "Editor/EditorCommands.h"
#endif

Dystopia::SpriteRenderer::SpriteRenderer(void) noexcept
	: Renderer{}, mAnimations{ 1 }, mnID{ 0 }, mnCol{ 0 }, mnRow{ 0 },
	mfFrameTime{ 0.016f }, mfAccTime{ 0 }, mpAtlas{ nullptr }, mNextSectionPos{0,0}
{

}

Dystopia::SpriteRenderer::SpriteRenderer(Dystopia::SpriteRenderer&& _rhs) noexcept
	: Renderer{ Ut::Move(_rhs) }, mAnimations{ Ut::Move(_rhs.mAnimations) },
	mnID{ Ut::Move(_rhs.mnID) }, mnCol{ Ut::Move(_rhs.mnCol) }, mnRow{ Ut::Move(_rhs.mnRow) },
	mfFrameTime{ Ut::Move(_rhs.mfFrameTime) }, mfAccTime{ Ut::Move(_rhs.mfAccTime) }, mpAtlas{ Ut::Move(_rhs.mpAtlas) }, 
	mNextSectionPos{ Ut::Move(_rhs.mNextSectionPos) }
{
	_rhs.mAnimations.clear();
	_rhs.mpAtlas = nullptr;
}

Dystopia::SpriteRenderer::SpriteRenderer(const SpriteRenderer& _rhs) noexcept
	: Renderer{ _rhs }, mAnimations{ _rhs.mAnimations },
	mnID{ _rhs.mnID }, mnCol{ _rhs.mnCol }, mnRow{_rhs.mnRow },
	mfFrameTime{ _rhs.mfFrameTime }, mfAccTime{ _rhs.mfAccTime }, mpAtlas{ _rhs.mpAtlas }, mNextSectionPos{ _rhs.mNextSectionPos }
{
}

void Dystopia::SpriteRenderer::Awake(void)
{
	Renderer::Awake();
	mpAtlas = mpTexture ? EngineCore::GetInstance()->GetSubSystem<TextureSystem>()->GetAtlas(mpTexture->GetName()) : nullptr;
	mnID = mnCol = mnRow = 0;
	if (mpAtlas && !mpAtlas->GetAllSections().size())
		mpAtlas->AddSection(Math::Vec2{ 0,0 }, mpTexture->GetWidth(), mpTexture->GetHeight());
}

void Dystopia::SpriteRenderer::Init(void)
{
	Renderer::Init();
	if (mpTexture && mpAtlas && !mpAtlas->GetAllSections().size())
		mpAtlas->AddSection(Math::Vec2{ 0,0 }, mpTexture->GetWidth(), mpTexture->GetHeight());
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
	if (mpAtlas && mnID < mAnimations.size())
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
	AtlasFields();
	AddAnimations();

	EGUI::PushLeftAlign(110);
	if (mpAtlas && mpTexture)
	{
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
	}
	EGUI::PopLeftAlign();
	EGUI::PopLeftAlign();
#endif
}

void Dystopia::SpriteRenderer::SetTexture(Texture* _pTexture) noexcept
{
	Renderer::SetTexture(_pTexture);
	mpAtlas = _pTexture ? EngineCore::GetInstance()->GetSubSystem<TextureSystem>()->GetAtlas(_pTexture->GetName()) : nullptr;
	if (mpTexture && mpAtlas && !mpAtlas->GetAllSections().size())
		mpAtlas->AddSection(Math::Vec2{ 0,0 }, mpTexture->GetWidth(), mpTexture->GetHeight());
}

void Dystopia::SpriteRenderer::TextureFields(void)
{
#if EDITOR

	auto cmd = Editor::EditorMain::GetInstance()->GetSystem<Editor::EditorCommands>();
	EGUI::Display::EmptyBox("Texture", 150, (mpTexture) ? mpTexture->GetName() : "-empty-", true);
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
		EGUI::SameLine(DefaultAlighnmentSpacing, 80);
		const float ratio = static_cast<float>(mpTexture->GetHeight()) / static_cast<float>(mpTexture->GetWidth());
		EGUI::Display::Image(mpTexture->GetID(), Math::Vec2{ 140, 140 * ratio }, false, true);

		EGUI::SameLine();
		if (EGUI::Display::Button("Auto", Math::Vec2{ 35, 30 }))
		{
			auto scale = GetOwner()->GetComponent<Transform>()->GetScale();
			auto nScale = scale;
			nScale.x = static_cast<float>(mpTexture->GetWidth()) / 10;
			nScale.y = static_cast<float>(mpTexture->GetHeight()) / 10;
			cmd->FunctionCommand(GetOwnerID(), cmd->MakeFnCommand<Transform, const Math::Vec4&>(&Transform::SetScale, scale),
											   cmd->MakeFnCommand<Transform, const Math::Vec4&>(&Transform::SetScale, nScale));
		}
	}

	if (EGUI::Display::EmptyBox("Mesh", 150, (mpMesh) ? mpMesh->GetName() : "-no mesh-", true))
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

void Dystopia::SpriteRenderer::AtlasFields(void)
{
#if EDITOR



#endif
}

void Dystopia::SpriteRenderer::AddAnimations(void)
{
#if EDITOR
	EGUI::Display::Label("Animations");
	EGUI::SameLine(DefaultAlighnmentSpacing, 80);
	if (EGUI::Display::Button("Add", Math::Vec2{90, 24}))
	{
		SpriteSheet ss;
		ss.mstrName = "Rename this";
		ss.mbLoop = false;
		ss.mnCol = 0;
		ss.mnRow = 0;
		ss.mnID = 0;
		mAnimations.Insert(Ut::Move(ss));
	}
#endif 
}

bool Dystopia::SpriteRenderer::SpriteSheetFields(const size_t& _i)
{
#if EDITOR

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
			anim.mstrName = buffer;
		}

		/******** Section ID *******/
		auto sID = static_cast<int>(anim.mnID);
		auto max = static_cast<unsigned>(mpAtlas->GetAllSections().size());
		EGUI::Display::DropDownSelection<21>("Section", sID, max);

		/********** Loop **********/
		EGUI::Display::CheckBox("Loop", &anim.mbLoop);

		/******** Columns *********/
		auto col = static_cast<int>(anim.mnCol);
		switch (EGUI::Display::DragInt("Columns", &col, 1.f, 0, INT_MAX))
		{
		case EGUI::eDragStatus::eDRAGGING:
			anim.mnCol = static_cast<unsigned>(col);
			break;
		case EGUI::eDragStatus::eSTART_DRAG:
		case EGUI::eDragStatus::eEND_DRAG:
		case EGUI::eDragStatus::eENTER:
		case EGUI::eDragStatus::eTABBED:
		case EGUI::eDragStatus::eDEACTIVATED:
		case EGUI::eDragStatus::eNO_CHANGE:
		default:
			break;
		}

		/********* Rows *********/
		auto row = static_cast<int>(anim.mnRow);
		switch (EGUI::Display::DragInt("Rows", &row, 1.f, 0, INT_MAX))
		{
		case EGUI::eDragStatus::eDRAGGING:
			anim.mnRow = static_cast<unsigned>(row);
			break;
		case EGUI::eDragStatus::eSTART_DRAG:
		case EGUI::eDragStatus::eEND_DRAG:
		case EGUI::eDragStatus::eENTER:
		case EGUI::eDragStatus::eTABBED:
		case EGUI::eDragStatus::eDEACTIVATED:
		case EGUI::eDragStatus::eNO_CHANGE:
		default:
			break;
		}

		EGUI::UnIndent();
		EGUI::Display::EndTreeNode();
	}
	return toRemove;

#endif
}






