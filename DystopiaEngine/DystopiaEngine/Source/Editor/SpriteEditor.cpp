/* HEADER *********************************************************************************/
/*!
\file	SpriteEditor.cpp
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#include "Editor/SpriteEditor.h"
#include "Editor/EGUI.h"
#include "Editor/Payloads.h"
#include "Editor/ConsoleLog.h"
#include "Editor/EditorMain.h"

#include "Component/SpriteRenderer.h"
#include "System/Driver/Driver.h"
#include "System/Graphics/Texture2D.h"
#include "System/Graphics/TextureAtlas.h"
#include "System/Graphics/TextureSystem.h"
#include "System/Graphics/GraphicsSystem.h"

Editor::SpriteEditor::SpriteEditor(void)
	: 
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
	const float sides = 5;
	const float tops = 3;
	auto size = Size();
	size.y -= EGUI::TabsImageOffsetY;

	mSectionEditArea = Math::Vec2{size.x - sides, (size.y - tops) * halfhori };
	mSettingsArea = Math::Vec2{ (size.x- sides) * halfvert, (size.y - tops) * (1 - halfhori) };
	mPreviewArea = Math::Vec2{ (size.x - sides) * (1 - halfvert), (size.y - tops) * (1 - halfhori) };
}

void Editor::SpriteEditor::EditorUI(void)
{
	EGUI::StartChild("Section Area", mSectionEditArea);
	if (mpTexture)
		AtlasArea(mSectionEditArea.x, mSectionEditArea.y);
	EGUI::EndChild();

	EGUI::Display::HorizontalSeparator();

	EGUI::StartChild("Settings Area", mSettingsArea);
	SettingsArea(mSettingsArea.x, mSettingsArea.y);
	EGUI::EndChild();

	ImGui::SameLine(0, 1);

	EGUI::StartChild("Preview Area", mPreviewArea);
	PreviewArea(mSettingsArea.x, mSettingsArea.y);
	EGUI::EndChild();

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

void Editor::SpriteEditor::AtlasArea(float _x, float _y)
{
	const auto fSize = AdjustAspectSize(static_cast<float>(mpTexture->GetWidth()),
										static_cast<float>(mpTexture->GetHeight()),
										_x - 1, _y - 1);

	float xDiff = _sX - _iX;
	float yDiff = _sY - _iY;
	return Math::Vec2{ xDiff / 2, yDiff / 2 };

	EGUI::Display::Image(mpTexture->GetID(), fSize);
}

void Editor::SpriteEditor::SettingsArea(float _x, float _y)
{
	EGUI::Display::EmptyBox("Sprite Sheet", 150, mpTexture ? mpTexture->GetName() : "-empty-");
	if (auto t = EGUI::Display::StartPayloadReceiver<Editor::File>(EGUI::ePayloadTags::ALL_IMG))
	{
		mpTexture = Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::GraphicsSystem>()->LoadTexture(t->mPath.c_str());
		EGUI::Display::EndPayloadReceiver();
	}
}

void Editor::SpriteEditor::PreviewArea(float _x, float _y)
{
}

Math::Vec2 Editor::SpriteEditor::AdjustAspectSize(float _imgX, float _imgY, float _canvasX, float _canvasY)
{
	const float iRatio = _imgX / _imgY;
	const float sRatio = _canvasX / _canvasY;
	return  sRatio > iRatio ? Math::Vec2{ _imgX * (_canvasY / _imgY), _canvasY }: 
							  Math::Vec2{ _canvasX, _imgY * (_canvasX / _imgX) };
}

#endif 

















