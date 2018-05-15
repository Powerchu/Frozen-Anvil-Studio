/* HEADER *********************************************************************************/
/*!
\file	Dock.h
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
#ifndef _DOCK_H_
#define _DOCK_H_
#include "../../Dependancies/ImGui/imgui.h"

namespace Dystopia
{
	namespace EGUI
	{
		namespace Dock
		{
			enum eDockSlot
			{
				eDOCK_LEFT = 0,
				eDOCK_RIGHT,
				eDOCK_TOP,
				eDOCK_BOTTOM,
				eDOCK_TAB,

				eDOCK_FLOAT,
				eDOCK_NONE
			};


			class DockContext
			{
			private:


			public:

				enum eEndAction
				{
					eEND_ACT_NONE = 0,
					eEND_ACT_PANEL,
					eEND_ACT_END,
					eEND_ACT_END_CHILD
				};
				enum eStatus
				{
					eSTAT_DOCK = 0,
					eSTAT_FLOAT,
					eSTAT_DRAGGED
				};

				class Dock
				{
				public:
					Dock();
					~Dock();
					ImVec2 GetMinSize() const;
					bool IsHorizontal() const;

				private:
					Dock* mpNextTab;
					Dock* mpPrevTab;
					Dock* mpChildren[2];
					Dock* mpParent;
					ImU32 mId;
					ImVec2 mPos;
					ImVec2 mSize;
					ImVec2 mFloatModeSize;
					eStatus mStatus;
					bool mActive;
					bool mOpened;
					bool mFirst;
					int mLastFrame;
					int mInvalidFrame;
					char mLocation[16];
					char* mLabel;
				};

			};

			DockContext* CreateContext();
			void DestroyContext(DockContext*);
			void SetCurrentContext(DockContext*);
			DockContext* GetCurrentContext();

			void BeginSpace();
			void EndSpace();
			void Shutdown();
			void SetNext(eDockSlot);

			void Begin(const char*, bool* _openVar = nullptr, ImGuiWindowFlags _extraFlags = 0, const ImVec2& _defSize = ImVec2{ 0,0 });
			void End();
			void SetActive();
			void DebugWindow(eDockSlot);

		}
	}
}

#endif // _DOCK_H_
#endif // EDITOR

