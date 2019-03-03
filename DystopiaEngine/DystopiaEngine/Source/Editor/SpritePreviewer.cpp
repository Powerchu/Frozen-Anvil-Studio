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
	ImGui::BeginGroup();
		EGUI::Display::EmptyBox("Previewing: ", 150, (mpTargetTexture) ? mTextureName.c_str() : "-empty-", true);
		
		if (const auto t = EGUI::Display::StartPayloadReceiver<::Editor::File>(EGUI::PNG))
		{
			mpTargetTexture = mpGfxSys->LoadTexture(t->mPath.c_str());
			mTextureName = GetTextureName(mpTargetTexture);
			mImageW = static_cast<float>(mpTargetTexture->GetWidth());
			mImageH = static_cast<float>(mpTargetTexture->GetHeight());
			mImageRatio = mImageW / mImageH;
			EGUI::Display::EndPayloadReceiver();
		}
		if (const auto t = EGUI::Display::StartPayloadReceiver<::Editor::File>(EGUI::BMP))
		{
			mpTargetTexture = mpGfxSys->LoadTexture(t->mPath.c_str());
			mTextureName = GetTextureName(mpTargetTexture);
			mImageW = static_cast<float>(mpTargetTexture->GetWidth());
			mImageH = static_cast<float>(mpTargetTexture->GetHeight());
			mImageRatio = mImageW / mImageH;
			EGUI::Display::EndPayloadReceiver();
		}
		if (const auto t = EGUI::Display::StartPayloadReceiver<::Editor::File>(EGUI::DDS))
		{
			mpTargetTexture = mpGfxSys->LoadTexture(t->mPath.c_str());
			mTextureName = GetTextureName(mpTargetTexture);
			mImageW = static_cast<float>(mpTargetTexture->GetWidth());
			mImageH = static_cast<float>(mpTargetTexture->GetHeight());
			mImageRatio = mImageW / mImageH;
			EGUI::Display::EndPayloadReceiver();
		}

		
		EGUI::Display::Label("Resolution: [X: %d, Y: %d]", mpTargetTexture ? mpTargetTexture->GetWidth() : 0,
														   mpTargetTexture ? mpTargetTexture->GetHeight() : 0);

		EGUI::Display::HorizontalSeparator();
		if (mpTargetTexture)
		{
			ImGui::BeginChild("imagePreview", ImGui::GetContentRegionAvail(), false, ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar);
				const float sw = Size().x - 6.f;
				const float sh = Size().y - (2 * EGUI::TabsImageOffsetY) - 4.f;
				const float sratio = sw / sh;
				mDisplaySize = sratio > mImageRatio ? Math::Vec2{ mImageW * (sh / mImageH), sh } :
													  Math::Vec2{ sw, mImageH * (sw / mImageW) };
				EGUI::Display::Image(mpTargetTexture->GetID(), mDisplaySize, false, true);
			ImGui::EndChild();
		}
	ImGui::EndGroup();
}

void Editor::SpritePreviewer::Shutdown(void)
{}

void Editor::SpritePreviewer::Message(eEMessage)
{}

void Editor::SpritePreviewer::SaveSettings(Dystopia::TextSerialiser&) const
{}

void Editor::SpritePreviewer::LoadSettings(Dystopia::TextSerialiser&)
{}

const HashString& Editor::SpritePreviewer::GetLabel(void) const
{
	return mLabel;
}

HashString Editor::SpritePreviewer::GetTextureName(Dystopia::Texture* _pTex) const
{
	if (_pTex == nullptr) return "invalid";
	HashString path{ _pTex->GetPath().c_str() };
	const size_t pos = path.find_last_of("/\\") + 1;
	if (pos < path.length())
		return HashString{ path.begin() + pos, path.end() };
	return "";
}

#endif //EDITOR
