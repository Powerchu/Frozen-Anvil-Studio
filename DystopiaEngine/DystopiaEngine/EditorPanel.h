#ifndef _EDITOR_PANEL_H_
#define _EDITOR_PANEL_H_
#include "DataStructure/HashString.h"
#include "EditorMessages.h"
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



