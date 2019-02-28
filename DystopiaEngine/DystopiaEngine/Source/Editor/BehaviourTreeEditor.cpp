/* HEADER *********************************************************************************/
/*!
\file	BehaviourTreeEditor.cpp
\author Aaron Chu (100%)
\par    email: m.chu\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#include "Editor/BehaviourTreeEditor.h"
#include "Editor/EGUI.h"
#include "Editor/Payloads.h"

#include "System/Graphics/Texture2D.h"
#include "System/Graphics/GraphicsSystem.h"
#include "System/Driver/Driver.h"

//Dystopia::SpritePreviewer* gpInstance = 0;
//
//Dystopia::SpritePreviewer* Dystopia::SpritePreviewer::GetInstance(void)
//{
//	if (gpInstance) return gpInstance;
//
//	gpInstance = new SpritePreviewer{};
//	return gpInstance;
//}

Editor::BehaviourTreeEditor::BehaviourTreeEditor(void)
	: //EditorTab{ false },
	mLabel{ "Behaviour Tree" },
	mpTargetTexture{ nullptr }, 
	mpGfxSys{ nullptr },
	mDisplaySize{ Math::Vec2{0,0} },
	mTextureName{},
	mImageRatio{ 0 },
	mImageH{ 0 },
	mImageW{ 0 }
{
}

Editor::BehaviourTreeEditor::~BehaviourTreeEditor(void)
{}

void Editor::BehaviourTreeEditor::Load(void)
{}

bool Editor::BehaviourTreeEditor::Init(void)
{
	mpGfxSys = Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::GraphicsSystem>();
	return true;
}

void Editor::BehaviourTreeEditor::Update(float)
{}

void Editor::BehaviourTreeEditor::EditorUI(void)
{
	if (EGUI::Display::EmptyBox("WORK IN PROGRESS (GAM250) Behaviour Tree  ", 150.f, "WIP", false, true))
	{

	}

	/*if (::Editor::File *t = EGUI::Display::StartPayloadReceiver<::Editor::File>(EGUI::PNG))
	{
		mpTargetTexture = mpGfxSys->LoadTexture(t->mPath.c_str());
		mTextureName = GetTextureName();
		mImageW = static_cast<float>(mpTargetTexture->GetWidth());
		mImageH = static_cast<float>(mpTargetTexture->GetHeight());
		mImageRatio = mImageW / mImageH;
		EGUI::Display::EndPayloadReceiver();
	}
	if (::Editor::File *t = EGUI::Display::StartPayloadReceiver<::Editor::File>(EGUI::BMP))
	{
		mpTargetTexture = mpGfxSys->LoadTexture(t->mPath.c_str());
		mTextureName = GetTextureName();
		mImageW = static_cast<float>(mpTargetTexture->GetWidth());
		mImageH = static_cast<float>(mpTargetTexture->GetHeight());
		mImageRatio = mImageW / mImageH;
		EGUI::Display::EndPayloadReceiver();
	}
	if (::Editor::File *t = EGUI::Display::StartPayloadReceiver<::Editor::File>(EGUI::DDS))
	{
		mpTargetTexture = mpGfxSys->LoadTexture(t->mPath.c_str());
		mTextureName = GetTextureName();
		mImageW = static_cast<float>(mpTargetTexture->GetWidth());
		mImageH = static_cast<float>(mpTargetTexture->GetHeight());
		mImageRatio = mImageW / mImageH;
		EGUI::Display::EndPayloadReceiver();
	}

	EGUI::SameLine();
	EGUI::Display::Label("Resolution:  x[%d]  y[%d]", mpTargetTexture ? mpTargetTexture->GetWidth() : 0,
		mpTargetTexture ? mpTargetTexture->GetHeight() : 0);

	EGUI::Display::HorizontalSeparator();

	if (mpTargetTexture)
	{
		float sw = Size().x - 6.f;
		float sh = Size().y - (2 * EGUI::TabsImageOffsetY) - 4.f;
		float sratio = sw / sh;
		mDisplaySize = sratio > mImageRatio ? Math::Vec2{ mImageW * (sh / mImageH), sh } :
											  Math::Vec2{ sw, mImageH * (sw / mImageW) };
		EGUI::Display::Image(mpTargetTexture->GetID(), mDisplaySize, false, true);
	}*/
}

void Editor::BehaviourTreeEditor::Shutdown(void)
{}

void Editor::BehaviourTreeEditor::Message(eEMessage)
{}

void Editor::BehaviourTreeEditor::SaveSettings(Dystopia::TextSerialiser&) const
{}

void Editor::BehaviourTreeEditor::LoadSettings(Dystopia::TextSerialiser&)
{}

const HashString& Editor::BehaviourTreeEditor::GetLabel(void) const
{
	return mLabel;
}

HashString Editor::BehaviourTreeEditor::GetTextureName(void)
{
	HashString path{ mpTargetTexture->GetPath().c_str() };
	size_t pos = path.find_last_of("/\\") + 1;
	if (pos < path.length())
		return HashString{ path.begin() + pos, path.end() };
	return "";
}

#endif //EDITOR
