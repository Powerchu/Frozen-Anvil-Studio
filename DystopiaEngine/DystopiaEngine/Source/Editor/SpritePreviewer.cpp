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

Dystopia::SpritePreviewer* gpInstance = 0;

Dystopia::SpritePreviewer* Dystopia::SpritePreviewer::GetInstance(void)
{
	if (gpInstance) return gpInstance;

	gpInstance = new SpritePreviewer{};
	return gpInstance;
}

Dystopia::SpritePreviewer::SpritePreviewer(void)
	: EditorTab{ false },
	mLabel{ "Previewer" },
	mpTargetTexture{ nullptr },
	mDisplaySize{ Math::Vec2{0,0} },
	mTextureName{},
	mImageRatio{ 0 },
	mImageH{ 0 },
	mImageW{ 0 }
{
}

Dystopia::SpritePreviewer::~SpritePreviewer(void)
{
}

void Dystopia::SpritePreviewer::Init(void)
{

}

void Dystopia::SpritePreviewer::Update(const float&)
{

}

void Dystopia::SpritePreviewer::EditorUI(void)
{
	if (EGUI::Display::EmptyBox("Previewing  ", 150, (mpTargetTexture) ? mTextureName : "-empty-", true))
	{
	}

	if (Dystopia::File *t = EGUI::Display::StartPayloadReceiver<Dystopia::File>(EGUI::PNG))
	{
		mpTargetTexture = EngineCore::GetInstance()->GetSystem<GraphicsSystem>()->LoadTexture(t->mPath);
		mTextureName = GetTextureName();
		mImageW = static_cast<float>(mpTargetTexture->GetWidth());
		mImageH = static_cast<float>(mpTargetTexture->GetHeight());
		mImageRatio = mImageW / mImageH;
		EGUI::Display::EndPayloadReceiver();
	}

	EGUI::Display::HorizontalSeparator();

	if (mpTargetTexture)
	{
		float sw		= Size().x - 6.f;
		float sh		= Size().y - (2*EGUI::TabsImageOffsetY) - 4.f;
		float sratio	= sw / sh;
		mDisplaySize	= sratio > mImageRatio ? Math::Vec2{ mImageW * (sh / mImageH), sh } :
												 Math::Vec2{ sw, mImageH * (sw / mImageW) };
		EGUI::Display::Image(mpTargetTexture->GetID(), mDisplaySize, false, true);
	}
}

void Dystopia::SpritePreviewer::Shutdown(void)
{

}

std::string Dystopia::SpritePreviewer::GetLabel(void) const
{
	return mLabel;
}

void Dystopia::SpritePreviewer::SaveSettings(TextSerialiser& /*_out*/) const
{

}

void Dystopia::SpritePreviewer::LoadSettings(TextSerialiser& /*_in*/)
{

}

std::string Dystopia::SpritePreviewer::GetTextureName(void)
{
	std::string path = mpTargetTexture->GetPath();
	size_t pos = path.find_last_of("/\\") + 1;
	if (pos < path.length())
		return std::string{ path.begin() + pos, path.end() };
	return "";
}

#endif //EDITOR
