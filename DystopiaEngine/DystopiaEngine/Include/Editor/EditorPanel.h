/* HEADER *********************************************************************************/
/*!
\file	EditorPanel.h
\author Digipen (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#ifndef _EDITOR_PANEL_H_
#define _EDITOR_PANEL_H_
#include "DataStructure/HashString.h"
#include "Editor/EditorMessages.h"
#include "IO/TextSerialiser.h"

namespace Editor
{
	class EditorPanel
	{
	public:
		virtual void Load(void) = 0;
		virtual void Init(void) = 0;
		virtual void Update(float) = 0;
		virtual void EditorUI(void) = 0;
		virtual void Shutdown(void) = 0;
		virtual void Message(eEMessage) = 0;
		virtual void SaveSettings(Dystopia::TextSerialiser& _out) const = 0;
		virtual void LoadSettings(Dystopia::TextSerialiser& _in) = 0;
		virtual HashString GetLabel(void) const = 0;

		virtual ~EditorPanel(void) {}
	};
	
}
#endif 
#endif



