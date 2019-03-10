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
#include "Math/MathLib.h"

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
	mSectionDime{ 1, 1}
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
	static constexpr float halfY = 0.7f;
	static constexpr float halfX = 0.7f;
	const float sides = 5;
	const float tops = 3;
	auto size = Size();
	size.y -= EGUI::TabsImageOffsetY;

	mSectionEditArea = Math::Vec2{size.x - sides, (size.y - tops) * halfY };
	mSettingsArea = Math::Vec2{ (size.x- sides) * halfX, (size.y - tops) * (1 - halfY) };
	mPreviewArea = Math::Vec2{ (size.x - sides) * (1 - halfX), (size.y - tops) * (1 - halfY) };

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

const HashString& Editor::SpriteEditor::GetLabel(void) const
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

	DrawSelectedGrid(originPos.x, originPos.y, fSize.x, fSize.y);
	DrawTempGrid(originPos.x, originPos.y, fSize.x, fSize.y);
}

void Editor::SpriteEditor::SettingsArea(float, float)
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
	EGUI::Display::EmptyBox("Sprite Sheet", FIELD_SIZE, mpTexture ? mpTexture->GetName().c_str() : "-empty-");
	if (auto t = EGUI::Display::StartPayloadReceiver<Editor::File>(EGUI::ePayloadTags::ALL_IMG))
	{
		mpAtlas = nullptr;
		mpTexture = Dystopia::EngineCore::Get<Dystopia::GraphicsSystem>()->LoadTexture(t->mPath.c_str());
		if (mpTexture)
		{
			mpAtlas = Dystopia::EngineCore::Get<Dystopia::TextureSystem>()->GetAtlas(t->mName.c_str());
			if (!mpAtlas)
				mpAtlas = Dystopia::EngineCore::Get<Dystopia::TextureSystem>()->GenAtlas(mpTexture);

			mnSelectedSection = 0;
			if (!mpAtlas->GetAllSections().size())
				mpAtlas->AddSection(Math::Vec2{ 0,0 }, mpTexture->GetWidth(), mpTexture->GetHeight(), 1, 1);

			auto& sec = mpAtlas->GetAllSections()[mnSelectedSection];
			mSectionPos = Math::Vec2{ sec.uStart, sec.vStart };
			mSectionSize = Math::Vec2{ sec.uEnd - sec.uStart, sec.vEnd - sec.vStart };
			mSectionDime = Math::Vec2{ sec.mCol, sec.mRow };
		}
		EGUI::Display::EndPayloadReceiver();
	}

	if (!mpTexture) return;

	ImGui::SameLine();
	EGUI::Display::Label("Resolution:   < %d x >   < %d  y>", mpTexture->GetWidth(), mpTexture->GetHeight());
}

void Editor::SpriteEditor::FieldAtlas(void)
{
	if (!mpAtlas) return;

	static constexpr float itemWidth = 60.f;
	const auto& allSections = mpAtlas->GetAllSections();

	if (EGUI::Display::DropDownSelection<59>("Sections", mnSelectedSection, static_cast<unsigned>(allSections.size()), FIELD_SIZE))
	{
		auto& e = mpAtlas->GetAllSections()[mnSelectedSection];
		mSectionPos  = Math::Vec2{ e.uStart, e.vStart };
		mSectionSize = Math::Vec2{ e.uEnd - e.uStart, e.vEnd - e.vStart };
		mSectionDime = Math::Vec2{ roundf(mSectionSize.x / e.mCol), roundf(mSectionSize.y / e.mRow)};
		mSectionDime.x = Math::Clamp(static_cast<float>(mSectionDime.x), 0, 100);
		mSectionDime.y = Math::Clamp(static_cast<float>(mSectionDime.y), 0, 100);
	}
	else
	{
		mpAtlas->UpdateSection(mnSelectedSection,
			Math::Vec2{ mSectionPos.x * static_cast<float>(mpTexture->GetWidth()), mSectionPos.y * static_cast<float>(mpTexture->GetHeight()) },
			static_cast<unsigned>(mSectionSize.x * static_cast<float>(mpTexture->GetWidth())),
			static_cast<unsigned>(mSectionSize.y * static_cast<float>(mpTexture->GetHeight())),
			static_cast<unsigned>(mSectionDime.x), static_cast<unsigned>(mSectionDime.y)
		);
	}

	EGUI::Display::HorizontalSeparator();

	EGUI::Display::VectorFields("Start Pos", &mSectionPos, .01f, 0.f, 1, itemWidth);

	EGUI::Display::VectorFields("Size", &mSectionSize, 0.01f, 0.f, 1, itemWidth);
	EGUI::Display::VectorFieldsInt("Col & Row", &mSectionDime, 1, 1, 100, itemWidth);

	ImGui::SameLine(0.f, 5.f);
	if (EGUI::Display::Button("Grow Col", Math::Vec2{ 100, 24 }))
	{
		mSectionSize.x += mSectionSize.x / mSectionDime.x;
		mSectionDime.x += 1.f;
	}
	ImGui::SameLine(0.f, 5.f);
	if (EGUI::Display::Button("Grow Row", Math::Vec2{ 100, 24 }))
	{
		mSectionSize.y += mSectionSize.y / mSectionDime.y;
		mSectionDime.y += 1.f;
	}

	if (EGUI::Display::Button("Add New Section", Math::Vec2{150, 24}))
	{
		mnSelectedSection = mpAtlas->AddSection(Math::Vec2{ static_cast<float>(mpTexture->GetWidth()),static_cast<float>(mpTexture->GetHeight()) },
												mpTexture->GetWidth(), mpTexture->GetHeight(),
												static_cast<unsigned>(mSectionDime.x), static_cast<unsigned>(mSectionDime.y));
		//mnSelectedSection = mpAtlas->AddSection(Math::Vec2{mSectionPos.x * static_cast<float>(mpTexture->GetWidth()), mSectionPos.y * static_cast<float>(mpTexture->GetHeight()) },
		//										static_cast<unsigned>(mSectionSize.x * static_cast<float>(mpTexture->GetWidth())),
		//										static_cast<unsigned>(mSectionSize.y * static_cast<float>(mpTexture->GetHeight())),
		//										static_cast<unsigned>(mSectionDime.x), static_cast<unsigned>(mSectionDime.y));
	}
	if (EGUI::Display::Button("Save Atlas", Math::Vec2{ 150, 24 }))
	{
		if (mpTextSys)
			mpTextSys->SaveAtlases();
	}
}

void Editor::SpriteEditor::DrawSelectedGrid(float _ox, float _oy, float _ix, float _iy)
{
	//auto pCanvas = ImGui::GetWindowDrawList();
	//auto& section = mpAtlas->GetAllSections()[mnSelectedSection];
	//
	//ImGui::SetCursorPos(ImVec2{ _ox, _oy });
	//auto screenOrigin = ImGui::GetCursorScreenPos();
	//float ratioW = section.uEnd - section.uStart;
	//float ratioH = section.vEnd - section.vStart;
	//unsigned col = static_cast<unsigned>(ratioW / section.mCol);
	//unsigned row = static_cast<unsigned>(ratioH / section.mRow);
	//unsigned total = col * row;
	//float xLen = ratioW / col;
	//float yLen = ratioH / row;
	//
	//for (unsigned i = 0; i < total; ++i)
	//{
	//	Math::Vec2 rectMin{ screenOrigin.x + (((i % col) * xLen) * _ix) + (section.uStart * _ix),
	//						screenOrigin.y + (((i / col) * yLen) * _iy) + (section.vStart * _iy) };
	//	Math::Vec2 rectMax{ rectMin.x + (xLen * _ix),
	//						rectMin.y + (yLen * _iy) };
	//	//pCanvas->AddRect(rectMin, rectMax, ImGui::GetColorU32(ImVec4{ 1,1,1,1}));
	//	ImGui::SetCursorScreenPos(rectMin + ImVec2{ 2,1 });
	//	EGUI::Display::Label("%d", i);
	//	ImGui::SetCursorScreenPos(screenOrigin);
	//}

	//ImGui::GetWindowDrawList()->AddRect(rectMin, rectMax, ImGui::GetColorU32(ImVec4{ 1,1,1,1 }));
}

void Editor::SpriteEditor::DrawTempGrid(float _ox, float _oy, float _ix, float _iy)
{
	if (!mSectionPos.x && !mSectionPos.y && !mSectionSize.x && !mSectionSize.y)
		return;

	auto pCanvas = ImGui::GetWindowDrawList();
	static const ImVec4 red{ 1, 0, 0, 1 };

	ImGui::SetCursorPos(ImVec2{ _ox, _oy });
	auto screenOrigin = ImGui::GetCursorScreenPos();

	Math::Vec2 iStart{ screenOrigin.x + (_ix * mSectionPos.x), screenOrigin.y + (_iy * mSectionPos.y) };
	Math::Vec2 iEnd{ iStart.x + (_ix * mSectionSize.x), iStart.y + (_iy *  mSectionSize.y) };;

	unsigned col = static_cast<unsigned>(mSectionDime.x);
	unsigned row = static_cast<unsigned>(mSectionDime.y);
	col = Math::Clamp(col, 0, 100);
	row = Math::Clamp(row, 0, 100);
	unsigned total = col * row;
	float xLen = (iEnd.x - iStart.x) / col;
	float yLen = (iEnd.y - iStart.y) / row;

	ImGui::PushStyleColor(ImGuiCol_Text, red);
	for (unsigned i = 0; i < total; ++i)
	{
		Math::Vec2 rectMin{ screenOrigin.x + (_ix * mSectionPos.x) + (((i % col) * xLen)),
							screenOrigin.y + (_iy * mSectionPos.y) + (((i / col) * yLen)) };
		Math::Vec2 rectMax{ rectMin.x + xLen,
							rectMin.y + yLen };
		pCanvas->AddRect(rectMin, rectMax, ImGui::GetColorU32(red));
		ImGui::SetCursorScreenPos(rectMin + ImVec2{ 2,1 });
		EGUI::Display::Label("%d", i);
		ImGui::SetCursorScreenPos(screenOrigin);
	}
	ImGui::PopStyleColor();
}

#endif 

















