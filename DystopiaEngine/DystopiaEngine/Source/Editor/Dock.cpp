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
#include "../../DepEndancies/ImGui/imgui.h"
#include "../../DepEndancies/ImGui/imgui_internal.h"
#include "DataStructure\Stack.h"
#include <iostream>
#include <map>
#include <string>

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
	ImVec2 temp{ lhs.x * rhs, lhs.y * rhs };
	return temp;
}

namespace EGUI{
namespace Docking{

/*========================================================= Declaration ==========================================================*/

static Stack<ImVec2> gSizeStack{ MAX_TAB_SIZE_STACK };

enum eEndAction
{
	eEND_ACTION_NONE,
	eEND_ACTION_PANEL,
	eEND_ACTION_END,
	eEND_ACTION_END_CHILD
};

enum eStatus
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
	eStatus mStatus;

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
	ImVector<Tabs*>	mArrTabs;

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
	void TabBarDesign(Tabs *, ImDrawList *, const ImVec2&, const ImVec2&);
	void DockTab(Tabs&, Tabs*, eDockSlot);
	void SetRootTabPosSize(const ImVec2&, const ImVec2&);
	void TryDockTabToStoredLocation(Tabs&);
	void End();

	static ImRect GetDockedTabRect(const ImRect&, eDockSlot);
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
	for (; pTempTab; pTempTab = pTempTab->mpPrevTab)
		pTempTab->mpParentTab = _pTab;

	pTempTab = mpNextTab;
	for (; pTempTab; pTempTab = pTempTab->mpNextTab)
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
	for (; pTempTab; pTempTab = pTempTab->mpPrevTab)
		pTempTab->mActive = false;

	pTempTab = mpNextTab;
	for (; pTempTab; pTempTab = pTempTab->mpNextTab)
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

Tabs* DockSpace::CreateNewTab(const char *_pLabel, bool _opened, ImU32 _id)
{
	Tabs *pNewTab = static_cast<Tabs*>(ImGui::MemAlloc(sizeof(Tabs)));
	IM_PLACEMENT_NEW(pNewTab) Tabs();
	mArrTabs.push_back(pNewTab);
	pNewTab->mpLabel = ImStrdup(_pLabel);
	IM_ASSERT(pNewTab->mpLabel);
	pNewTab->mId = _id;
	pNewTab->SetActive();
	pNewTab->mStatus = (mArrTabs.size() == 1) ? eSTATUS_DOCKED : eSTATUS_FLOATING;
	pNewTab->mPos = ImVec2{ 0, 0 };
	pNewTab->mSize = gSizeStack.IsEmpty() ? ImGui::GetIO().DisplaySize * 0.5f : gSizeStack.Peek();
	pNewTab->mOpened = _opened;
	pNewTab->mFirst = true;
	pNewTab->mLastFrame = 0;
	pNewTab->mInvalidFrames = 0;
	pNewTab->mLocation[0] = 0;
	return pNewTab;
}

Tabs& DockSpace::GetTab(const char *_pLabel, bool _opened)
{
	ImU32 id = ImHash(_pLabel, 0);
	for (int i = 0; i < mArrTabs.size(); ++i)
		if (mArrTabs[i]->mId == id) return *mArrTabs[i];

	return *CreateNewTab(_pLabel, _opened, id);
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

	ImGui::SetCursorScreenPos(ImVec2(_pTab->mPos.x + _oSize0.x - 5, _pTab->mPos.y));
	ImGui::InvisibleButton("split", ImVec2(10, _pTab->mSize.y));
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
	ImGui::SetCursorScreenPos(ImVec2(_pTab->mPos.x, _pTab->mPos.y + _oSize0.y - 5));
	ImGui::InvisibleButton("split", ImVec2(_pTab->mSize.x, 10));
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
	for (auto& e : mArrTabs)
	{
		if (!e->mpParentTab && (e->mStatus == eSTATUS_DOCKED))
			e->SetPosSize(mPanelPos, mPanelSize);
	}

	//ImDrawList* pCanvas = ImGui::GetWindowDrawList();
	for (int i = 0; i < mArrTabs.size(); ++i)
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
			SplitTabVertical(pTab, dsize, pos0, pos1, size0, size1);
		}
		pTab->mArrChildPtr[0]->SetPosSize(pos0, size0);
		pTab->mArrChildPtr[1]->SetPosSize(pos1, size1);

		if (ImGui::IsItemHovered())
		{
			ImGui::SetMouseCursor(cursor);
			if (ImGui::IsMouseClicked(0))
				pTab->mStatus = eSTATUS_DRAGGED;
		}

		//pCanvas->AddRectFilled(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), 
		//					   ImGui::IsItemHovered() 
		//					   ? ImGui::GetColorU32(ImGuiCol_Border)
		//					   : ImGui::GetColorU32(ImGuiCol_Border));
		ImGui::PopID();
	}
}

void DockSpace::CheckForNonExistentTab()
{
	int frame_limit = ImMax(0, ImGui::GetFrameCount() - 2);
	for (int i = 0; i < mArrTabs.size(); ++i)
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
	for (int i = 0; i < mArrTabs.size(); ++i)
	{
		Tabs *pTab = mArrTabs[i];
		if (pTab->IsAContainer()) continue;
		if (pTab->mStatus != eSTATUS_DOCKED) continue;
		if (ImGui::IsMouseHoveringRect(pTab->mPos, pTab->mPos + pTab->mSize, false))
			return pTab;
	}
	return nullptr;
}

ImRect DockSpace::GetDockedTabRect(const ImRect& _rect, eDockSlot _slot)
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
	for (int i = 0; i < mArrTabs.size(); ++i)
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
		ImRect rect = _onBorder ? GetSlotRectOnBorder(_rect, static_cast<eDockSlot>(i))
								: GetSlotRect(_rect, static_cast<eDockSlot>(i));
		bool hovered = rect.Contains(mousePos);
		pCanvas->AddRectFilled(rect.Min, rect.Max, 
								hovered ? ImGui::GetColorU32(ImGuiCol_ButtonHovered) 
										: ImGui::GetColorU32(ImGuiCol_Button));
		if (!hovered) continue;
		if (!ImGui::IsMouseDown(0))
		{
			DockTab(_tab, _pDestTab ? _pDestTab : GetMainRootTab(), static_cast<eDockSlot>(i));
			return true;
		}
		ImRect dockedRect = GetDockedTabRect(_rect, static_cast<eDockSlot>(i));
		pCanvas->AddRectFilled(dockedRect.Min, dockedRect.Max, ImGui::GetColorU32(ImGuiCol_Button));
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

	//ImU32 dockedCol = (ImGui::GetColorU32(ImGuiCol_FrameBg) & 0x00ffFFFF) | 0x80000000;
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
	//pCanvas->AddRectFilled(_tab.mPos, _tab.mPos + _tab.mSize, dockedCol);
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
			for (int i = 0; i < mArrTabs.size(); ++i)
			{
				if (mArrTabs[i] == pHolder)
				{
					mArrTabs.erase(mArrTabs.begin() + i);
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

	if (ImGui::InvisibleButton("list", ImVec2{ 16, 16 }))
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
	pCanvas->AddRectFilled(ImVec2{ center.x - 4, min.y + 3 }, ImVec2{ center.x + 4, min.y + 5 },
						   ImGui::IsItemHovered() ? ImGui::GetColorU32(ImGuiCol_FrameBgActive) : ImGui::GetColorU32(ImGuiCol_Text));

	pCanvas->AddTriangleFilled(ImVec2{ center.x - 4, min.y + 7 }, ImVec2{ center.x + 4, min.y + 7 }, ImVec2{ center.x, min.y + 12 },
							   ImGui::IsItemHovered() ? ImGui::GetColorU32(ImGuiCol_FrameBgActive) : ImGui::GetColorU32(ImGuiCol_Text));
}

void DockSpace::TabBarDesign(Tabs *_pDockTab, ImDrawList *_pCanvas, const ImVec2& _pos, const ImVec2& _size)
{
	_pCanvas->PathClear();
	_pCanvas->PathLineTo(_pos + ImVec2{ -7, _size.y - 1});
	_pCanvas->PathLineTo(_pos + ImVec2{ -7, 0 });
	_pCanvas->PathLineTo(_pos + ImVec2{ _size.x + 7, 0 });
	_pCanvas->PathLineTo(_pos + ImVec2{ _size.x + 7, _size.y - 1});
	//_pCanvas->PathLineTo(_pos + ImVec2{ -15, _size.y });
	//_pCanvas->PathBezierCurveTo(_pos + ImVec2{ -10, _size.y },
	//							_pos + ImVec2{-5, 0 },
	//							_pos + ImVec2{ 0, 0 }, 
	//						    10);
	//_pCanvas->PathLineTo(_pos + ImVec2{ _size.x, 0 });
	//_pCanvas->PathBezierCurveTo(_pos + ImVec2{ _size.x + 5, 0 },
	//							_pos + ImVec2{ _size.x + 10, _size.y },
	//							_pos + ImVec2{ _size.x + 15, _size.y },
	//							10);
	_pCanvas->PathFillConvex(ImGui::IsItemHovered() ? ImGui::GetColorU32(ImGuiCol_FrameBgHovered) 
												    : (_pDockTab->mActive ? ImGui::GetColorU32(ImGuiCol_FrameBgActive)
																		  : ImGui::GetColorU32(ImGuiCol_FrameBg)));
	_pCanvas->AddText(_pos + ImVec2{ 0,1 }, ImGui::GetColorU32(ImGuiCol_Text), _pDockTab->mpLabel, ImGui::FindRenderedTextEnd(_pDockTab->mpLabel));
}

bool DockSpace::TabBar(Tabs& _tab, bool _closeBtn)
{
	float tabBarHeight = 1.5f * ImGui::GetTextLineHeightWithSpacing();
	ImVec2 size{ _tab.mSize.x - 4, tabBarHeight - 6};
	bool tabClosed = false;
	char buffer[20];

	//ImGui::PushStyleColor(ImGuiCol_Border, ImVec4{1,1, 1, 1});
	ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4{ 0,0,0,-1});
	ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4{ 0.5f, 0.5f, 0.5f, 0.5f });
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4{ 0.7f, 0.7f, 0.7f, 0.8f });
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ 0.9f, 0.9f, 0.9f, 1.f });
	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4{ 0.4f, 0.4f, 0.4f, 0.3f });
	ImGui::SetCursorScreenPos(_tab.mPos + ImVec2{ 2, 6 });
	ImFormatString(buffer, IM_ARRAYSIZE(buffer), "tabs%d", static_cast<int>(_tab.mId));
	if (ImGui::BeginChild(buffer, size, true, ImGuiWindowFlags_NoScrollbar))
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
			size = ImVec2{ ImGui::CalcTextSize(pDockTab->mpLabel, pTextEnd).x, lineH };

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

			ImVec2 pos = ImGui::GetItemRectMin();
			if (pDockTab->mActive && _closeBtn)
			{
				size.x += 16 + ImGui::GetStyle().ItemSpacing.x;
				ImGui::SameLine();
				tabClosed = ImGui::InvisibleButton("close", ImVec2{ 16,16 });
				ImVec2 center = (ImGui::GetItemRectMin() + ImGui::GetItemRectMax()) * 0.5f;
				pCanvas->AddLine(
					center + ImVec2{ -3.5f, -3.5f }, center + ImVec2{ 3.5f, 3.5f }, textCol);
				pCanvas->AddLine(
					center + ImVec2{ 3.5f, -3.5f }, center + ImVec2{ -3.5f, 3.5f }, textCol);
			}
			tabBase = pos.y;
			TabBarDesign(pDockTab, pCanvas, pos, size);
			pDockTab = pDockTab->mpNextTab;
		}		
		ImVec2 cp(_tab.mPos.x, tabBase + lineH);
		pCanvas->AddLine(cp, cp + ImVec2{ _tab.mSize.x, 0 }, ImGui::GetColorU32(ImGuiCol_FrameBgActive), 3);
	}
	ImGui::EndChild();
	//ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
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

void DockSpace::SetRootTabPosSize(const ImVec2& _pos, const ImVec2& _size)
{
	Tabs *pTab = GetMainRootTab();
	if (!pTab) return;

	ImVec2 minS = pTab->GetMinSize();
	ImVec2 reqS = _size;
	pTab->SetPosSize(_pos, ImMax(minS, reqS));
}

eDockSlot DockSpace::GetSlotFromLocationCode(char _c)
{
	switch (_c)
	{
	case '1': return eDOCK_SLOT_LEFT;
	case '2': return eDOCK_SLOT_TOP;
	case '3': return eDOCK_SLOT_BOTTOM;
	default: return eDOCK_SLOT_RIGHT;
	}
}

char DockSpace::GetLocationCode(Tabs *_tab)
{
	if (!_tab) return '0';

	if (_tab->mpParentTab->IsHorizontal())
	{
		if (_tab->mPos.x < _tab->mpParentTab->mArrChildPtr[0]->mPos.x) return '1';
		if (_tab->mPos.x < _tab->mpParentTab->mArrChildPtr[1]->mPos.x) return '1';
		return '0';
	}
	else
	{
		if (_tab->mPos.y < _tab->mpParentTab->mArrChildPtr[0]->mPos.y) return '2';
		if (_tab->mPos.y < _tab->mpParentTab->mArrChildPtr[1]->mPos.y) return '2';
		return '3';
	}
}

void DockSpace::TryDockTabToStoredLocation(Tabs& _tab)
{
	if (_tab.mStatus == eSTATUS_DOCKED) return;
	if (_tab.mLocation[0] == 0) return;

	Tabs* tmp = GetMainRootTab();
	if (!tmp) return;

	Tabs* prev = nullptr;
	char* c = _tab.mLocation + strlen(_tab.mLocation) - 1;
	while (c >= _tab.mLocation && tmp)
	{
		prev = tmp;
		tmp = *c == GetLocationCode(tmp->mArrChildPtr[0]) ? tmp->mArrChildPtr[0] : tmp->mArrChildPtr[1];
		if (tmp) --c;
	}
	DockTab(_tab, tmp ? tmp : prev, tmp ? eDOCK_SLOT_TAB : GetSlotFromLocationCode(*c));
}

bool DockSpace::Begin(const char *_pLabel, bool *_pOpened, ImGuiWindowFlags _flags)
{
	eDockSlot nextSlot = mNextSlot;
	mNextSlot = eDOCK_SLOT_TAB;
	Tabs& _tab = GetTab(_pLabel, !_pOpened || *_pOpened);
	if (!_tab.mOpened && (!_pOpened || *_pOpened)) 
		TryDockTabToStoredLocation(_tab);

	_tab.mLastFrame = ImGui::GetFrameCount();
	if (strcmp(_tab.mpLabel, _pLabel) != 0)
	{
		ImGui::MemFree(_tab.mpLabel);
		_tab.mpLabel = ImStrdup(_pLabel);
	}

	mEndAction = eEND_ACTION_NONE;

	bool prevOpened = _tab.mOpened;
	bool first = _tab.mFirst;
	if (_tab.mFirst && _pOpened) *_pOpened = _tab.mOpened;
	_tab.mFirst = false;
	if (_pOpened && !*_pOpened)
	{
		if (_tab.mStatus != eSTATUS_FLOATING)
		{
			FillTabLocation(_tab);
			UndockTab(_tab);
			_tab.mStatus = eSTATUS_FLOATING;
		}
		_tab.mOpened = false;
		return false;
	}
	_tab.mOpened = true;

	CheckForNonExistentTab();

	if (first || (prevOpened != _tab.mOpened)) 
	{
		Tabs* root = mpNextParentTab ? mpNextParentTab : GetMainRootTab();
		if (root && (&_tab != root) && !_tab.mpParentTab)
			DockTab(_tab, root, nextSlot);
		mpNextParentTab = &_tab;
	}
	mCurrentTab = &_tab;
	if (_tab.mStatus == eSTATUS_DRAGGED) HandleDragging(_tab);
	if (_tab.mStatus == eSTATUS_FLOATING)
	{
		ImGui::SetNextWindowPos(first ? ImVec2{ 100, 100 } : _tab.mPos);
		ImGui::SetNextWindowSize(_tab.mSize, ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowBgAlpha(.9f);
		bool ret = ImGui::Begin(_pLabel, _pOpened, ImGuiWindowFlags_NoCollapse | _flags); 
		mEndAction = eEND_ACTION_END;
		_tab.mPos = ImGui::GetWindowPos();
		_tab.mSize = gSizeStack.IsEmpty() ? ImGui::GetWindowSize() : gSizeStack.Peek();

		ImGuiContext& g = *GImGui;

		if (g.ActiveId == ImGui::GetCurrentWindow()->GetID("#MOVE") && g.IO.MouseDown[0])
		{
			mDragOffset = ImGui::GetMousePos() - _tab.mPos;
			UndockTab(_tab);
			_tab.mStatus = eSTATUS_DRAGGED;
		}
		return ret;
	}

	if (!_tab.mActive && _tab.mStatus != eSTATUS_DRAGGED) return false;
	mEndAction = eEND_ACTION_END_CHILD;
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 1));
	ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4{ 0.5f, 0.5f, 0.5f, 0.5f });
	ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0);
	SplitTabs();
	float tabH = ImGui::GetTextLineHeightWithSpacing();
	if (TabBar(_tab.GetFirstTab(), _pOpened != nullptr))
	{
		FillTabLocation(_tab);
		*_pOpened = false;
	}
	ImVec2 pos = _tab.mPos;
	ImVec2 size = _tab.mSize;
	pos.y += tabH + ImGui::GetStyle().WindowPadding.y;
	size.y -= tabH + ImGui::GetStyle().WindowPadding.y;
	pos.x += 2;
	size.x -= 4;
	ImGui::SetCursorScreenPos(pos);
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
							 ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
							 ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus |
							 ImGuiWindowFlags_AlwaysUseWindowPadding | _flags;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 1,1 });
	bool ret = ImGui::BeginChild(_pLabel, size, true, flags);
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar();
	ImGui::PopStyleVar();
	return ret;
}

void DockSpace::End()
{
	mCurrentTab = nullptr;
	if (mEndAction != eEND_ACTION_NONE) 
	{
		if (mEndAction == eEND_ACTION_END)
			ImGui::End();
		else if (mEndAction == eEND_ACTION_END_CHILD)
		{
			ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 1));
			ImGui::EndChild();
			ImGui::PopStyleColor();
		}
	}
}

/*=========================================================== Wrapper ============================================================*/

static std::map<std::string, DockSpace> gDockable;
static const char* curPanel = nullptr;

int GetTabIndex(const DockSpace& _space, Tabs *_pTab)
{
	if (!_pTab) return -1;
	for (int i = 0; i < _space.mArrTabs.size(); ++i)
	{
		if (_pTab == _space.mArrTabs[i])
			return i;
	}
	IM_ASSERT(false);
	return -1;
}

Tabs* GetTabByIndex(const DockSpace& _space, int _idx)
{
	return (_idx >= 0 && _idx < _space.mArrTabs.size()) ? _space.mArrTabs[_idx] : nullptr;
}

void ShutdownTabs()
{
	int count = 0;
	for (auto& e : gDockable)
	{
		DockSpace& _space = e.second;

		count = static_cast<int>(_space.mArrTabs.size());
		for (int k = 0; k < count; k++)
		{
			_space.mArrTabs[k]->~Tabs();
			ImGui::MemFree(_space.mArrTabs[k]);
			_space.mArrTabs[k] = nullptr;
		}
	}
	gDockable.clear();
}

void SetNextTabs(const char *_pLabel, eDockSlot _slot) 
{
	if (_pLabel && gDockable.find(_pLabel) != gDockable.end())
		gDockable[_pLabel].mNextSlot = _slot;
}

bool BeginDockableSpace()
{
	ImGuiContext& ctx = *GImGui;
	curPanel = ctx.CurrentWindow->Name;
	IM_ASSERT(curPanel);

	if (!curPanel)	return false;
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar;
	char bufferLabel[1024];
	sprintf_s(bufferLabel, "##%s", curPanel);
	bool result = ImGui::BeginChild(bufferLabel, ImVec2{ 0, 0 }, false, flags);

	DockSpace& space = gDockable[curPanel];
	space.mPanelPos = ImGui::GetWindowPos();
	space.mPanelSize = ImGui::GetWindowSize();

	return result;
}

void EndDockableSpace()
{
	ImGui::EndChild();
	curPanel = nullptr;
}

bool BeginTabs(const char *_pLabel, bool *_pOpened, ImGuiWindowFlags _flags)
{
	IM_ASSERT(curPanel);

	if (!curPanel)	return false;

	if (gDockable.find(curPanel) != gDockable.end())
	{
		DockSpace& space = gDockable[curPanel];

		char bufferLabel[128];
		sprintf_s(bufferLabel, "%s##%s", _pLabel, curPanel);
		return space.Begin(bufferLabel, _pOpened, _flags);
	}
	return false;
}

void EndTabs()
{
	IM_ASSERT(curPanel);

	if (!curPanel)	return;

	if (gDockable.find(curPanel) != gDockable.end())
	{
		DockSpace& space = gDockable[curPanel];
		space.End();
	}
}

void PushTabSize(const Math::Vec4& _size)
{
	gSizeStack.Push(ImVec2{_size.x, _size.y});
}

void PushTabSize(const float& _x, const float& _y)
{
	gSizeStack.Push(ImVec2{ _x, _y });
}

void PopTabSize()
{
	if (gSizeStack.IsEmpty()) return;
	gSizeStack.Pop();
}

Math::Vec2 GetTabSize(const char *_pTabName)
{
	IM_ASSERT(curPanel);

	if (!curPanel)	return Math::Vec2{};

	if (gDockable.find(curPanel) != gDockable.end())
	{
		DockSpace& space = gDockable[curPanel];
		char bufferLabel[128];
		sprintf_s(bufferLabel, "%s##%s", _pTabName, curPanel);
		ImU32 id = ImHash(bufferLabel, 0);
		for (int i = 0; i < space.mArrTabs.size(); ++i)
			if (space.mArrTabs[i]->mId == id)
				return Math::Vec2{ space.mArrTabs[i]->mSize.x, space.mArrTabs[i]->mSize.y };
	}
	return Math::Vec2{};
}

Math::Vec2 GetTabPosition(const char *_pTabName)
{
	IM_ASSERT(curPanel);

	if (!curPanel)	return Math::Vec2{};

	if (gDockable.find(curPanel) != gDockable.end())
	{
		DockSpace& space = gDockable[curPanel];
		char bufferLabel[128];
		sprintf_s(bufferLabel, "%s##%s", _pTabName, curPanel);
		ImU32 id = ImHash(bufferLabel, 0);
		for (int i = 0; i < space.mArrTabs.size(); ++i)
			if (space.mArrTabs[i]->mId == id)
				return Math::Vec2{ space.mArrTabs[i]->mPos.x, space.mArrTabs[i]->mPos.y };
	}
	return Math::Vec2{};
}

/*========================================================= Serializer ===========================================================*/

struct readHelper
{
	DockSpace* context = nullptr;
	Tabs* dock = nullptr;
};
static readHelper rhelper;

static void* readOpen(ImGuiContext* /*ctx*/, ImGuiSettingsHandler* /*handler*/, const char* name)
{
	static std::string context_panel = "";

	rhelper.context = nullptr;
	rhelper.dock = nullptr;

	std::string tag(name);

	if (tag.substr(0, 6) == "panel:")
		context_panel = tag.substr(6);
	// specific size of docks
	else if (tag.substr(0, 5) == "Size:")
	{
		DockSpace& context = gDockable[context_panel.c_str()];

		std::string size = tag.substr(5);
		int dockSize = atoi(size.c_str());

		for (int i = 0; i < dockSize; i++)
		{
			Tabs* new_dock = (Tabs*) ImGui::MemAlloc(sizeof(Tabs));
			context.mArrTabs.push_back(IM_PLACEMENT_NEW(new_dock) Tabs());
		}

		return (void*)NULL;
	}
	// specific index of dock
	else if (tag.substr(0, 5) == "Tabs:")
	{
		if (gDockable.find(context_panel.c_str()) != gDockable.end())
		{
			DockSpace& context = gDockable[context_panel.c_str()];

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

static void readLine(ImGuiContext* /*ctx*/, ImGuiSettingsHandler* /*handler*/, void* entry, const char* line_start)
{
	readHelper* userdata = (readHelper*)entry;

	if (userdata)
	{
		int mActive, opened, status;
		int x, y, size_x, size_y;
		int prev, next, child0, child1, mpParentTab;
		char label[64], mLocation[64];

		if (sscanf_s(line_start, "label=%[^\n^\r]", label) == 1)
		{
			userdata->dock->mpLabel = ImStrdup(label);
			userdata->dock->mId = ImHash(userdata->dock->mpLabel, 0);
		}
		else if (sscanf_s(line_start, "x=%d", &x) == 1)
		{
			userdata->dock->mPos.x = static_cast<float>(x);
		}
		else if (sscanf_s(line_start, "y=%d", &y) == 1)
		{
			userdata->dock->mPos.y = static_cast<float>(y);
		}
		else if (sscanf_s(line_start, "size_x=%d", &size_x) == 1)
		{
			userdata->dock->mSize.x = static_cast<float>(size_x);
		}
		else if (sscanf_s(line_start, "size_y=%d", &size_y) == 1)
		{
			userdata->dock->mSize.y = static_cast<float>(size_y);
		}
		else if (sscanf_s(line_start, "mActive=%d", &mActive) == 1)
		{
			userdata->dock->mActive = (mActive) ? true : false;
		}
		else if (sscanf_s(line_start, "opened=%d", &opened) == 1)
		{
			userdata->dock->mOpened = (opened) ? true : false ;
		}
		else if (sscanf_s(line_start, "mLocation=%[^\n^\r]", mLocation) == 1)
		{
			strcpy_s(userdata->dock->mLocation, mLocation);
		}
		else if (sscanf_s(line_start, "status=%d", &status) == 1)
		{
			userdata->dock->mStatus = static_cast<eStatus>(status);
		}
		else if (sscanf_s(line_start, "prev=%d", &prev) == 1)
		{
			userdata->dock->mpPrevTab = GetTabByIndex(*(userdata->context), prev);
		}
		else if (sscanf_s(line_start, "next=%d", &next) == 1)
		{
			userdata->dock->mpNextTab = GetTabByIndex(*(userdata->context), next);
		}
		else if (sscanf_s(line_start, "child0=%d", &child0) == 1)
		{
			userdata->dock->mArrChildPtr[0] = GetTabByIndex(*(userdata->context), child0);
		}
		else if (sscanf_s(line_start, "child1=%d", &child1) == 1)
		{
			userdata->dock->mArrChildPtr[1] = GetTabByIndex(*(userdata->context), child1);
		}
		else if (sscanf_s(line_start, "mpParentTab=%d", &mpParentTab) == 1)
		{
			userdata->dock->mpParentTab = GetTabByIndex(*(userdata->context), mpParentTab);
		}
	}
}

static void writeAll(ImGuiContext* /*ctx*/, ImGuiSettingsHandler* handler, ImGuiTextBuffer* buf)
{
	int totalTabsNum = 0;
	for (const auto& iter : gDockable)
	{
		const DockSpace& context = iter.second;
		totalTabsNum += context.mArrTabs.size();
	}

	// Write a buffer
	buf->reserve(buf->size() + totalTabsNum * sizeof(Tabs) + 32 * (totalTabsNum + (int)gDockable.size() * 2));

	// output size
	for (const auto& iter : gDockable)
	{
		const DockSpace& context = iter.second;

		buf->appendf("[%s][panel:%s]\n", handler->TypeName, iter.first.c_str());
		buf->appendf("[%s][Size:%d]\n", handler->TypeName, (int)context.mArrTabs.size());

		for (int i = 0, docksize = context.mArrTabs.size(); i < docksize; i++)
		{
			const Tabs* d = context.mArrTabs[i];

			// some docks invisible but do exist
			buf->appendf("[%s][Tabs:%d]\n", handler->TypeName, i);
			buf->appendf("label=%s\n", d->mpLabel);
			buf->appendf("x=%d\n", (int)d->mPos.x);
			buf->appendf("y=%d\n", (int)d->mPos.y);
			buf->appendf("size_x=%d\n", (int)d->mSize.x);
			buf->appendf("size_y=%d\n", (int)d->mSize.y);
			buf->appendf("mActive=%d\n", (int)d->mActive);
			buf->appendf("opened=%d\n", (int)d->mOpened);
			buf->appendf("mLocation=%s\n", d->mLocation);
			buf->appendf("status=%d\n", (int)d->mStatus);
			buf->appendf("prev=%d\n", (int)GetTabIndex(context, d->mpPrevTab));
			buf->appendf("next=%d\n", (int)GetTabIndex(context, d->mpNextTab));
			buf->appendf("child0=%d\n", (int)GetTabIndex(context, d->mArrChildPtr[0]));
			buf->appendf("child1=%d\n", (int)GetTabIndex(context, d->mArrChildPtr[1]));
			buf->appendf("mpParentTab=%d\n", (int)GetTabIndex(context, d->mpParentTab));
		}
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


}} // namespace EGUI::Docking

#endif //EDITOR










