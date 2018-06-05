/* HEADER *********************************************************************************/
/*!
\file	Dock.cpp
\author Digipen (100%)
\par    email: t.shannon\@digipen.edu
\brief
NOTE TO SELF: On 22/5/2018, stopped at DrawTabbarListButton
							(finished that, proceed to the one after)

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#include "Editor\Dock.h"
#include <iostream>

namespace Dystopia {
namespace EGUI {	
namespace Dock {

static DockContext *g_pDock = nullptr;

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
	return *temp;
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


DockContext::DockContext()
	: mpCurrent{ nullptr }, mpNextParent{ nullptr }, mDragOffset{ 0,0 }, mWorkspacePos{ 0,0 }, mWorkspaceSize{ 0,0 },
	mLastFrame{ 0 }, mEndAction{ eEnd_ACTION_NONE }, mNextDockSlot{ eDOCK_TAB }, mIsBeginOpen{ false }, mIsFirstCall{ true }
{
}

DockContext::~DockContext()
{
	Shutdown();
}

void DockContext::Shutdown()
{
	for (auto e : mDocksArr)
	{
		e->~Dock();
		ImGui::MemFree(e);
	}
	mDocksArr.clear();
}

void DockContext::PutInBG()
{
	ImGuiWindow *pWin = ImGui::GetCurrentWindow();
	ImGuiContext& ctx = *GImGui;
	if (ctx.Windows[0] == pWin) return;

	for (int i = 0; i < ctx.Windows.size(); i++)
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

void DockContext::Split()
{
	if (ImGui::GetFrameCount() == mLastFrame) return;

	mLastFrame = ImGui::GetFrameCount();

	PutInBG();

	for (auto e : mDocksArr)
	{
		if (!e->mpParent && (e->mStatus == eSTAT_DOCK))
			e->SetPosSize(mWorkspacePos, mWorkspaceSize);
	}

	ImU32 color = ImGui::GetColorU32(ImGuiCol_Button);
	ImU32 colorHovered = ImGui::GetColorU32(ImGuiCol_ButtonHovered);
	ImDrawList *drawList = ImGui::GetWindowDrawList();
	ImGuiIO& io = ImGui::GetIO();

	for (unsigned int i = 0; i < mDocksArr.size(); ++i)
	{
		Dock& dock = *mDocksArr[i];
		if (!dock.HasChildren()) continue;

		ImGui::PushID(i);
		if (!ImGui::IsMouseDown(0)) dock.mStatus = eSTAT_DOCK;

		ImVec2 pos0 = dock.mpChildren[0]->mPos;
		ImVec2 pos1 = dock.mpChildren[1]->mPos;
		ImVec2 size0 = dock.mpChildren[0]->mSize;
		ImVec2 size1 = dock.mpChildren[1]->mSize;

		ImGuiMouseCursor cursor;

		ImVec2 dSize{ 0,0 };
		ImVec2 minSize0 = dock.mpChildren[0]->GetMinSize();
		ImVec2 minSize1 = dock.mpChildren[1]->GetMinSize();
		if (dock.IsHorizontal())
		{
			cursor = ImGuiMouseCursor_ResizeEW;
			ImGui::SetCursorScreenPos(ImVec2{ dock.mPos.x + size0.x, dock.mPos.y });
			ImGui::InvisibleButton("split", ImVec2{ 3, dock.mSize.y });

			if (dock.mStatus == eSTAT_DRAGGED) dSize.x = io.MouseDelta.x;

			dSize.x = -ImMin(-dSize.x, dock.mpChildren[0]->mSize.x - minSize0.x);
			dSize.x = ImMin(dSize.x, dock.mpChildren[1]->mSize.x - minSize1.x);
			size0 = ImVec2{ size0.x + dSize.x, size0.y + dSize.y};
			size1 = ImVec2{ size0.x - dSize.x, size0.y - dSize.y };
			pos0 = dock.mPos;
			pos1.x = pos0.x + size0.x;
			pos1.y = dock.mPos.y;
			size0.y = size1.y = dock.mSize.y;
			size1.x = ImMax(minSize1.x, dock.mSize.x - size0.x);
			size0.x = ImMax(minSize0.x, dock.mSize.x - size1.x);
		}
		else
		{
			cursor = ImGuiMouseCursor_ResizeNS;
			ImGui::SetCursorScreenPos(ImVec2{ dock.mPos.x, dock.mPos.y + size0.y });
			ImGui::InvisibleButton("split", ImVec2{ dock.mSize.x, 3 });

			if (dock.mStatus == eSTAT_DRAGGED) dSize.y = io.MouseDelta.y;

			dSize.y = -ImMin(-dSize.y, dock.mpChildren[0]->mSize.y - minSize0.y);
			dSize.y = ImMin(dSize.y, dock.mpChildren[1]->mSize.y - minSize1.y);
			size0 = ImVec2{ size0.x + dSize.x, size0.y + dSize.y };
			size1 = ImVec2{ size0.x - dSize.x, size0.y - dSize.y };
			pos0 = dock.mPos;
			pos1.x = dock.mPos.x;
			pos1.y = pos0.y + size0.y;
			size0.x = size1.x = dock.mSize.x;
			size1.y = ImMax(minSize1.y, dock.mSize.y - size0.y);
			size0.y = ImMax(minSize0.y, dock.mSize.y - size1.y);
		}
		dock.mpChildren[0]->SetPosSize(pos0, size0);
		dock.mpChildren[1]->SetPosSize(pos1, size1);

		if (ImGui::IsItemHovered())
		{
			ImGui::SetMouseCursor(cursor);
			ImGui::SetHoveredID(GImGui->CurrentWindow->DC.LastItemId);
		}

		if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
			dock.mStatus = eSTAT_DRAGGED;

		drawList->AddRectFilled(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(),
								ImGui::IsItemHovered() ? colorHovered : color);
		ImGui::PopID();
	}
}

void DockContext::CheckNonexistent()
{
	int frameLimit = ImMax(0, ImGui::GetFrameCount() - 2);
	for (auto e : mDocksArr)
	{
		if (e->HasChildren()) continue;
		if (e->mStatus == eSTAT_FLOAT) continue;
		if (e->mLastFrame < frameLimit)
		{
			++e->mInvalidFrame;
			if (e->mInvalidFrame > 2)
			{
				DoUndock(*e);
				e->mStatus = eSTAT_FLOAT;
			}
			return;
		}
		e->mInvalidFrame = 0;
	}
}

void DockContext::SetDockActive()
{
	IM_ASSERT(mpCurrent);
	if (mpCurrent) mpCurrent->SetActive();
}

void DockContext::HandleDrag(Dock& _dock)
{
	Dock* pDestDock = GetDockAt(ImGui::GetIO().MousePos);

	ImGuiWindowFlags flags = ImGuiWindowFlags_Tooltip | 
							ImGuiWindowFlags_NoTitleBar |
							ImGuiWindowFlags_NoMove | 
							ImGuiWindowFlags_NoResize |
							ImGuiWindowFlags_NoSavedSettings |
							ImGuiWindowFlags_AlwaysAutoResize;

	ImGui::SetNextWindowBgAlpha(0.0f);
	ImGui::Begin("##Overlay", nullptr, flags);

	ImDrawList *canvas = ImGui::GetWindowDrawList();
	canvas->PushClipRectFullScreen();

	ImU32 dockedColor = ImGui::GetColorU32(ImGuiCol_FrameBg);
	dockedColor = (dockedColor & 0x00ffFFFF | 0x80000000);
	_dock.mPos = ImVec2{ ImGui::GetIO().MousePos.x - mDragOffset.x ,
						 ImGui::GetIO().MousePos.y - mDragOffset.y };

	if (pDestDock)
	{
		ImVec2 max{ pDestDock->mPos.x + pDestDock->mSize.x , pDestDock->mPos.y + pDestDock->mSize.y };
		if (DockSlots(_dock, pDestDock, ImRect{pDestDock->mPos, max}, false))
		{
			canvas->PopClipRect();
			ImGui::End();
			return;
		}
	}
	else
	{
		ImVec2 max{ mWorkspacePos.x + mWorkspaceSize.x, mWorkspacePos.y + mWorkspaceSize.y };
		if (DockSlots(_dock, nullptr, ImRect{ mWorkspacePos, max }, true))
		{
			canvas->PopClipRect();
			ImGui::End();
			return;
		}
	}

	ImVec2 max{_dock.mPos.x + _dock.mSize.x, _dock.mPos.y + _dock.mSize.y };
	canvas->AddRectFilled(_dock.mPos, max, dockedColor);
	canvas->PopClipRect();
	if (!ImGui::IsMouseDown(0))
	{
		_dock.mStatus = eSTAT_FLOAT;
		_dock.mLocation[0] = 0;
		_dock.SetActive();
	}
	ImGui::End();
}

void DockContext::FillLocation(Dock& _dock)
{
	if (_dock.mStatus == eSTAT_FLOAT) return;

	char *c = _dock.mLocation;
	Dock* tempDock = &_dock;
	while (tempDock->mpParent)
	{
		*c = GetLocationCode(tempDock);
		tempDock = tempDock->mpParent;
		++c;
	}
	*c = 0;
}

void DockContext::DoUndock(Dock& _dock)
{
	if (_dock.mpPrevTab)
		_dock.mpPrevTab->SetActive();
	else if (_dock.mpNextTab)
		_dock.mpNextTab->SetActive();
	else
		_dock.mActive = false;
	
	Dock *pParent = _dock.mpParent;
	if (pParent)
	{
		Dock& sibling = _dock.GetSibling();
		if (pParent->mpChildren[0] == &_dock)
			pParent->mpChildren[0] = _dock.mpNextTab;
		else if (pParent->mpChildren[1] == &_dock)
			pParent->mpChildren[1] = _dock.mpNextTab;

		bool removeParent = !pParent->mpChildren[0] || !pParent->mpChildren[1];
		if (removeParent)
		{
			if (pParent->mpParent)
			{
				Dock*& pChild = (pParent->mpParent->mpChildren[0] == pParent) ?
								pParent->mpParent->mpChildren[0] :
								pParent->mpParent->mpChildren[1];

				pChild = &sibling;
				pChild->SetPosSize(pParent->mPos, pParent->mSize);
				pChild->SetParent(pParent->mpParent);
			}
			else
			{
				if (pParent->mpChildren[0])
				{
					pParent->mpChildren[0]->SetParent(nullptr);
					pParent->mpChildren[0]->SetPosSize(pParent->mPos, pParent->mSize);
				}
				if (pParent->mpChildren[1])
				{
					pParent->mpChildren[1]->SetParent(nullptr);
					pParent->mpChildren[1]->SetPosSize(pParent->mPos, pParent->mSize);
				}
			}

			for (unsigned int i = 0; i < mDocksArr.size(); ++i)
			{
				if (mDocksArr[i] == pParent)
				{
					mDocksArr.FastRemove(i);
					break;
				}
			}
			if (pParent == mpNextParent)
				mpNextParent = nullptr;
			pParent->~Dock();
			ImGui::MemFree(pParent);
		}
	}

	if (_dock.mpPrevTab) _dock.mpPrevTab->mpNextTab = _dock.mpNextTab;
	if (_dock.mpNextTab) _dock.mpNextTab->mpPrevTab = _dock.mpPrevTab;
	_dock.mpParent =_dock.mpPrevTab = _dock.mpNextTab = nullptr;

	if (mpNextParent == &_dock)
		mpNextParent = nullptr;
}

void DockContext::DoDock(Dock& _dock, Dock* _dest, eDockSlot _slot)
{
	IM_ASSERT(!_dock.mpParent);
	IM_ASSERT(!_dock.mpNextTab);
	IM_ASSERT(!_dock.mpPrevTab);

	if (!_dest)
	{
		_dock.mStatus = eSTAT_DOCK;
		_dock.SetPosSize(mWorkspacePos, mWorkspaceSize);
	}
	else if (_slot == eDOCK_TAB)
	{
		_dest = FindNonContainer(_dest);
		IM_ASSERT(_dest);

		Dock *tempDock = _dest;
		while (tempDock->mpNextTab)
		{
			tempDock = tempDock->mpNextTab;
		}

		tempDock->mpNextTab = &_dock;
		_dock.mpPrevTab = tempDock;
		_dock.mSize = tempDock->mSize;
		_dock.mPos = tempDock->mPos;
		_dock.mpParent = _dest->mpParent;
		_dock.mStatus = eSTAT_DOCK;
	}
	else if (_slot == eDOCK_NONE)
	{
		_dock.mStatus = eSTAT_DOCK;
	}
	else
	{
		Dock* pContainer = (Dock*)ImGui::MemAlloc(sizeof(Dock));
		IM_PLACEMENT_NEW(pContainer) Dock();
		mDocksArr.push_back(pContainer);
		pContainer->mpChildren[0] = &_dest->GetFirstTab();
		pContainer->mpChildren[1] = &_dock;
		pContainer->mpNextTab = nullptr;
		pContainer->mpPrevTab = nullptr;
		pContainer->mpParent = _dest->mpParent;
		pContainer->mSize = _dest->mSize;
		pContainer->mPos = _dest->mPos;
		pContainer->mStatus = eSTAT_DOCK;
		pContainer->mpLabel = ImStrdup("");

		if (!_dest->mpParent)
		{
		}
		else if (&_dest->GetFirstTab() == _dest->mpParent->mpChildren[0])
		{
			_dest->mpParent->mpChildren[0] = pContainer;
		}
		else
		{
			_dest->mpParent->mpChildren[1] = pContainer;
		}

		_dest->SetParent(pContainer);
		_dock.mpParent = pContainer;
		_dock.mStatus = eSTAT_DOCK;

		SetDockPosSize(*_dest, _dock, _slot, *pContainer);
	}
	_dock.SetActive();
}

void DockContext::DrawTabbarListBtn(Dock& _dock)
{
	if (!_dock.mpNextTab) return;

	ImDrawList *drawList = ImGui::GetWindowDrawList();
	if (ImGui::InvisibleButton("list", ImVec2{ 16, 16 }))		
		ImGui::OpenPopup("tab_list_popup");

	if (ImGui::BeginPopup("tab_list_popup"))
	{
		Dock* pTmpDock = &_dock;
		while (pTmpDock)
		{
			bool dummy = false;
			if (ImGui::Selectable(pTmpDock->mpLabel, &dummy))
			{
				pTmpDock->SetActive();
				mpNextParent = pTmpDock;
			}
			pTmpDock = pTmpDock->mpNextTab;
		}
		ImGui::EndPopup();
	}

	bool hovered = ImGui::IsItemHovered();
	ImVec2 min = ImGui::GetItemRectMin();
	ImVec2 max = ImGui::GetItemRectMax();
	ImVec2 center{(min.x + max.x) * 0.5f, (min.y + max.y) * 0.5f};
	ImU32 textColor = ImGui::GetColorU32(ImGuiCol_Text);
	ImU32 activeColor = ImGui::GetColorU32(ImGuiCol_FrameBgActive);

	drawList->AddRectFilled(ImVec2{ center.x - 4, min.y + 3 },
							ImVec2{ center.x + 4, min.y + 5 },
							hovered ? activeColor : textColor);

	drawList->AddTriangleFilled(ImVec2{ center.x - 4, min.y + 7 },
								ImVec2{ center.x + 4, min.y + 7 },
								ImVec2{ center.x, min.y + 12 },
								hovered ? activeColor : textColor);
}

void DockContext::RootDock(const ImVec2& _pos, const ImVec2& _size)
{
	Dock* pRoot = GetRootDock();
	if (!pRoot) return;

	ImVec2 minSize = pRoot->GetMinSize();
	ImVec2 reqSize = _size;
	pRoot->SetPosSize(_pos, ImMax(minSize, reqSize));

	if (!mIsFirstCall)
	{
		for (unsigned int i = 0; i < mDocksArr.size(); ++i)
		{
			Dock* e = mDocksArr[i];
			if (!e->HasChildren() && e != pRoot && (ImGui::GetFrameCount() - e->mLastFrame) > 1)
			{
				DoUndock(*e);
				e->~Dock();
				ImGui::MemFree(e);
				mDocksArr.FastRemove(i);
				--i;
			}
		}
	}
	mIsFirstCall = false;
}

void DockContext::TryDockToStoredLocation(Dock& _dock)
{
	if (_dock.mStatus == eSTAT_DOCK) return;
	if (_dock.mLocation[0] == 0) return;

	Dock *tempDock = GetRootDock();
	if (!tempDock) return;

	Dock *tempPrev = nullptr;
	char *c = _dock.mLocation + strlen(_dock.mLocation) - 1;
	while (c >= _dock.mLocation && tempDock)
	{
		tempPrev = tempDock;
		tempDock = *c == GetLocationCode(tempDock->mpChildren[0]) ? tempDock->mpChildren[0] : tempDock->mpChildren[1];
		if (tempDock) --c;
	}
	if (tempDock && tempDock->mpChildren[0]) tempDock = tempDock->mpParent;
	DoDock(_dock, tempDock ? tempDock : tempPrev, tempDock && !tempDock->mpChildren[0] ? eDOCK_TAB : GetSlotFromCode(*c));
}

void DockContext::CleanDocks()
{
	bool restart = false;

	for (int i = 0, c = mDocksArr.size(); i < c; ++i)
	{
		Dock& dock = *mDocksArr[i];
		if (!dock.mLastFrame && dock.mStatus != eSTAT_FLOAT && !dock.mpChildren[0])
		{
			FillLocation(*mDocksArr[i]);
			DoUndock(*mDocksArr[i]);
			mDocksArr[i]->mStatus = eSTAT_FLOAT;
			restart = true;
			break;
		}
	}

	if (restart)
		CleanDocks();
}

void DockContext::End()
{
	mpCurrent = nullptr;
	if (mEndAction != eEnd_ACTION_NONE)
	{
		if (mEndAction == eEnd_ACTION_END)
			ImGui::End();
		else if (mEndAction == eEnd_ACTION_END_CHILD)
		{
			ImGui::PushStyleColor(ImGuiCol_Border, ImVec4{ 0,0,0,0 });
			ImGui::PushStyleColor(ImGuiCol_BorderShadow, ImVec4{ 0,0,0,0 });
			ImGui::EndChild();
			ImGui::PopStyleColor(2);
		}
	}
	mIsBeginOpen = false;
}

void DockContext::SetDockPosSize(Dock& _dest, Dock& _dock, eDockSlot _slot, Dock& _container)
{
	IM_ASSERT(!_dock.mpPrevTab && !_dock.mpNextTab && !_dock.mpChildren[0] && !_dock.mpChildren[1]);
	_dest.mPos = _container.mPos;
	_dest.mSize = _container.mSize;
	_dock.mPos = _container.mPos;
	_dock.mSize = _container.mSize;

	switch (_slot)
	{
	case eDOCK_BOTTOM:
		_dest.mSize.y *= 0.5f;
		_dock.mSize.y *= 0.5f;
		_dock.mPos.y += _dest.mSize.y;
		break;
	case eDOCK_RIGHT:
		_dest.mSize.x *= 0.5f;
		_dock.mSize.x *= 0.5f;
		_dock.mPos.x += _dest.mSize.x;
		break;
	case eDOCK_LEFT:
		_dest.mSize.x *= 0.5f;
		_dock.mSize.x *= 0.5f;
		_dest.mPos.x += _dock.mSize.x;
		break;
	case eDOCK_TOP:
		_dest.mSize.y *= 0.5f;
		_dock.mSize.y *= 0.5f;
		_dest.mPos.y += _dock.mSize.y;
		break;
	default: IM_ASSERT(false); break;
	}
	_dest.SetPosSize(_dest.mPos, _dest.mSize);

	if (_container.mpChildren[1]->mPos.x < _container.mpChildren[0]->mPos.x ||
		_container.mpChildren[1]->mPos.y < _container.mpChildren[0]->mPos.y)
	{
		Dock* tempDock = _container.mpChildren[0];
		_container.mpChildren[0] = _container.mpChildren[1];
		_container.mpChildren[1] = tempDock;
	}
}

ImRect DockContext::GetDockedRect(const ImRect& _rect, eDockSlot _slot)
{
	ImVec2 size = _rect.GetSize();
	ImVec2 extension, max, min;
	switch (_slot)
	{
	case eDOCK_TOP: 
		extension = ImVec2{ size.x, size.y * 0.5f };
		max = ImVec2{_rect.Min.x + extension.x, _rect.Min.y + extension.y};
		return ImRect{ _rect.Min, max };
	case eDOCK_RIGHT:
		extension = ImVec2{ size.x * 0.5f, 0 };
		min = ImVec2{ _rect.Min.x + extension.x, _rect.Min.y + extension.y };
		return ImRect{ min, _rect.Max };
	case eDOCK_BOTTOM:
		extension = ImVec2{ 0, size.y * 0.5f };
		min = ImVec2{ _rect.Min.x + extension.x, _rect.Min.y + extension.y };
		return ImRect{ min, _rect.Max };
	case eDOCK_LEFT:
		extension = ImVec2{ size.x * 0.5f, size.y };
		max = ImVec2{ _rect.Min.x + extension.x, _rect.Min.y + extension.y };
		return ImRect{ _rect.Min, max };
	}
	return ImRect{};
}

ImRect DockContext::GetSlotRect(ImRect _parentRect, eDockSlot _slot)
{
	ImVec2 size{ _parentRect.Max.x - _parentRect.Min.x, _parentRect.Max.y - _parentRect.Min.y };
	ImVec2 center{_parentRect.Min.x + size.x * 0.5f, _parentRect.Min.y + size.y * 0.5f };
	ImVec2 offsetMin, offsetMax, min, max;
	switch (_slot)
	{
	default:
		offsetMin = ImVec2{ 20, 20 };
		offsetMax = ImVec2{ 20, 20 };
		min = ImVec2{center.x + offsetMin.x, center.y + offsetMin.y};
		max = ImVec2{ center.x + offsetMax.x, center.y + offsetMax.y };
		return ImRect{ min, max };
	case eDOCK_TOP:
		offsetMin = ImVec2{ -20, -50 };
		offsetMax = ImVec2{ 20, -30 };
		min = ImVec2{ center.x + offsetMin.x, center.y + offsetMin.y };
		max = ImVec2{ center.x + offsetMax.x, center.y + offsetMax.y };
		return ImRect{ min, max };
	case eDOCK_RIGHT:
		offsetMin = ImVec2{ 30, -20 };
		offsetMax = ImVec2{ 50, 20 };
		min = ImVec2{ center.x + offsetMin.x, center.y + offsetMin.y };
		max = ImVec2{ center.x + offsetMax.x, center.y + offsetMax.y };
		return ImRect{ min, max };
	case eDOCK_BOTTOM:
		offsetMin = ImVec2{ -20, 30 };
		offsetMax = ImVec2{ 20, 50 };
		min = ImVec2{ center.x + offsetMin.x, center.y + offsetMin.y };
		max = ImVec2{ center.x + offsetMax.x, center.y + offsetMax.y };
		return ImRect{ min, max };
	case eDOCK_LEFT:
		offsetMin = ImVec2{ -50, -20 };
		offsetMax = ImVec2{ -30, 20 };
		min = ImVec2{ center.x + offsetMin.x, center.y + offsetMin.y };
		max = ImVec2{ center.x + offsetMax.x, center.y + offsetMax.y };
		return ImRect{ min, max };
	}
}

ImRect DockContext::GetSlotRectOnBorder(ImRect _parentRect, eDockSlot _slot)
{
	ImVec2 size{ _parentRect.Max.x - _parentRect.Min.x, _parentRect.Max.y - _parentRect.Min.y };
	ImVec2 center{ _parentRect.Min.x + size.x * 0.5f, _parentRect.Min.y + size.y * 0.5f };

	switch (_slot)
	{
	case eDOCK_TOP:
		return ImRect{ ImVec2{ center.x - 20, _parentRect.Min.y + 10 },
					   ImVec2{ center.x + 20, _parentRect.Min.y + 30 } };
	case eDOCK_LEFT:
		return ImRect{ ImVec2{ _parentRect.Min.x + 10, center.y - 20 },
					   ImVec2{ _parentRect.Min.x + 30, center.y + 20 } };
	case eDOCK_BOTTOM:
		return ImRect{ ImVec2{ center.x - 20, _parentRect.Max.y - 30 },
					   ImVec2{ center.x + 20, _parentRect.Max.y - 10 } };
	case eDOCK_RIGHT:
		return ImRect{ ImVec2{ _parentRect.Max.x - 30, center.y - 20 },
					   ImVec2{ _parentRect.Max.x - 10, center.y + 20 } };
	default:
		IM_ASSERT(false);
	}
	IM_ASSERT(false);
	return ImRect{};
}

eDockSlot DockContext::GetSlotFromCode(char _code)
{
	switch (_code)
	{
	case '1': return eDOCK_LEFT;
	case '2': return eDOCK_TOP;
	case '3': return eDOCK_BOTTOM;
	default: return eDOCK_RIGHT;
	}
}

char DockContext::GetLocationCode(Dock* _dock)
{
	if (!_dock) return '0';

	if (_dock->mpParent->IsHorizontal())
	{
		if (_dock->mPos.x < _dock->mpParent->mpChildren[0]->mPos.x) return '1';
		if (_dock->mPos.x < _dock->mpParent->mpChildren[1]->mPos.x) return '1';
		return '0';
	}
	else if (_dock->mpParent->IsHorizontal())
	{
		if (_dock->mPos.y < _dock->mpParent->mpChildren[0]->mPos.y) return '2';
		if (_dock->mPos.y < _dock->mpParent->mpChildren[1]->mPos.y) return '2';
		return '3';
	}
	return '0';
}

DockContext::Dock* DockContext::GetDockAt(const ImVec2& /*_pos*/) const
{
	for (auto e : mDocksArr)
	{
		if (e->HasChildren()) continue;
		if (e->mStatus != eSTAT_DOCK) continue;

		ImVec2 max{ e->mPos.x + e->mSize.x, e->mPos.y + e->mSize.y };
		if (ImGui::IsMouseHoveringRect(e->mPos, max, false))
			return e;
	}
	return nullptr;
}

DockContext::Dock& DockContext::GetDock(const char* _label, bool _opened, const ImVec2& _defSize)
{
	ImU32 id = ImHash(_label, 0);
	for (auto e : mDocksArr)
	{
		if (e->mId == id)
			return *e;
	}
	
	Dock* newDock = (Dock*)ImGui::MemAlloc(sizeof(Dock)); IM_PLACEMENT_NEW(newDock) Dock();
	mDocksArr.push_back(newDock);

	newDock->mpLabel = ImStrdup(_label); IM_ASSERT(newDock->mpLabel);
	newDock->mId = id;
	newDock->SetActive();
	newDock->mStatus = (mDocksArr.size() == 1) ? eSTAT_DOCK : eSTAT_FLOAT;
	newDock->mPos = ImVec2{ 0,0 };
	newDock->mSize.x = _defSize.x < 0 ? ImGui::GetIO().DisplaySize.x : _defSize.x;
	newDock->mSize.y = _defSize.y < 0 ? ImGui::GetIO().DisplaySize.y : _defSize.y;
	newDock->mFloatmodeSize = _defSize;
	newDock->mOpened = _opened;
	newDock->mFirst = true;
	newDock->mLastFrame = 0;
	newDock->mLocation[0] = 0;
	return *newDock;
}

DockContext::Dock* DockContext::GetRootDock()
{
	for (auto e : mDocksArr)
	{
		if (!e->mpParent && (e->mStatus == eSTAT_DOCK || e->mpChildren[0]))
			return e;
	}
	return nullptr;
}

DockContext::Dock* DockContext::FindNonContainer(Dock* _dock)
{
	if (_dock->HasChildren())
	{
		Dock* tempDock = nullptr;
		tempDock = FindNonContainer(_dock->mpChildren[0]);
		if (!tempDock)
			tempDock = FindNonContainer(_dock->mpChildren[1]);
		return tempDock;
	}
	return _dock;
}

bool DockContext::Begin(const char *_label, bool *_opened, ImGuiWindowFlags _flags, const ImVec2& _size)
{
	IM_ASSERT(!mIsBeginOpen);
	mIsBeginOpen = true;

	Dock& dock = GetDock(_label, !_opened || *_opened, _size);
	if (dock.mLastFrame != 0 && mLastFrame != ImGui::GetFrameCount())
		CleanDocks();

	dock.mLastFrame = ImGui::GetFrameCount();

	eDockSlot nextSlot = mNextDockSlot;
	mNextDockSlot = eDOCK_TAB;

	if (!dock.mOpened && (!_opened || *_opened)) TryDockToStoredLocation(dock);
	if (strcmp(dock.mpLabel, _label) != 0)
	{
		ImGui::MemFree(dock.mpLabel);
		dock.mpLabel = ImStrdup(_label);
	}
	
	mEndAction = eEnd_ACTION_NONE;
	
	bool prevOpened = dock.mOpened;
	bool first = dock.mFirst;
	if (dock.mFirst && _opened) *_opened = dock.mOpened;
	
	dock.mFirst = false;
	if (_opened && !*_opened)
	{
		if (dock.mStatus != eSTAT_FLOAT)
		{
			FillLocation(dock);
			DoUndock(dock);
			dock.mStatus = eSTAT_FLOAT;
		}
		dock.mOpened = false;
		return false;
	}
	dock.mOpened = true;

	CheckNonexistent();

	if (first || (prevOpened != dock.mOpened))
	{
		Dock* pRoot = mpNextParent ? mpNextParent : GetRootDock();
		if (pRoot && (&dock != pRoot) && (!dock.mpParent))
			DoDock(dock, pRoot, nextSlot);
		mpNextParent = &dock;
	}
	
	mpCurrent = &dock;
	if (dock.mStatus == eSTAT_DRAGGED) HandleDrag(dock);

	bool isFloat = dock.mStatus == eSTAT_FLOAT;
	if (isFloat)
	{
		ImGui::SetNextWindowPos(dock.mPos);
		const ImVec2 oldSize{ dock.mFloatmodeSize.x > 0 ? dock.mFloatmodeSize.x : dock.mSize.x ,
							dock.mFloatmodeSize.y > 0 ? dock.mFloatmodeSize.y : dock.mSize.y };
		ImGui::SetNextWindowSize(oldSize);
		dock.mSize = oldSize;
		bool ret = ImGui::Begin(_label, _opened, ImGuiWindowFlags_NoCollapse | _flags);
		mEndAction = eEnd_ACTION_END;
		dock.mPos = ImGui::GetWindowPos();
		dock.mSize = ImGui::GetWindowSize();
		if (dock.mSize.x != oldSize.x && dock.mFloatmodeSize.x >= 0) dock.mFloatmodeSize.x = dock.mSize.x;
		if (dock.mSize.y != oldSize.y && dock.mFloatmodeSize.y >= 0) dock.mFloatmodeSize.y = dock.mSize.y;
		
		ImGuiContext &ctx = *GImGui;
		if (ctx.ActiveId == ImGui::GetCurrentWindow()->MoveId && ctx.IO.MouseDown[0])
		{
			mDragOffset = ImVec2{ImGui::GetMousePos().x - dock.mPos.x , ImGui::GetMousePos().y - dock.mPos.y };
			DoUndock(dock);
			dock.mStatus = eSTAT_DRAGGED;
		}
		return ret;
	}

	if (!dock.mActive && dock.mStatus != eSTAT_DRAGGED) return false;

	mEndAction = eEnd_ACTION_END_CHILD;
	Split();

	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4{ 0,0,0,0 });
	ImGui::PushStyleColor(ImGuiCol_BorderShadow, ImVec4{ 0,0,0,0 });
	float tabbarHeight = ImGui::GetTextLineHeightWithSpacing();

	bool drawTabbar = true;
	Dock& firstTab = dock.GetFirstTab();
	IM_ASSERT(!firstTab.mpPrevTab);
	if (!firstTab.mpNextTab)
	{
		if (_flags & ImGuiWindowFlags_NoTitleBar)
		{
			if (!firstTab.mpParent && !firstTab.HasChildren())
				drawTabbar = false;
		}
	}

	ImVec2 pos = dock.mPos;
	ImVec2 size = dock.mSize;

	if (drawTabbar)
	{
		if (Tabbar(firstTab, _opened != nullptr))
		{
			FillLocation(dock);
			*_opened = false;
		}
		pos.y += tabbarHeight + ImGui::GetStyle().WindowPadding.y;
		size.y -= tabbarHeight + ImGui::GetStyle().WindowPadding.y;
	}

	ImGui::SetCursorScreenPos(pos);
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoTitleBar |
							 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
							 ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings |
							 ImGuiWindowFlags_NoBringToFrontOnFocus | _flags;

	std::string tempBuffer;
	tempBuffer = _label;
	tempBuffer += "_docled";
	bool ret = ImGui::BeginChild(tempBuffer.c_str(), size, true, flags);
	ImGui::PopStyleColor(2);
	
	if (dock.mStatus == eSTAT_DRAGGED)
	{
		ImGuiWindow *pWin = ImGui::GetCurrentWindow();
		pWin->BeginOrderWithinParent = 100000;
	}

	return ret;
}

bool DockContext::DockSlots(Dock& _dock, Dock* _destDock, const ImRect& _rect, bool _border)
{
	ImDrawList *canvas = ImGui::GetWindowDrawList();
	ImU32 color = ImGui::GetColorU32(ImGuiCol_Button);
	ImU32 colorHovered = ImGui::GetColorU32(ImGuiCol_ButtonHovered);
	ImU32 dockedRectCol = color;
	ImVec2 mousePos = ImGui::GetIO().MousePos;
	ImTextureID texture = nullptr;

	for (int i = 0; i < (_border ? 4 : 5); ++i)
	{
		const eDockSlot iSlot = static_cast<eDockSlot>(i);
		ImRect r = _border ? GetSlotRectOnBorder(_rect, iSlot) : GetSlotRect(_rect, iSlot);
		bool hovered = r.Contains(mousePos);
		ImU32 colorToUse = hovered ? colorHovered : color;
		if (!texture) 
			canvas->AddRectFilled(r.Min, r.Max, colorToUse);

		if (!hovered) continue;
		if (!ImGui::IsMouseDown(0))
		{
			DoDock(_dock, _destDock ? _destDock : GetRootDock(), iSlot);
			return true;
		}
		ImRect dockedRect = GetDockedRect(_rect, iSlot);
		canvas->AddRectFilled(dockedRect.Min, dockedRect.Max, dockedRectCol);
	}
	return false;
}

bool DockContext::Tabbar(Dock& _dock, bool _closeBtn)
{
	float tabbarHeight = 2 * ImGui::GetTextLineHeightWithSpacing();
	ImVec2 sizeTmp{ _dock.mSize.x, tabbarHeight };
	bool tabClosed = false;

	ImGui::SetCursorScreenPos(_dock.mPos);
	char tempBuffer[20];
	ImFormatString(tempBuffer, IM_ARRAYSIZE(tempBuffer), "tabs%d", static_cast<int>(_dock.mId));

	if (ImGui::BeginChild(tempBuffer, sizeTmp, true))
	{
		Dock* dockTab = &_dock;
		ImDrawList *drawList = ImGui::GetWindowDrawList();
		ImU32 color = ImGui::GetColorU32(ImGuiCol_FrameBg);
		ImU32 colorActivate = ImGui::GetColorU32(ImGuiCol_FrameBgActive);
		ImU32 colorHover = ImGui::GetColorU32(ImGuiCol_FrameBgHovered);
		ImU32 buttonHovered = ImGui::GetColorU32(ImGuiCol_ButtonHovered);
		ImU32 textColor = ImGui::GetColorU32(ImGuiCol_Text);
		float lineHeight = ImGui::GetTextLineHeightWithSpacing();
		float tabBase;

		DrawTabbarListBtn(_dock);

		while (dockTab)
		{
			ImGui::SameLine(0, 15);
			const char* textEnd = ImGui::FindRenderedTextEnd(dockTab->mpLabel);
			ImVec2 size{ImGui::CalcTextSize(dockTab->mpLabel, textEnd).x, lineHeight};
			if (ImGui::InvisibleButton(dockTab->mpLabel, size))
			{
				dockTab->SetActive();
				mpNextParent = dockTab;
			}

			if (ImGui::IsItemActive() && ImGui::IsMouseDragging())
			{
				mDragOffset = ImVec2{ ImGui::GetMousePos().x - dockTab->mPos.x, ImGui::GetMousePos().y - dockTab->mPos.y };
				DoUndock(*dockTab);
				dockTab->mStatus = eSTAT_DRAGGED;
			}

			if (dockTab->mActive && _closeBtn)
				size.x += 16 + ImGui::GetStyle().ItemSpacing.x;

			bool hovered = ImGui::IsItemHovered();
			ImVec2 pos = ImGui::GetItemRectMin();
			tabBase = pos.y;
			drawList->PathClear();
			drawList->PathLineTo(ImVec2{ pos.x -15,pos.y + size.y });
			drawList->PathBezierCurveTo(ImVec2{pos.x - 10, pos.y + size.y},
										ImVec2{ pos.x - 5, pos.x },
										pos, 10);
			drawList->PathLineTo(ImVec2{pos.x + size.x, pos.y});
			drawList->PathBezierCurveTo(ImVec2{pos.x + size.x + 5, pos.y},
										ImVec2{pos.x + size.x + 10, pos.y + size.y},
										ImVec2{pos.x + size.x + 15, pos.y + size.y}, 10);
			drawList->PathFillConvex(hovered ? colorHover : (dockTab->mActive ? colorActivate : color));
			drawList->AddText(ImVec2{ pos.x, pos.y + 1 }, textColor, dockTab->mpLabel, textEnd);
			
			if (dockTab->mActive && _closeBtn)
			{
				size.x += 16 + ImGui::GetStyle().ItemSpacing.x;
				ImGui::SameLine();
				tabClosed = ImGui::InvisibleButton("close", ImVec2{ 16,16 });
				ImVec2 v1 = ImGui::GetItemRectMin();
				ImVec2 v2 = ImGui::GetItemRectMax();
				ImVec2 center{(v1.x + v2.x) * 0.5f, (v1.y + v2.y) * 0.5f};
				if (ImGui::IsItemHovered())
				{
					drawList->AddRectFilled(ImVec2{ center.x - 6.f, center.y - 6.f },
											ImVec2{ center.x + 7.f, center.y + 7.f },
											buttonHovered);
				}
				drawList->AddLine(ImVec2{ center.x - 3.5f, center.y - 3.5f },
								  ImVec2{ center.x + 3.5f, center.y + 3.5f }, textColor);
				drawList->AddLine(ImVec2{ center.x - 3.5f, center.y - 3.5f },
								  ImVec2{ center.x + 3.5f, center.y + 3.5f }, textColor);
			}
			dockTab = dockTab->mpNextTab;
		}
		ImVec2 cp{_dock.mPos.x, tabBase + lineHeight};
		drawList->AddLine(cp, ImVec2{ cp.x + _dock.mSize.x, cp.y }, color);
	}
	ImGui::EndChild();
	return tabClosed;
}

int DockContext::GetDockIndex(Dock* _dock)
{
	if (!_dock) return -1;

	for (unsigned int i = 0; i < mDocksArr.size(); ++i)
	{
		if (_dock == mDocksArr[i]) return i;
	}

	IM_ASSERT(false);
	return -1;
}

void DockContext::SetNextDockSlot(eDockSlot e) { mNextDockSlot = e; }
void DockContext::SetWorkspacePos(ImVec2 e) { mWorkspacePos = e; }
void DockContext::SetWorkspaceSize(ImVec2 e) { mWorkspaceSize = e; }

void DockContext::DebugWindow()
{
	if (ImGui::Begin("Dock Debug Info"))
	{
		for (unsigned int i = 0; i < mDocksArr.size(); ++i)
		{
			if (ImGui::TreeNode(reinterpret_cast<void*>(static_cast<int>(i)), "Dock %d (%p)", i, mDocksArr[i]))
			{
				Dock& dock = *mDocksArr[i];
				ImGui::Text("pos-(%.1f %.1f) size = (%.1f %.1f)",
					dock.mPos.x, dock.mPos.y, dock.mSize.x, dock.mSize.y);
				ImGui::Text("parent = %p\n", dock.mpParent);
				ImGui::Text("isContainer() == %s\n", dock.HasChildren() ? "true" : "false");
				ImGui::Text("status = %s\n",
					(dock.mStatus == eSTAT_DOCK) ? "Docked" : ((dock.mStatus == eSTAT_DRAGGED) ? "Dragged" : ((dock.mStatus == eSTAT_FLOAT) ? "Float" : "?")));
				ImGui::TreePop();
			}
		}
	}
	ImGui::End();
}

DockContext* CreateContext()
{
	DockContext *pDockCtx = (DockContext*)ImGui::MemAlloc(sizeof(DockContext));
	IM_PLACEMENT_NEW(pDockCtx) DockContext();
	return pDockCtx;
}

void DestroyContext(DockContext *_dock)
{
	if (_dock == g_pDock)
		g_pDock = nullptr;
	if (_dock != nullptr)
	{
		_dock->~DockContext();
		ImGui::MemFree(_dock);
	}
}

void SetContext(DockContext *_ctx)
{
	g_pDock = _ctx;
}

DockContext* GetContext()
{
	return g_pDock;
}

void BeginSpace()
{
	IM_ASSERT(g_pDock != NULL && "No current context. Did you call ImGui::CreateDockContext() or ImGui::SetCurrentDockContext()?");
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar;
	
	ImGui::BeginChild("###workspace", ImVec2{ 0,0 }, false, flags);
	g_pDock->SetWorkspacePos(ImGui::GetWindowPos());
	g_pDock->SetWorkspaceSize(ImGui::GetWindowSize());
}

void EndSpace()
{
	ImGui::EndChild();
}

void Shutdown()
{
	IM_ASSERT(g_pDock != nullptr && "No current context.Did you call ImGui::CreateDockContext() or ImGui::SetCurrentDockContext() ? ");
	g_pDock->Shutdown();
}

void SetNextDock(eDockSlot _slot)
{
	IM_ASSERT(g_pDock != NULL && "No current context. Did you call ImGui::CreateDockContext() or ImGui::SetCurrentDockContext()?");
	g_pDock->SetNextDockSlot(_slot);
}

bool BeginDock(const char *_label, bool *_opened, ImGuiWindowFlags _flags, const ImVec2& _defSize)
{
	IM_ASSERT(g_pDock != NULL && "No current context. Did you call ImGui::CreateDockContext() or ImGui::SetCurrentDockContext()?");
	return g_pDock->Begin(_label, _opened, _flags, _defSize);
}

void EndDock()
{
	IM_ASSERT(g_pDock != NULL && "No current context. Did you call ImGui::CreateDockContext() or ImGui::SetCurrentDockContext()?");
	g_pDock->End();
}

void SetActive()
{
	IM_ASSERT(g_pDock != NULL && "No current context. Did you call ImGui::CreateDockContext() or ImGui::SetCurrentDockContext()?");
	g_pDock->SetDockActive();
}

void DebugWindow()
{
	IM_ASSERT(g_pDock != NULL && "No current context. Did you call ImGui::CreateDockContext() or ImGui::SetCurrentDockContext()?");
	g_pDock->DebugWindow();
}




}}} // namespace DYSTOPIA::EGUI::DOCK

#endif //EDITOR










