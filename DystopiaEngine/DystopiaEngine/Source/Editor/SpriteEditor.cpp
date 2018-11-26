/* HEADER *********************************************************************************/
/*!
\file	SpriteEditor.cpp
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#include "Editor/SpriteEditor.h"
#include "Editor/EGUI.h"
#include "Editor/Payloads.h"
#include "Editor/ConsoleLog.h"

#include "Component/SpriteRenderer.h"
#include "System/Driver/Driver.h"
#include "System/Graphics/Texture2D.h"
#include "System/Graphics/TextureAtlas.h"
#include "System/Graphics/TextureSystem.h"
#include "System/Graphics/GraphicsSystem.h"

Editor::SpriteEditor::SpriteEditor(void)
	: //EditorTab{ false }, 
	mLabel{ "Sprite Editor" }, 
	mpAtlas{ nullptr },
	mpGfxSys{ nullptr },
	mpTextSys{ nullptr },
	mpTexture{ nullptr },
	mSectionEditArea{ 1.f, 0.7f},
	mSettingsArea{ 0.7f, 0.3f },
	mPreviewArea{ 0.3f, 0.3f }
{}

Editor::SpriteEditor::~SpriteEditor(void)
{}

void Editor::SpriteEditor::Load(void)
{}

bool Editor::SpriteEditor::Init(void)
{
	mpGfxSys = Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::GraphicsSystem>();
	mpTextSys = Dystopia::EngineCore::GetInstance()->Get<Dystopia::TextureSystem>();
	return true;
}

void Editor::SpriteEditor::Update(float)
{
	static constexpr float halfhori = 0.7f;
	static constexpr float halfvert = 0.7f;
	const auto size = Size();

	mSectionEditArea = Math::Vec2{size.x, size.y * halfhori };
	mSettingsArea = Math::Vec2{ size.x * halfvert, size.y * (1 - halfhori) };
	mPreviewArea = Math::Vec2{ size.x* (1 - halfvert), size.y * (1 - halfhori) };
}

void Editor::SpriteEditor::EditorUI(void)
{
	EGUI::StartChild("Section Edit Area", mSectionEditArea);

	EGUI::EndChild();

	EGUI::StartChild("Section Edit Area", mSettingsArea);

	EGUI::EndChild();
	ImGui::SameLine();
	EGUI::StartChild("Section Edit Area", mPreviewArea);

	EGUI::EndChild();


	const float sx = Size().x;
	const float sy = (Size().y - EGUI::TabsImageOffsetY) * 0.75f;
	if (mpTexture && mpAtlas)
		AtlasEditing(sx, sy);

	EGUI::Display::HorizontalSeparator();

	EGUI::Display::EmptyBox("Sprite Sheet", 150, mpTexture ? mpTexture->GetName() : "");
	if (::Editor::File *t1 = EGUI::Display::StartPayloadReceiver<::Editor::File>(EGUI::ALL_IMG))
	{
		mpTexture = mpGfxSys->LoadTexture(t1->mPath.c_str());
		mpAtlas = mpTextSys->GetAtlas(mpTexture->GetName());
		EGUI::Display::EndPayloadReceiver();
	}
}

void Editor::SpriteEditor::Shutdown(void)
{}

void Editor::SpriteEditor::Message(eEMessage)
{}

void Editor::SpriteEditor::SaveSettings(Dystopia::TextSerialiser&) const
{}

void Editor::SpriteEditor::LoadSettings(Dystopia::TextSerialiser&)
{}

HashString Editor::SpriteEditor::GetLabel(void) const
{
	return mLabel;
}

void Editor::SpriteEditor::AtlasEditing(float sx, float sy)
{
	const float ix = static_cast<float>(mpTexture->GetWidth());
	const float iy = static_cast<float>(mpTexture->GetHeight());
	const float iRatio = ix / iy;
	const float sRatio = sx / sy;
	const auto fSize = sRatio > iRatio ? Math::Vec2{ ix * (sy / iy), sy } : Math::Vec2{ sx, iy * (sx / ix) };

	EGUI::Display::Image(mpTexture->GetID(), fSize);
}

#endif 

















