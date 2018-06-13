/* HEADER *********************************************************************************/
/*!
\file	Dock.h
\author Shannon Tan (100%)
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

// Work-In-Progress
namespace EGUI2
{
	typedef int ImGuiWindowFlags;

	namespace Docking
	{
		enum eDockSlot
		{
			eDOCK_SLOT_LEFT,
			eDOCK_SLOT_RIGHT,
			eDOCK_SLOT_TOP,
			eDOCK_SLOT_BOTTOM,
			eDOCK_SLOT_TAB,

			eDOCK_SLOT_FLOATING,
			eDOCK_SLOT_NONE
		};

		bool BeginDockspace();
		void EndDockspace();
		void ShutdownDock();
		void SetNextDock(const char*, eDockSlot);
		bool BeginDock(const char*, bool* = nullptr, ImGuiWindowFlags = 0);
		void EndDock();
		void DockDebugWindow(const char*);
		void InitDock();
	}
}

#endif //_DOCK_H_
#endif //EDITOR










