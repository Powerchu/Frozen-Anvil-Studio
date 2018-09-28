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
#include "Math\Vector2.h"

// Work-In-Progress
namespace EGUI
{
	typedef int ImGuiWindowFlags;
	namespace Docking
	{
		constexpr unsigned int MAX_TAB_SIZE_STACK = 10;
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

		bool		BeginDockableSpace();
		void		EndDockableSpace();
		void		ShutdownTabs();
		void		SetNextTabs(const char*, eDockSlot);
		bool		BeginTabs(const char*, bool* = nullptr, ImGuiWindowFlags = 0);
		void		EndTabs();
		void		InitTabs();
		void		PushTabSize(const Math::Vec2&);
		void		PushTabSize(const float&, const float&);
		void		PopTabSize();
		Math::Vec2	GetTabSize(const char*_tabName);
		Math::Vec2	GetTabPosition(const char*_tabName);
	}
}

#endif //_DOCK_H_
#endif //EDITOR










