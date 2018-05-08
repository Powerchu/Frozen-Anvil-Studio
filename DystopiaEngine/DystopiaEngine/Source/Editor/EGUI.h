/* HEADER *********************************************************************************/
/*!
\file	EGUI.h
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief
BRIEF HERE

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR

#ifndef _EDITOR_GUI_H_
#define _EDITOR_GUI_H_

namespace Dystopia
{
	class WindowManager;
	class GraphicsSystem;
	class InputManager;

	namespace EGUI
	{
		bool Init(WindowManager *_pWin, GraphicsSystem *_pGfx, InputManager *_pInputMgr);
		void StartFrame();
		void Render();
		void Shutdown();
	}
}

#endif // !_EDITOR_GUI_H_

#endif // EDITOR ONLY