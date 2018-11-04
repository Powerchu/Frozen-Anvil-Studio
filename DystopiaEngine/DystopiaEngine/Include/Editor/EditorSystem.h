/* HEADER *********************************************************************************/
/*!
\file	EditorSystem.h
\author Digipen (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#ifndef _EDITOR_SYSTEM_H_
#define _EDITOR_SYSTEM_H_
#include "Editor/EditorMessages.h"
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

	protected :
		static constexpr size_t nPos = -1;
	};
}
#endif
#endif



