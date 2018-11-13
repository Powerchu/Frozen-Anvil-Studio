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
#include "Editor/Editor.h"
#include "Editor/ConsoleLog.h"

#include "Component/SpriteRenderer.h"
#include "System/Driver/Driver.h"
#include "System/Graphics/Texture2D.h"
#include "System/Graphics/TextureAtlas.h"
#include "System/Graphics/TextureSystem.h"
#include "System/Graphics/GraphicsSystem.h"

Dystopia::SpriteEditor* gpInstance = 0;
Dystopia::SpriteEditor* Dystopia::SpriteEditor::GetInstance(void)
{
	if (gpInstance) return gpInstance;

	gpInstance = new SpriteEditor{};
	return gpInstance;
}

Dystopia::SpriteEditor::SpriteEditor(void)
	: EditorTab{ false }, 
	mLabel{ "Sprite Editor" }
{
}

Dystopia::SpriteEditor::~SpriteEditor(void)
{
}

void Dystopia::SpriteEditor::Init(void)
{
}

void Dystopia::SpriteEditor::Update(const float&)
{
}

void Dystopia::SpriteEditor::EditorUI(void)
{
	EGUI::Display::EmptyBox("Sprite Sheet", 150, mpTexture ? mpTexture->GetName() : "");
	if (::Editor::File *t1 = EGUI::Display::StartPayloadReceiver<::Editor::File>(EGUI::PNG))
	{
		mpTexture = EngineCore::GetInstance()->GetSystem<GraphicsSystem>()->LoadTexture(t1->mPath.c_str());
		mpAtlas = EngineCore::GetInstance()->Get<TextureSystem>()->GetAtlas(mpTexture->GetName());
		EGUI::Display::EndPayloadReceiver();
	}
	if (::Editor::File *t2 = EGUI::Display::StartPayloadReceiver<::Editor::File>(EGUI::BMP))
	{
		mpTexture = EngineCore::GetInstance()->GetSystem<GraphicsSystem>()->LoadTexture(t2->mPath.c_str());
		mpAtlas = EngineCore::GetInstance()->Get<TextureSystem>()->GetAtlas(mpTexture->GetName());
		EGUI::Display::EndPayloadReceiver();
	}
	if (::Editor::File *t3 = EGUI::Display::StartPayloadReceiver<::Editor::File>(EGUI::DDS))
	{
		mpTexture = EngineCore::GetInstance()->GetSystem<GraphicsSystem>()->LoadTexture(t3->mPath.c_str());
		mpAtlas = EngineCore::GetInstance()->Get<TextureSystem>()->GetAtlas(mpTexture->GetName());
		EGUI::Display::EndPayloadReceiver();
	}

	if (!mpAtlas) return;

	EGUI::Display::HorizontalSeparator();

}

void Dystopia::SpriteEditor::Shutdown(void)
{
}

std::string Dystopia::SpriteEditor::GetLabel(void) const
{
	return mLabel;
}

void Dystopia::SpriteEditor::SaveSettings(Dystopia::TextSerialiser& /*_out*/) const
{
}

void Dystopia::SpriteEditor::LoadSettings(Dystopia::TextSerialiser& /*_in*/)
{
}





#endif 
