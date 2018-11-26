/* HEADER *********************************************************************************/
/*!
\file	SpritePreviewer.cpp
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#include "Editor/SpritePreviewer.h"
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

Editor::SpritePreviewer::SpritePreviewer(void)
	: //EditorTab{ false },
	mLabel{ "Previewer" },
	mpTargetTexture{ nullptr }, 
	mpGfxSys{ nullptr },
	mDisplaySize{ Math::Vec2{0,0} },
	mTextureName{},
	mImageRatio{ 0 },
	mImageH{ 0 },
	mImageW{ 0 }
{
}

Editor::SpritePreviewer::~SpritePreviewer(void)
{}

void Editor::SpritePreviewer::Load(void)
{}

bool Editor::SpritePreviewer::Init(void)
{
	mpGfxSys = Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::GraphicsSystem>();
	return true;
}

void Editor::SpritePreviewer::Update(float)
{}

void Editor::SpritePreviewer::EditorUI(void)
{
	if (EGUI::Display::EmptyBox("Previewing  ", 150, (mpTargetTexture) ? mTextureName.c_str() : "-empty-", true))
	{
	}

	if (::Editor::File *t = EGUI::Display::StartPayloadReceiver<::Editor::File>(EGUI::PNG))
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
	EGUI::Display::Label("Resolution:   < %d x >   < %d  y>", mpTargetTexture ? mpTargetTexture->GetWidth() : 0,
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
	}
}

void Editor::SpritePreviewer::Shutdown(void)
{}

void Editor::SpritePreviewer::Message(eEMessage)
{}

void Editor::SpritePreviewer::SaveSettings(Dystopia::TextSerialiser&) const
{}

void Editor::SpritePreviewer::LoadSettings(Dystopia::TextSerialiser&)
{}

HashString Editor::SpritePreviewer::GetLabel(void) const
{
	return mLabel;
}

HashString Editor::SpritePreviewer::GetTextureName(void)
{
	HashString path{ mpTargetTexture->GetPath().c_str() };
	size_t pos = path.find_last_of("/\\") + 1;
	if (pos < path.length())
		return HashString{ path.begin() + pos, path.end() };
	return "";
}

#endif //EDITOR
