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
#include "../../Dependancies/ImGui/imgui.h"
#include "../../Dependancies/ImGui/imgui_internal.h"
#include "Editor\EGUI.h"
#include "Editor\Dock.h"

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

	ImVec2 max{ pDestDock->mPos.x + pDestDock->mSize.x , pDestDock->mPos.y + pDestDock->mSize.y };
	if (pDestDock)
	{
		if (DockSlots(_dock, pDestDock, ImRect{pDestDock->mPos, max}, false))
		{
			canvas->PopClipRect();
			End();
			return;
		}
	}
	
	max = ImVec2{mWorkspacePos.x + mWorkspaceSize.x, mWorkspacePos .y + mWorkspaceSize.y};
	if (DockSlots(_dock, nullptr, ImRect{ mWorkspacePos, max }, true))
	{
		canvas->PopClipRect();
		End();
		return;
	}

	max = ImVec2{_dock.mPos.x + _dock.mSize.x, _dock.mPos.y + _dock.mSize.y };
	canvas->AddRectFilled(_dock.mPos, max, dockedColor);
	canvas->PopClipRect();
	if (!ImGui::IsMouseDown(0))
	{
		_dock.mStatus = eSTAT_FLOAT;
		_dock.mLocation[0] = 0;
		_dock.SetActive();
	}
	End();
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

			for (int i = 0; i < mDocksArr.size(); ++i)
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

void DockContext::DoDock(Dock&, Dock*, eDockSlot)
{
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

void DockContext::RootDock(const ImVec2&, const ImVec2&)
{
}

void DockContext::TryDockToStoredLocation(Dock&)
{
}

void DockContext::CleanDocks()
{
}

void DockContext::End()
{
}

void DockContext::SetDockPosSize(Dock&, Dock&, eDockSlot, Dock&)
{
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

eDockSlot DockContext::GetSlotFromCode(char code)
{
}

char DockContext::GetLocationCode(Dock* dock)
{
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

DockContext::Dock* DockContext::FindNonContainer(Dock&)
{
}

bool DockContext::Begin()
{
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

bool DockContext::Tabbar(Dock&, bool)
{
}

int DockContext::GetDockIndex(Dock*)
{
}







}}} // namespace DYSTOPIA::EGUI::DOCK

#endif //EDITOR










