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
#include "../../Dependancies/ImGui/imgui.h"
#include "../../Dependancies/ImGui/imgui_internal.h"
#include "Editor\EGUI.h"
#include "Editor\Dock.h"

namespace Dystopia {
namespace EGUI {
namespace Dock {

DockContext::Dock::Dock()
	: mpLabel{ nullptr }, mpPrevTab{ nullptr }, mpNextTab{ nullptr }, mpChildren{ nullptr, nullptr }, mpParent{ nullptr },
	mPos{ ImVec2{0,0} }, mSize{ ImVec2{ -1,-1 } }, mFloatmodeSize{ ImVec2{ 0,0 } }, mStatus{ eSTAT_FLOAT },
	mId{ 0 }, mLastFrame{0}, mInvalidFrame{ 0 }, mLocation{ 0 }, mActive{ true }, mOpened{ false }, mFirst{ false }
{}

DockContext::Dock::~Dock()
{
	ImGui::MemFree(mpLabel);
}

ImVec2 DockContext::Dock::GetMinSize() const
{
	if (!mpChildren[0])
		return ImVec2{16, 16 + ImGui::GetTextLineHeightWithSpacing()};
	
	ImVec2 s0 = mpChildren[0]->GetMinSize();
	ImVec2 s1 = mpChildren[1]->GetMinSize();

	return IsHorizontal() ? ImVec2{ s0.x + s1.x, ImMax(s0.y, s1.y) }
						  : ImVec2{ ImMax(s0.x, s1.x), s0.y + s1.y };
}

bool DockContext::Dock::IsHorizontal() const
{
	return mpChildren[0]->mPos.x < mpChildren[1]->mPos.x;
}

void DockContext::Dock::SetParent(Dock* _pDock)
{
	mpParent = _pDock;
	for (Dock *temp = mpPrevTab; temp; temp = temp->mpPrevTab) temp->mpParent = _pDock;
	for (Dock *temp = mpNextTab; temp; temp = temp->mpNextTab) temp->mpParent = _pDock;
}

DockContext::Dock& DockContext::Dock::GetRoot()
{
	Dock *pDock = this;
	while (pDock->mpParent)
		pDock = pDock->mpParent;
	return *pDock;
}

DockContext::Dock& DockContext::Dock::GetSibling()
{
	IM_ASSERT(mpParent);
	if (mpParent->mpChildren[0] == &GetFirstTab())
		return *mpParent->mpChildren[1];
	return *mpParent->mpChildren[0];
}

DockContext::Dock& DockContext::Dock::GetFirstTab()
{
	Dock* temp = this;
	while (temp->mpPrevTab)
		temp = temp->mpPrevTab;
	return *this;
}

void DockContext::Dock::SetActive()
{
	mActive = true;
	for (Dock* temp = mpPrevTab; temp; temp = temp->mpPrevTab) temp->mActive = false;
	for (Dock* temp = mpNextTab; temp; temp = temp->mpNextTab) temp->mActive = false;
}

bool DockContext::Dock::HasChildren() const
{
	return mpChildren[0] != nullptr;
}

void DockContext::Dock::SetChildPosSize(const ImVec2& _pos, const ImVec2& _size)
{
	ImVec2 s = mpChildren[0]->mSize;
	if (IsHorizontal())
	{
		s.y = _size.y;
		s.x = static_cast<float>( int(_size.x * mpChildren[0]->mSize.x 
			/ (mpChildren[0]->mSize.x + mpChildren[1]->mSize.x)));

		if (s.x < mpChildren[0]->GetMinSize().x)
			s.x = mpChildren[0]->GetMinSize().x;
		else if (_size.x - s.x < mpChildren[1]->GetMinSize().x)
			s.x = _size.x - mpChildren[1]->GetMinSize().x;

		mpChildren[0]->SetPosSize(_pos, s);
		s.x = _size.x - mpChildren[0]->mSize.x;
		ImVec2 p = _pos;
		p.x += mpChildren[0]->mSize.x;
		mpChildren[1]->SetPosSize(p, s);
	}
	else
	{
		s.x = _size.x;
		s.y = static_cast<float>(int(_size.y * mpChildren[0]->mSize.y / 
			(mpChildren[0]->mSize.y + mpChildren[1]->mSize.y)));

		if (s.y < mpChildren[0]->GetMinSize().y)
		{
			s.y = mpChildren[0]->GetMinSize().y;
		}
		else if (_size.y - s.y < mpChildren[1]->GetMinSize().y)
		{
			s.y = _size.y - mpChildren[1]->GetMinSize().y;
		}
		mpChildren[0]->SetPosSize(_pos, s);

		s.y = _size.y - mpChildren[0]->mSize.y;
		ImVec2 p = _pos;
		p.y += mpChildren[0]->mSize.y;
		mpChildren[1]->SetPosSize(p, s);
	}
}

void DockContext::Dock::SetPosSize(const ImVec2& _pos, const ImVec2& _size)
{
	mSize = _size;
	mPos = _pos;
	for (Dock* temp = mpPrevTab; temp; temp = temp->mpPrevTab)
	{
		temp->mSize = _size;
		temp->mPos = _pos;
	}
	for (Dock* temp = mpNextTab; temp; temp = temp->mpNextTab)
	{
		temp->mSize = _size;
		temp->mPos = _pos;
	}

	if (!HasChildren()) return;
	SetChildPosSize(_pos, _size);
}



}}} // namespace DYSTOPIA::EGUI::DOCK

#endif //EDITOR










