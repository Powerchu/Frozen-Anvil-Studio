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
#include "Editor/EditorMain.h"

#include "Component/SpriteRenderer.h"
#include "System/Driver/Driver.h"
#include "System/Graphics/Texture2D.h"
#include "System/Graphics/TextureAtlas.h"
#include "System/Graphics/TextureSystem.h"
#include "System/Graphics/GraphicsSystem.h"

#define FIELD_SIZE 140

static constexpr Math::Vec2 g_pad = { 2, 2 };

Editor::SpriteEditor::SpriteEditor(void)
	: 
	mLabel{ "Sprite Editor" }, 
	mpAtlas{ nullptr },
	mpGfxSys{ nullptr },
	mpTextSys{ nullptr },
	mpTexture{ nullptr },
	mSectionEditArea{ 1.f, 0.7f},
	mSettingsArea{ 0.7f, 0.3f },
	mPreviewArea{ 0.3f, 0.3f },
	mnSelectedSection{ 0 },
	mSectionPos{ 0,0 },
	mSectionSize{ 0,0 },
	mSectionDime{ 0,0 }
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
	mSettingsArea -= g_pad;
	mPreviewArea -= g_pad;

	ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, ImVec4{ 0.2f, 0.2f, 0.2f, 1});
	EGUI::StartChild("Section Area", mSectionEditArea);
	if (mpTexture)
		AtlasArea(mSectionEditArea.x, mSectionEditArea.y);
	EGUI::EndChild();
	ImGui::PopStyleColor();

	EGUI::Display::HorizontalSeparator();

	EGUI::Indent(g_pad.x);

	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + g_pad.y);

	EGUI::StartChild("Settings Area", mSettingsArea);
	SettingsArea(mSettingsArea.x, mSettingsArea.y);
	EGUI::EndChild();

	ImGui::SameLine(0, 1);

	EGUI::StartChild("Preview Area", mPreviewArea);
	PreviewArea(mSettingsArea.x, mSettingsArea.y);
	EGUI::EndChild();

	EGUI::UnIndent(g_pad.x);

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
	_x -= 1;
	_y -= 1;
	const auto fSize = AdjustAspectSize(static_cast<float>(mpTexture->GetWidth()),
										static_cast<float>(mpTexture->GetHeight()),
										_x, _y);
	float xDiff = _x - fSize.x;
	float yDiff = _y - fSize.y;
	Math::Vec2 diff{ xDiff / 2, yDiff / 2};
	ImGui::SetCursorPos(ImGui::GetCursorPos() + diff);

	auto originPos = ImGui::GetCursorPos();
	EGUI::Display::Image(mpTexture->GetID(), fSize);

	if (!mpAtlas) return;

	DrawGrid(originPos.x, originPos.y, fSize.x, fSize.y);
}

void Editor::SpriteEditor::SettingsArea(float _x, float _y)
{
	EGUI::PushLeftAlign(80);
	FieldTexture();
	FieldAtlas();
	EGUI::PopLeftAlign();
}

void Editor::SpriteEditor::PreviewArea(float _x, float _y)
{
	_y -= (ImGui::GetTextLineHeightWithSpacing() + g_pad.y + 2);

	EGUI::Display::Label("Preview");

	ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, ImVec4{ 0.2f, 0.2f, 0.2f, 1 });

	EGUI::StartChild("Preview Anim", Math::Vec2{ _x, _y });

	EGUI::EndChild();

	ImGui::PopStyleColor();
}

Math::Vec2 Editor::SpriteEditor::AdjustAspectSize(float _imgX, float _imgY, float _canvasX, float _canvasY)
{
	_canvasX -= 1;
	_canvasY -= 1;
	const float iRatio = _imgX / _imgY;
	const float sRatio = _canvasX / _canvasY;
	return  sRatio > iRatio ? Math::Vec2{ _imgX * (_canvasY / _imgY), _canvasY }: 
							  Math::Vec2{ _canvasX, _imgY * (_canvasX / _imgX) };
}

void Editor::SpriteEditor::FieldTexture(void)
{
	EGUI::Display::EmptyBox("Sprite Sheet", FIELD_SIZE, mpTexture ? mpTexture->GetName() : "-empty-");
	if (auto t = EGUI::Display::StartPayloadReceiver<Editor::File>(EGUI::ePayloadTags::ALL_IMG))
	{
		mpTexture = Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::GraphicsSystem>()->LoadTexture(t->mPath.c_str());
		mpAtlas = Dystopia::EngineCore::GetInstance()->GetSubSystem<Dystopia::TextureSystem>()->GetAtlas(t->mPath.c_str());
		mpAtlas->SetTexture(mpTexture);
		mnSelectedSection = 0;
		if (!mpAtlas->GetAllSections().size())
			mpAtlas->AddSection(Math::Vec2{ 0,0 }, mpTexture->GetWidth(), mpTexture->GetHeight());
		EGUI::Display::EndPayloadReceiver();
	}
}

void Editor::SpriteEditor::FieldAtlas(void)
{
	if (!mpAtlas) return;

	static constexpr float itemWidth = 60.f;
	const auto& allSections = mpAtlas->GetAllSections();

	if (EGUI::Display::DropDownSelection<21>("Sections", mnSelectedSection, allSections.size(), FIELD_SIZE))
	{

	}

	EGUI::Display::HorizontalSeparator();

	EGUI::Display::VectorFields("Start Pos", &mSectionPos, .1f, 0.f, INT_MAX, itemWidth);
	EGUI::Display::VectorFieldsInt("Size", &mSectionSize, 1.f, 0.f, INT_MAX, itemWidth);
	EGUI::Display::VectorFieldsInt("Col & Row", &mSectionDime, 1.f, 0.f, INT_MAX, itemWidth);

	if (EGUI::Display::Button("Add New Section", Math::Vec2{150, 24}))
	{
	

	}
}

void Editor::SpriteEditor::DrawGrid(float _ox, float _oy, float _ix, float _y)
{
	ImGui::SetCursorPos(ImVec2{ _ox, _oy });

	auto screenOrigin = ImGui::GetCursorScreenPos();
	auto pCanvas = ImGui::GetWindowDrawList();
	auto screenEndPt = ImVec2{ screenOrigin.x + 10 , screenOrigin.y + 10};
	pCanvas->AddRect(screenOrigin, screenEndPt, ImGui::GetColorU32(ImVec4{ 1,1,1,1 }));
}

#endif 

















