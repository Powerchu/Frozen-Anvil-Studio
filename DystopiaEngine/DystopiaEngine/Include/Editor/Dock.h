/* HEADER *********************************************************************************/
/*!
\file	Dock.h
\author Digipen (100%)
\par    email: 
\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#ifndef _DOCK_H_
#define _DOCK_H_
#include "Math\Vector4.h"
#include "DataStructure\AutoArray.h"

#ifndef IMGUI_API
#include <imgui.h>
#endif //IMGUI_API

namespace Dystopia
{
	namespace EGUI
	{
		namespace Dock
		{
			typedef enum eDockSlot
			{
				eDOCK_LEFT = 0,
				eDOCK_RIGHT,
				eDOCK_TOP,
				eDOCK_BOTTOM,
				eDOCK_TAB,

				eDOCK_FLOAT,
				eDOCK_NONE

			} eDockSlot;

			class DockContext
			{
			public:
				enum eEndAction
				{
					eEnd_ACTION_NONE,
					eEnd_ACTION_PANEL,
					eEnd_ACTION_END,
					eEnd_ACTION_END_CHILD
				};

				enum eStatus
				{
					eSTAT_DOCK,
					eSTAT_FLOAT,
					eSTAT_DRAGGED
				};

				struct Dock
				{
				public:
					Dock();
					~Dock();
					Dock& GetRoot();
					Dock& GetSibling();
					Dock& GetFirstTab();
					ImVec2 GetMinSize() const;
					bool IsHorizontal() const;
					void SetParent(Dock*);
					void SetActive();
					bool HasChildren() const;
					void SetPosSize(const ImVec2&, const ImVec2&);
					void SetChildPosSize(const ImVec2&, const ImVec2&);

					char* mpLabel;
					Dock* mpNextTab;
					Dock* mpPrevTab;
					Dock* mpChildren[2];
					Dock* mpParent;
					ImVec2 mPos;
					ImVec2 mSize;
					ImVec2 mFloatmodeSize;
					eStatus mStatus;
					ImU32 mId;
					int mLastFrame;
					int mInvalidFrame;
					char mLocation[16];
					bool mActive;
					bool mOpened;
					bool mFirst;
				};

				DockContext();
				~DockContext();
				void Shutdown();
				void PutInBG();
				void Split();
				void CheckNonexistent();
				void SetDockActive();
				void HandleDrag(Dock&);
				void FillLocation(Dock&);
				void DoUndock(Dock&);
				void DoDock(Dock&, Dock*, eDockSlot);
				void DrawTabbarListBtn(Dock&);
				void RootDock(const ImVec2&, const ImVec2&);
				void TryDockToStoredLocation(Dock&);
				void CleanDocks();
				void End();
				static void SetDockPosSize(Dock&, Dock&, eDockSlot, Dock&);
				static ImRect GetDockedRect(const ImRect&, eDockSlot);
				static ImRect GetSlotRect(ImRect, eDockSlot);
				static ImRect GetSlotRectOnBorder(ImRect, eDockSlot);
				static eDockSlot GetSlotFromCode(char code);
				static char GetLocationCode(Dock* dock);

				Dock* GetDockAt(const ImVec2&) const;
				Dock& GetDock(const char*, bool, const ImVec2&);
				Dock* GetRootDock();
				Dock* FindNonContainer(Dock&);

				bool Begin();
				bool DockSlots(Dock&, Dock*, const ImRect&, bool);
				bool Tabbar(Dock&, bool);

				int GetDockIndex(Dock*);

			private:
				//ImVector<Dock*> m_docks;
				AutoArray<Dock*> mDocksArr;
				Dock *mpCurrent;
				Dock *mpNextParent;
				ImVec2 mDragOffset;
				ImVec2 mWorkspacePos;
				ImVec2 mWorkspaceSize;
				int mLastFrame;
				eEndAction mEndAction;
				eDockSlot mNextDockSlot;
				bool mIsBeginOpen;
				bool mIsFirstCall;
			};

			DockContext* CreateContext();
			void DestroyContext(DockContext*);
			void SetContext(DockContext*);
			DockContext* GetContext();

			void BeginSpace();
			void EndSpace();
			void Shutdown();
			void SetNextDock(eDockSlot);

			bool Begin(const char*, bool *_opened = nullptr, ImGuiWindowFlags _flags = 0, const Math::Vector4& _defSize = Math::Vector4{ 0, 0, 0, 0 });
			void End();
			void SetActive();
			void DebugWindow();

		}
	}
}

#endif //_DOCK_H_
#endif //EDITOR










