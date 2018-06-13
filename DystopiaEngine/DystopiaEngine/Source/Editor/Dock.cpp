/* HEADER *********************************************************************************/
/*!
\file	Tabs.cpp
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief 

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#include "Editor\Dock.h"
#include "DataStructure\AutoArray.h"
#include "../../DepEndancies/ImGui/imgui.h"
#include "../../DepEndancies/ImGui/imgui_internal.h"
#include <iostream>
#include <map>
#include <string>
/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/
/*================================================== NEW VERSION TESTING W-I-P ===================================================*/

ImVec2& operator+=(ImVec2& lhs, const ImVec2& rhs)
{
	lhs.x += rhs.x;
	lhs.y += rhs.y;
	return lhs;
}

ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs)
{
	ImVec2 temp = lhs;
	temp += rhs;
	return temp;
}

ImVec2& operator-=(ImVec2& lhs, const ImVec2& rhs)
{
	lhs.x -= rhs.x;
	lhs.y -= rhs.y;
	return lhs;
}

ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs)
{
	ImVec2 temp = lhs;
	temp -= rhs;
	return temp;
}

ImVec2 operator*(const ImVec2& lhs, const float& rhs)
{
	ImVec2 temp{lhs.x * rhs, lhs.y * rhs };
	return temp;
}

namespace EGUI2{
namespace Docking{
/*========================================================= Declaration ==========================================================*/
enum eEndAction
{
	eEND_ACTION_NONE,
	eEND_ACTION_PANEL,
	eEND_ACTION_END,
	eEND_ACTION_END_CHILD
};

enum eSTATUS
{
	eSTATUS_DOCKED,
	eSTATUS_FLOATING,
	eSTATUS_DRAGGED
};

struct Tabs
{
	Tabs();
	~Tabs();

	ImVec2	GetMinSize() const;
	bool	IsHorizontal() const;
	void	SetParent(Tabs*);
	Tabs&	GetRootTab();
	Tabs&	GetLeftSiblingTab();
	Tabs&	GetFirstTab();
	void	SetActive();
	bool	IsAContainer() const;
	void	SetChildPosSize(const ImVec2&, const ImVec2&);
	void	SetPosSize(const ImVec2&, const ImVec2&);
	void	SetChildPosSizeVertical(const ImVec2&, const ImVec2&);
	void	SetChildPosSizeHorizontal(const ImVec2&, const ImVec2&);

	char	*mpLabel;
	Tabs	*mpNextTab;
	Tabs	*mpPrevTab;
	Tabs	*mArrChildPtr[2];
	Tabs	*mpParentTab;
	int		mLastFrame;
	int		mInvalidFrames;
	char	mLocation[16];
	bool	mOpened;
	bool	mFirst;
	bool	mActive;
	ImU32	mId;
	ImVec2	mPos;
	ImVec2	mSize;
	eSTATUS mStatus;

};

struct DockSpace
{
	Tabs		*mCurrentTab;
	Tabs		*mpNextParentTab;
	ImVec2		mDragOffset;
	ImVec2		mPanelPos;
	ImVec2		mPanelSize;
	int			mLastFrame;
	eDockSlot	mNextSlot;
	eEndAction	mEndAction;
	AutoArray<Tabs*>	mArrTabs;

	DockSpace();
	~DockSpace();

	Tabs& GetTab(const char*, bool);
	Tabs* CreateNewTab(const char*, bool, ImU32);
	Tabs* GetTabAtMouse() const;
	Tabs* GetMainRootTab();

	bool TabCanSlot(Tabs&, Tabs*, const ImRect&, bool);
	bool TabBar(Tabs&, bool);
	bool Begin(const char*, bool*, ImGuiWindowFlags);

	void PlaceIntoBG();
	void SplitTabHorizontal(Tabs *, ImVec2&, ImVec2&, ImVec2&, ImVec2&, ImVec2&);
	void SplitTabVertical(Tabs *, ImVec2&, ImVec2&, ImVec2&, ImVec2&, ImVec2&);
	void SplitTabs();
	void CheckForNonExistentTab();
	void HandleDragging(Tabs&);
	void FillTabLocation(Tabs&);
	void UndockTab(Tabs&);
	void DrawTabBarListBtn(Tabs&);
	void TabBarDesign(Tabs&, Tabs *, ImDrawList *, const ImVec2&, const ImVec2&);
	void DockTab(Tabs&, Tabs*, eDockSlot);
	void SetRootTabPosSize(const ImVec2&, const ImVec2&);
	void TryDockTabToStoredLocation(Tabs&);
	void End();

	static ImRect GetTabedRect(const ImRect&, eDockSlot);
	static ImRect GetSlotRect(ImRect, eDockSlot);
	static ImRect GetSlotRectOnBorder(ImRect, eDockSlot);
	static eDockSlot GetSlotFromLocationCode(char);
	static char GetLocationCode(Tabs*);
	static void SetTabPosSize(Tabs&, Tabs&, eDockSlot, Tabs&);
};

/*========================================================= Definitions ==========================================================*/
Tabs::Tabs()
	: mpLabel{ nullptr }, mpNextTab{ nullptr }, mpPrevTab{ nullptr }, mArrChildPtr{ nullptr }, mpParentTab{ nullptr },
	mLastFrame{ 0 }, mInvalidFrames{ 0 }, mLocation{ 0 }, mOpened{ false }, mFirst{ true }, mActive{ true }, 
	mId{ 0 }, mPos{ ImVec2{ 0, 0 } }, mSize{ ImVec2{ 0, 0 } }, mStatus{ eSTATUS_FLOATING }
{
	mLocation[0] = 0;
	mArrChildPtr[0] = mArrChildPtr[1] = nullptr;
}

Tabs::~Tabs() 
{ 
	ImGui::MemFree(mpLabel); 
}

ImVec2 Tabs::GetMinSize() const
{
	if (!mArrChildPtr[0]) 
		return ImVec2{ 16, 16 + ImGui::GetTextLineHeightWithSpacing() };

	ImVec2 s0 = mArrChildPtr[0]->GetMinSize();
	ImVec2 s1 = mArrChildPtr[1]->GetMinSize();
	return IsHorizontal() ? ImVec2(s0.x + s1.x, ImMax(s0.y, s1.y))
						  : ImVec2(ImMax(s0.x, s1.x), s0.y + s1.y);
}

bool Tabs::IsHorizontal() const 
{ 
	return mArrChildPtr[0]->mPos.x < mArrChildPtr[1]->mPos.x;
}

void Tabs::SetParent(Tabs *_pTab)
{
	mpParentTab = _pTab;
	Tabs *pTempTab = mpPrevTab;
	for (;; pTempTab = pTempTab->mpPrevTab) 
		pTempTab->mpParentTab = _pTab;

	pTempTab = mpNextTab;
	for (;; pTempTab = pTempTab->mpNextTab) 
		pTempTab->mpParentTab = _pTab;
}

Tabs& Tabs::GetRootTab()
{
	Tabs *pTempTab = this;
	while (pTempTab->mpParentTab)
		pTempTab = pTempTab->mpParentTab;
	return *pTempTab;
}

Tabs& Tabs::GetLeftSiblingTab()
{
	IM_ASSERT(mpParentTab);
	if (mpParentTab->mArrChildPtr[0] == &GetFirstTab()) 
		return *mpParentTab->mArrChildPtr[1];
	return *mpParentTab->mArrChildPtr[0];
}

Tabs& Tabs::GetFirstTab()
{
	Tabs *pTempTab = this;
	while (pTempTab->mpPrevTab) 
		pTempTab = pTempTab->mpPrevTab;
	return *pTempTab;
}

void Tabs::SetActive()
{
	mActive = true;
	Tabs *pTempTab = mpPrevTab;
	for (;; pTempTab = pTempTab->mpPrevTab)
		pTempTab->mActive = false;

	pTempTab = mpNextTab;
	for (;; pTempTab = pTempTab->mpNextTab)
		pTempTab->mActive = false;
}

bool Tabs::IsAContainer() const
{ 
	return mArrChildPtr[0] != nullptr; 
}

void Tabs::SetChildPosSizeHorizontal(const ImVec2& _pos, const ImVec2& _size)
{
	float sumChildMinX = mArrChildPtr[0]->mSize.x + mArrChildPtr[1]->mSize.x;
	float scaleSizeX = _size.x * mArrChildPtr[0]->mSize.x;

	ImVec2 newSize = mArrChildPtr[0]->mSize;
	newSize.y = _size.y;
	newSize.x = static_cast<float>(static_cast<int>(scaleSizeX / sumChildMinX));

	// Clamp newSize x value
	if (newSize.x < mArrChildPtr[0]->GetMinSize().x)
		newSize.x = mArrChildPtr[0]->GetMinSize().x;
	else if (_size.x - newSize.x < mArrChildPtr[1]->GetMinSize().x)
		newSize.x = _size.x - mArrChildPtr[1]->GetMinSize().x;

	// Adjust child[0]
	mArrChildPtr[0]->SetPosSize(_pos, newSize);
	newSize.x = _size.x - mArrChildPtr[0]->mSize.x;

	// Adjust child[1] according to child[0]
	ImVec2 newPos = _pos;
	newPos.x += mArrChildPtr[0]->mSize.x;
	mArrChildPtr[1]->SetPosSize(newPos, newSize);
}

void Tabs::SetChildPosSizeVertical(const ImVec2& _pos, const ImVec2& _size)
{
	float sumChildMinY = mArrChildPtr[0]->mSize.y + mArrChildPtr[1]->mSize.y;
	float scaleSizeY = _size.y * mArrChildPtr[0]->mSize.y;

	ImVec2 newSize = mArrChildPtr[0]->mSize;
	newSize.x = _size.x;
	newSize.y = static_cast<float>(static_cast<int>(scaleSizeY / sumChildMinY));

	// Clamp newSize y value
	if (newSize.y < mArrChildPtr[0]->GetMinSize().y)
		newSize.y = mArrChildPtr[0]->GetMinSize().y;
	else if (_size.y - newSize.y < mArrChildPtr[1]->GetMinSize().y)
		newSize.y = _size.y - mArrChildPtr[1]->GetMinSize().y;

	// Adjust child[0] 
	mArrChildPtr[0]->SetPosSize(_pos, newSize);
	newSize.y = _size.y - mArrChildPtr[0]->mSize.y;

	// Adjust child[1] according to child[0]
	ImVec2 newPos = _pos;
	newPos.y += mArrChildPtr[0]->mSize.y;
	mArrChildPtr[1]->SetPosSize(newPos, newSize);
}

void Tabs::SetChildPosSize(const ImVec2& _pos, const ImVec2& _size)
{
	if (IsHorizontal())
		SetChildPosSizeHorizontal(_pos, _size);
	else
		SetChildPosSizeVertical(_pos, _size);
}

void Tabs::SetPosSize(const ImVec2& _pos, const ImVec2& _size)
{
	Tabs *pTempTab;
	mSize = _size;
	mPos = _pos;

	pTempTab = mpPrevTab;
	while (pTempTab)
	{
		pTempTab->mSize = _size;
		pTempTab->mPos = _pos;
		pTempTab = pTempTab->mpPrevTab;
	}

	pTempTab = mpNextTab;
	while (pTempTab)
	{
		pTempTab->mSize = _size;
		pTempTab->mPos = _pos;
		pTempTab = pTempTab->mpNextTab;
	}

	if (!IsAContainer()) return;

	SetChildPosSize(_pos, _size);
}

DockSpace::DockSpace()
	: mCurrentTab{ nullptr }, mpNextParentTab{ nullptr }, mDragOffset{ ImVec2{0,0} }, mPanelPos{ ImVec2{0,0} }, 
	mPanelSize{ ImVec2 {0,0} }, mLastFrame{ 0 }, mNextSlot{ eDOCK_SLOT_TAB }, mEndAction{ eEND_ACTION_NONE }, mArrTabs{}
{
}

DockSpace::~DockSpace() 
{
}

Tabs* DockSpace::CreateNewTab(const char *_label, bool _opened, ImU32 _id)
{
	Tabs *pNewTab = static_cast<Tabs*>(ImGui::MemAlloc(sizeof(Tabs)));
	IM_PLACEMENT_NEW(pNewTab) Tabs();
	mArrTabs.push_back(pNewTab);
	pNewTab->mpLabel = ImStrdup(_label);
	IM_ASSERT(pNewTab->mpLabel);
	pNewTab->mId = _id;
	pNewTab->SetActive();
	pNewTab->mStatus = (mArrTabs.size() == 1) ? eSTATUS_DOCKED : eSTATUS_FLOATING;
	pNewTab->mPos = ImVec2{ 0, 0 };
	pNewTab->mSize = ImGui::GetIO().DisplaySize;
	pNewTab->mOpened = _opened;
	pNewTab->mFirst = true;
	pNewTab->mLastFrame = 0;
	pNewTab->mInvalidFrames = 0;
	pNewTab->mLocation[0] = 0;
	return pNewTab;
}

Tabs& DockSpace::GetTab(const char *_label, bool _opened)
{
	ImU32 id = ImHash(_label, 0);
	for (unsigned int i = 0; i < mArrTabs.size(); ++i)
	{
		if (mArrTabs[i]->mId == id) return *mArrTabs[i];
	}

	return *CreateNewTab(_label, _opened, id);
}

void DockSpace::PlaceIntoBG()
{
	ImGuiWindow *pWin = ImGui::GetCurrentWindow();
	ImGuiContext& ctx = *GImGui;
	if (ctx.Windows[0] == pWin) return;

	for (int i = 0; i < ctx.Windows.Size; i++)
	{
		if (ctx.Windows[i] == pWin)
		{
			for (int j = i - 1; j >= 0; --j)
				ctx.Windows[j + 1] = ctx.Windows[j];
			ctx.Windows[0] = pWin;
			break;
		}
	}
}

void DockSpace::SplitTabHorizontal(Tabs *_pTab, ImVec2& _oDSize, ImVec2& _oPos0, ImVec2& _oPos1, ImVec2& _oSize0, ImVec2& _oSize1)
{
	ImVec2 min_size0 = _pTab->mArrChildPtr[0]->GetMinSize();
	ImVec2 min_size1 = _pTab->mArrChildPtr[1]->GetMinSize();

	ImGui::SetCursorScreenPos(ImVec2(_pTab->mPos.x + _oSize0.x, _pTab->mPos.y));
	ImGui::InvisibleButton("split", ImVec2(3, _pTab->mSize.y));
	if (_pTab->mStatus == eSTATUS_DRAGGED)
		_oDSize.x = ImGui::GetIO().MouseDelta.x;
	_oDSize.x = -ImMin(-_oDSize.x, _pTab->mArrChildPtr[0]->mSize.x - min_size0.x);
	_oDSize.x = ImMin(_oDSize.x, _pTab->mArrChildPtr[1]->mSize.x - min_size1.x);
	_oSize0 += _oDSize;
	_oSize1 -= _oDSize;
	_oPos0 = _pTab->mPos;
	_oPos1.x = _oPos0.x + _oSize0.x;
	_oPos1.y = _pTab->mPos.y;
	_oSize0.y = _oSize1.y = _pTab->mSize.y;
	_oSize1.x = ImMax(min_size1.x, _pTab->mSize.x - _oSize0.x);
	_oSize0.x = ImMax(min_size0.x, _pTab->mSize.x - _oSize1.x);
}

void DockSpace::SplitTabVertical(Tabs *_pTab, ImVec2& _oDSize, ImVec2& _oPos0, ImVec2& _oPos1, ImVec2& _oSize0, ImVec2& _oSize1)
{
	ImVec2 min_size0 = _pTab->mArrChildPtr[0]->GetMinSize();
	ImVec2 min_size1 = _pTab->mArrChildPtr[1]->GetMinSize();
	ImGui::SetCursorScreenPos(ImVec2(_pTab->mPos.x, _pTab->mPos.y + _oSize0.y - 3));
	ImGui::InvisibleButton("split", ImVec2(_pTab->mSize.x, 3));
	if (_pTab->mStatus == eSTATUS_DRAGGED)
		_oDSize.y = ImGui::GetIO().MouseDelta.y;
	_oDSize.y = -ImMin(-_oDSize.y, _pTab->mArrChildPtr[0]->mSize.y - min_size0.y);
	_oDSize.y = ImMin(_oDSize.y, _pTab->mArrChildPtr[1]->mSize.y - min_size1.y);
	_oSize0 += _oDSize;
	_oSize1 -= _oDSize;
	_oPos0 = _pTab->mPos;
	_oPos1.x = _pTab->mPos.x;
	_oPos1.y = _oPos0.y + _oSize0.y;
	_oSize0.x = _oSize1.x = _pTab->mSize.x;
	_oSize1.y = ImMax(min_size1.y, _pTab->mSize.y - _oSize0.y);
	_oSize0.y = ImMax(min_size0.y, _pTab->mSize.y - _oSize1.y);
}

void DockSpace::SplitTabs()
{
	if (ImGui::GetFrameCount() == mLastFrame) return;

	mLastFrame = ImGui::GetFrameCount();
	PlaceIntoBG();
	for (auto e : mArrTabs)
	{
		if (e->mpParentTab && (e->mStatus == eSTATUS_DOCKED))
			e->SetPosSize(mPanelPos, mPanelSize);
	}

	ImDrawList* pCanvas = ImGui::GetWindowDrawList();
	for (unsigned int i = 0; i < mArrTabs.size(); ++i)
	{
		Tabs *pTab = mArrTabs[i];
		if (!pTab->IsAContainer()) continue;

		ImGui::PushID(i);
		if (!ImGui::IsMouseDown(0)) pTab->mStatus = eSTATUS_DOCKED;

		ImGuiMouseCursor cursor;
		ImVec2 pos0 = pTab->mArrChildPtr[0]->mPos;
		ImVec2 pos1 = pTab->mArrChildPtr[1]->mPos;
		ImVec2 size0 = pTab->mArrChildPtr[0]->mSize;
		ImVec2 size1 = pTab->mArrChildPtr[1]->mSize;
		ImVec2 dsize{ 0, 0};

		if (pTab->IsHorizontal())
		{
			cursor = ImGuiMouseCursor_ResizeEW;
			SplitTabHorizontal(pTab, dsize, pos0, pos1, size0, size1);
		}
		else
		{
			cursor = ImGuiMouseCursor_ResizeNS;
			SplitTabHorizontal(pTab, dsize, pos0, pos1, size0, size1);
		}
		pTab->mArrChildPtr[0]->SetPosSize(pos0, size0);
		pTab->mArrChildPtr[1]->SetPosSize(pos1, size1);

		if (ImGui::IsItemHovered())
		{
			ImGui::SetMouseCursor(cursor);
			if (ImGui::IsMouseClicked(0))
				pTab->mStatus = eSTATUS_DRAGGED;
		}

		pCanvas->AddRectFilled(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), 
							   ImGui::IsItemHovered() 
							   ? ImGui::GetColorU32(ImGuiCol_ButtonHovered) 
							   : ImGui::GetColorU32(ImGuiCol_Button));
		ImGui::PopID();
	}
}

void DockSpace::CheckForNonExistentTab()
{
	int frame_limit = ImMax(0, ImGui::GetFrameCount() - 2);
	for (unsigned int i = 0; i < mArrTabs.size(); ++i)
	{
		Tabs *pTab = mArrTabs[i];
		if (pTab->IsAContainer()) continue;
		if (pTab->mStatus == eSTATUS_FLOATING) continue;
		if (pTab->mLastFrame < frame_limit)
		{
			++pTab->mInvalidFrames;
			if (pTab->mInvalidFrames > 2)
			{
				UndockTab(*pTab);
				pTab->mStatus = eSTATUS_FLOATING;
			}
			return;
		}
		pTab->mInvalidFrames = 0;
	}
}

Tabs* DockSpace::GetTabAtMouse() const
{
	for (unsigned int i = 0; i < mArrTabs.size(); ++i)
	{
		Tabs *pTab = mArrTabs[i];
		if (pTab->IsAContainer()) continue;
		if (pTab->mStatus != eSTATUS_DOCKED) continue;
		if (ImGui::IsMouseHoveringRect(pTab->mPos, pTab->mPos + pTab->mSize, false))
			return pTab;
	}
	return nullptr;
}

ImRect DockSpace::GetTabedRect(const ImRect& _rect, eDockSlot _slot)
{
	ImVec2 halfRect = _rect.GetSize() * 0.5f;
	switch (_slot)
	{
	default: return _rect;
	case eDOCK_SLOT_TOP: 
		return ImRect{ _rect.Min, ImVec2{ _rect.Max.x, _rect.Min.y + halfRect.y } };
	case eDOCK_SLOT_RIGHT: 
		return ImRect{ _rect.Min + ImVec2{ halfRect.x, 0 }, _rect.Max };
	case eDOCK_SLOT_BOTTOM: 
		return ImRect{ _rect.Min + ImVec2{ 0, halfRect.y }, _rect.Max };
	case eDOCK_SLOT_LEFT: 
		return ImRect{ _rect.Min, ImVec2{ _rect.Min.x + halfRect.x, _rect.Max.y } };
	}
}

ImRect DockSpace::GetSlotRect(ImRect _parentR, eDockSlot _slot)
{
	ImVec2 size = _parentR.Max - _parentR.Min;
	ImVec2 center = _parentR.Min + size * 0.5f;
	switch (_slot)
	{
	default: 
		return ImRect{ center - ImVec2{ 20, 20 }, center + ImVec2{ 20, 20 } };
	case eDOCK_SLOT_TOP:
		return ImRect{ center + ImVec2{ -20, -50 }, center + ImVec2{ 20, -30 } };
	case eDOCK_SLOT_RIGHT:
		return ImRect{ center + ImVec2{ 30, -20 }, center + ImVec2{ 50, 20 } };
	case eDOCK_SLOT_BOTTOM:
		return ImRect{ center + ImVec2{ -20, +30 }, center + ImVec2{ 20, 50 } };
	case eDOCK_SLOT_LEFT:
		return ImRect{ center + ImVec2{ -50, -20 }, center + ImVec2{ -30, 20 } };
	}
}

ImRect DockSpace::GetSlotRectOnBorder(ImRect _parentR, eDockSlot _slot)
{
	ImVec2 size = _parentR.Max - _parentR.Min;
	ImVec2 center = _parentR.Min + size * 0.5f;
	switch (_slot)
	{
	case eDOCK_SLOT_TOP:
		return ImRect{ ImVec2{ center.x - 20, _parentR.Min.y + 10 }, ImVec2{ center.x + 20, _parentR.Min.y + 30 } };
	case eDOCK_SLOT_LEFT:
		return ImRect{ ImVec2{ _parentR.Min.x + 10, center.y - 20 }, ImVec2{ _parentR.Min.x + 30, center.y + 20 } };
	case eDOCK_SLOT_BOTTOM:
		return ImRect{ ImVec2{ center.x - 20, _parentR.Max.y - 30 }, ImVec2{ center.x + 20, _parentR.Max.y - 10 } };
	case eDOCK_SLOT_RIGHT:
		return ImRect{ ImVec2{ _parentR.Max.x - 30, center.y - 20 }, ImVec2{ _parentR.Max.x - 10, center.y + 20 } };
	default: IM_ASSERT(false);
	}
	IM_ASSERT(false);
	return ImRect{};
}

Tabs* DockSpace::GetMainRootTab()
{
	for (unsigned int i = 0; i < mArrTabs.size(); ++i)
	{
		if (!mArrTabs[i]->mpParentTab)
			if (mArrTabs[i]->mStatus == eSTATUS_DOCKED || mArrTabs[i]->mArrChildPtr[0])
				return mArrTabs[i];
	}
	return nullptr;
}

bool DockSpace::TabCanSlot(Tabs& _tab, Tabs *_pDestTab, const ImRect& _rect, bool _onBorder)
{
	ImDrawList *pCanvas = ImGui::GetWindowDrawList();
	ImVec2 mousePos = ImGui::GetIO().MousePos;
	for (int i = 0; i < (_onBorder ? 4 : 5); ++i)
	{
		ImRect rect = _onBorder ? GetSlotRectOnBorder(rect, static_cast<eDockSlot>(i))
								: GetSlotRect(rect, static_cast<eDockSlot>(i));
		bool hovered = rect.Contains(mousePos);
		pCanvas->AddRectFilled(rect.Min, rect.Max, 
								hovered ? ImGui::GetColorU32(ImGuiCol_ButtonHovered) 
										: ImGui::GetColorU32(ImGuiCol_Button));
		if (!hovered) continue;
		if (!ImGui::IsMouseDown(0))
		{
			DockTab(_tab, _pDestTab ? _pDestTab : GetMainRootTab(), (eDockSlot)i);
			return true;
		}
		ImRect docked_rect = GetTabedRect(rect, (eDockSlot)i);
		pCanvas->AddRectFilled(docked_rect.Min, docked_rect.Max, ImGui::GetColorU32(ImGuiCol_Button));
	}
	return false;
}

void DockSpace::HandleDragging(Tabs& _tab)
{
	ImGuiWindowFlags flags = ImGuiWindowFlags_Tooltip | ImGuiWindowFlags_NoTitleBar | 
							 ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | 
							 ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize;

	ImGui::SetNextWindowPos(ImVec2{ 0,0 });
	ImGui::SetNextWindowBgAlpha(0.f);
	ImGui::Begin("##Overlay", nullptr, flags);

	ImU32 dockedCol = (ImGui::GetColorU32(ImGuiCol_FrameBg) & 0x00ffFFFF) | 0x80000000;
	ImDrawList *pCanvas = ImGui::GetWindowDrawList();
	pCanvas->PushClipRectFullScreen();

	Tabs *pDestTab = GetTabAtMouse();

	_tab.mPos = ImGui::GetIO().MousePos - mDragOffset;
	if (pDestTab)
	{
		if (TabCanSlot(_tab, pDestTab, ImRect{ pDestTab->mPos, pDestTab->mPos + pDestTab->mSize },false))
		{
			pCanvas->PopClipRect();
			ImGui::End();
			return;
		}
	}
	if (TabCanSlot(_tab, nullptr, ImRect(mPanelPos, mPanelPos + mPanelSize), true))
	{
		pCanvas->PopClipRect();
		ImGui::End();
		return;
	}
	pCanvas->AddRectFilled(_tab.mPos, _tab.mPos + _tab.mSize, dockedCol);
	pCanvas->PopClipRect();

	if (!ImGui::IsMouseDown(0))
	{
		_tab.mStatus = eSTATUS_FLOATING;
		_tab.mLocation[0] = 0;
		_tab.SetActive();
	}
	ImGui::End();
}

void DockSpace::FillTabLocation(Tabs& _tab)
{
	if (_tab.mStatus == eSTATUS_FLOATING) return;
	char *c = _tab.mLocation;
	Tabs *pTmp = &_tab;
	while (pTmp->mpParentTab)
	{
		*c = GetLocationCode(pTmp);
		pTmp = pTmp->mpParentTab;
		++c;
	}
	*c = 0;
}

void DockSpace::UndockTab(Tabs& _tab)
{
	if (_tab.mpPrevTab)
		_tab.mpPrevTab->SetActive();
	else if (_tab.mpNextTab)
		_tab.mpNextTab->SetActive();
	else
		_tab.mActive = false;
	Tabs *pHolder = _tab.mpParentTab;

	if (pHolder)
	{
		Tabs& sibling = _tab.GetLeftSiblingTab();

		if (pHolder->mArrChildPtr[0] == &_tab)
			pHolder->mArrChildPtr[0] = _tab.mpNextTab;
		else if (pHolder->mArrChildPtr[1] == &_tab)
			pHolder->mArrChildPtr[1] = _tab.mpNextTab;

		bool toRemove = !pHolder->mArrChildPtr[0] || !pHolder->mArrChildPtr[1];
		if (toRemove)
		{
			if (pHolder->mpParentTab)
			{
				Tabs*& child = pHolder->mpParentTab->mArrChildPtr[0] == pHolder
					? pHolder->mpParentTab->mArrChildPtr[0]
					: pHolder->mpParentTab->mArrChildPtr[1];
				child = &sibling;
				child->SetPosSize(pHolder->mPos, pHolder->mSize);
				child->SetParent(pHolder->mpParentTab);
			}
			else
			{
				if (pHolder->mArrChildPtr[0])
				{
					pHolder->mArrChildPtr[0]->SetParent(nullptr);
					pHolder->mArrChildPtr[0]->SetPosSize(pHolder->mPos, pHolder->mSize);
				}
				if (pHolder->mArrChildPtr[1])
				{
					pHolder->mArrChildPtr[1]->SetParent(nullptr);
					pHolder->mArrChildPtr[1]->SetPosSize(pHolder->mPos, pHolder->mSize);
				}
			}
			for (unsigned int i = 0; i < mArrTabs.size(); ++i)
			{
				if (mArrTabs[i] == pHolder)
				{
					mArrTabs.FastRemove(i);
					break;
				}
			}
			if (pHolder == mpNextParentTab)
				mpNextParentTab = nullptr;
			pHolder->~Tabs();
			ImGui::MemFree(pHolder);
		}
	}
	if (_tab.mpPrevTab) _tab.mpPrevTab->mpNextTab = _tab.mpNextTab;
	if (_tab.mpNextTab) _tab.mpNextTab->mpPrevTab = _tab.mpPrevTab;
	_tab.mpParentTab = nullptr;
	_tab.mpPrevTab = _tab.mpNextTab = nullptr;
}

void DockSpace::DrawTabBarListBtn(Tabs& _tab)
{
	if (!_tab.mpNextTab) return;

	ImDrawList* pCanvas = ImGui::GetWindowDrawList();

	if (ImGui::InvisibleButton("list", ImVec2(16, 16)))
		ImGui::OpenPopup("tab_list_popup");

	if (ImGui::BeginPopup("tab_list_popup"))
	{
		Tabs *pTmp = &_tab;
		while (pTmp)
		{
			bool dummy = false;
			if (ImGui::Selectable(pTmp->mpLabel, &dummy))
			{
				pTmp->SetActive();
				mpNextParentTab = pTmp;
			}
			pTmp = pTmp->mpNextTab;
		}
		ImGui::EndPopup();
	}

	ImVec2 min = ImGui::GetItemRectMin();
	ImVec2 max = ImGui::GetItemRectMax();
	ImVec2 center = (min + max) * 0.5f;
	pCanvas->AddRectFilled(ImVec2(center.x - 4, min.y + 3), ImVec2(center.x + 4, min.y + 5),
						   ImGui::IsItemHovered() ? ImGui::GetColorU32(ImGuiCol_FrameBgActive) : ImGui::GetColorU32(ImGuiCol_Text));

	pCanvas->AddTriangleFilled(ImVec2(center.x - 4, min.y + 7), ImVec2(center.x + 4, min.y + 7), ImVec2(center.x, min.y + 12),
							   ImGui::IsItemHovered() ? ImGui::GetColorU32(ImGuiCol_FrameBgActive) : ImGui::GetColorU32(ImGuiCol_Text));
}

void DockSpace::TabBarDesign(Tabs& _tab, Tabs *_pDockTab, ImDrawList *_pCanvas, const ImVec2& _pos, const ImVec2& _size)
{
	_pCanvas->PathClear();
	_pCanvas->PathLineTo(_pos + ImVec2(-15, _size.y));
	_pCanvas->PathBezierCurveTo(_pos + ImVec2(-10, _size.y),
								_pos + ImVec2(-5, 0),
								_pos + ImVec2{ 0, 0 }, 10);
	_pCanvas->PathLineTo(_pos + ImVec2(_size.x, 0));
	_pCanvas->PathBezierCurveTo(_pos + ImVec2(_size.x + 5, 0),
								_pos + ImVec2(_size.x + 10, _size.y),
								_pos + ImVec2(_size.x + 15, _size.y),
								10);
	_pCanvas->PathFillConvex(ImGui::IsItemHovered() ? ImGui::GetColorU32(ImGuiCol_FrameBgHovered) 
												    : (_pDockTab->mActive ? ImGui::GetColorU32(ImGuiCol_FrameBgActive)
																		  : ImGui::GetColorU32(ImGuiCol_FrameBg)));
	_pCanvas->AddText(_pos + ImVec2(0, 1), ImGui::GetColorU32(ImGuiCol_Text), _pDockTab->mpLabel, ImGui::FindRenderedTextEnd(_pDockTab->mpLabel));
}

bool DockSpace::TabBar(Tabs& _tab, bool _closeBtn)
{
	float tabBarHeight = 2 * ImGui::GetTextLineHeightWithSpacing();
	ImVec2 size(_tab.mSize.x, tabBarHeight);
	bool tabClosed = false;
	char buffer[20];

	ImGui::SetCursorScreenPos(_tab.mPos);
	ImFormatString(buffer, IM_ARRAYSIZE(buffer), "tabs%d", static_cast<int>(_tab.mId));

	if (ImGui::BeginChild(buffer, size, true))
	{
		Tabs *pDockTab = &_tab;
		ImDrawList *pCanvas = ImGui::GetWindowDrawList();
		ImU32 textCol = ImGui::GetColorU32(ImGuiCol_Text);
		float lineH = ImGui::GetTextLineHeightWithSpacing();
		float tabBase;

		DrawTabBarListBtn(_tab);

		while (pDockTab)
		{
			ImGui::SameLine(0, 15);
			const char *pTextEnd = ImGui::FindRenderedTextEnd(pDockTab->mpLabel);
			ImVec2 size(ImGui::CalcTextSize(pDockTab->mpLabel, pTextEnd).x, lineH);

			if (ImGui::InvisibleButton(pDockTab->mpLabel, size))
			{
				pDockTab->SetActive();
				mpNextParentTab = pDockTab;
			}

			if (ImGui::IsItemActive() && ImGui::IsMouseDragging())
			{
				mDragOffset = ImGui::GetMousePos() - pDockTab->mPos;
				UndockTab(*pDockTab);
				pDockTab->mStatus = eSTATUS_DRAGGED;
			}

			bool hovered = ImGui::IsItemHovered();
			ImVec2 pos = ImGui::GetItemRectMin();
			if (pDockTab->mActive && _closeBtn)
			{
				size.x += 16 + ImGui::GetStyle().ItemSpacing.x;
				ImGui::SameLine();
				tabClosed = ImGui::InvisibleButton("close", ImVec2(16, 16));
				ImVec2 center = (ImGui::GetItemRectMin() + ImGui::GetItemRectMax()) * 0.5f;
				pCanvas->AddLine(
					center + ImVec2(-3.5f, -3.5f), center + ImVec2(3.5f, 3.5f), textCol);
				pCanvas->AddLine(
					center + ImVec2(3.5f, -3.5f), center + ImVec2(-3.5f, 3.5f), textCol);
			}
			tabBase = pos.y;
			TabBarDesign(_tab, pDockTab, pCanvas, pos, size);
			pDockTab = pDockTab->mpNextTab;
		}		
		ImVec2 cp(_tab.mPos.x, tabBase + lineH);
		pCanvas->AddLine(cp, cp + ImVec2(_tab.mSize.x, 0), ImGui::GetColorU32(ImGuiCol_FrameBg));
	}
	ImGui::EndChild();
	return tabClosed;
}

void DockSpace::SetTabPosSize(Tabs& _destTab, Tabs& _tab, eDockSlot _slot, Tabs& _holder)
{
	IM_ASSERT(!_tab.mpPrevTab && !_tab.mpNextTab && !_tab.mArrChildPtr[0] && !_tab.mArrChildPtr[1]);

	_destTab.mPos = _tab.mPos = _holder.mPos;
	_destTab.mSize = _tab.mSize = _holder.mSize;
	switch (_slot)
	{
	case eDOCK_SLOT_BOTTOM:
		_destTab.mSize.y *= 0.5f;
		_tab.mSize.y *= 0.5f;
		_tab.mPos.y += _destTab.mSize.y;
		break;
	case eDOCK_SLOT_RIGHT:
		_destTab.mSize.x *= 0.5f;
		_tab.mSize.x *= 0.5f;
		_tab.mPos.x += _destTab.mSize.x;
		break;
	case eDOCK_SLOT_LEFT:
		_destTab.mSize.x *= 0.5f;
		_tab.mSize.x *= 0.5f;
		_destTab.mPos.x += _tab.mSize.x;
		break;
	case eDOCK_SLOT_TOP:
		_destTab.mSize.y *= 0.5f;
		_tab.mSize.y *= 0.5f;
		_destTab.mPos.y += _tab.mSize.y;
		break;
	default: 
		IM_ASSERT(false); 
		break;
	}
	_destTab.SetPosSize(_destTab.mPos, _destTab.mSize);

	if (_holder.mArrChildPtr[1]->mPos.x < _holder.mArrChildPtr[0]->mPos.x ||
		_holder.mArrChildPtr[1]->mPos.y < _holder.mArrChildPtr[0]->mPos.y)
	{
		Tabs *pTmp = _holder.mArrChildPtr[0];
		_holder.mArrChildPtr[0] = _holder.mArrChildPtr[1];
		_holder.mArrChildPtr[1] = pTmp;
	}
}

void DockSpace::DockTab(Tabs& _tab, Tabs* _pDestTab, eDockSlot _slot)
{
	IM_ASSERT(!_tab.mpParentTab);
	if (!_pDestTab)
	{
		_tab.mStatus = eSTATUS_DOCKED;
		_tab.SetPosSize(mPanelPos, mPanelSize);
	}
	else if (_slot == eDOCK_SLOT_TAB)
	{
		Tabs *pTmp = _pDestTab;
		while (pTmp->mpNextTab)	pTmp = pTmp->mpNextTab;

		auto isInLinkList = [](const Tabs *pList, const Tabs *pCheckTarget)
		{
			bool isLinkNode = (pList == pCheckTarget);

			const Tabs *pTmpTraverse = pList;
			while (!isLinkNode  && pTmpTraverse->mpPrevTab)
			{
				pTmpTraverse = pTmpTraverse->mpPrevTab;
				isLinkNode = (pTmpTraverse == pCheckTarget);
			}

			pTmpTraverse = pList;
			while (!isLinkNode && pTmpTraverse->mpNextTab)
			{
				pTmpTraverse = pTmpTraverse->mpNextTab;
				isLinkNode = (pTmpTraverse == pCheckTarget);
			}
			return isLinkNode;
		};

		if (!isInLinkList(_pDestTab, &_tab))
		{
			pTmp->mpNextTab = &_tab;
			_tab.mpPrevTab = pTmp;
			_tab.mSize = pTmp->mSize;
			_tab.mPos = pTmp->mPos;
			_tab.mpParentTab = _pDestTab->mpParentTab;
			_tab.mStatus = eSTATUS_DOCKED;
		}
	}
	else if (_slot == eDOCK_SLOT_NONE)
		_tab.mStatus = eSTATUS_FLOATING;
	else
	{
		Tabs* container = static_cast<Tabs*>(ImGui::MemAlloc(sizeof(Tabs)));
		IM_PLACEMENT_NEW(container) Tabs();
		mArrTabs.push_back(container);
		container->mArrChildPtr[0] = &_pDestTab->GetFirstTab();
		container->mArrChildPtr[1] = &_tab;
		container->mpNextTab = nullptr;
		container->mpPrevTab = nullptr;
		container->mpParentTab = _pDestTab->mpParentTab;
		container->mSize = _pDestTab->mSize;
		container->mPos = _pDestTab->mPos;
		container->mStatus = eSTATUS_DOCKED;
		container->mpLabel = ImStrdup("");

		if (!_pDestTab->mpParentTab)
		{
		}
		else if (&_pDestTab->GetFirstTab() == _pDestTab->mpParentTab->mArrChildPtr[0])
			_pDestTab->mpParentTab->mArrChildPtr[0] = container;
		else
			_pDestTab->mpParentTab->mArrChildPtr[1] = container;

		_pDestTab->SetParent(container);
		_tab.mpParentTab = container;
		_tab.mStatus = eSTATUS_DOCKED;

		SetTabPosSize(*_pDestTab, _tab, _slot, *container);
	}
	_tab.SetActive();
}

/*//////////////////////////////////////////////////////////////////////// STOPPED HERE /////////////////////////////////////////////////////////////////////////////////////////*/

void DockSpace::SetRootTabPosSize(const ImVec2& pos, const ImVec2& size)
{
	Tabs* root = GetMainRootTab();
	if (!root) return;

	ImVec2 min_size = root->GetMinSize();
	ImVec2 requested_size = size;
	root->SetPosSize(pos, ImMax(min_size, requested_size));
}

eDockSlot DockSpace::GetSlotFromLocationCode(char code)
{
	switch (code)
	{
	case '1': return eDockSlot_Left;
	case '2': return eDockSlot_Top;
	case '3': return eDockSlot_Bottom;
	default: return eDockSlot_Right;
	}
}

char DockSpace::GetLocationCode(Tabs* dock)
{
	if (!dock) return '0';

	if (dock->mpParentTab->IsHorizontal())
	{
		if (dock->pos.x < dock->mpParentTab->mArrChildPtr[0]->pos.x) return '1';
		if (dock->pos.x < dock->mpParentTab->mArrChildPtr[1]->pos.x) return '1';
		return '0';
	}
	else
	{
		if (dock->pos.y < dock->mpParentTab->mArrChildPtr[0]->pos.y) return '2';
		if (dock->pos.y < dock->mpParentTab->mArrChildPtr[1]->pos.y) return '2';
		return '3';
	}
}

void DockSpace::TryDockTabToStoredLocation(Tabs& dock)
{
	if (dock.status == eSTATUS_DOCKED) return;
	if (dock.mLocation[0] == 0) return;

	Tabs* tmp = GetMainRootTab();
	if (!tmp) return;

	Tabs* prev = nullptr;
	char* c = dock.mLocation + strlen(dock.mLocation) - 1;
	while (c >= dock.mLocation && tmp)
	{
		prev = tmp;
		tmp = *c == GetLocationCode(tmp->mArrChildPtr[0]) ? tmp->mArrChildPtr[0] : tmp->mArrChildPtr[1];
		if (tmp) --c;
	}
	DockTab(dock, tmp ? tmp : prev, tmp ? eDockSlot_Tab : GetSlotFromLocationCode(*c));
}

bool DockSpace::Begin(const char* label, bool* opened, ImGuiWindowFlags extra_flags)
{
	eDockSlot next_slot = mNextSlot;
	mNextSlot = eDockSlot_Tab;
	Tabs& dock = GetTab(label, !opened || *opened);
	if (!dock.opened && (!opened || *opened)) TryDockTabToStoredLocation(dock);
	dock.mLastFrame = ImGui::GetFrameCount();
	if (strcmp(dock.label, label) != 0)
	{
		MemFree(dock.label);
		dock.label = ImStrdup(label);
	}

	mEndAction = eEND_ACTION_NONE;

	bool prev_opened = dock.opened;
	bool first = dock.first;
	if (dock.first && opened) *opened = dock.opened;
	dock.first = false;
	if (opened && !*opened)
	{
		if (dock.status != eSTATUS_FLOATING)
		{
			FillTabLocation(dock);
			UndockTab(dock);
			dock.status = eSTATUS_FLOATING;
		}
		dock.opened = false;
		return false;
	}
	dock.opened = true;

	CheckForNonExistentTab();

	if (first || (prev_opened != dock.opened)) {
		Tabs* root = mpNextParentTab ? mpNextParentTab : GetMainRootTab();
		if (root && (&dock != root) && !dock.mpParentTab) {
			DockTab(dock, root, next_slot);
		}
		mpNextParentTab = &dock;
	}

	mCurrentTab = &dock;
	if (dock.status == eSTATUS_DRAGGED) HandleDragging(dock);

	bool is_float = dock.status == eSTATUS_FLOATING;

	if (is_float)
	{
		SetNextWindowPos(dock.pos);
		SetNextWindowSize(dock.size);
		bool ret = Begin(label,
			opened,
			dock.size,
			-1.0f,
			ImGuiWindowFlags_NoCollapse /*| ImGuiWindowFlags_ShowBorders*/ | extra_flags); // ImGuiWindowFlags_ShowBorders not used in new version of ImGui
		mEndAction = eEND_ACTION_END;
		dock.pos = GetWindowPos();
		dock.size = GetWindowSize();

		ImGuiContext& g = *GImGui;

		if (g.ActiveId == GetCurrentWindow()->GetID("#MOVE") && g.IO.MouseDown[0])
		{
			mDragOffset = GetMousePos() - dock.pos;
			UndockTab(dock);
			dock.status = eSTATUS_DRAGGED;
		}
		return ret;
	}

	if (!dock.mActive && dock.status != eSTATUS_DRAGGED) return false;

	//BeginPanel();

	mEndAction = eEND_ACTION_END_CHILD;

	SplitTabs();

	PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
	float TabBar_height = GetTextLineHeightWithSpacing();
	if (TabBar(dock.GetFirstTab(), opened != nullptr))
	{
		FillTabLocation(dock);
		*opened = false;
	}
	ImVec2 pos = dock.pos;
	ImVec2 size = dock.size;
	pos.y += TabBar_height + GetStyle().WindowPadding.y;
	size.y -= TabBar_height + GetStyle().WindowPadding.y;

	SetCursorScreenPos(pos);
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus |
		extra_flags;
	bool ret = BeginChild(label, size, true, flags);
	PopStyleColor();

	return ret;
}

void DockSpace::End()
{
	mCurrentTab = nullptr;
	if (mEndAction != eEND_ACTION_NONE) {
		if (mEndAction == eEND_ACTION_END)
		{
			ImGui::End();
		}
		else if (mEndAction == eEND_ACTION_END_CHILD)
		{
			PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
			EndChild();
			PopStyleColor();
		}
		//EndPanel();
	}
}

/*=========================================================== Wrapper ============================================================*/

static std::map<std::string, DockSpace> g_docklist;
static const char* cur_dock_panel = nullptr;

int GetTabIndex(const DockSpace& context, DockSpace::Tabs* dock)
{
	if (!dock) return -1;

	for (unsignedint i = 0; i < context.mArrTabs.size(); ++i)
	{
		if (dock == context.mArrTabs[i])
			return i;
	}

	IM_ASSERT(false);
	return -1;
}

DockSpace::Tabs* GetTabByIndex(const DockSpace& context, int idx)
{
	if (idx >= 0 && idx < context.mArrTabs.size())
	{
		return context.mArrTabs[idx];
	}

	return nullptr;
}


void ShutdownTabs()
{
	for (auto& iter : g_docklist)
	{
		DockSpace& context = iter.second;

		for (int k = 0, dock_count = (int)context.mArrTabs.size(); k < dock_count; k++)
		{
			context.mArrTabs[k]->~Tabs();
			MemFree(context.mArrTabs[k]);
			context.mArrTabs[k] = nullptr;
		}
	}
	g_docklist.clear();
}

void SetNextTabs(const char* panel, eTabsSlot slot) {
	if (panel && g_docklist.find(panel) != g_docklist.End())
	{
		g_docklist[panel].mNextSlot = slot;
	}
}

bool BeginTabsspace()
{
	ImGuiContext& g = *GImGui;
	cur_dock_panel = g.CurrentWindow->Name;

	IM_ASSERT(cur_dock_panel);

	if (!cur_dock_panel)	return false;

	ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar;
	char child_name[1024];
	sprintf(child_name, "##%s", cur_dock_panel);
	bool result = BeginChild(child_name, ImVec2{ 0, 0}, false, flags);

	DockSpace& dock = g_docklist[cur_dock_panel];
	dock.mPanelPos = GetWindowPos();
	dock.mPanelSize = GetWindowSize();

	return result;
}

void EndTabsspace()
{
	EndChild();

	cur_dock_panel = nullptr;
}

bool BeginTabs(const char* label, bool* opened, ImGuiWindowFlags extra_flags)
{
	IM_ASSERT(cur_dock_panel);

	if (!cur_dock_panel)	return false;

	if (g_docklist.find(cur_dock_panel) != g_docklist.End())
	{
		DockSpace& context = g_docklist[cur_dock_panel];

		char new_label[128];
		sprintf_s(new_label, "%s##%s", label, cur_dock_panel);

		return context.Begin(new_label, opened, extra_flags);
	}

	return false;
}

void EndTabs()
{
	IM_ASSERT(cur_dock_panel);

	if (!cur_dock_panel)	return;

	if (g_docklist.find(cur_dock_panel) != g_docklist.End())
	{
		DockSpace& context = g_docklist[cur_dock_panel];
		context.End();
	}
}

void TabsDebugWindow(const char* dock_panel)
{
	if (dock_panel && g_docklist.find(dock_panel) != g_docklist.End())
	{
		DockSpace& context = g_docklist[dock_panel];
		context.debugWindow();
	}
}

void InitTabs()
{
	ImGuiContext& g = *GImGui;
	ImGuiSettingsHandler ini_handler;
	ini_handler.TypeName = "Tabs";
	ini_handler.TypeHash = ImHash("Tabs", 0, 0);
	ini_handler.ReadOpenFn = readOpen;
	ini_handler.ReadLineFn = readLine;
	ini_handler.WriteAllFn = writeAll;
	g.SettingsHandlers.push_front(ini_handler);
}

/*========================================================= Serializer ===========================================================*/

struct readHelper
{
	DockSpace* context = nullptr;
	DockSpace::Tabs* dock = nullptr;
};
static readHelper rhelper;

static void* readOpen(ImGuiContext* ctx, ImGuiSettingsHandler* handler, const char* name)
{
	static std::string context_panel = "";

	rhelper.context = nullptr;
	rhelper.dock = nullptr;

	std::string tag(name);

	if (tag.substr(0, 6) == "panel:")
	{
		context_panel = tag.substr(6);
	}
	// specific size of docks
	else if (tag.substr(0, 5) == "Size:")
	{
		DockSpace& context = g_docklist[context_panel.c_str()];

		std::string size = tag.substr(5);
		int dockSize = atoi(size.c_str());

		for (int i = 0; i < dockSize; i++)
		{
			DockSpace::Tabs* new_dock = (DockSpace::Tabs*) MemAlloc(sizeof(DockSpace::Tabs));
			context.mArrTabs.push_back(IM_PLACEMENT_NEW(new_dock) DockSpace::Tabs());
		}

		return (void*)NULL;
	}
	// specific index of dock
	else if (tag.substr(0, 5) == "Tabs:")
	{
		if (g_docklist.find(context_panel.c_str()) != g_docklist.End())
		{
			DockSpace& context = g_docklist[context_panel.c_str()];

			std::string indexStr = tag.substr(5);
			int index = atoi(indexStr.c_str());
			if (index >= 0 && index < (int)context.mArrTabs.size())
			{
				rhelper.dock = context.mArrTabs[index];
				rhelper.context = &context;
			}
		}
	}

	return (void*)&rhelper;
}

static void readLine(ImGuiContext* ctx, ImGuiSettingsHandler* handler, void* entry, const char* line_start)
{
	readHelper* userdata = (readHelper*)entry;

	if (userdata)
	{
		int mActive, opened, status;
		int x, y, size_x, size_y;
		int prev, next, child0, child1, mpParentTab;
		char label[64], mLocation[64];

		if (sscanf(line_start, "label=%[^\n^\r]", label) == 1)
		{
			userdata->dock->label = ImStrdup(label);
			userdata->dock->id = ImHash(userdata->dock->label, 0);
		}
		else if (sscanf(line_start, "x=%d", &x) == 1)
		{
			userdata->dock->pos.x = (float)x;
		}
		else if (sscanf(line_start, "y=%d", &y) == 1)
		{
			userdata->dock->pos.y = (float)y;
		}
		else if (sscanf(line_start, "size_x=%d", &size_x) == 1)
		{
			userdata->dock->size.x = (float)size_x;
		}
		else if (sscanf(line_start, "size_y=%d", &size_y) == 1)
		{
			userdata->dock->size.y = (float)size_y;
		}
		else if (sscanf(line_start, "mActive=%d", &mActive) == 1)
		{
			userdata->dock->mActive = (bool)mActive;
		}
		else if (sscanf(line_start, "opened=%d", &opened) == 1)
		{
			userdata->dock->opened = (bool)opened;
		}
		else if (sscanf(line_start, "mLocation=%[^\n^\r]", mLocation) == 1)
		{
			strcpy(userdata->dock->mLocation, mLocation);
		}
		else if (sscanf(line_start, "status=%d", &status) == 1)
		{
			userdata->dock->status = (DockSpace::eSTATUS_) status;
		}
		else if (sscanf(line_start, "prev=%d", &prev) == 1)
		{
			userdata->dock->mpPrevTab = GetTabByIndex(*(userdata->context), prev);
		}
		else if (sscanf(line_start, "next=%d", &next) == 1)
		{
			userdata->dock->mpNextTab = GetTabByIndex(*(userdata->context), next);
		}
		else if (sscanf(line_start, "child0=%d", &child0) == 1)
		{
			userdata->dock->mArrChildPtr[0] = GetTabByIndex(*(userdata->context), child0);
		}
		else if (sscanf(line_start, "child1=%d", &child1) == 1)
		{
			userdata->dock->mArrChildPtr[1] = GetTabByIndex(*(userdata->context), child1);
		}
		else if (sscanf(line_start, "mpParentTab=%d", &mpParentTab) == 1)
		{
			userdata->dock->mpParentTab = GetTabByIndex(*(userdata->context), mpParentTab);
		}
	}
}

static void writeAll(ImGuiContext* ctx, ImGuiSettingsHandler* handler, ImGuiTextBuffer* buf)
{
	int totalTabsNum = 0;
	for (const auto& iter : g_docklist)
	{
		const DockSpace& context = iter.second;
		totalTabsNum += context.mArrTabs.size();
	}

	// Write a buffer
	buf->reserve(buf->size() + totalTabsNum * sizeof(DockSpace::Tabs) + 32 * (totalTabsNum + (int)g_docklist.size() * 2));

	// output size
	for (const auto& iter : g_docklist)
	{
		const DockSpace& context = iter.second;

		buf->appEndf("[%s][panel:%s]\n", handler->TypeName, iter.first.c_str());
		buf->appEndf("[%s][Size:%d]\n", handler->TypeName, (int)context.mArrTabs.size());

		for (int i = 0, docksize = context.mArrTabs.size(); i < docksize; i++)
		{
			const DockSpace::Tabs* d = context.mArrTabs[i];

			// some docks invisible but do exist
			buf->appEndf("[%s][Tabs:%d]\n", handler->TypeName, i);
			buf->appEndf("label=%s\n", d->label);
			buf->appEndf("x=%d\n", (int)d->pos.x);
			buf->appEndf("y=%d\n", (int)d->pos.y);
			buf->appEndf("size_x=%d\n", (int)d->size.x);
			buf->appEndf("size_y=%d\n", (int)d->size.y);
			buf->appEndf("mActive=%d\n", (int)d->mActive);
			buf->appEndf("opened=%d\n", (int)d->opened);
			buf->appEndf("mLocation=%s\n", d->mLocation);
			buf->appEndf("status=%d\n", (int)d->status);
			buf->appEndf("prev=%d\n", (int)GetTabIndex(context, d->mpPrevTab));
			buf->appEndf("next=%d\n", (int)GetTabIndex(context, d->mpNextTab));
			buf->appEndf("child0=%d\n", (int)GetTabIndex(context, d->mArrChildPtr[0]));
			buf->appEndf("child1=%d\n", (int)GetTabIndex(context, d->mArrChildPtr[1]));
			buf->appEndf("mpParentTab=%d\n", (int)GetTabIndex(context, d->mpParentTab));
		}
	}
}

}} // namespace EGUI2::Tabsing

#endif //EDITOR










