/* HEADER *********************************************************************************/
/*!
\file	Commands.h
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
#ifndef _COMMANDS_H_
#define _COMMANDS_H_
#include <deque>

namespace Dystopia
{
	class Commands
	{
	public:
		virtual void ExecuteDo() const=0;
		virtual void ExecuteUndo() const=0;
	};

	// Command handler to record all commands made in the editor. *Intended only for editor class to use*
	class CommandHandler
	{
	public:
		// Constructs with a fixed size	maximum history of commands recorded).
		CommandHandler(size_t _nHistory=30);

		// Destructor
		~CommandHandler();

		// Calls the ExecuteDo function of the param command and passes it into the undo deque, also empties the redo deque
		void InvokeCommand(Commands *_comd);

		// Calls the ExecuteUndo function of latest command in the undo deque and puts it into the redo deque
		void UndoCommand();

		// Performs InvokeCommand on the latest command in the redo deque 
		void RedoCommand();

	private:
		std::deque<Commands*> mDeqRedo;
		std::deque<Commands*> mDeqUndo;
		unsigned mNextActionID;
	};
}

#endif // _COMMANDS_H_
#endif // EDITOR



