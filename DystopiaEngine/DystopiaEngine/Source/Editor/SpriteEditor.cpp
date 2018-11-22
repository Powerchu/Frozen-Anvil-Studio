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

//Dystopia::SpriteEditor* gpInstance = 0;
//Dystopia::SpriteEditor* Dystopia::SpriteEditor::GetInstance(void)
//{
//	if (gpInstance) return gpInstance;
//
//	gpInstance = new SpriteEditor{};
//	return gpInstance;
//}

Editor::SpriteEditor::SpriteEditor(void)
	: //EditorTab{ false }, 
	mLabel{ "Sprite Editor" }, 
	mpAtlas{ nullptr },
	mpGfxSys{ nullptr },
	mpTextSys{ nullptr },
	mpTexture{ nullptr }
{
}

Editor::SpriteEditor::~SpriteEditor(void)
{
}

void Editor::SpriteEditor::Load(void)
{}

bool Editor::SpriteEditor::Init(void)
{
	mpGfxSys = Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::GraphicsSystem>();
	mpTextSys = Dystopia::EngineCore::GetInstance()->Get<Dystopia::TextureSystem>();
	return true;
}

void Editor::SpriteEditor::Update(float)
{}

void Editor::SpriteEditor::EditorUI(void)
{
	EGUI::Display::EmptyBox("Sprite Sheet", 150, mpTexture ? mpTexture->GetName() : "");
	if (::Editor::File *t1 = EGUI::Display::StartPayloadReceiver<::Editor::File>(EGUI::PNG))
	{
		mpTexture = mpGfxSys->LoadTexture(t1->mPath.c_str());
		mpAtlas = mpTextSys->GetAtlas(mpTexture->GetName());
		EGUI::Display::EndPayloadReceiver();
	}
	if (::Editor::File *t2 = EGUI::Display::StartPayloadReceiver<::Editor::File>(EGUI::BMP))
	{
		mpTexture = mpGfxSys->LoadTexture(t2->mPath.c_str());
		mpAtlas = mpTextSys->GetAtlas(mpTexture->GetName());
		EGUI::Display::EndPayloadReceiver();
	}
	if (::Editor::File *t3 = EGUI::Display::StartPayloadReceiver<::Editor::File>(EGUI::DDS))
	{
		mpTexture = mpGfxSys->LoadTexture(t3->mPath.c_str());
		mpAtlas = mpTextSys->GetAtlas(mpTexture->GetName());
		EGUI::Display::EndPayloadReceiver();
	}

	if (!mpAtlas) return;

	EGUI::Display::HorizontalSeparator();

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


#endif 

















