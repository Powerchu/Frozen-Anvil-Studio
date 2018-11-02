#ifndef _EDITOR_SYSTEM_H_
#define _EDITOR_SYSTEM_H_
#include "EditorMessages.h"
#include "IO/TextSerialiser.h"

namespace Editor
{
	class EditorSystem
	{
	public:
		virtual void Load(void) = 0;
		virtual bool Init(void) = 0;
		virtual void StartFrame(void) = 0;
		virtual void Update(float) = 0;
		virtual void EndFrame(void) = 0;
		virtual void Shutdown(void) = 0;
		virtual void Message(eEMessage) = 0;
		virtual void SaveSettings(Dystopia::TextSerialiser& _out) const = 0;
		virtual void LoadSettings(Dystopia::TextSerialiser& _in) = 0;
		virtual ~EditorSystem(void) {}
	};
}
#endif


