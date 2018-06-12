/* HEADER *********************************************************************************/
/*!
\file	Dock.cpp
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
#include <iostream>
#include <map>
#include <string>

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

	//_dock.mPos.x = (_dock.mPos.x < 0) ? 0 : ((_dock.mPos.x > 100) ? 100 : _dock.mPos.x);
	//_dock.mPos.y = (_dock.mPos.y < 0) ? 0 : _dock.mPos.y;

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

	for (unsigned int i = 0, c = mDocksArr.size(); i < c; ++i)
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



/*========================================================= Declaration ==========================================================*/

namespace EGUI2
{
	namespace Docking
	{
		enum eAction
		{
			eACTION_NONE,
			eACTION_PANEL,
			eACTION_END,
			eACTION_END_CHILD
		};

		enum eStatus
		{
			eSTATUS_DOCKED,
			eSTATUS_FLOATING,
			eSTATUS_DRAGGED
		};

		struct DockTab
		{
			char	*mpLabel;
			DockTab *mpNextTab;
			DockTab *mpPrevTab;
			DockTab *mpParentTab;
			DockTab *mpChildren[2];
			ImVec2	mPosition;
			ImVec2	mSize;
			eStatus	mStatus;
			ImU32	mID;
			bool	mActive;
			bool	mOpened;
			bool	mFirst;
			int		mLastFrame;
			int		mInvalidFrame;
			char	mLocation[16];

			DockTab();
			~DockTab();

			bool IsHorizontal() const;
			ImVec2 GetMinSize() const;
			void SetParent(DockTab *_tab);
			DockTab& GetRootTab();
			DockTab& GetFirstTab();
			DockTab& GetSiblingTab();
			void SetActive();
			bool IsContainer() const;
			void SetPositionSize(const ImVec2&, const ImVec2&);
			void SetChildPositionSize(const ImVec2&, const ImVec2&);
		};
	}
}

/*========================================================= Definitions ==========================================================*/

namespace EGUI2 {
namespace Docking {

	DockTab::DockTab()
		: mID{ 0 }, mpNextTab{ nullptr }, mpPrevTab{ nullptr }, mpParentTab{ nullptr },
		mPosition{ ImVec2{ 0, 0 } }, mSize{ ImVec2{ 0,0 } }, mActive{ true }, mStatus{ eSTATUS_FLOATING },
		mpLabel{ nullptr }, mOpened{ false }, mLocation{ 0 }, mpChildren{ nullptr }, mFirst{ false },
		mLastFrame{ 0 }, mInvalidFrame{ 0 }
	{
	}

	DockTab::~DockTab()
	{
		ImGui::MemFree(mpLabel);
	}

	bool DockTab::IsHorizontal() const
	{
		return mpChildren[0]->mPosition.x < mpChildren[1]->mPosition.x;
	}

	ImVec2 DockTab::GetMinSize() const
	{
		if (!mpChildren[0]) return ImVec2{ 16, 16 + ImGui::GetTextLineHeightWithSpacing() };

		ImVec2 size0 = mpChildren[0]->GetMinSize();
		ImVec2 size1 = mpChildren[1]->GetMinSize();
		return IsHorizontal() ? ImVec2{ size0.x + size1.x, ImMax(size0.y, size1.y) }
		: ImVec2{ ImMax(size0.x, size1.x), size0.y + size1.y };
	}

	void DockTab::SetParent(DockTab *_tab)
	{
		mpParentTab = _tab;
		for (DockTab *tempTab = mpPrevTab; tempTab; tempTab = tempTab->mpPrevTab) tempTab->mpParentTab = _tab;
		for (DockTab *tempTab = mpNextTab; tempTab; tempTab = tempTab->mpNextTab) tempTab->mpParentTab = _tab;
	}

	DockTab& DockTab::GetRootTab()
	{
		DockTab *tab = this;
		while (tab->mpParentTab)
			tab = tab->mpParentTab;
		return *tab;
	}

	DockTab& DockTab::GetFirstTab()
	{
		DockTab *tmpTab = this;
		while (tmpTab->mpPrevTab)
			tmpTab = tmpTab->mpPrevTab;
		return *tmpTab;
	}

	DockTab& DockTab::GetSiblingTab()
	{
		IM_ASSERT(mpParentTab);
		return (mpParentTab->mpChildren[0] == &GetFirstTab()) ? *mpParentTab->mpChildren[1]
			: *mpParentTab->mpChildren[0];
	}

	void DockTab::SetActive()
	{
		mActive = true;
		for (DockTab *tmp = mpPrevTab; tmp; tmp = tmp->mpPrevTab) tmp->mActive = false;
		for (DockTab *tmp = mpNextTab; tmp; tmp = tmp->mpNextTab) tmp->mActive = false;
	}

	bool DockTab::IsContainer() const
	{
		return mpChildren[0] != nullptr;
	}

	void DockTab::SetPositionSize(const ImVec2& _pos, const ImVec2& _size)
	{
		mSize = _size;
		mPosition = _pos;
		for (DockTab *tmp = mpPrevTab; tmp; tmp = tmp->mpPrevTab)
		{
			tmp->mSize = _size;
			tmp->mPosition = _pos;
		}
		for (DockTab *tmp = mpNextTab; tmp; tmp = tmp->mpNextTab)
		{
			tmp->mSize = _size;
			tmp->mPosition = _pos;
		}

		if (!IsContainer()) return;
		SetChildPositionSize(_pos, _size);
	}

	void DockTab::SetChildPositionSize(const ImVec2& _pos, const ImVec2& _size)
	{
		ImVec2 newSize = mpChildren[0]->mSize;
		if (IsHorizontal())
		{
			newSize.y = _size.y;
			float ratio = _size.x * mpChildren[0]->mSize.x;
			ratio /= (mpChildren[0]->mSize.x + mpChildren[1]->mSize.x);
			newSize.x = static_cast<float>(ratio);

			if (newSize.x < mpChildren[0]->GetMinSize().x)
				newSize.x = mpChildren[0]->GetMinSize().x;
			else if (_size.x - newSize.x < mpChildren[1]->GetMinSize().x)
				newSize.x = _size.x - mpChildren[1]->GetMinSize().x;

			mpChildren[0]->SetPositionSize(_pos, newSize);
			newSize.x = _size.x - mpChildren[0]->mSize.x;

			ImVec2 newPos = _pos;
			newPos.x += mpChildren[0]->mSize.x;
			mpChildren[1]->SetPositionSize(newPos, newSize);
		}
		else
		{
			newSize.x = _size.x;
			float ratio = _size.y * mpChildren[0]->mSize.y;
			ratio /= (mpChildren[0]->mSize.y + mpChildren[1]->mSize.y);
			newSize.y = static_cast<float>(ratio);

			if (newSize.y < mpChildren[0]->GetMinSize().y)
				newSize.y = mpChildren[0]->GetMinSize().y;
			else if (_size.y - newSize.y < mpChildren[1]->GetMinSize().y)
				newSize.y = _size.y - mpChildren[1]->GetMinSize().y;

			mpChildren[0]->SetPositionSize(_pos, newSize);
			newSize.y = _size.y - mpChildren[0]->mSize.y;

			ImVec2 newPos = _pos;
			newPos.y += mpChildren[0]->mSize.y;
			mpChildren[1]->SetPositionSize(newPos, newSize);
		}
	}

	struct DockPanel
	{
		AutoArray<DockTab*> mArrDockTabs;
		DockTab		*mpCurrentTab;
		DockTab		*mpNextParentTab;
		ImVec2		mDragOffset;
		int			mLastFrame;
		eAction		mAction;
		ImVec2		mPanelPosition;
		ImVec2		mPanelSize;
		eDockSlot	mNextDockSlot;

		DockPanel()
			: mpCurrentTab{ nullptr }, mpNextParentTab{ nullptr }, mDragOffset{ ImVec2{ 0, 0 } },
			mLastFrame{ 0 }, mAction{ eACTION_NONE }, mPanelPosition{ ImVec2{ 0, 0} }, mPanelSize{ ImVec2{ 0, 0 } },
			mNextDockSlot{ eDOCK_SLOT_TAB }
		{
		}

		~DockPanel()
		{
		}

		DockTab& GetDockTab(const char* _label, bool _opened)
		{
			ImU32 id = ImHash(_label, 0);
			for (auto e : mArrDockTabs)
			{
				if (e->mID == id)
					return *e;
			}

			DockTab* newTab = new DockTab;
			mArrDockTabs.push_back(newTab);

			newTab->mpLabel = new char[strlen(_label) + 1];
			newTab->mpLabel = ImStrdup(_label);
			newTab->mID = id;
			newTab->SetActive();
			newTab->mStatus = (mArrDockTabs.size() == 1) ? eSTATUS_DOCKED : eSTATUS_FLOATING;
			newTab->mSize = ImGui::GetIO().DisplaySize;
			newTab->mOpened = _opened;
			newTab->mFirst = true;
			newTab->mLocation[0] = 0;
			return *newTab;
		}

		void PutInBackground()
		{
			ImGuiWindow *pWin = ImGui::GetCurrentWindow();
			ImGuiContext& ctx = *GImGui;
			if (ctx.Windows[0] == pWin) return;

			for (int i = 0; i < ctx.Windows.Size; ++i)
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

		void GetDrawPresets(ImU32 *_outColor, ImU32 *_outColorHovered, ImDrawList **_outDrawList)
		{
			*_outColor = ImGui::GetColorU32(ImGuiCol_Button);
			*_outColorHovered = ImGui::GetColorU32(ImGuiCol_ButtonHovered);
			*_outDrawList = ImGui::GetWindowDrawList();
		}

		void Split()
		{
			if (ImGui::GetFrameCount() == mLastFrame) return;

			mLastFrame = ImGui::GetFrameCount();
			PutInBackground();

			for (auto e : mArrDockTabs)
			{
				if (!e->mpParentTab && (e->mStatus == eSTATUS_DOCKED))
					e->SetPositionSize(mPanelPosition, mPanelSize);
			}

			ImU32 color;
			ImU32 colorHovered;
			ImDrawList *pDrawList;
			GetDrawPresets(&color, &colorHovered, &pDrawList);

			ImGuiIO& io = ImGui::GetIO();

			for (unsigned int i = 0; i < mArrDockTabs.size(); ++i)
			{
				DockTab& tab = *mArrDockTabs[i];
				if (!tab.IsContainer()) continue;

				ImGui::PushID(i);
				if (!ImGui::IsMouseDown(0)) tab.mStatus = eSTATUS_DOCKED;

				ImVec2 pos0 = tab.mpChildren[0]->mPosition;
				ImVec2 pos1 = tab.mpChildren[1]->mPosition;
				ImVec2 size0 = tab.mpChildren[0]->mSize;
				ImVec2 size1 = tab.mpChildren[1]->mSize;

				ImGuiMouseCursor cursor;

				ImVec2 dSize{ 0,0 };
				ImVec2 minSize0 = tab.mpChildren[0]->GetMinSize();
				ImVec2 minSize1 = tab.mpChildren[1]->GetMinSize();

				if (tab.IsHorizontal())
				{
					cursor = ImGuiMouseCursor_ResizeEW;
					ImGui::SetCursorScreenPos(ImVec2{ tab.mPosition.x + size0.x, tab.mPosition.y });
					ImGui::InvisibleButton("split", ImVec2{ 3, tab.mSize.y });

					if (tab.mStatus == eSTATUS_DRAGGED) dSize.x = io.MouseDelta.x;

					dSize.x = -ImMin(-dSize.x, tab.mpChildren[0]->mSize.x - minSize0.x);
					dSize.x = ImMin(dSize.x, tab.mpChildren[1]->mSize.x - minSize1.x);

					size0 += dSize;
					size1 -= dSize;
					pos0 = tab.mPosition;
					pos1.x = pos0.x + size0.x;
					pos1.y = tab.mPosition.y;
					size0.y = size1.y = tab.mSize.y;
					size0.x = ImMax(minSize0.x, tab.mSize.x - size1.x);
					size1.x = ImMax(minSize1.x, tab.mSize.x - size0.x);
				}
				else
				{
					cursor = ImGuiMouseCursor_ResizeNS;
					ImGui::SetCursorScreenPos(ImVec2{ tab.mPosition.x, tab.mPosition.y + size0.y - 3 });
					ImGui::InvisibleButton("split", ImVec2{ tab.mSize.x, 3 });
					if (tab.mStatus == eSTATUS_DRAGGED) dSize.y = io.MouseDelta.y;

					dSize.y = -ImMin(-dSize.y, tab.mpChildren[0]->mSize.y - minSize0.y);
					dSize.y = ImMin(dSize.y, tab.mpChildren[1]->mSize.y - minSize1.y);


					size0 += dSize;
					size1 -= dSize;
					pos0 = tab.mPosition;
					pos1.x = tab.mPosition.x;
					pos1.y = pos0.y + size0.y;
					size0.x = size1.x = tab.mSize.x;
					size0.y = ImMax(minSize0.y, tab.mSize.y - size1.y);
					size1.y = ImMax(minSize1.y, tab.mSize.y - size0.y);
				}

				tab.mpChildren[0]->SetPositionSize(pos0, size0);
				tab.mpChildren[1]->SetPositionSize(pos1, size1);

				if (ImGui::IsItemHovered())
				{
					ImGui::SetMouseCursor(cursor);
					if (ImGui::IsMouseClicked(0)) tab.mStatus = eSTATUS_DRAGGED;
				}

				pDrawList->AddRectFilled(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(),
					ImGui::IsItemHovered() ? colorHovered : color);
				ImGui::PopID();
			}
		}

		void CheckNonExistent()
		{
			int frameLimit = ImMax(0, ImGui::GetFrameCount() - 2);
			for (auto e : mArrDockTabs)
			{
				if (e->IsContainer()) continue;
				if (e->mStatus == eSTATUS_FLOATING) continue;
				if (e->mLastFrame < frameLimit)
				{
					++e->mInvalidFrame;
					if (e->mInvalidFrame > 2)
					{
						DoUndockTab(e);
						e->mStatus = eSTATUS_FLOATING;
					}
					return;
				}
				e->mInvalidFrame = 0;
			}
		}

		DockTab* GetTabAt() const
		{
			for (auto e : mArrDockTabs)
			{
				if (e->IsContainer()) continue;
				if (e->mStatus != eSTATUS_DOCKED) continue;
				if (ImGui::IsMouseHoveringRect(e->mPosition, e->mPosition + e->mSize, false))
					return e;

			}
			return nullptr;
		}

		static ImRect GetDockedTabRect(const ImRect& _rect, eDockSlot _slot)
		{
			ImVec2 halfSize = _rect.GetSize() * 0.5f;
			ImVec2 maxMidY{ _rect.Max.x, _rect.Min.y + halfSize.y };
			ImVec2 midX{ halfSize.x, 0 };
			ImVec2 midY{ 0, halfSize.y };
			ImVec2 maxMidX{ _rect.Min.x + halfSize.x, _rect.Max.y };
			switch (_slot)
			{
			default: return _rect;
			case eDOCK_SLOT_TOP:
				return ImRect(_rect.Min, maxMidY);
			case eDOCK_SLOT_RIGHT:
				return ImRect(_rect.Min + midX, _rect.Max);
			case eDOCK_SLOT_BOTTOM:
				return ImRect(_rect.Min + midY, _rect.Max);
			case eDOCK_SLOT_LEFT:
				return ImRect(_rect.Min, maxMidX);
			}
		}

		static ImRect GetSlotRect(ImRect _parentRect, eDockSlot _slot)
		{
			ImVec2 size = _parentRect.Max - _parentRect.Min;
			ImVec2 center = _parentRect.Min + size * 0.5f;
			ImVec2 diagonalOffset{ 20, 20 };
			ImVec2 topSlotMin{ -20, -50 };
			ImVec2 topSlotMax{ 20, -30 };
			ImVec2 rightSlotMin{ 30, -20 };
			ImVec2 rightSlotMax{ 50, 20 };
			ImVec2 bottomSlotMin{ -20, 30 };
			ImVec2 bottomSlotMax{ 20, 50 };
			ImVec2 leftSlotMin{ -50, -20 };
			ImVec2 leftSlotMax{ -30, 20 };
			switch (_slot)
			{
			default:
				return ImRect{ center - diagonalOffset, center + diagonalOffset };
			case eDOCK_SLOT_TOP:
				return ImRect{ center + topSlotMin, center + topSlotMax };
			case eDOCK_SLOT_RIGHT:
				return ImRect{ center + rightSlotMin, center + rightSlotMax };
			case eDOCK_SLOT_BOTTOM:
				return ImRect{ center + bottomSlotMin, center + bottomSlotMax };
			case eDOCK_SLOT_LEFT:
				return ImRect{ center + leftSlotMin, center + leftSlotMax };
			}
		}

		static ImRect GetBorderSlotRect(ImRect _parentRect, eDockSlot _slot)
		{
			ImVec2 size = _parentRect.Max - _parentRect.Min;
			ImVec2 center = _parentRect.Min + size * 0.5f;
			ImVec2 topSlotMin{ center.x - 20, _parentRect.Min.y + 10 };
			ImVec2 topSlotMax{ center.x + 20, _parentRect.Min.y + 30 };
			ImVec2 rightSlotMin{ _parentRect.Min.x + 10, center.y - 20 };
			ImVec2 rightSlotMax{ _parentRect.Min.x + 30, center.y + 20 };
			ImVec2 bottomSlotMin{ center.x - 20, _parentRect.Max.y - 30 };
			ImVec2 bottomSlotMax{ center.x + 20, _parentRect.Max.y - 10 };
			ImVec2 leftSlotMin{ _parentRect.Max.x - 30, center.y - 20 };
			ImVec2 leftSlotMax{ _parentRect.Max.x - 10, center.y + 20 };

			switch (_slot)
			{
			case eDOCK_SLOT_TOP:
				return ImRect{ topSlotMin, topSlotMax };
			case eDOCK_SLOT_LEFT:
				return ImRect{ center + rightSlotMin, center + rightSlotMax };
			case eDOCK_SLOT_BOTTOM:
				return ImRect{ center + bottomSlotMin, center + bottomSlotMax };
			case eDOCK_SLOT_RIGHT:
				return ImRect{ center + leftSlotMin, center + leftSlotMax };
			default: IM_ASSERT(false);
			}
			IM_ASSERT(false);
			return ImRect{};
		}

		DockTab* GetRootTab()
		{
			for (auto e : mArrDockTabs)
			{
				if (!e->mpParentTab && (e->mStatus == eSTATUS_DOCKED || e->mpChildren[0]))
					return e;
			}
			return nullptr;
		}

		bool DockTabSlots(DockTab& _tab, DockTab *_dest, const ImRect& _rect, bool _border)
		{
			ImDrawList *pDrawList;
			ImU32 color;
			ImU32 colorHovered;
			ImVec2 mousePos = ImGui::GetIO().MousePos;
			
			GetDrawPresets(&color, &colorHovered, &pDrawList);
			
			for (int i = 0; i < (_border ? 4 : 5); ++i)
			{
				ImRect tempRect = _border ? GetBorderSlotRect(_rect, static_cast<eDockSlot>(i))
										  : GetSlotRect(_rect, static_cast<eDockSlot>(i));

				bool hovered = tempRect.Contains(mousePos);
				pDrawList->AddRectFilled(tempRect.Min, tempRect.Max, hovered ? colorHovered : color);
				if (!hovered) continue;

				if (!ImGui::IsMouseDown(0))
				{
					DoDockTab(_tab, _dest ? _dest : GetRootTab(), static_cast<eDockSlot>(i));
					return true;
				}

				ImRect dockedRect = GetDockedTabRect(_rect, static_cast<eDockSlot>(i));
				pDrawList->AddRectFilled(dockedRect.Min, dockedRect.Max, ImGui::GetColorU32(ImGuiCol_Button));
			}
			return false;
		}

		void HandleDragging(DockTab& _tab)
		{
			DockTab* dest = GetTabAt();
			ImGuiWindowFlags flags = ImGuiWindowFlags_Tooltip | ImGuiWindowFlags_NoTitleBar |
									 ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
									 ImGuiWindowFlags_NoSavedSettings | 
									 ImGuiWindowFlags_AlwaysAutoResize;
			
			ImGui::SetNextWindowPos(ImVec2{ 0,0 });
			ImGui::SetNextWindowBgAlpha(0.f);
			ImGui::Begin("##overlay", nullptr, flags);
			
			ImDrawList *pDrawList = ImGui::GetWindowDrawList();
			pDrawList->PushClipRectFullScreen();

			ImU32 dockedColor = ImGui::GetColorU32(ImGuiCol_FrameBg);
			dockedColor = (dockedColor & 0x00ffFFFF) | 0x80000000;
			_tab.mPosition = ImGui::GetIO().MousePos - mDragOffset;

			if (dest)
			{
				if (DockTabSlots(_tab, dest, ImRect{dest->mPosition, dest->mPosition + dest->mSize}, false))
				{
					pDrawList->PopClipRect();
					ImGui::End();
					return;
				}
			}

			if (DockTabSlots(_tab, nullptr, ImRect{ mPanelPosition, mPanelPosition + mPanelSize }, true))
			{
				pDrawList->PopClipRect();
				ImGui::End();
				return;
			}

			pDrawList->AddRectFilled(_tab.mPosition, _tab.mPosition + _tab.mSize, dockedColor);
			pDrawList->PopClipRect();

			if (!ImGui::IsMouseDown(0))
			{
				_tab.mStatus = eSTATUS_FLOATING;
				_tab.mLocation[0] = 0;
				_tab.SetActive();
			}
			ImGui::End();
		}

		void FillLocation(DockTab& _tab)
		{
			if (_tab.mStatus == eSTATUS_FLOATING) return;

			char *c = _tab.mLocation;
			DockTab* temp = &_tab;
			while (temp->mpParentTab)
			{
				*c = GetLocationCode(temp);
				temp = temp->mpParentTab;
				++c;
			}
			*c = 0;
		}

		void DoUndockTab(DockTab *_tab)
		{
			if (_tab->mpPrevTab)
				_tab->mpPrevTab->SetActive();
			else if (_tab->mpNextTab)
				_tab->mpNextTab->SetActive();
			else
				_tab->mActive = false;
			
			DockTab *pContainer = _tab->mpParentTab;
			if (pContainer)
			{
				DockTab& siblingTab = _tab->GetSiblingTab();
				if (pContainer->mpChildren[0] == _tab)
					pContainer->mpChildren[0] = _tab->mpNextTab;
				else if (pContainer->mpChildren[1] == _tab)
					pContainer->mpChildren[1] = _tab->mpNextTab;

				bool remove = !pContainer->mpChildren[0] || !pContainer->mpChildren[1];
				if (remove)
				{
					if (pContainer->mpParentTab)
					{
						DockTab *pChild = pContainer->mpParentTab->mpChildren[0] == pContainer 
							? pContainer->mpParentTab->mpChildren[0]
							: pContainer->mpParentTab->mpChildren[1];

						pChild = &siblingTab;
						pChild->SetPositionSize(pContainer->mPosition, pContainer->mSize);
						pChild->SetParent(pContainer->mpParentTab);
					}
					else
					{
						if (pContainer->mpChildren[0])
						{
							pContainer->mpChildren[0]->SetParent(nullptr);
							pContainer->mpChildren[0]->SetPositionSize(pContainer->mPosition, pContainer->mSize);
						}
						if (pContainer->mpChildren[1])
						{
							pContainer->mpChildren[1]->SetParent(nullptr);
							pContainer->mpChildren[1]->SetPositionSize(pContainer->mPosition, pContainer->mSize);
						}


					}
					for (unsigned int i = 0; i < mArrDockTabs.size(); ++i)
					{
						if (mArrDockTabs[i] == pContainer)
						{
							mArrDockTabs.FastRemove(i);
							break;
						}
					}

					if (pContainer == mpNextParentTab)
						mpNextParentTab = nullptr;
					pContainer->~DockTab();
					delete pContainer;
				}
			}
			if (_tab->mpPrevTab) _tab->mpPrevTab->mpNextTab = _tab->mpNextTab;
			if (_tab->mpNextTab) _tab->mpNextTab->mpPrevTab = _tab->mpPrevTab;
			_tab->mpParentTab = _tab->mpPrevTab = _tab->mpNextTab = nullptr;
		}

		void DrawTabbarListButton(DockTab& _tab)
		{
			if (!_tab.mpNextTab) return;

			ImDrawList *pDrawList = ImGui::GetWindowDrawList();
			if (ImGui::InvisibleButton("list", ImVec2{ 16,16 }))
				ImGui::OpenPopup("tabListPopup");

			if (ImGui::BeginPopup("tabListPopup"))
			{
				DockTab *temp = &_tab;
				while (temp)
				{
					bool dummy = false;
					if (ImGui::Selectable(temp->mpLabel, &dummy))
					{
						temp->SetActive();
						mpNextParentTab = temp;
					}
					temp = temp->mpNextTab;
				}
				ImGui::EndPopup();
			}

			ImVec2 min = ImGui::GetItemRectMin();
			ImVec2 max = ImGui::GetItemRectMax();
			ImVec2 center = (min + max) * 0.5f;
			ImU32 color = ImGui::IsItemHovered() 
						? ImGui::GetColorU32(ImGuiCol_FrameBgActive)
						: ImGui::GetColorU32(ImGuiCol_Text);
			pDrawList->AddRectFilled(
				ImVec2{ center.x - 4, min.y + 3 },
				ImVec2{ center.x + 4, min.y + 5 }, 
				color);
			pDrawList->AddTriangleFilled(
				ImVec2{ center.x - 4, min.y + 7 },
				ImVec2{ center.x + 4, min.y + 7 },
				ImVec2{ center.x, min.y + 12 },
				color);
		}

		bool Tabbar(DockTab& _tab, bool _closeBtn)
		{
			float height = 2 * ImGui::GetTextLineHeightWithSpacing();
			ImVec2 size{ _tab.mSize.x, height };
			bool tabClosed = false;

			ImGui::SetCursorScreenPos(_tab.mPosition);
			char buffer[20];
			ImFormatString(buffer, IM_ARRAYSIZE(buffer), "tabs%d", static_cast<int>(_tab.mID));
			if (ImGui::BeginChild(buffer, size, true))
			{
				DockTab* dockTab = &_tab;

				ImDrawList *pDrawList = ImGui::GetWindowDrawList();
				ImU32 color = ImGui::GetColorU32(ImGuiCol_FrameBg);
				ImU32 colorActive = ImGui::GetColorU32(ImGuiCol_FrameBgActive);
				ImU32 colorHovered = ImGui::GetColorU32(ImGuiCol_FrameBgHovered);
				ImU32 textCol = ImGui::GetColorU32(ImGuiCol_Text);
				float lineHeight = ImGui::GetTextLineHeightWithSpacing();
				float tabBase;

				DrawTabbarListButton(_tab);
				while (dockTab)
				{
					ImGui::SameLine(0, 15);

					const char *textEnd = ImGui::FindRenderedTextEnd(dockTab->mpLabel);
					size = ImVec2{ ImGui::CalcTextSize(dockTab->mpLabel, textEnd).x, lineHeight };
					if (ImGui::InvisibleButton(dockTab->mpLabel, size))
					{
						dockTab->SetActive();
						mpNextParentTab = dockTab;
					}

					if (ImGui::IsItemActive() && ImGui::IsMouseDragging())
					{
						mDragOffset = ImGui::GetMousePos() - dockTab->mPosition;
						DoUndockTab(dockTab);
						dockTab->mStatus = eSTATUS_DRAGGED;
					}

					bool hovered = ImGui::IsItemHovered();
					ImVec2 pos = ImGui::GetItemRectMin();
					if (dockTab->mActive && _closeBtn)
					{
						size.x += 16 + ImGui::GetStyle().ItemSpacing.x;
						ImGui::SameLine();
						tabClosed = ImGui::InvisibleButton("close", ImVec2{ 16, 16 });
						ImVec2 center = (ImGui::GetItemRectMin() + ImGui::GetItemRectMax()) * 0.5f;
						ImVec2 offset1{ 3.5f, 3.5f };
						ImVec2 offset2{ -3.5f, 3.5f };
						pDrawList->AddLine(center - offset1, center + offset1, textCol);
						pDrawList->AddLine(center - offset2, center + offset2, textCol);
					}
					
					tabBase = pos.y;
					pDrawList->PathClear();
					pDrawList->PathLineTo(pos + ImVec2{ -15, size.y });
					pDrawList->PathBezierCurveTo(pos + ImVec2{ -10, size.y },
												 pos + ImVec2{ -5, 0 }, 
												 pos, 
												 10);
					pDrawList->PathLineTo(pos + ImVec2{ size.x, 0 });
					pDrawList->PathBezierCurveTo(pos + ImVec2{ size.x + 5, 0 },
												 pos + ImVec2{ size.x + 10, size.y },
												 pos + ImVec2{ size.x + 15, size.y },
												 10);
					pDrawList->PathFillConvex(hovered ? colorHovered : (dockTab->mActive ? colorActive : color));
					pDrawList->AddText(pos + ImVec2{ 0,1 }, textCol, dockTab->mpLabel, textEnd);
					dockTab = dockTab->mpNextTab;
				}
				ImVec2 cp{ _tab.mPosition.x , tabBase + lineHeight };
				pDrawList->AddLine(cp, cp + ImVec2{ _tab.mSize.x, 0 }, color);
			}
			ImGui::EndChild();
			return tabClosed;
		}

		static void SetDockPosSize(DockTab& _dest, DockTab& _tab, eDockSlot _slot, DockTab& _container)
		{
			IM_ASSERT(!_tab.mpPrevTab && !_tab.mpNextTab && !_tab.mpChildren[0] && !_tab.mpChildren[1]);

			_dest.mPosition = _tab.mPosition = _container.mPosition;
			_dest.mSize = _tab.mSize = _container.mSize;

			switch (_slot)
			{
			case eDOCK_SLOT_BOTTOM:
				_dest.mSize.y *= 0.5f;
				_tab.mSize.y *= 0.5f;
				_tab.mPosition.y += _dest.mSize.y;
				break;
			case eDOCK_SLOT_RIGHT:
				_dest.mSize.x *= 0.5f;
				_tab.mSize.x *= 0.5f;
				_tab.mPosition.x += _dest.mSize.x;
				break;
			case eDOCK_SLOT_LEFT:
				_dest.mSize.x *= 0.5f;
				_tab.mSize.x *= 0.5f;
				_dest.mPosition.x += _tab.mSize.x;
				break;
			case eDOCK_SLOT_TOP:
				_dest.mSize.y *= 0.5f;
				_tab.mSize.y *= 0.5f;
				_dest.mPosition.y += _tab.mSize.y;
				break;
			default: 
				IM_ASSERT(false); 
			}
			
			_dest.SetPositionSize(_dest.mPosition, _dest.mSize);

			if (_container.mpChildren[1]->mPosition.x < _container.mpChildren[0]->mPosition.x ||
				_container.mpChildren[1]->mPosition.y < _container.mpChildren[1]->mPosition.y)
			{
				std::swap(_container.mpChildren[0], _container.mpChildren[1]);
			}
		}

		void DoDockTab(DockTab& _tab, DockTab *_dest, eDockSlot _slot)
		{
			IM_ASSERT(!_tab.mpParentTab);
			if (!_dest)
			{
				_tab.mStatus = eSTATUS_DOCKED;
				_tab.SetPositionSize(mPanelPosition, mPanelSize);
			}
			else if (_slot == eDOCK_SLOT_TAB)
			{
				DockTab *temp = _dest;
				while (temp->mpNextTab) temp = temp->mpNextTab;

				auto inLinkList = [](const DockTab *linkedList, const DockTab *checkNode)
				{
					bool isLinkedNode = (linkedList == checkNode);

					const DockTab *tempTab = linkedList;
					while (!isLinkedNode && tempTab->mpPrevTab)
					{
						tempTab = tempTab->mpPrevTab;
						isLinkedNode = (tempTab == checkNode);
					}

					tempTab = linkedList;

					while (!isLinkedNode && tempTab->mpNextTab)
					{
						tempTab = tempTab->mpNextTab;
						isLinkedNode = (tempTab == checkNode);
					}

					return isLinkedNode;
				};

				if (!inLinkList(_dest, &_tab))
				{
					temp->mpNextTab = &_tab;
					_tab.mpPrevTab = temp;
					_tab.mSize = temp->mSize;
					_tab.mPosition = temp->mPosition;
					_tab.mpParentTab = _dest->mpParentTab;
					_tab.mStatus = eSTATUS_DOCKED;
				}
			}
			else if (_slot == eDOCK_SLOT_NONE)
			{
				_tab.mStatus = eSTATUS_FLOATING;
			}
			else
			{
				DockTab *newContainer = new DockTab{};
				mArrDockTabs.push_back(newContainer);
				newContainer->mpChildren[0] = &(_dest->GetFirstTab());
				newContainer->mpChildren[1] = &_tab;
				newContainer->mpParentTab = _dest->mpParentTab;
				newContainer->mSize = _dest->mSize;
				newContainer->mPosition = _dest->mPosition;
				newContainer->mStatus = eSTATUS_DOCKED;
				newContainer->mpLabel = ImStrdup("");

				if (!_dest->mpParentTab)
				{
				}
				else if (&(_dest->GetFirstTab()) == _dest->mpParentTab->mpChildren[0])
					_dest->mpParentTab->mpChildren[0] = newContainer;
				else
					_dest->mpParentTab->mpChildren[1] = newContainer;

				_dest->SetParent(newContainer);
				_tab.mpParentTab = newContainer;
				_tab.mStatus = eSTATUS_DOCKED;
				SetDockPosSize(*_dest, _tab, _slot, *newContainer);
			}
			_tab.SetActive();
		}

		void RootDockTab(const ImVec2& _pos, const ImVec2& _size)
		{
			DockTab *pRoot = GetRootTab();
			if (!pRoot) return;

			ImVec2 minSize = pRoot->GetMinSize();
			ImVec2 requestedSize = _size;
			pRoot->SetPositionSize(_pos, ImMax(minSize, requestedSize));
		}

		static eDockSlot GetSlotFromLocationCode(char code)
		{
			switch (code)
			{
			case '1' : 
				return eDOCK_SLOT_LEFT;
			case '2':
				return eDOCK_SLOT_TOP;
			case '3':
				return eDOCK_SLOT_BOTTOM;
			default :
				return eDOCK_SLOT_RIGHT;
			}
		}
		
		static char GetLocationCode(DockTab *_tab)
		{
			if (!_tab) return '0';
			if (_tab->mpParentTab->IsHorizontal())
			{
				if (_tab->mPosition.x < _tab->mpParentTab->mpChildren[0]->mPosition.x) return '1';
				if (_tab->mPosition.x < _tab->mpParentTab->mpChildren[1]->mPosition.x) return '1';
				return '0';
			}
			else
			{
				if (_tab->mPosition.y < _tab->mpParentTab->mpChildren[0]->mPosition.y) return '2';
				if (_tab->mPosition.y < _tab->mpParentTab->mpChildren[1]->mPosition.y) return '2';
				return '3';
			}
		}

		void TryDockToStoredLocation(DockTab& _tab)
		{
			if (_tab.mStatus == eSTATUS_DOCKED) return;
			if (_tab.mLocation[0] == 0) return;

			DockTab *temp = GetRootTab();
			if (!temp) return;

			DockTab *prev = nullptr;
			char *c = _tab.mLocation + strlen(_tab.mLocation) - 1;
			while (c >= _tab.mLocation && temp)
			{
				prev = temp;
				temp = (*c == GetLocationCode(temp->mpChildren[0])) ? temp->mpChildren[0] : temp->mpChildren[1];
				if (temp) --c;
			}
			DoDockTab(_tab, temp ? temp : prev, temp ? eDOCK_SLOT_TAB : GetSlotFromLocationCode(*c));
		}
		
		bool Begin(const char *_label, bool *_open, ImGuiWindowFlags _flags)
		{
			eDockSlot nextSlot = mNextDockSlot;
			mNextDockSlot = eDOCK_SLOT_TAB;
			DockTab& tab = GetDockTab(_label, !_open || *_open);

			if (!tab.mOpened && (!_open || *_open)) TryDockToStoredLocation(tab);

			tab.mLastFrame = ImGui::GetFrameCount();
			if (strcmp(tab.mpLabel, _label) != 0)
			{
				ImGui::MemFree(tab.mpLabel);
				tab.mpLabel = ImStrdup(_label);
			}
			mAction = eACTION_NONE;

			bool prevOpen = tab.mOpened;
			bool first = tab.mFirst;

			if (tab.mFirst && _open) *_open = tab.mOpened;

			tab.mFirst = first;
			if (_open && !*_open)
			{
				if (tab.mStatus != eSTATUS_FLOATING)
				{
					FillLocation(tab);
					DoUndockTab(&tab);
					tab.mStatus = eSTATUS_FLOATING;
				}
				tab.mOpened = false;
				return false;
			}
			tab.mOpened = true;
			CheckNonExistent();

			if (first || (prevOpen != tab.mOpened))
			{
				DockTab *pRoot = mpNextParentTab ? mpNextParentTab : GetRootTab();
				if (pRoot && (&tab != pRoot) && !tab.mpParentTab)
					DoDockTab(tab, pRoot, nextSlot);
				mpNextParentTab = &tab;
			}

			mpCurrentTab = &tab;
			if (tab.mStatus == eSTATUS_DRAGGED) HandleDragging(tab);
			
			if (tab.mStatus == eSTATUS_FLOATING)
			{
				ImGui::SetNextWindowPos(tab.mPosition);
				ImGui::SetNextWindowSize(tab.mSize);
				ImGui::SetNextWindowBgAlpha(-1.f);
				bool ret = ImGui::Begin(_label, _open, ImGuiWindowFlags_NoCollapse | _flags);
				mAction = eACTION_END;
				tab.mPosition = ImGui::GetWindowPos();
				tab.mSize = ImGui::GetWindowSize();

				ImGuiContext& ctx = *GImGui;

				if (ctx.ActiveId == ImGui::GetCurrentWindow()->GetID("#MOVE") && ctx.IO.MouseDown[0])
				{
					mDragOffset = ImGui::GetMousePos() - tab.mPosition;
					DoUndockTab(&tab); 
					tab.mStatus = eSTATUS_DRAGGED;
				}
				return ret;
			}

			if (!tab.mActive && tab.mStatus != eSTATUS_DRAGGED) return false;

			mAction = eACTION_END_CHILD;
			Split();
			ImGui::PushStyleColor(ImGuiCol_Border, ImVec4{ 0,0,0,0 });
			float tabbarHeight = ImGui::GetTextLineHeightWithSpacing();
			if (Tabbar(tab.GetFirstTab(), _open != nullptr))
			{
				FillLocation(tab);
				*_open = false;
			}

			ImVec2 pos = tab.mPosition;
			ImVec2 size = tab.mSize;
			pos.y += tabbarHeight + ImGui::GetStyle().WindowPadding.y;
			size.y -= tabbarHeight + ImGui::GetStyle().WindowPadding.y;

			ImGui::SetCursorScreenPos(pos);
			ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
									 ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
									 ImGuiWindowFlags_NoSavedSettings |
									 ImGuiWindowFlags_NoBringToFrontOnFocus | _flags;
			bool ret = ImGui::BeginChild(_label, size, true, flags);
			ImGui::PopStyleColor();
			return ret;
		}

		void End()
		{
			mpCurrentTab = nullptr;
			if (mAction == eACTION_END)
				ImGui::End();
			else if (mAction == eACTION_END_CHILD)
			{
				ImGui::PushStyleColor(ImGuiCol_Border, ImVec4{ 0,0,0,0 });
				ImGui::EndChild();
				ImGui::PopStyleColor();
			}
		}
	};

	static std::map<std::string, DockPanel> gPanelList;
	static const char *curDockPanel = nullptr;

	int GetDockPanelIndex(const DockPanel& _context, DockTab *_tab)
	{
		if (!_tab) return -1;

		for (unsigned int i = 0; i < _context.mArrDockTabs.size(); ++i)
		{
			if (_tab == _context.mArrDockTabs[i])
				return i;
		}

		IM_ASSERT(false);
		return -1;
	}

	DockTab* GetDockByIndex(const DockPanel& _context, unsigned int _index)
	{
		return (_index >= 0 && _index < _context.mArrDockTabs.size())
				? _context.mArrDockTabs[_index]
				: nullptr;
	}

	// TODO SOME SERIALIZATION


	bool BeginPanel()
	{
		ImGuiContext& ctx = *GImGui;
		curDockPanel = ctx.CurrentWindow->Name;

		IM_ASSERT(curDockPanel);

		if (!curDockPanel) return false;

		ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar;
		char child_name[1024];
		sprintf_s(child_name, "##%s", curDockPanel);
		bool result = ImGui::BeginChild(child_name, ImVec2{ 0,0 }, false, flags);

		DockPanel& panel = gPanelList[curDockPanel];
		panel.mPanelPosition = ImGui::GetWindowPos();
		panel.mPanelSize = ImGui::GetWindowSize();

		return result;
	}
	
	void EndPanel()
	{
		ImGui::EndChild();
		curDockPanel = nullptr;
	}

	void ShutdownPanel()
	{
		for (auto& e : gPanelList)
		{
			DockPanel& context = e.second;
			for (unsigned int k = 0, count = context.mArrDockTabs.size(); k < count; k++)
			{
				context.mArrDockTabs[k]->~DockTab();
				delete context.mArrDockTabs[k];
				context.mArrDockTabs[k] = nullptr;
			}
			context.mArrDockTabs.clear();
		}
		gPanelList.clear();
	}

	void SetNextSlot(const char *_label, eDockSlot _slot)
	{
		if (_label && gPanelList.find(_label) != gPanelList.end())
			gPanelList[_label].mNextDockSlot = _slot;
	}

	bool BeginTab(const char *_label, bool *_open, ImGuiWindowFlags _flags)
	{
		IM_ASSERT(curDockPanel);
		if (!curDockPanel) return false;

		if (gPanelList.find(curDockPanel) != gPanelList.end())
		{
			DockPanel& context = gPanelList[curDockPanel];

			char newLabel[128];
			sprintf_s(newLabel, "%s##%s", _label, curDockPanel);
			return context.Begin(newLabel, _open, _flags);
		}
		return false;
	}

	void EndTab()
	{
		IM_ASSERT(curDockPanel);
		if (!curDockPanel) return;

		if (gPanelList.find(curDockPanel) != gPanelList.end())
		{
			DockPanel& context = gPanelList[curDockPanel];
			context.End();
		}
	}

	void InitTab()
	{
		ImGuiContext& gtx = *GImGui;
		gtx;
	}


}} // namespace EGUI::Dock



#endif //EDITOR










