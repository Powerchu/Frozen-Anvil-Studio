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

Dystopia::SpriteEditor* gpInstance = 0;
Dystopia::SpriteEditor* Dystopia::SpriteEditor::GetInstance(void)
{
	if (gpInstance) return gpInstance;

	gpInstance = new SpriteEditor{};
	return gpInstance;
}

Dystopia::SpriteEditor::SpriteEditor(void)
	: EditorTab{ false }, 
	mLabel{ "Sprite Editor" },
	mStartPt{ 0, 0 },
	mEndPt{ 0, 0 },
	mStartPlotting{ false },
	mSectionPlotted{ false }
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
	Math::Vec2 childSize = Size() - Math::Vec2{ 5, 30 };
	if (EGUI::StartChild("Sprite Editor Child", childSize))
	{
		if (mStartPlotting)
		{
			ImGuiContext& g = *GImGui;
			ImGuiWindow* window = g.CurrentWindow;
			ImVec2 pos = ImGui::GetCursorScreenPos();
			mStartPt.x = Math::Clamp(static_cast<int>(mStartPt.x), static_cast<int>(pos.x), static_cast<int>(childSize.x + pos.x));
			mStartPt.y = Math::Clamp(static_cast<int>(mStartPt.y), static_cast<int>(pos.y), static_cast<int>(childSize.y + pos.y));
			mEndPt.x = Math::Clamp(static_cast<int>(mEndPt.x), static_cast<int>(pos.x), static_cast<int>(childSize.x + pos.x));
			mEndPt.y = Math::Clamp(static_cast<int>(mEndPt.y), static_cast<int>(pos.y), static_cast<int>(childSize.y + pos.y));
			ImRect r{ mStartPt, mEndPt };

			r.Expand(1.f);
			bool push_clip_rect = !window->ClipRect.Contains(r);
			if (push_clip_rect) window->DrawList->PushClipRectFullScreen();
			window->DrawList->AddRect(r.Min, r.Max, ImGui::GetColorU32(ImGuiCol_DragDropTarget), 0.0f, ~0, 1.0f);
			if (push_clip_rect) window->DrawList->PopClipRect();

			ImGui::SetCursorScreenPos(pos);
		}
		if (!mSectionPlotted)
		{
			if (ImGui::IsMouseHoveringWindow())
			{
				if (!mStartPlotting && ImGui::IsMouseClicked(0))
				{
					mStartPlotting = true;
					mStartPt = ImGui::GetMousePos();
					PrintToConsoleLog("StartPlotting " + std::to_string(mStartPt.x) + " " + std::to_string(mStartPt.y));
				}
				else if (mStartPlotting && ImGui::IsMouseReleased(0))
				{
					mSectionPlotted = true;
					PrintToConsoleLog("Released " + std::to_string(mEndPt.x) + " " + std::to_string(mEndPt.y));
				}
				else if (mStartPlotting && ImGui::IsMouseDown(0))
				{
					PrintToConsoleLog("Held");
				}
				mEndPt = ImGui::GetMousePos();
			}
		}
	}
	EGUI::EndChild();
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
