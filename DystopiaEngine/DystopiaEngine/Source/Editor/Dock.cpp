/* HEADER *********************************************************************************/
/*!
\file	Dock.cpp
\author Digipen (100%)
\par    email: t.shannon\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#include "Editor\Dock.h"
#include "../../Dependancies/ImGui/imgui_internal.h"

namespace Dystopia{
namespace EGUI{
namespace Dock{


DockContext::Dock::Dock()
	: mpNextTab{ nullptr }, mpPrevTab{ nullptr }, mpChildren{ nullptr, nullptr }, mpParent{ nullptr }, mId{ 0 },
	mPos{ ImVec2{ 0,0 } }, mSize{ ImVec2{ 0,0 } }, mFloatModeSize{ ImVec2{ 0,0 } }, mStatus{ eSTAT_FLOAT },
	mActive{ true }, mOpened{ false }, mFirst{ false }, mLastFrame{ 0 }, mInvalidFrame{ 0 }, mLocation{ { 0 } },
	mLabel{ nullptr }
{}

DockContext::Dock::~Dock()
{
	ImGui::MemFree(mLabel);
}

ImVec2 DockContext::Dock::GetMinSize() const
{
	if (!mpChildren[0]) return ImVec2(16, 16 + ImGui::GetTextLineHeightWithSpacing());

	ImVec2 s0 = mpChildren[0]->GetMinSize();
	ImVec2 s1 = mpChildren[1]->GetMinSize();
	return IsHorizontal() ? ImVec2(s0.x + s1.x, ImMax(s0.y, s1.y))
		: ImVec2(ImMax(s0.x, s1.x), s0.y + s1.y);
}

bool DockContext::Dock::IsHorizontal() const
{
	return mpChildren[0]->mPos.x < mpChildren[1]->mPos.x;
}

}}}

#endif // EDITOR



