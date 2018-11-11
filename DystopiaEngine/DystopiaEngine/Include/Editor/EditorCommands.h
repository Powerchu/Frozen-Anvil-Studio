/* HEADER *********************************************************************************/
/*!
\file	EditorCommands.h
\author Digipen (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#ifndef _EDITOR_COMMANDS_H_
#define _EDITOR_COMMANDS_H_
#include "Editor/EditorSystem.h"
#include "Editor/CommandTypes.h"

#include <deque>

namespace Editor
{
	class EditorCommands : public EditorSystem
	{
	public:
		EditorCommands(void);
		~EditorCommands(void);

		void Load(void);
		bool Init(void);
		void StartFrame(void);
		void Update(float);
		void EndFrame(void);
		void Shutdown(void);
		void Message(eEMessage);
		void SaveSettings(Dystopia::TextSerialiser& _out) const;
		void LoadSettings(Dystopia::TextSerialiser& _in);

		void SavedChanges(void);
		bool HasChanges(void) const;

		void Undo(void);
		void Redo(void);

	private:

		std::deque<Command*> mDeqRedo;
		std::deque<Command*> mDeqUndo;
		bool mbChangesMade;
		size_t mnUndo;
		size_t mnRedo;

		void RemoveStray(std::deque<Command*>&);
	};
}

#endif
#endif 








