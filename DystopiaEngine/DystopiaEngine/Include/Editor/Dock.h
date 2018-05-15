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

typedef int ImGuiWindowFlags;
typedef unsigned int ImU32;

namespace ImGui
{
	struct ImVec2;
}

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

				class Dock
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

				private:
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

			private:

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










